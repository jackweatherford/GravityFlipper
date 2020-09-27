#include <stdlib.h>  // srand, rand
#include <time.h>    // time

#include <iostream> // TODO: REMOVE in final product, only for debugging

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

float missile_half_width = 5, missile_half_height = 3;
float missile_p_x = -game_half_width*2 - missile_half_width, missile_p_y = 0;
float missile_dp_x = 150;

int score = 0;
int frame_count = 0;

bool first = true;
bool in_air = false;
bool on_floor = true;
bool queue_jump = false;

Gamemode current_gamemode = MENU;

internal_function void
simulate_game(Input * input, float dt, POINT click, bool * new_click) {
	
	if (current_gamemode == INGAME) {
		// First time setup
		if (first) {
			// Pause on menu
			Sleep(250);
			// Seed randomness
			srand(time(NULL));
			// Coin init
			// min + rand() % ( max - min + 1 )
			coin_p_x = -game_half_width + 15 + rand() % (int)(game_half_width * 2 - 29);
			coin_p_y = -game_half_height + 15 + rand() % (int)(game_half_height * 2 - 29);
			
			// Missile init
			missile_p_y = -game_half_height + 15 + rand() % (int)(game_half_height * 2 - 29);
			
			player_p_x = 0, player_dp_x = 0, player_ddp_x = 0;

			player_p_y = -game_half_height;

			missile_p_x = -game_half_width*2 - missile_half_width;

			score = 0;
			frame_count = 0;

			in_air = false;
			on_floor = true;
			queue_jump = false;
			
			first = false;
		}
		frame_count++;
		
		// Background color
		clear_screen(0x1D2953);
		
		player_ddp_x = 0;
		// Check key presses
		if (is_down(BUTTON_RIGHT)) player_ddp_x += 1000; // 2000
		if (is_down(BUTTON_LEFT)) player_ddp_x -= 1000;
		if (pressed(BUTTON_SPACE)) {
			if (in_air && !queue_jump && ( (on_floor && player_p_y >= game_half_height - 15)
				|| (!on_floor && player_p_y <= - game_half_height + 15) ) )
			{ queue_jump = true; } else { in_air = true; }
		}
		
		// Increase/decrease vertical position
		if (in_air) {
			if (on_floor) { player_p_y += player_dp_y * dt; } else { player_p_y -= player_dp_y * dt; }
		}
		
		// Stop player if they hit roof/floor
		if (player_p_y + player_half_height > game_half_height) {
			player_p_y = game_half_height - player_half_height;
			in_air = false;
			on_floor = false;
		} else if (player_p_y - player_half_height < -game_half_height) {
			player_p_y = -game_half_height + player_half_height;
			in_air = false;
			on_floor = true;
		}
		
		// Handle queued jump
		if (!in_air && queue_jump) { in_air = true; queue_jump = false; }
		
		// Handle left + right movement
		player_ddp_x -= player_dp_x * 10; // Friction
		player_p_x = player_p_x + player_dp_x * dt + player_ddp_x * dt * dt * .5;
		player_dp_x = player_dp_x + player_ddp_x * dt;
		
		// Stop player if they hit walls
		if (player_p_x + player_half_width > game_half_width) {
			player_p_x = game_half_width - player_half_width;
			player_dp_x = 0;
		} else if (player_p_x - player_half_width < -game_half_width) {
			player_p_x = -game_half_width + player_half_width;
			player_dp_x = 0;
		}
		
		// Missile movement
		missile_p_x += missile_dp_x * dt;
		
		// Reset missile if it goes off screen
		if (missile_p_x - missile_half_width > game_half_width) {
			missile_p_x = -game_half_width*2 - missile_half_width;
			missile_p_y = -game_half_height + 15 + rand() % (int)(game_half_height * 2 - 29);
		}
		
		// Draw coin
		draw_rect(coin_p_x, coin_p_y, coin_half_width, coin_half_height, 0xFFDF00);
		
		// If player collides with coin
		if (player_p_x - player_half_width/2 < coin_p_x + coin_half_width/2 && 
			player_p_x + player_half_width/2 > coin_p_x - coin_half_width/2 &&
			player_p_y - player_half_height < coin_p_y + coin_half_height &&
			player_p_y + player_half_height > coin_p_y - coin_half_height) {
			// min + rand() % ( max - min + 1 )
			coin_p_x = -game_half_width + 15 + rand() % (int)(game_half_width * 2 - 29);
			coin_p_y = -game_half_height + 15 + rand() % (int)(game_half_height * 2 - 29);
			
			score++;
		}
		
		// Draw player
		draw_rect(player_p_x, player_p_y, player_half_width, player_half_height, 0xFF5500);
		
		// Draw score
		draw_number(score, game_half_width/1.1, game_half_height/1.13, 1.f, 0x00FF00);
		
		// Draw timer
		// draw_number(frame_count / 180, -game_half_width/1.1, game_half_height/1.13, 1.f, 0x00FF00);
		
		// Draw missile
		draw_rect(missile_p_x, missile_p_y, missile_half_width, missile_half_height, 0xFF0000);
		
		// If player collides with missile
		if (player_p_x - player_half_width/2 < missile_p_x + missile_half_width/2 && 
			player_p_x + player_half_width/2 > missile_p_x - missile_half_width/2 &&
			player_p_y - player_half_height < missile_p_y + missile_half_height &&
			player_p_y + player_half_height > missile_p_y - missile_half_height) {
			current_gamemode = GAMEOVER;
		}
	} else if (current_gamemode == MENU) {
		// Background color
		clear_screen(0x1D2953);
		
		if (*new_click) {
			*new_click = false;
			if(click.x >= 536 && click.y >= 355 &&
			   click.x <= 742 && click.y <= 437) {
			    draw_text("GRAVITY FLIPPER", -43, 40, 2, 0xFF5500);
				draw_rect(0, -3, 16, 6, 0xCCCCCC);
				draw_text("START", -7, 0, 1, 0x000000);
				current_gamemode = INGAME;
			}
		} else {
			draw_text("GRAVITY FLIPPER", -43, 40, 2, 0xFF5500);
			draw_rect(0, -3, 16, 6, 0x00FF00);
			draw_text("START", -7, 0, 1, 0x000000);
		}
	} else if (current_gamemode == GAMEOVER) {
		if (*new_click) {
			*new_click = false;
			if(click.x >= 497 && click.y >= 355 &&
			   click.x <= 780 && click.y <= 437) {
				draw_text("GAME OVER", -25, 40, 2, 0xFF0000);
				draw_rect(0, -3, 22, 6, 0xCCCCCC);
				draw_text("RESTART", -10, 0, 1, 0x000000);
				current_gamemode = INGAME;
				first = true;
			 }
		} else {
			draw_text("GAME OVER", -25, 40, 2, 0xFF0000);
			draw_rect(0, -3, 22, 6, 0x00FF00);
			draw_text("RESTART", -10, 0, 1, 0x000000);
		}
	}
}
