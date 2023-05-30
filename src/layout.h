#include "indicator.h"

const unsigned char* batteryIndicatorVariants[] = {
	bitmap_battery_0_24x24,
	bitmap_battery_1_24x24,
	bitmap_battery_2_24x24,
	bitmap_battery_3_24x24,
	bitmap_battery_4_24x24,
	bitmap_battery_5_24x24
};

const unsigned char* wifiIndicatorVariants[] = {
	bitmap_no_wifi_24x24,
	bitmap_wifi_0_24x24,
	bitmap_wifi_1_24x24,
	bitmap_wifi_2_24x24,
	bitmap_wifi_3_24x24,
	bitmap_wifi_4_24x24
};

const unsigned char* internetIndicatorVariants[] = {
	bitmap_internet_24x24,
	bitmap_no_internet_24x24
};

const unsigned char* apiIndicatorVariants[] = {
	bitmap_sync_24x24,
	bitmap_sync_error_24x24
};

Indicator batteryIndicator(0, 0, batteryIndicatorVariants, sizeof(batteryIndicatorVariants) / sizeof(batteryIndicatorVariants[0]), 24, 24);
Indicator wifiIndicator(24, 0, wifiIndicatorVariants, sizeof(wifiIndicatorVariants) / sizeof(wifiIndicatorVariants[0]), 24, 24);

Indicator internetIndicator(48, 0, internetIndicatorVariants, sizeof(internetIndicatorVariants) / sizeof(internetIndicatorVariants[0]), 24, 24);
Indicator apiIndicator(72, 0, apiIndicatorVariants, sizeof(apiIndicatorVariants) / sizeof(apiIndicatorVariants[0]), 24, 24);