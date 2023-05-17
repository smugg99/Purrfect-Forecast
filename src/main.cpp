#include <Arduino.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <DHTesp.h>
#include <SPI.h>

#include "config.h"
#include "data.h"
#include "display.h" // for example Waveshare 2.9" E-paper display (12956)

const int JSON_BUFFER_SIZE = 1024;  // Adjust the buffer size as per your JSON data size
const String PASSWORD_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

WiFiClient client;
HTTPClient http;

DHTesp dht;

// In the future below settings will be stored in the EEPROM
FetchDataUnits FETCH_DATA_UNITS = FetchDataUnits::METRIC;
float LONGITUDE = DEFAULT_LONGITUDE;
float LATITUDE = DEFAULT_LATITUTE;

String WIFI_SSID = DEFAULT_WIFI_SSID;
String WIFI_PASSWORD = DEFAULT_WIFI_PASSWORD;

FetchedData lastCurrentWeatherData;
FetchedData* lastWeatherForecastData;

//==============================================================
//	Functions
//==============================================================

template <typename T>
void debugPrint(T const& value, bool nl = true) {
	if (!DEBUG) { return; }
	if (nl) { Serial.println(value); }
	else { Serial.print(value); }
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

void launchAccessPoint() {
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

	//displayAccessPointScreen(_ssid, _password, _APIP);
}

void connectToWiFi() {
	debugPrint("\nConnecting to: ");
	debugPrint(WIFI_SSID);

	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	if (waitForWiFiConnection()) {
		debugPrint("Connection established!");
		debugPrint(WiFi.localIP());
		debugPrint(WiFi.macAddress());
	}
	else {
		debugPrint("Connection failed! Launching AP...");
		launchAccessPoint();
	}
}

void checkWiFiConnection() {
	if (WiFi.status() != WL_CONNECTED) {
		debugPrint("Connection lost! Trying to reconnect...");
		connectToWiFi();
	}
	else {
		debugPrint("Connection ok!");
	}
}


void ICACHE_RAM_ATTR wakeUp() {
	wifi_set_sleep_type(NONE_SLEEP_T);

	displayPrimaryScreen();
	delay(FIXED_DELAY);

	debugPrint("Waking up!");

	ESP.deepSleep(SLEEP_DURATION);
}

void goToSleep() {
	debugPrint("Going to sleep!");

	// Idk why but this shit beeps when going to sleep...
	digitalWrite(BUZZER_PIN, LOW);

	ESP.deepSleep(SLEEP_DURATION);
}


FetchedData* _fetchWeatherForecast() {
	debugPrint("Fetching weather forecast...");
	checkWiFiConnection();

	String currentWeatherUrl = buildUrl(FetchDataType::WEATHER_FORECAST);
	GETResult getResult = simpleGET(currentWeatherUrl);

	return lastWeatherForecastData;
}

FetchedData _fetchCurrentWeather() {
	debugPrint("Fetching current weather...");
	checkWiFiConnection();

	String currentWeatherUrl = buildUrl(FetchDataType::CURRENT_WEATHER);
	GETResult getResult = simpleGET(currentWeatherUrl);

	if (!getResult.ok) {
		debugPrint("Failed fetching current weather!");
		return lastCurrentWeatherData;
	}

	DynamicJsonDocument document(JSON_BUFFER_SIZE);
	deserializeJson(document, getResult.payload);

	FetchedData fetchedCurrentWeatherData;
	fetchedCurrentWeatherData.temperature = document["main"]["temp"];
	fetchedCurrentWeatherData.feelsLikeTemperature = document["main"]["feels_like"];
	fetchedCurrentWeatherData.humidity = document["main"]["humidity"];
	fetchedCurrentWeatherData.pressure = document["main"]["pressure"];

	debugPrint("FHumidity: "); debugPrint(fetchedCurrentWeatherData.humidity);
	debugPrint("FTemperature: "); debugPrint(fetchedCurrentWeatherData.temperature);
	debugPrint("FPressure: "); debugPrint(fetchedCurrentWeatherData.pressure);
	debugPrint("FTemperature feels like: "); debugPrint(fetchedCurrentWeatherData.feelsLikeTemperature);

	return fetchedCurrentWeatherData;
}

QuantifiedData _quantifyData() {
	float _humidity = dht.getHumidity();
	float _temperature = dht.getTemperature();
	float _absoluteHumidity = dht.computeAbsoluteHumidity(_temperature, _humidity, false);
	float _dewPoint = dht.computeDewPoint(_temperature, _humidity);

	debugPrint("Humidity: "); debugPrint(_humidity);
	debugPrint("Temperature: "); debugPrint(_temperature);
	debugPrint("Absoulute humidity: "); debugPrint(_absoluteHumidity);
	debugPrint("Dew point: "); debugPrint(_dewPoint);

	QuantifiedData quantifiedData;
	quantifiedData.humidity = _humidity;
	quantifiedData.temperature = _temperature;

	return quantifiedData;
}

// Implement last quantified data etc. caching logic to handle http errors
void handleData() {
	FetchedData currentWeatherData = _fetchCurrentWeather();
	FetchedData* weatherForecastData = _fetchWeatherForecast();
	QuantifiedData quantifiedData = _quantifyData();
}


void setup() {
	pinMode(ADC_PIN, INPUT);
	pinMode(BUTTON_PIN, INPUT);
	pinMode(BUZZER_PIN, OUTPUT);

	digitalWrite(BUZZER_PIN, LOW);

	delay(FIXED_DELAY);

	if (DEBUG) {
		Serial.begin(BAUD_RATE);

		digitalWrite(BUZZER_PIN, HIGH);
		delay(20);
		digitalWrite(BUZZER_PIN, LOW);
		delay(50);
		digitalWrite(BUZZER_PIN, HIGH);
		delay(50);
		digitalWrite(BUZZER_PIN, LOW);
	}

	dht.setup(DHT_DATA_PIN, DHTesp::DHT11);

	//setupDisplay();
	connectToWiFi();
	handleData();

	//goToSleep();
}

// Not used because the ESP wakes itself repeatedly
void loop() {}

//==============================================================
//	Functions
//==============================================================