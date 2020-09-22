internal_function void
clear_screen(u32 color) {
	u32* pixel = (u32*)render_state.memory;
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = color;
		}
	}
}

internal_function void
draw_rect_in_pixels(int x0, int y0, int x1, int y1, u32 color) {
	x0 = clamp(0, x0, render_state.width);
	x1 = clamp(0, x1, render_state.width);
	y0 = clamp(0, y0, render_state.height);
	y1 = clamp(0, y1, render_state.height);
	
	u32* pixel = (u32*)render_state.memory;
	for (int y = y0; y < y1; y++) {
		pixel = (u32*)render_state.memory + x0 + y*render_state.width;
		for (int x = x0; x < x1; x++) {
			*pixel++ = color;
		}
	}
}

global_variable float render_scale = 0.01f;

internal_function void
draw_rect(float x, float y, float rect_width, float rect_height, u32 color) {
	
	x *= render_state.width * render_scale;
	y *= render_state.height * render_scale;
	rect_width *= render_state.width * render_scale;
	rect_height *= render_state.height * render_scale;
	
	x += render_state.width / 2.f;
	y += render_state.height / 2.f;
	
	// Change to pixels
	int x0 = x - (rect_width / 2.f);
	int x1 = x + (rect_width / 2.f);
	int y0 = y - rect_height;
	int y1 = y + rect_height;
	
	draw_rect_in_pixels(x0, y0, x1, y1, color);
}
