void render_background() {
	unsigned int* pixel = (unsigned int*)buffer_memory;
	for (int y = 0; y < (buffer_height); y++) {
		for (int x = 0; x < (buffer_width); x++) {
			unsigned int color = 0x000000;
			int distanceFromPlayer = abs(y - (buffer_height / 2));
			distanceFromPlayer = distanceFromPlayer * 256;
			distanceFromPlayer = distanceFromPlayer / (buffer_height);
			color = color + ((int)(distanceFromPlayer) * 0x010000); //red
			color = color + ((int)(distanceFromPlayer) * 0x000100); //green
			color = color + ((int)(distanceFromPlayer) * 0x000001); //blue
			*pixel = color;
			pixel = pixel + 1;
		}
	}
}

void draw_pixel(int x, int y, unsigned int color) {
	if (x < 0 || x > buffer_width || y < 0 || y > buffer_width) {
		return;
	}

	unsigned int* pixel = (unsigned int*)buffer_memory;
	int pixel_number = (y * buffer_width) + x;
	pixel = pixel + pixel_number;
	*pixel = color;
}

void draw_sized_column(int x, int size, unsigned int color) {
	int current_y = (buffer_height - size) / 2;
	for (int c = 0; c < size; c++) {
		draw_pixel(x, current_y, color);
		current_y = current_y + 1;
	}
}

void clear_screen(unsigned int color) {
	unsigned int* pixel = (unsigned int*)buffer_memory;
	for (int y = 0; y < (buffer_height); y++) {
		for (int x = 0; x < (buffer_width); x++) {
			*pixel++ = color;
		}
	}
}