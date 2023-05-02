class WeatherData {
public:
	struct InsideParameters {
		float temperature;
		float humidity;
	};

	struct Location {
		float longitude;
		float latitude;
		char* name;
	};

	struct OutsideParameters {
		Location location;
		float temperature;
		float humidity;

		float feelsLikeTemperature;
		float minTemperature;
		float maxTemperature;

		float pressure;
	};

	WeatherData(/* args */);
	~WeatherData();
private:
	InsideParameters _insideParameters;
	OutsideParameters _outsideParameters;
};

WeatherData::WeatherData(/* args */) {
}

WeatherData::~WeatherData() {
}