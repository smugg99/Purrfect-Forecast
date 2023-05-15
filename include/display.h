#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMono12pt7b.h>
#include <Fonts/FreeMono18pt7b.h>

GxEPD2_BW<GxEPD2_290_T94_V2, GxEPD2_290_T94_V2::HEIGHT> display(GxEPD2_290_T94_V2(CS_PIN, DC_PIN, RST_PIN, BUSY_PIN)); // GDEM029T94 128x296, SSD1680, Waveshare 2.9" V2 variant

#define DISPLAY_WIDTH display.width()
#define DISPLAY_HEIGHT display.height()

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
	int16_t tbx, tby; uint16_t tbw, tbh;
	display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);

	int16_t centerX, centerY = 0;
	int16_t x, y = 0;

	switch (displayPoint) {
	case CENTER:
		x = DISPLAY_WIDTH / 2;
		y = DISPLAY_HEIGHT / 2;
		centerX = x - (tbw / 2);
		centerY = y - (tbh / 2);

		break;
	case TOP_LEFT:
		centerY = y + tbh;

		break;
	case TOP_RIGHT:
		x = DISPLAY_WIDTH;
		centerX = x - tbw;
		centerY = y + tbh;

		break;
	case TOP_CENTER:
		x = DISPLAY_WIDTH / 2;
		centerX = x - (tbw / 2);
		centerY = y + tbh;

		break;
	case BOTTOM_LEFT:
		y = DISPLAY_HEIGHT;
		centerY = y - (tbh / 2);

		break;
	case BOTTOM_RIGHT:
		x = DISPLAY_WIDTH;
		y = DISPLAY_HEIGHT;

		centerX = x - tbw;
		centerY = y - (tbh / 2);

		break;
	case BOTTOM_CENTER:
		x = DISPLAY_WIDTH / 2;
		y = DISPLAY_HEIGHT;

		centerX = x - (tbw / 2);
		centerY = y - (tbh / 2);

		break;
	case RIGHT_CENTER:
		x = DISPLAY_WIDTH;
		y = DISPLAY_HEIGHT / 2;

		centerX = x - tbw;
		centerY = y - (tbh / 2);

		break;
	case LEFT_CENTER:
		y = DISPLAY_HEIGHT / 2;

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
	//display.fillScreen(GxEPD_WHITE);

	display.setTextWrap(false);
	display.setFont(&FreeMono12pt7b);
	display.setTextColor(GxEPD_BLACK);
	display.setTextSize(1);

	display.setFullWindow();
	display.firstPage();
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

		printPointedText(SPLASH_SCREEN_FOOTER, DISPLAY_POINT::CENTER, 0, 24);
	} while (display.nextPage());
}

void _setup() {
	display.init(115200, true, 2, false); // Clever refresh for Waveshare displays, 2ms pulse
	display.setRotation(DISPLAY_ROTATION);
	display.fillScreen(GxEPD_WHITE);


	displayBootSplash();
	//display.hibernate();
}

//==============================================================
//	Functions
//==============================================================