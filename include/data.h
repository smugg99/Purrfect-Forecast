enum ToneType {
	SUCCESS,
	ERROR,
	INFO
};

enum FetchDataUnits {
	METRIC,
	IMPERIAL
};

enum FetchDataType {
	CURRENT_WEATHER,
	WEATHER_FORECAST
};

struct StationStatus {
	bool internetAccess;
	bool wifiConnected;
	bool apiAccess;
};

struct APCredentials {
	String ssid;
	String password;
	String APIP;
};

struct GETResult {
	String payload;
	int responseCode;
	bool ok = true; // Set to false when error occurs
};

// Calculated or measured using hardware
struct QuantifiedData {
	float humidity;
	float temperature;
};

// Fetched using software
struct FetchedData {
	float humidity;
	float temperature;
	float pressure;
	float feelsLikeTemperature;
};

const int JSON_BUFFER_SIZE = 1024;
const String PASSWORD_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

// In the future below settings will be stored in the EEPROM
FetchDataUnits FETCH_DATA_UNITS = FetchDataUnits::METRIC;
String TEMPERATURE_UNIT;
String HUMIDITY_UNIT;

float LONGITUDE = DEFAULT_LONGITUDE;
float LATITUDE = DEFAULT_LATITUTE;

String WIFI_SSID = DEFAULT_WIFI_SSID;
String WIFI_PASSWORD = DEFAULT_WIFI_PASSWORD;

FetchedData currentWeatherData;
FetchedData weatherForecastData[FORECAST_DAYS_AMOUNT];
QuantifiedData quantifiedData;

StationStatus stationStatus;

WiFiClient client;
HTTPClient http;

DHTesp dht;

AsyncWebServer server(DEFAULT_PORT);

//==============================================================
//	Functions
//==============================================================

// Call this function AFTER reading the config from EEPROM!
void setupUnits() {
	HUMIDITY_UNIT = "%";

	switch (FETCH_DATA_UNITS) {
	case FetchDataUnits::METRIC:
		TEMPERATURE_UNIT = "*C";
		break;

	case FetchDataUnits::IMPERIAL:
		TEMPERATURE_UNIT = "*F";
		break;
	}
}

String removeTrailingZeros(String value) {
	value.trim();
	float floatValue = value.toFloat();

	String str = String(floatValue);
	str.trim();

	return str;
}

String toTemperature(float temperature) {
	return removeTrailingZeros(String(temperature)) + TEMPERATURE_UNIT;
}

String toHumidity(float humidity) {
	return removeTrailingZeros(String(humidity)) + HUMIDITY_UNIT;
}

String toBool(bool value) {
	return value ? "true" : "false";
}

//==============================================================
//	Functions
//==============================================================