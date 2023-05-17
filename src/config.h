#include "secrets.h"

#ifndef CONFIG_H
#define CONFIG_H

//==============================================================
//	API
//==============================================================

// Insert api key here
#define API_KEY _API_KEY
#define API_ENDPOINT "http://api.openweathermap.org/data/2.5/"

#define API_CURRENT_WEATHER "weather"
#define API_WEATHER_FORECAST "forecast"
//#define API_AIR_POLLUTION "air_pollution"

#define API_METRIC_UNITS "metric"
#define API_IMPERIAL_UNITS "imperial"

//==============================================================
//	API
//==============================================================

//==============================================================
//	Defaults
//==============================================================

#define DEFAULT_WIFI_SSID "Funbox6-420D"
#define DEFAULT_WIFI_PASSWORD "Wysowianka1"

#define DEFAULT_LATITUTE 49.5693
#define DEFAULT_LONGITUDE 20.8849

#define FORECAST_TIMESTAMPS_AMOUNT 3
#define FORECAST_DAYS_AMOUNT 5

#define BAUD_RATE 115200
#define DEBUG true

#define FIXED_DELAY 1000

// It's in MICRO seconds!
#define SLEEP_DURATION 5000000

#define AP_SSID_LENGTH 18
#define AP_PASSWORD_LENGTH 8

#define CONNECTION_MAX_RETRIES 20
#define CONNECTION_DELAY 2000

#define MAX_BATTERY_VOLTAGE 4.2
#define MIN_BATTERY_VOLTAGE 2.9

#define SPLASH_SCREEN_TITLE "Purrfect Forecast"
#define SPLASH_SCREEN_FOOTER "Made by Karol Duda"
#define SPLASH_DURATION 1500

#define HARDWARE_VERSION = "V1.0"
#define SOFTWARE_VERSION = "V0.1"

#define DISPLAY_ROTATION 3  // Rotate the display 180* because I am retarded and designed the case in the wrong way

#define FOOTER_OFFSET_X 0
#define FOOTER_OFFSET_Y 25

//==============================================================
//	Defaults
//==============================================================

//==============================================================
//	Pins
//==============================================================

#define RST_PIN 2
#define DC_PIN 4
#define CS_PIN 15
#define BUSY_PIN 5

#define DHT_DATA_PIN 0
#define BUZZER_PIN 12
#define BUTTON_PIN 3
#define ADC_PIN A0

//==============================================================
//	Pins
//==============================================================

#endif