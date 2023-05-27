#include "fonts.h"
#include "bitmaps.h"

GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> display(GxEPD2_290_T94_V2(CS_PIN, DC_PIN, RST_PIN, BUSY_PIN)); // GDEM029T94 128x296, SSD1680, Waveshare 2.9" V2 variant

const u_int16_t DISPLAY_WIDTH = display.width();
const u_int16_t DISPLAY_HEIGHT = display.height();

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

//==============================================================
//	Functions
//==============================================================

void printPointedText(const String text, DISPLAY_POINT displayPoint = DISPLAY_POINT::CENTER, uint16_t offsetX = 0, uint16_t offsetY = 0) {
	if (DISABLE_DISPLAY) { return; }

	int16_t tbx, tby;
	uint16_t tbw, tbh;

	display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);

	// Compiler gave me warning that below variables might not be initialized...
	int16_t centerX = 0;
	int16_t centerY = 0;

	int16_t x = 0;
	int16_t y = 0;

	switch (displayPoint) {
	case CENTER:
		x = display.width() / 2;
		y = display.height() / 2;
		centerX = x - (tbw / 2);
		centerY = y - (tbh / 2);

		break;
	case TOP_LEFT:
		centerY = y + tbh + SAFETY_MARGIN_Y;
		centerX = x + SAFETY_MARGIN_X;

		break;
	case TOP_RIGHT:
		x = display.width();
		centerX = x - tbw - SAFETY_MARGIN_X;
		centerY = y + tbh + SAFETY_MARGIN_Y;

		break;
	case TOP_CENTER:
		x = display.width() / 2;
		centerX = x - (tbw / 2);
		centerY = y + tbh + SAFETY_MARGIN_Y;

		break;
	case BOTTOM_LEFT:
		y = display.height();
		centerY = y - (tbh / 2) - SAFETY_MARGIN_Y;
		centerX = x + SAFETY_MARGIN_X;

		break;
	case BOTTOM_RIGHT:
		x = display.width();
		y = display.height();

		centerX = x - tbw - SAFETY_MARGIN_X;
		centerY = y - (tbh / 2) - SAFETY_MARGIN_Y;

		break;
	case BOTTOM_CENTER:
		x = display.width() / 2;
		y = display.height();

		centerX = x - (tbw / 2);
		centerY = y - (tbh / 2) - SAFETY_MARGIN_Y;

		break;
	case RIGHT_CENTER:
		x = display.width();
		y = display.height() / 2;

		centerX = x - tbw - SAFETY_MARGIN_X;
		centerY = y - (tbh / 2);

		break;
	case LEFT_CENTER:
		y = display.height() / 2;

		centerX = x + SAFETY_MARGIN_X;
		centerY = y - (tbh / 2);

		break;
	default:
		break;
	}

	display.setCursor(centerX + offsetX, centerY + offsetY);
	display.println(text);
}

void clearDisplay() {
	display.fillScreen(GxEPD_WHITE);
}

void setDisplayDefaults() {
	display.setTextWrap(false);
	display.setFont(&Linerama_Regular12pt7b);
	display.setTextColor(GxEPD_BLACK);
	display.setTextSize(1);

	display.setFullWindow();
	display.firstPage();
}

void displayAccessPointScreen(APCredentials apCredentials) {
	if (DISABLE_DISPLAY) { return; }

	String qrCodeContent = qrEncodeAPCredentials(apCredentials);

	do {
		display.fillScreen(GxEPD_WHITE);

		display.setFont(&Linerama_Bold12pt7b);
		printPointedText("No WiFi!", DISPLAY_POINT::TOP_CENTER);

		display.setFont(&Linerama_Regular8pt7b);

		printPointedText(apCredentials.ssid, DISPLAY_POINT::CENTER, FOOTER_OFFSET_X, -FOOTER_OFFSET_Y * 3);
		printPointedText(apCredentials.password, DISPLAY_POINT::CENTER, FOOTER_OFFSET_X, -FOOTER_OFFSET_Y * 2);
		printPointedText(apCredentials.APIP, DISPLAY_POINT::CENTER, FOOTER_OFFSET_X, -FOOTER_OFFSET_Y);
	} while (display.nextPage());

	setDisplayDefaults();
	display.hibernate();
}

void displayBootSplash() {
	if (DISABLE_DISPLAY) { return; }

	do {
		display.fillScreen(GxEPD_WHITE);

		display.setFont(&Linerama_Bold12pt7b);
		printPointedText(SPLASH_SCREEN_TITLE, DISPLAY_POINT::TOP_CENTER);

		display.setFont(&Linerama_Regular8pt7b);
		printPointedText("Connecting to " + String(WIFI_SSID) + "...");

		display.setFont(&Linerama_Regular6pt7b);
		printPointedText(SPLASH_SCREEN_FOOTER, DISPLAY_POINT::BOTTOM_CENTER);
		printPointedText("Software " + String(SOFTWARE_VERSION), DISPLAY_POINT::BOTTOM_LEFT);
		printPointedText("Hardware " + String(HARDWARE_VERSION), DISPLAY_POINT::BOTTOM_RIGHT);
	} while (display.nextPage());

	setDisplayDefaults();
	display.hibernate();
}

void displayPrimaryScreen() {
	if (DISABLE_DISPLAY) { return; }

	do {
		display.fillScreen(GxEPD_WHITE);
		display.setFont(&Linerama_Regular8pt7b);

		printPointedText("Api access: " + toBool(stationStatus.apiAccess), DISPLAY_POINT::CENTER, FOOTER_OFFSET_X, -FOOTER_OFFSET_Y * 3);
		printPointedText("Internet access: " + toBool(stationStatus.internetAccess), DISPLAY_POINT::CENTER, FOOTER_OFFSET_X, -FOOTER_OFFSET_Y * 2);
		printPointedText("WiFi connected: " + toBool(stationStatus.wifiConnected), DISPLAY_POINT::CENTER, FOOTER_OFFSET_X, -FOOTER_OFFSET_Y);

		printPointedText(toHumidity(currentWeatherData.humidity), DISPLAY_POINT::TOP_LEFT);
		printPointedText(toTemperature(currentWeatherData.temperature), DISPLAY_POINT::TOP_RIGHT);

		printPointedText(toHumidity(quantifiedData.humidity), DISPLAY_POINT::BOTTOM_LEFT);
		printPointedText(toTemperature(quantifiedData.temperature), DISPLAY_POINT::BOTTOM_RIGHT);

		display.drawBitmap(48, 70, bitmap_sun[0], 24, 24, GxEPD_BLACK);
		display.drawBitmap(120, 70, bitmap_sun[1], 32, 32, GxEPD_BLACK);
		display.drawBitmap(170, 70, bitmap_sun[2], 48, 48, GxEPD_BLACK);
	} while (display.nextPage());

	setDisplayDefaults();
	display.hibernate();
}

void displayGoingToSleepScreen() {
	do {
		display.fillCircle(0, 0, 15, GxEPD_BLACK);
	} while (display.nextPage());

	setDisplayDefaults();
	display.hibernate();
}

void setupDisplay() {
	if (DISABLE_DISPLAY) { return; }

	display.init(BAUD_RATE, true, 2, false); // Clever refresh for Waveshare displays, 2ms pulse
	display.setRotation(DISPLAY_ROTATION);

	setDisplayDefaults();
	clearDisplay();
}

//==============================================================
//	Functions
//==============================================================