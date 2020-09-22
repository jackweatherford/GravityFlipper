#include <iostream> // TODO: REMOVE in final product, only for debugging
#include <stdlib.h>  // srand, rand
#include <time.h>    // time

#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

float player_p_x = 0, player_dp_x = 0, player_ddp_x = 0;

float player_half_width = 5, player_half_height = 5;
float game_half_width = 52.5, game_half_height = 50;

float player_p_y = -game_half_height;
float player_dp_y = 150;

float coin_half_width = 2.5, coin_half_height = 3;
float coin_p_x = 0, coin_p_y =  0;

bool first = true;
bool in_air = false;
bool on_floor = true;
bool queue_jump = false;

internal_function void
simulate_game(Input * input, float dt) {
	
	if (first) {
		// Seed randomness
		srand(time(NULL));
		coin_p_x = -game_half_width + 15 + rand() % (int)(game_half_width * 2 - 29);
		coin_p_y = -game_half_height + 15 + rand() % (int)(game_half_height * 2 - 29);
		first = false;
	}
	
	clear_screen(0x0055ff);
	
	player_ddp_x = 0;
	if (is_down(BUTTON_RIGHT)) player_ddp_x += 1000; // 2000
	if (is_down(BUTTON_LEFT)) player_ddp_x -= 1000;
	if (pressed(BUTTON_SPACE)) {
		if (in_air && !queue_jump && ( (on_floor && player_p_y >= game_half_height - 15)
			|| (!on_floor && player_p_y <= - game_half_height + 15) ) )
		{ queue_jump = true; } else { in_air = true; }
	}
	
	if (in_air) {
		if (on_floor) { player_p_y += player_dp_y * dt; } else { player_p_y -= player_dp_y * dt; }
	}
	
	if (player_p_y + player_half_height > game_half_height) {
		player_p_y = game_half_height - player_half_height;
		in_air = false;
		on_floor = false;
	} else if (player_p_y - player_half_height < -game_half_height) {
		player_p_y = -game_half_height + player_half_height;
		in_air = false;
		on_floor = true;
	}
	
	if (!in_air && queue_jump) { in_air = true; queue_jump = false; }
	
	player_ddp_x -= player_dp_x * 10; // Friction
	player_p_x = player_p_x + player_dp_x * dt + player_ddp_x * dt * dt * .5;
	player_dp_x = player_dp_x + player_ddp_x * dt;
	
	if (player_p_x + player_half_width > game_half_width) {
		player_p_x = game_half_width - player_half_width;
		player_dp_x = 0;
	} else if (player_p_x - player_half_width < -game_half_width) {
		player_p_x = -game_half_width + player_half_width;
		player_dp_x = 0;
	}
	
	draw_rect(coin_p_x, coin_p_y, coin_half_width, coin_half_height, 0xffdf00); // Coin
	
	if (player_p_x - player_half_width/2 < coin_p_x + coin_half_width/2 && 
		player_p_x + player_half_width/2 > coin_p_x - coin_half_width/2 &&
		player_p_y - player_half_height < coin_p_y + coin_half_height &&
		player_p_y + player_half_height > coin_p_y - coin_half_height) {
		// min + rand() % ( max - min + 1 )
		coin_p_x = -game_half_width + 15 + rand() % (int)(game_half_width * 2 - 29);
		coin_p_y = -game_half_height + 15 + rand() % (int)(game_half_height * 2 - 29);
	}
	
	draw_rect(player_p_x, player_p_y, player_half_width, player_half_height, 0xff5500); // Player
}
