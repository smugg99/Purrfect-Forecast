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

void fetchWeatherDataTask();

Task weatherTask(10000, TASK_FOREVER, &fetchWeatherDataTask);

//==============================================================
//	Task Declarations
//==============================================================

//==============================================================
//	Functions
//==============================================================

void connectToWiFi() {
	Serial.print("\nConnecting to: ");
	Serial.println(DEFAULT_WIFI_SSID);

	Serial.println("Waiting for connection...");
	while (WiFi.status() != WL_CONNECTED) {
		delay(100);
		Serial.print(".");
	};

	Serial.println("\nConnected!");
	Serial.println(WiFi.localIP());
	Serial.println(WiFi.macAddress());
}

void setup() {
	Serial.begin(BAUD_RATE);

	WiFi.mode(WIFI_STA);
	WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD);

	connectToWiFi();

	runner.init();
	runner.addTask(weatherTask);

	weatherTask.enable();
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

void fetchWeatherDataTask() {
	if (WiFi.status() != WL_CONNECTED) {
		Serial.println("Connection lost!");
		connectToWiFi();
	}

	http.begin(client, "http://api.openweathermap.org/data/2.5/weather?q=Bogusza&appid=" + String(API_KEY) + "&mode=json&units=metric&cnt=2");

	int httpResponseCode = http.GET();

	if (httpResponseCode <= 0) {
		Serial.println("Http request error of code: ");
		Serial.print(httpResponseCode);
	}

	String payload = http.getString();
	Serial.println(payload);

	http.end();
}

//==============================================================
//	Task Functions
//==============================================================