#include <Arduino.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <DHTesp.h>
#include <SPI.h>

#include "config.h"
#include "data.h"
#include "display.h" // for example Waveshare 2.9" E-paper display (12956)

WiFiClient client;
HTTPClient http;

DHTesp dht;

//==============================================================
//	Functions
//==============================================================

template <typename T>
void debugPrint(T const& value, bool nl = true) {
	if (!DEBUG) { return; }
	if (nl) { Serial.println(value); }
	else { Serial.print(value); }
}

void debugTone(ToneType toneType) {
	switch (toneType) {
	case ToneType::SUCCESS:
		digitalWrite(BUZZER_PIN, HIGH);
		delay(100);
		digitalWrite(BUZZER_PIN, LOW);
		delay(30);
		digitalWrite(BUZZER_PIN, HIGH);
		delay(30);
		digitalWrite(BUZZER_PIN, LOW);
		delay(30);
		digitalWrite(BUZZER_PIN, HIGH);
		delay(250);
		digitalWrite(BUZZER_PIN, LOW);

		break;

	case ToneType::ERROR:
		digitalWrite(BUZZER_PIN, HIGH);
		delay(250);
		digitalWrite(BUZZER_PIN, LOW);
		delay(10);
		digitalWrite(BUZZER_PIN, HIGH);
		delay(10);
		digitalWrite(BUZZER_PIN, LOW);
		delay(10);
		digitalWrite(BUZZER_PIN, HIGH);
		delay(100);
		digitalWrite(BUZZER_PIN, LOW);

		break;

	case ToneType::INFO:
		digitalWrite(BUZZER_PIN, HIGH);
		delay(100);
		digitalWrite(BUZZER_PIN, LOW);
		delay(20);
		digitalWrite(BUZZER_PIN, HIGH);
		delay(50);
		digitalWrite(BUZZER_PIN, LOW);

		break;
	}
}

String generatePassword(uint8_t length = AP_PASSWORD_LENGTH) {
	String password;

	for (int i = 0; i < length; ++i) {
		int randomIndex = random(PASSWORD_CHARS.length());
		password += PASSWORD_CHARS[randomIndex];
	}

	return password;
}

String buildUrl(FetchDataType fetchDataType = FetchDataType::CURRENT_WEATHER) {
	String url = API_ENDPOINT;
	String secondaryUrl = "?lat=" + String(LATITUDE) + "&lon=" + String(LONGITUDE) + "&mode=json&units=";

	switch (FETCH_DATA_UNITS) {
	case FetchDataUnits::METRIC:
		secondaryUrl.concat(API_METRIC_UNITS);
		break;
	case FetchDataUnits::IMPERIAL:
		secondaryUrl.concat(API_IMPERIAL_UNITS);
		break;
	}

	switch (fetchDataType) {
	case FetchDataType::CURRENT_WEATHER:
		url.concat(API_CURRENT_WEATHER);
		break;
	case FetchDataType::WEATHER_FORECAST:
		url.concat(API_WEATHER_FORECAST);
		secondaryUrl.concat("&cn=" + FORECAST_TIMESTAMPS_AMOUNT);
		break;
	}

	// Compilator throws an error without explicitly stating the api key is a string...
	secondaryUrl.concat("&appid=" + String(API_KEY));
	url.concat(secondaryUrl);

	debugPrint("Url built: ");
	debugPrint(url);

	return url;
}

GETResult simpleGET(String url) {
	http.begin(client, url);

	int _responseCode = http.GET();
	String _payload = http.getString();

	debugPrint("Payload received: ");
	debugPrint(_payload);

	http.end();

	GETResult getResult;
	getResult.payload = _payload;
	getResult.responseCode = _responseCode;

	if (getResult.responseCode != HTTP_CODE_OK) {
		debugPrint("Http request error of code: ");
		debugPrint(getResult.responseCode);

		getResult.ok = false;
	}

	return getResult;
}


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

bool checkWiFiConnection() {
	if (WiFi.status() != WL_CONNECTED) {
		debugPrint("Connection not ok!");
		return false;
	}
	else {
		debugPrint("Connection ok!");
		return true;
	}
}

void goToSleep() {
	debugPrint("Going to sleep!");

	// Idk why but this shit beeps when going to sleep...
	digitalWrite(BUZZER_PIN, LOW);
	displayGoingToSleepScreen();

	delay(FIXED_DELAY);

	ESP.deepSleep(SLEEP_DURATION);
}

void _fetchWeatherForecast() {
	debugPrint("Fetching weather forecast...");

	if (!checkWiFiConnection()) {
		return;
	}

	String currentWeatherUrl = buildUrl(FetchDataType::WEATHER_FORECAST);
	GETResult getResult = simpleGET(currentWeatherUrl);

	if (!getResult.ok) {
		debugPrint("Failed fetching weather forecast!");
		return;
	}

	DynamicJsonDocument document(JSON_BUFFER_SIZE);
	deserializeJson(document, getResult.payload);

	if (!document.containsKey("list")) {
		debugPrint("Invalid JSON format!");
		return;
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
}

void _fetchCurrentWeather() {
	debugPrint("Fetching current weather...");

	if (!checkWiFiConnection()) {
		return;
	}

	String currentWeatherUrl = buildUrl(FetchDataType::CURRENT_WEATHER);
	GETResult getResult = simpleGET(currentWeatherUrl);

	if (!getResult.ok) {
		debugPrint("Failed fetching current weather!");
		return;
	}

	DynamicJsonDocument document(JSON_BUFFER_SIZE);
	deserializeJson(document, getResult.payload);

	if (!document.containsKey("main")) {
		debugPrint("Invalid JSON format!");
		return;
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
void updateData() {
	_fetchCurrentWeather();
	_fetchWeatherForecast();
	_quantifyData();
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
		APCredentials apCredentials = launchAccessPoint();
		displayAccessPointScreen(apCredentials);

		// Go to sleep after some time when user didn't configure the WiFi yet
		// delay(INACTIVE_DURATION);
		// goToSleep();
	}
	else {
		updateData();
		displayPrimaryScreen();
		goToSleep();
	}
}

// Not used because the ESP wakes itself repeatedly
void loop() {}

//==============================================================
//	Functions
//==============================================================