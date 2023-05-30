class Indicator {
private:
	const unsigned char** _bitmapVariants;
	const size_t _bitmapVariantsSize;
	size_t _currentVariantIndex = 0;
	uint16_t _x, _y, _width, _height;
public:
	Indicator(uint16_t x, uint16_t y, const unsigned char** bitmapVariants, const size_t bitmapVariantsSize, uint16_t width, uint16_t height) : _bitmapVariants(bitmapVariants), _bitmapVariantsSize(bitmapVariantsSize) {
		this->_x = x;
		this->_y = y;

		this->_width = width;
		this->_height = height;
	};

	inline std::tuple<uint16_t, uint16_t, uint16_t, uint16_t> getPositionAndSize() {
		return std::make_tuple(this->_x, this->_y, this->_width, this->_height);
	}
	
	inline size_t getBitmapVariantsSize() {
		return this->_bitmapVariantsSize;
	}

	void move(uint16_t x, uint16_t y) {
		draw(false, true);
		this->_x = x;
		this->_y = y;
		draw(false);
	}
	
	void draw(bool lazy = true, bool inverted = false) {
		display.setPartialWindow(this->_x, this->_y, this->_width, this->_height);
		display.firstPage();
		do {
			if (lazy) {
				display.fillRect(this->_x, this->_y, this->_width, this->_height, inverted ? GxEPD_BLACK : GxEPD_WHITE);
			}
			else {
				display.drawBitmap(this->_x, this->_y, this->_bitmapVariants[this->_currentVariantIndex], this->_width, this->_height, inverted ? GxEPD_WHITE : GxEPD_BLACK);
			}

			display.drawBitmap(this->_x, this->_y, this->_bitmapVariants[this->_currentVariantIndex], this->_width, this->_height, inverted ? GxEPD_WHITE : GxEPD_BLACK);
		} while (display.nextPage());

		display.setFullWindow();
	}

	void changeBitmapVariant(size_t bitmapIndex = 0, bool skipRedraw = true) {
		this->_currentVariantIndex = bitmapIndex;

		if (skipRedraw) { return; }
		draw(false);
	}
};