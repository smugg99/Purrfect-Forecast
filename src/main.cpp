#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <TaskScheduler.h>

#include "WeatherData.h"
#include "config.h"

Scheduler runner;
WiFiClient client;
HTTPClient http;
WeatherData weatherData;

//==============================================================
//	Task Declarations
//==============================================================

void _fetchWeatherData();
void _updateDisplay();

Task fetchWeatherDataTask(10000, TASK_FOREVER, &_fetchWeatherData);
Task updateDisplayTask(12000, TASK_FOREVER, &_updateDisplay);

Task* startupTasks[] = { &fetchWeatherDataTask, &updateDisplayTask };

//==============================================================
//	Task Declarations
//==============================================================

//==============================================================
//	Functions
//==============================================================

template <typename T>
void debugPrint(T const& value, bool nl = true) {
	if (!DEBUG) { return; }
	if (nl) { Serial.println(value); }
	else { Serial.print(value); }
}

void connectToWiFi() {
	debugPrint("\nConnecting to: ");
	debugPrint(DEFAULT_WIFI_SSID, false);

	debugPrint("Waiting for connection...");
	while (WiFi.status() != WL_CONNECTED) {
		delay(100);
		Serial.print(".");
	};

	debugPrint("Connected!");
	debugPrint(WiFi.localIP());
	debugPrint(WiFi.macAddress());
}

void setup() {
	Serial.begin(BAUD_RATE);

	WiFi.mode(WIFI_STA);
	WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD);

	connectToWiFi();

	runner.init();

	for (uint8_t i = 0; i < sizeof(startupTasks) / sizeof(Task*); i++) {
		runner.addTask(*startupTasks[i]);
		startupTasks[i]->enable();
	}
}

void loop() {
	runner.execute();

	delay(5000);
	yield();
}

//==============================================================
//	Functions
//==============================================================

//==============================================================
//	Task Functions
//==============================================================

void _fetchWeatherData() {
	debugPrint("Fetching weather data...");

	if (WiFi.status() != WL_CONNECTED) {
		debugPrint("Connection lost! Trying to reconnect...");
		connectToWiFi();
	}

	http.begin(client, "http://api.openweathermap.org/data/2.5/weather?q=Bogusza&appid=" + String(API_KEY) + "&mode=json&units=metric&cnt=2");

	int httpResponseCode = http.GET();

	if (httpResponseCode <= 0) {
		Serial.println("Http request error of code: ");
		Serial.print(httpResponseCode);
	}

	String payload = http.getString();
	debugPrint(payload);

	http.end();
}

void _updateDisplay() {
	debugPrint("Updating display...");
}

//==============================================================
//	Task Functions
//==============================================================