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

const char* letters[][7] = {
    " 00",
    "0  0",
    "0  0",
    "0000",
    "0  0",
    "0  0",
    "0  0",

	"000",
	"0  0",
	"0  0",
	"000",
	"0  0",
	"0  0",
	"000",

	" 000",
	"0",
	"0",
	"0",
	"0",
	"0",
	" 000",

	"000",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"000",

	"0000",
	"0",
	"0",
	"000",
	"0",
	"0",
	"0000",

	"0000",
	"0",
	"0",
	"000",
	"0",
	"0",
	"0",

	" 000",
	"0",
	"0",
	"0 00",
	"0  0",
	"0  0",
	" 000",

	"0  0",
	"0  0",
	"0  0",
	"0000",
	"0  0",
	"0  0",
	"0  0",

	"000",
	" 0",
	" 0",
	" 0",
	" 0",
	" 0",
	"000",

	" 000",
	"   0",
	"   0",
	"   0",
	"0  0",
	"0  0",
	" 000",

	"0  0",
	"0  0",
	"0 0",
	"00",
	"0 0",
	"0  0",
	"0  0",

	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0000",

	"00 00",
	"0 0 0",
	"0 0 0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",

	"00  0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0  00",

	"0000",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0000",

	" 000",
	"0  0",
	"0  0",
	"000",
	"0",
	"0",
	"0",

	" 000 ",
	"0   0",
	"0   0",
	"0   0",
	"0 0 0",
	"0  0 ",
	" 00 0",

	"000",
	"0  0",
	"0  0",
	"000",
	"0  0",
	"0  0",
	"0  0",

	" 000",
	"0",
	"0 ",
	" 00",
	"   0",
	"   0",
	"000 ",

	"00000",
	"  0",
	"  0",
	"  0",
	"  0",
	"  0",
	"  0",

	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	"0  0",
	" 00",

	"0   0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",
	" 0 0",
	"  0",

	"0   0 ",
	"0   0",
	"0   0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	" 0 0 ",

	"0   0",
	"0   0",
	" 0 0",
	"  0",
	" 0 0",
	"0   0",
	"0   0",

	"0   0",
	"0   0",
	" 0 0",
	"  0",
	"  0",
	"  0",
	"  0",

	"0000",
	"   0",
	"  0",
	" 0",
	"0",
	"0",
	"0000",

	"",
	"",
	"",
	"",
	"",
	"",
	"0",

	"   0",
	"  0",
	"  0",
	" 0",
	" 0",
	"0",
	"0",
};

internal_function void
draw_text(const char *text, float x, float y, float size, u32 color) {
	float half_size = size * .5f;
	float original_y = y;

	while (*text) {
		if (*text != 32) {
			const char** letter;
			if (*text == 47) letter = letters[27];
			else if (*text == 46) letter = letters[26];
			else letter = letters[*text - 'A'];
			float original_x = x;

			for (int i = 0; i < 7; i++) {
				const char* row = letter[i];
				while (*row) {
					if (*row == '0') {
						draw_rect(x, y, half_size, half_size, color);
					}
					x += half_size;
					row++;
				}
				y -= size;
				x = original_x;
			}
		}
		text++;
		x += half_size * 6.f;
		y = original_y;
	}
}


internal_function void
draw_number(int number, float x, float y, float size, u32 color) {
	float half_size = size * .5f;
	float quart_size = half_size * .5f;
	
	do {
		int digit = number % 10;
		number = number / 10;
		
		switch (digit) {
		case 0: {
			draw_rect(x - half_size, y, half_size, 2.5f * size, color);
			draw_rect(x + half_size, y, half_size, 2.5f * size, color);
			draw_rect(x, y + size * 2.f, half_size, half_size, color);
			draw_rect(x, y - size * 2.f, half_size, half_size, color);
		} break;
		
		case 1: {
			draw_rect(x + half_size, y, half_size, 2.5f * size, color);
		} break;
		
		case 2: {
			draw_rect(x, y + size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x, y, 1.5f * size, half_size, color);
			draw_rect(x, y - size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x + half_size, y + size, half_size, half_size, color);
			draw_rect(x - half_size, y - size, half_size, half_size, color);
		} break;
		
		case 3: {
			draw_rect(x - quart_size, y + size * 2.f, size, half_size, color);
			draw_rect(x - quart_size, y, size, half_size, color);
			draw_rect(x - quart_size, y - size * 2.f, size, half_size, color);
			draw_rect(x + half_size, y, half_size, 2.5f * size, color);
		} break;
		
		case 4: {
			draw_rect(x + half_size, y, half_size, 2.5f * size, color);
			draw_rect(x - half_size, y + size, half_size, 1.5f * size, color);
			draw_rect(x, y, half_size, half_size, color);
		} break;
		
		case 5: {
			draw_rect(x, y + size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x, y, 1.5f * size, half_size, color);
			draw_rect(x, y - size * 2.f, 1.5f * size, half_size, color);
			draw_rect(x - half_size, y + size, half_size, half_size, color);
			draw_rect(x + half_size, y - size, half_size, half_size, color);
		} break;
		
		case 6: {
			draw_rect(x + quart_size, y + size * 2.f, size, half_size, color);
			draw_rect(x + quart_size, y, size, half_size, color);
			draw_rect(x + quart_size, y - size * 2.f, size, half_size, color);
			draw_rect(x - half_size, y, half_size, 2.5f * size, color);
			draw_rect(x + half_size, y - size, half_size, half_size, color);
		} break;
		
		case 7: {
			draw_rect(x + half_size, y, half_size, 2.5f * size, color);
			draw_rect(x - quart_size, y + size * 2.f, size, half_size, color);
		} break;
		
		case 8: {
			draw_rect(x - half_size, y, half_size, 2.5f * size, color);
			draw_rect(x + half_size, y, half_size, 2.5f * size, color);
			draw_rect(x, y + size * 2.f, half_size, half_size, color);
			draw_rect(x, y - size * 2.f, half_size, half_size, color);
			draw_rect(x, y, half_size, half_size, color);
		} break;
		
		case 9: {
			draw_rect(x - quart_size, y + size * 2.f, size, half_size, color);
			draw_rect(x - quart_size, y, size, half_size, color);
			draw_rect(x - quart_size, y - size * 2.f, size, half_size, color);
			draw_rect(x + half_size, y, half_size, 2.5f * size, color);
			draw_rect(x - half_size, y + size, half_size, half_size, color);
		} break;
		}
		x -= size * 2.f;
	} while (number);
}
