struct Button_State {
	bool is_down;
	bool changed;
};

enum {
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_SPACE,
	
	BUTTON_COUNT, // Should be the last item
};

struct Input {
	Button_State buttons[BUTTON_COUNT];
};

enum Gamemode {
	MENU,
	INGAME,
	PAUSE,
	GAMEOVER
};
