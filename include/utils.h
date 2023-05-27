//==============================================================
//	Functions
//==============================================================

template <typename T>
void debugPrint(T const& value, bool nl = true) {
	if (!DEBUG) { return; }

	if (nl) { Serial.println(value); }
	else { Serial.print(value); }
	
	if (ALSO_USE_WEB_SERIAL) {
		if (nl) { WebSerial.println(value); }
		else { WebSerial.print(value); }
	}
}

void debugTone(ToneType toneType) {
	if (DISABLE_BUZZER) {
		return;
	}
	
	switch (toneType) {
	case ToneType::SUCCESS:
		digitalWrite(BUZZER_PIN, HIGH);
		delay(100);
		digitalWrite(BUZZER_PIN, LOW);
		delay(30);
		digitalWrite(BUZZER_PIN, HIGH);
		delay(30);
		digitalWrite(BUZZER_PIN, LOW);
		delay(30);
		digitalWrite(BUZZER_PIN, HIGH);
		delay(250);
		digitalWrite(BUZZER_PIN, LOW);

		break;

	case ToneType::ERROR:
		digitalWrite(BUZZER_PIN, HIGH);
		delay(250);
		digitalWrite(BUZZER_PIN, LOW);
		delay(10);
		digitalWrite(BUZZER_PIN, HIGH);
		delay(10);
		digitalWrite(BUZZER_PIN, LOW);
		delay(10);
		digitalWrite(BUZZER_PIN, HIGH);
		delay(100);
		digitalWrite(BUZZER_PIN, LOW);

		break;

	case ToneType::INFO:
		digitalWrite(BUZZER_PIN, HIGH);
		delay(100);
		digitalWrite(BUZZER_PIN, LOW);
		delay(20);
		digitalWrite(BUZZER_PIN, HIGH);
		delay(80);
		digitalWrite(BUZZER_PIN, LOW);

		break;
	}
}

String generatePassword(uint8_t length = AP_PASSWORD_LENGTH) {
	String password;

	for (int i = 0; i < length; ++i) {
		int randomIndex = random(PASSWORD_CHARS.length());
		password += PASSWORD_CHARS[randomIndex];
	}

	return password;
}

String buildUrl(FetchDataType fetchDataType = FetchDataType::CURRENT_WEATHER) {
	String url = API_ENDPOINT;
	String secondaryUrl = "?lat=" + String(LATITUDE) + "&lon=" + String(LONGITUDE) + "&mode=json&units=";

	switch (FETCH_DATA_UNITS) {
	case FetchDataUnits::METRIC:
		secondaryUrl.concat(API_METRIC_UNITS);
		break;
	case FetchDataUnits::IMPERIAL:
		secondaryUrl.concat(API_IMPERIAL_UNITS);
		break;
	}

	switch (fetchDataType) {
	case FetchDataType::CURRENT_WEATHER:
		url.concat(API_CURRENT_WEATHER);
		break;
	case FetchDataType::WEATHER_FORECAST:
		url.concat(API_WEATHER_FORECAST);
		secondaryUrl.concat("&cn=" + FORECAST_TIMESTAMPS_AMOUNT);
		break;
	}

	// Compilator throws an error without explicitly stating the api key is a string...
	secondaryUrl.concat("&appid=" + String(API_KEY));
	url.concat(secondaryUrl);

	debugPrint("Url built: ");
	debugPrint(url);

	return url;
}

String qrEncodeAPCredentials(APCredentials apCredentials, String encryptionType = "nopass") {
	return "Network Name: " + apCredentials.ssid + "\nType: " + encryptionType + "\nPassword: " + apCredentials.password;
}

GETResult simpleGET(String url) {
	http.begin(client, url);

	int _responseCode = http.GET();
	String _payload = http.getString();

	debugPrint("Payload received: ");
	debugPrint(_payload);

	http.end();

	GETResult getResult;
	getResult.payload = _payload;
	getResult.responseCode = _responseCode;

	if (getResult.responseCode != HTTP_CODE_OK) {
		debugPrint("Http request error of code: ");
		debugPrint(getResult.responseCode);

		getResult.ok = false;
	}

	return getResult;
}

//==============================================================
//	Functions
//==============================================================