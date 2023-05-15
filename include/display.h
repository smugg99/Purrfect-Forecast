#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMono18pt7b.h>

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
		centerY = y + tbh;

		break;
	case TOP_RIGHT:
		x = display.width();
		centerX = x - tbw;
		centerY = y + tbh;

		break;
	case TOP_CENTER:
		x = display.width() / 2;
		centerX = x - (tbw / 2);
		centerY = y + tbh;

		break;
	case BOTTOM_LEFT:
		y = display.height();
		centerY = y - (tbh / 2);

		break;
	case BOTTOM_RIGHT:
		x = display.width();
		y = display.height();

		centerX = x - tbw;
		centerY = y - (tbh / 2);

		break;
	case BOTTOM_CENTER:
		x = display.width() / 2;
		y = display.height();

		centerX = x - (tbw / 2);
		centerY = y - (tbh / 2);

		break;
	case RIGHT_CENTER:
		x = display.width();
		y = display.height() / 2;

		centerX = x - tbw;
		centerY = y - (tbh / 2);

		break;
	case LEFT_CENTER:
		y = display.height() / 2;

		centerX = x;
		centerY = y - (tbh / 2);

		break;
	default:
		break;
	}

	display.setCursor(centerX + offsetX, centerY + offsetY);
	display.println(text);
}

void displayBootSplash() {
	do {
		display.fillScreen(GxEPD_WHITE);
		printPointedText(SPLASH_SCREEN_TITLE);

		display.setFont(&FreeMono9pt7b);

		printPointedText("BC", DISPLAY_POINT::BOTTOM_CENTER);
		printPointedText("BR", DISPLAY_POINT::BOTTOM_RIGHT);
		printPointedText("RC", DISPLAY_POINT::RIGHT_CENTER);
		printPointedText("TR", DISPLAY_POINT::TOP_RIGHT);
		printPointedText("TC", DISPLAY_POINT::TOP_CENTER);
		printPointedText("TL", DISPLAY_POINT::TOP_LEFT);
		printPointedText("LC", DISPLAY_POINT::LEFT_CENTER);
		printPointedText("BL", DISPLAY_POINT::BOTTOM_LEFT);

		printPointedText(SPLASH_SCREEN_FOOTER, DISPLAY_POINT::CENTER, FOOTER_OFFSET_X, FOOTER_OFFSET_Y);
	} while (display.nextPage());

	display.hibernate();
}

void displayAccessPointScreen(String ssid, String password, String APIP) {
	do {
		display.fillScreen(GxEPD_WHITE);
		printPointedText("No WiFi!");

		display.setFont(&FreeMono9pt7b);
		display.setTextWrap(true);

		printPointedText(ssid + "\n" + password + "\n" + APIP, DISPLAY_POINT::CENTER, FOOTER_OFFSET_X, FOOTER_OFFSET_Y);
	} while (display.nextPage());

	// Go back to defaults
	display.setTextWrap(false);
	display.hibernate();
}

void displayPrimaryScreen() {
	do {
		display.fillScreen(GxEPD_WHITE);
		printPointedText("[Primary Screen]");

		display.setFont(&FreeMono9pt7b);

		printPointedText("1", DISPLAY_POINT::BOTTOM_CENTER);
		printPointedText("2", DISPLAY_POINT::BOTTOM_RIGHT);
		printPointedText("3", DISPLAY_POINT::RIGHT_CENTER);
		printPointedText("4", DISPLAY_POINT::TOP_RIGHT);
		printPointedText("5", DISPLAY_POINT::TOP_CENTER);
		printPointedText("6", DISPLAY_POINT::TOP_LEFT);
		printPointedText("7", DISPLAY_POINT::LEFT_CENTER);
		printPointedText("8", DISPLAY_POINT::BOTTOM_LEFT);

		printPointedText("shits not done yet :DD", DISPLAY_POINT::CENTER, 0, 24);
	} while (display.nextPage());

	display.hibernate();
}

void setupDisplay() {
	display.init(BAUD_RATE, true, 2, false); // Clever refresh for Waveshare displays, 2ms pulse
	display.setRotation(DISPLAY_ROTATION);
	display.fillScreen(GxEPD_WHITE);

	display.setTextWrap(false);
	display.setFont(&FreeMono12pt7b);
	display.setTextColor(GxEPD_BLACK);
	display.setTextSize(1);

	display.setFullWindow();
	display.firstPage();

	displayBootSplash();
	delay(SPLASH_DURATION);

	displayPrimaryScreen();
}

//==============================================================
//	Functions
//==============================================================