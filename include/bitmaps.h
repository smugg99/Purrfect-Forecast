#include "bitmaps/sun.h"
#include "bitmaps/moon.h"
#include "bitmaps/cloud.h"
#include "bitmaps/broken_cloud.h"
#include "bitmaps/rainy_cloud.h"
#include "bitmaps/snowy_cloud.h"

const u_int16_t bitmap_size_index[][2]{
	{24, 24},
	{32, 32},
	{48, 48}
};

#define BITMAP_SIZES sizeof(bitmap_size_index) / sizeof(bitmap_size_index[0])

const unsigned char** bitmaps[][BITMAP_SIZES]{
	bitmap_sun,
	bitmap_moon,
	bitmap_cloud,
	bitmap_broken_cloud,
	bitmap_rainy_cloud,
	bitmap_snowy_cloud,
};

#define NUM_OF_BITMAPS sizeof(bitmaps) / sizeof(*bitmaps[0])