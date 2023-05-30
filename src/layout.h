#include "indicator.h"

const unsigned char* batteryIndicatorVariants[] = {
	bitmap_battery_0_24x24,
	bitmap_battery_1_24x24,
	bitmap_battery_2_24x24,
	bitmap_battery_3_24x24,
	bitmap_battery_4_24x24,
	bitmap_battery_5_24x24
};

Indicator batteryIndicator(0, 0, batteryIndicatorVariants, sizeof(batteryIndicatorVariants) / sizeof(batteryIndicatorVariants[0]), 24, 24);