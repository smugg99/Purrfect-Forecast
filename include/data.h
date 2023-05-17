enum FetchDataUnits {
	METRIC,
	IMPERIAL
};

enum FetchDataType {
	CURRENT_WEATHER,
	WEATHER_FORECAST
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