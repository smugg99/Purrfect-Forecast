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
	float batteryPercentage;
};

// Fetched using software
struct FetchedData {
	float humidity;
	float temperature;
	float pressure;
	float feelsLikeTemperature;
};

enum DISPLAY_POINT {
	CENTER,
	TOP_LEFT,
	TOP_RIGHT,
	TOP_CENTER,
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	BOTTOM_CENTER,
	RIGHT_CENTER,
	LEFT_CENTER
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

GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> display(GxEPD2_290_T94_V2(CS_PIN, DC_PIN, RST_PIN, BUSY_PIN)); // GDEM029T94 128x296, SSD1680, Waveshare 2.9" V2 variant

const u_int16_t DISPLAY_WIDTH = display.width();
const u_int16_t DISPLAY_HEIGHT = display.height();

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

String removeTrailingZeros(const String& input) {
	int lastNonZeroIndex = -1;
	int length = input.length();

	for (int i = length - 1; i >= 0; i--) {
		if (input.charAt(i) != '0') {
			lastNonZeroIndex = i;
			break;
		}
	}

	if (lastNonZeroIndex == -1) {
		return input;
	}

	return input.substring(0, lastNonZeroIndex + 1);
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