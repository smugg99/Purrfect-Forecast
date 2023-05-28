#include <Arduino.h>

#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266Ping.h>

#include <ArduinoJson.h>
#include <AsyncElegantOTA.h>
#include <WebSerialLite.h>

#include <DHTesp.h>
#include <SPI.h>
#include <qrcode.h>

#include "config.h"
#include "data.h"
#include "utils.h"
#include "display.h" // for example Waveshare 2.9" E-paper display (12956)

//==============================================================
//	Functions
//==============================================================

bool waitForWiFiConnection() {
	debugPrint("Waiting for connection...");

	uint8_t retries = 0;

	while (WiFi.status() != WL_CONNECTED) {
		if (retries >= CONNECTION_MAX_RETRIES) {
			debugPrint("Max retries reached!");
			return false;
		}

		delay(CONNECTION_DELAY);
		retries++;

		debugPrint("Retrying: " + String(retries));
	};

	return true;
}

APCredentials launchAccessPoint() {
	WiFi.mode(WIFI_AP);

	String _password = generatePassword();
	String _ssid = "ESP-" + String(ESP.getChipId());

	WiFi.softAP(_ssid, _password);

	String _APIP = WiFi.softAPIP().toString();

	debugPrint("\nAP SSID: ");
	debugPrint(_ssid);
	debugPrint("AP Password: ");
	debugPrint(_password);
	debugPrint("AP IP: ");
	debugPrint(_APIP);

	APCredentials _apCredentials;
	_apCredentials.ssid = _ssid;
	_apCredentials.password = _password;
	_apCredentials.APIP = _APIP;

	return _apCredentials;
}

bool connectToWiFi() {
	debugPrint("\nConnecting to: ");
	debugPrint(WIFI_SSID);

	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	if (waitForWiFiConnection()) {
		debugPrint("Connection established!");
		debugPrint(WiFi.localIP());
		debugPrint(WiFi.macAddress());

		return true;
	}

	debugPrint("Connection failed!");

	return false;
}

bool checkInternetConnection() {
	if (!Ping.ping(PING_DNS)) {
		debugPrint("Internet connection not ok!");
		return false;
	}

	debugPrint("Internet connection ok!");
	return true;
}

bool checkWiFiConnection() {
	if (WiFi.status() != WL_CONNECTED) {
		debugPrint("WiFi connection not ok!");
		return false;
	}

	debugPrint("WiFi connection ok!");
	return true;
}

void messageReceived(uint8_t* data, size_t len) {
	debugPrint("[Callback] ", false);
	String d = "";
	for (size_t i = 0; i < len; i++) {
		d += char(data[i]);
	}
	debugPrint(d);
}

void startServer() {
	debugPrint("Starting server...");

	server.on("/", HTTP_GET, [] (AsyncWebServerRequest* request) {
		request->send(200, "text/plain", "\"/update\" ---> OTA\n\"/webserial\" ---> SERIAL");
		});

	WebSerial.begin(&server);
	WebSerial.onMessage(messageReceived);
	
	AsyncElegantOTA.begin(&server);

	server.begin();

	debugTone(ToneType::INFO);
}

void goToSleep() {
	if (DISABLE_SLEEP) {
		debugPrint("Sleep is disabled due to OTA functionality!");
		return;
	}

	debugPrint("Going to sleep!");
	debugTone(ToneType::INFO);

	// Idk why but this shit beeps when going to sleep...
	digitalWrite(BUZZER_PIN, LOW);
	displayGoingToSleepScreen();

	delay(FIXED_DELAY);

	ESP.deepSleep(SLEEP_DURATION);
}

bool _fetchWeatherForecast() {
	debugPrint("Fetching weather forecast...");

	if (!checkWiFiConnection()) {
		return false;
	}

	String currentWeatherUrl = buildUrl(FetchDataType::WEATHER_FORECAST);
	GETResult getResult = simpleGET(currentWeatherUrl);

	if (!getResult.ok) {
		debugPrint("Failed fetching weather forecast!");
		return false;
	}

	DynamicJsonDocument document(JSON_BUFFER_SIZE);
	deserializeJson(document, getResult.payload);

	if (!document.containsKey("list")) {
		debugPrint("Invalid JSON format!");
		return false;
	}

	JsonArray forecastArray = document["list"].as<JsonArray>();
	FetchedData newWeatherForecastData[forecastArray.size()] = {};

	if (forecastArray.size() > 0) {
		for (size_t i = 0; i < forecastArray.size(); i++) {
			JsonObject forecast = forecastArray[i].as<JsonObject>();

			FetchedData newWeatherData;
			newWeatherData.feelsLikeTemperature = forecast["main"]["feels_like"];
			newWeatherData.humidity = forecast["main"]["humidity"];
			newWeatherData.pressure = forecast["main"]["pressure"];
			newWeatherData.temperature = forecast["main"]["temp"];

			newWeatherForecastData[i] = newWeatherData;
		}
	}

	for (size_t i = 0; i < FORECAST_DAYS_AMOUNT; i++) {
		weatherForecastData[i] = newWeatherForecastData[i];
	}

	return true;
}

bool _fetchCurrentWeather() {
	debugPrint("Fetching current weather...");

	if (!checkWiFiConnection()) {
		return false;
	}

	String currentWeatherUrl = buildUrl(FetchDataType::CURRENT_WEATHER);
	GETResult getResult = simpleGET(currentWeatherUrl);

	if (!getResult.ok) {
		debugPrint("Failed fetching current weather!");
		return false;
	}

	DynamicJsonDocument document(JSON_BUFFER_SIZE);
	deserializeJson(document, getResult.payload);

	if (!document.containsKey("main")) {
		debugPrint("Invalid JSON format!");
		return false;
	}

	FetchedData newCurrentWeatherData;
	newCurrentWeatherData.temperature = document["main"]["temp"];
	newCurrentWeatherData.feelsLikeTemperature = document["main"]["feels_like"];
	newCurrentWeatherData.humidity = document["main"]["humidity"];
	newCurrentWeatherData.pressure = document["main"]["pressure"];

	debugPrint("FHumidity: "); debugPrint(newCurrentWeatherData.humidity);
	debugPrint("FTemperature: "); debugPrint(newCurrentWeatherData.temperature);
	debugPrint("FPressure: "); debugPrint(newCurrentWeatherData.pressure);
	debugPrint("FTemperature feels like: "); debugPrint(newCurrentWeatherData.feelsLikeTemperature);

	currentWeatherData = newCurrentWeatherData;

	return true;
}

void _quantifyData() {
	float _humidity = dht.getHumidity();
	float _temperature = dht.getTemperature();
	float _absoluteHumidity = dht.computeAbsoluteHumidity(_temperature, _humidity, false);
	float _dewPoint = dht.computeDewPoint(_temperature, _humidity);

	debugPrint("Humidity: "); debugPrint(_humidity);
	debugPrint("Temperature: "); debugPrint(_temperature);
	debugPrint("Absoulute humidity: "); debugPrint(_absoluteHumidity);
	debugPrint("Dew point: "); debugPrint(_dewPoint);

	QuantifiedData newQuantifiedData;
	newQuantifiedData.humidity = _humidity;
	newQuantifiedData.temperature = _temperature;

	quantifiedData = newQuantifiedData;
}

// Implement last quantified data etc. caching logic to handle http errors
bool updateData() {
	_quantifyData();

	return _fetchCurrentWeather() && _fetchWeatherForecast();
}

void setup() {
	pinMode(ADC_PIN, INPUT);
	pinMode(BUTTON_PIN, INPUT);
	pinMode(BUZZER_PIN, OUTPUT);

	digitalWrite(BUZZER_PIN, LOW);

	delay(FIXED_DELAY);

	if (DEBUG) {
		Serial.begin(BAUD_RATE);
		Serial.setTimeout(2000);

		while (!Serial) {
			delay(FIXED_DELAY);
		}

		debugTone(ToneType::ERROR);
		delay(1000);
		debugTone(ToneType::SUCCESS);
		delay(1000);
		debugTone(ToneType::INFO);
		delay(1000);
	}

	dht.setup(DHT_DATA_PIN, DHTesp::DHT11);

	setupUnits();
	setupDisplay();

	displayBootSplash();
	delay(BOOT_SPLASH_DURATION);

	bool connectionEstablished = connectToWiFi();

	if (!connectionEstablished) {
		debugTone(ToneType::ERROR);

		APCredentials apCredentials = launchAccessPoint();
		displayAccessPointScreen(apCredentials);

		// Go to sleep after some time when user didn't configure the WiFi yet
		// delay(INACTIVE_DURATION);
		// goToSleep();
	}
	else {
		debugTone(ToneType::SUCCESS);

		startServer();

		// Update the devices status report
		stationStatus.apiAccess = updateData();
		stationStatus.internetAccess = checkInternetConnection();
		stationStatus.wifiConnected = checkWiFiConnection();

		//displayPrimaryScreen();
		displayBitmaps();
		
		goToSleep();
	}
}

// Not used because the ESP wakes itself repeatedly
void loop() {}

//==============================================================
//	Functions
//==============================================================