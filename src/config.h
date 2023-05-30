#include "secrets.h"

// Note: Size of the display I am currently using is 296x128 px 

#ifndef CONFIG_H
#define CONFIG_H

//==============================================================
//	API
//==============================================================

// Insert api key here
const String API_KEY = _API_KEY;
const String API_ENDPOINT = "http://api.openweathermap.org/data/2.5/";

const String API_CURRENT_WEATHER = "weather";
const String API_WEATHER_FORECAST = "forecast";
const String API_AIR_POLLUTION = "air_pollution";

const String API_METRIC_UNITS = "metric";
const String API_IMPERIAL_UNITS = "imperial";

//==============================================================
//	API
//==============================================================

//==============================================================
//	Defaults
//==============================================================

const String DEFAULT_WIFI_SSID = "Funbox6-420D";
const String DEFAULT_WIFI_PASSWORD = "Wysowianka1";

const uint16_t DEFAULT_PORT = 80;
const String PING_DNS = "8.8.8.8"; // Le Google DNS server

const double DEFAULT_LATITUTE = 49.5693;
const double DEFAULT_LONGITUDE = 20.8849;

const uint16_t FORECAST_TIMESTAMPS_AMOUNT = 3;
const uint16_t FORECAST_DAYS_AMOUNT = 5;

const uint32_t BAUD_RATE = 115200;

const bool DEBUG = true;
const bool ALSO_USE_WEB_SERIAL = true;

const bool DISABLE_DISPLAY = false;
const bool DISABLE_SLEEP = true;
const bool DISABLE_BUZZER = true;

const uint16_t FIXED_DELAY = 500;

// It's in MICRO seconds!
const uint32_t SLEEP_DURATION = 5000000;

const uint8_t AP_SSID_LENGTH = 18;
const uint8_t AP_PASSWORD_LENGTH = 8;

const uint16_t CONNECTION_MAX_RETRIES = 20;
const uint16_t CONNECTION_DELAY = 1000;

// #define MAX_BATTERY_VOLTAGE 4.2
// #define MIN_BATTERY_VOLTAGE 2.9

// Calibrate this...
const uint32_t RESISTOR_1 = 100000;
const uint32_t RESISTOR_2 = 200000;

// Used for normalizing the percentage, basically "offsets" the lower voltage
const uint16_t ADC_MAX = 1023;
const uint16_t ADC_MIN = 0;

const String SPLASH_SCREEN_TITLE = "Purrfect Forecast";
const String SPLASH_SCREEN_FOOTER = "Made by Karol Duda";

const uint16_t BOOT_SPLASH_DURATION = 500;

const String HARDWARE_VERSION = "V1.0";
const String SOFTWARE_VERSION = "V0.1";

const uint8_t DISPLAY_ROTATION = 3;  // Rotate the display 180* because I am retarded and designed the case in the wrong way

const uint16_t SAFETY_MARGIN_X = 2;
const uint16_t SAFETY_MARGIN_Y = 2;

const uint16_t FOOTER_OFFSET_X = 0;
const uint16_t FOOTER_OFFSET_Y = 15;

//==============================================================
//	Defaults
//==============================================================

//==============================================================
//	Pins
//==============================================================

const uint8_t RST_PIN = 2;
const uint8_t DC_PIN = 4;
const uint8_t CS_PIN = 15;
const uint8_t BUSY_PIN = 5;

const uint8_t DHT_DATA_PIN = 0;
const uint8_t BUZZER_PIN = 12;
const uint8_t BUTTON_PIN = 3;
const uint8_t ADC_PIN = A0;

//==============================================================
//	Pins
//==============================================================

#endif