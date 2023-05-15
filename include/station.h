//==============================================================
//	Namespaces
//==============================================================

namespace Station {
	enum class FetchDataTypeEnum {
		CURRENT,
		FORECAST,
		AIR_POLLUTION
	};

	struct Location {
		float longitude = DEFAULT_LONGITUDE;
		float latitude = DEFAULT_LATITUTE;
		char* name;
	};

	// This gets calculated using values from DHT11
	struct InsideParameters {
		float temperature;
		float humidity;
		float dewPoint;
		float absoluteHumidity;
	};

	// This gets fetched from the openweathermap
	struct OutsideParameters {
		float temperature;
		float humidity;

		float feelsLikeTemperature;
		float minTemperature;
		float maxTemperature;

		float pressure;
	};
}

//==============================================================
//	Namespaces
//==============================================================

//==============================================================
//	Classes
//==============================================================

class WeatherData {
public:
	WeatherData(/* args */);
	~WeatherData();

	void setLocation(Station::Location location);
	void setInsideParameters(Station::InsideParameters insideParameters);
	void setOutsideParameters(Station::OutsideParameters outsideParameters);

	Station::Location getLocation();
	Station::InsideParameters getInsideParameters();
	Station::OutsideParameters getOutsideParameters();
private:
	Station::Location _location;
	Station::InsideParameters _insideParameters;
	Station::OutsideParameters _outsideParameters;
};

WeatherData::WeatherData(/* args */) {
}

WeatherData::~WeatherData() {
}

//==============================================================
//	Classes
//==============================================================

//==============================================================
//	Functions
//==============================================================

void WeatherData::setLocation(Station::Location location) {
	WeatherData::_location = location;
}

void WeatherData::setInsideParameters(Station::InsideParameters insideParameters) {
	WeatherData::_insideParameters = insideParameters;
}

void WeatherData::setOutsideParameters(Station::OutsideParameters outsideParameters) {
	WeatherData::_outsideParameters = outsideParameters;
}

Station::Location WeatherData::getLocation() {
	return WeatherData::_location;
}

Station::InsideParameters WeatherData::getInsideParameters() {
	return WeatherData::_insideParameters;
}

Station::OutsideParameters WeatherData::getOutsideParameters() {
	return WeatherData::_outsideParameters;
}

//==============================================================
//	Functions
//==============================================================