#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <DHTesp.h>
#include <SPI.h>

#include "config.h"
#include "station.h"
#include "display.h" // for example Waveshare 2.9" E-paper display (12956)

WiFiClient client;
HTTPClient http;
WeatherData weatherData;

DHTesp dht;

const String PASSWORD_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

//==============================================================
//	Functions
//==============================================================

template <typename T>
void debugPrint(T const& value, bool nl = true) {
	if (!DEBUG) { return; }
	if (nl) { Serial.println(value); }
	else { Serial.print(value); }
}

String generatePassword(uint8_t length) {
	String password;

	for (int i = 0; i < length; ++i) {
		int randomIndex = random(PASSWORD_CHARS.length());
		password += PASSWORD_CHARS[randomIndex];
	}

	return password;
}

bool _waitForWiFiConnection() {
	debugPrint("Waiting for connection...");

	uint8_t retries = 0;
	bool buzzerState = false;

	while (WiFi.status() != WL_CONNECTED) {
		if (retries >= CONNECTION_MAX_RETRIES) {
			debugPrint("Max retries reached!");
			break;
		}

		delay(CONNECTION_DELAY / 2);

		debugPrint("Retrying: " + String(retries));

		digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
		buzzerState = buzzerState ? false : true;

		delay(CONNECTION_DELAY / 2);

		retries++;
	};

	return (WiFi.status() != WL_CONNECTED) ? false : true;
}

void launchAccessPoint() {
	WiFi.mode(WIFI_AP);

	String _password = generatePassword(AP_PASSWORD_LENGTH);
	String _ssid = "ESP-%06X" + ESP.getChipId();

	WiFi.softAP(_ssid, _password);

	String _APIP = WiFi.softAPIP().toString();

	debugPrint("\nAP SSID: ");
	debugPrint(_ssid);
	debugPrint("AP Password: ");
	debugPrint(_password);
	debugPrint("AP IP: ");
	debugPrint(_APIP);

	displayAccessPointScreen(_ssid, _password, _APIP);
}

void connectToWiFi() {
	debugPrint("\nConnecting to: ");
	debugPrint(DEFAULT_WIFI_SSID);

	WiFi.mode(WIFI_STA);
	WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD);

	if (_waitForWiFiConnection()) {
		debugPrint("Connected successfully!");
		debugPrint(WiFi.localIP());
		debugPrint(WiFi.macAddress());
	}
	else {
		debugPrint("Connection failed! Launching AP...");
		launchAccessPoint();
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

void setup() {
	pinMode(ADC_PIN, INPUT);
	pinMode(BUTTON_PIN, INPUT);
	pinMode(BUZZER_PIN, OUTPUT);

	digitalWrite(BUZZER_PIN, LOW);

	if (DEBUG) {
		Serial.begin(BAUD_RATE);
	}

	digitalWrite(BUZZER_PIN, HIGH);
	delay(20);
	digitalWrite(BUZZER_PIN, LOW);
	delay(50);
	digitalWrite(BUZZER_PIN, HIGH);
	delay(50);
	digitalWrite(BUZZER_PIN, LOW);

	dht.setup(DHT_DATA_PIN, DHTesp::DHT11);

	setupDisplay();
	connectToWiFi();

	goToSleep();
}

void loop() {

}

//==============================================================
//	Functions
//==============================================================

//==============================================================
//	Task Functions
//==============================================================

void _fetchData() {
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

	//update the weather data object here after sucessfully parsing json etc

	http.end();
}

void _sampleDHT() {
	float _humidity = dht.getHumidity();
	float _temperature = dht.getTemperature();
	float _absoluteHumidity = dht.computeAbsoluteHumidity(_temperature, _humidity, false);
	float _dewPoint = dht.computeDewPoint(_temperature, _humidity);

	debugPrint("Humidity: "); debugPrint(_humidity, false);
	debugPrint("Temperature: "); debugPrint(_temperature, false);
	debugPrint("Absoulute humidity: "); debugPrint(_absoluteHumidity, false);
	debugPrint("Dew point: "); debugPrint(_dewPoint, false);

	weatherData.setInsideParameters(Station::InsideParameters {
		_temperature, _humidity, _dewPoint, _absoluteHumidity
	});
}

//==============================================================
//	Task Functions
//==============================================================