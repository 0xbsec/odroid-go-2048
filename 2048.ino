#include <odroid_go.h>

#define W 320 // screen width
#define H 240 // screen height
#define UNDO_STACK 20 // how many undos to save
#define N 4 // size of grid
#define LOG false // enable logging
#define COLOR true // show colors or keep it W & b
#define BX 0 // body start offset x
#define BY 0 // body start offset y
#define OX 79 // next element offset x 
#define OY 59 // next element offset y
#define A 6 // round angle
#define BO 6 // border offset

int log_cursor_x = 0;

/*
	TODO;
		- score
		- check left right sudden move
		- highscore
		- moves counter
		- font
		- add confetti when 2048
*/

enum status {
	ongoing,
	won,
	over,
};

status S = ongoing;

int m[N][N];
int prevm[N][N];
int undos[UNDO_STACK][N][N];
int moves_counter;
int brightness = 125;

class Move {
	Move();

	public:
		// source for the movements: https://gist.github.com/josephg/7745028 
		static void up() {
			for (int i = 0; i < N - 1; i++) {
				for (int x = 1; x < N; x++) {
					for (int y = 0; y < N; y++) {
						if (m[x - 1][y] == 0) {
							m[x - 1][y] = m[x][y];
							m[x][y] = 0;
						}
					}
				}
			}
			for (int x = 1; x < N; x++) {
				for (int y = 0; y < N; y++) {
					if (m[x - 1][y] == m[x][y]) {
						m[x - 1][y] += m[x][y];
						m[x][y] = 0;
					}
				}
			}
			for (int i = 0; i < N - 1; i++) {
				for (int x = 1; x < N; x++) {
					for (int y = 0; y < N; y++) {
						if (m[x - 1][y] == 0) {
							m[x - 1][y] = m[x][y];
							m[x][y] = 0;
						}
					}
				}
			}
		}

	static void down() {
		for (int i = 0; i < N - 1; i++) {
			for (int x = 0; x < N - 1; x++) {
				for (int y = 0; y < N; y++) {
					if (m[x + 1][y] == 0) {
						m[x + 1][y] = m[x][y];
						m[x][y] = 0;
					}
				}
			}
		}
		for (int x = 2; x >= 0; x--) {
			for (int y = 0; y < N; y++) {
				if (m[x + 1][y] == m[x][y]) {
					m[x + 1][y] += m[x][y];
					m[x][y] = 0;
				}
			}
		}
		for (int i = 0; i < N - 1; i++) {
			for (int x = 0; x < N - 1; x++) {
				for (int y = 0; y < N; y++) {
					if (m[x + 1][y] == 0) {
						m[x + 1][y] = m[x][y];
						m[x][y] = 0;
					}
				}
			}
		}
	}

	static void left() {
		for (int i = 0; i < N - 1; i++) {
			for (int x = 0; x < N; x++) {
				for (int y = 1; y < N; y++) {
					if (m[x][y - 1] == 0) {
						m[x][y - 1] = m[x][y];
						m[x][y] = 0;
					}
				}
			}
		}
		for (int x = 0; x < N; x++) {
			for (int y = 1; y < N; y++) {
				if (m[x][y - 1] == m[x][y]) {
					m[x][y - 1] += m[x][y];
					m[x][y] = 0;
				}
			}
		}
		for (int i = 0; i < N - 1; i++) {
			for (int x = 0; x < N; x++) {
				for (int y = 1; y < N; y++) {
					if (m[x][y - 1] == 0) {
						m[x][y - 1] = m[x][y];
						m[x][y] = 0;
					}
				}
			}
		}
	}

	static void right() {
		for (int i = 0; i < N - 1; i++) {
			for (int x = 0; x < N; x++) {
				for (int y = 0; y < N - 1; y++) {
					if (m[x][y + 1] == 0) {
						m[x][y + 1] = m[x][y];
						m[x][y] = 0;
					}
				}
			}
		}
		for (int x = 0; x < N; x++) {
			for (int y = 2; y >= 0; y--) {
				if (m[x][y + 1] == m[x][y]) {
					m[x][y + 1] += m[x][y];
					m[x][y] = 0;
				}
			}
		}
		for (int i = 0; i < N - 1; i++) {
			for (int x = 0; x < N; x++) {
				for (int y = 0; y < N - 1; y++) {
					if (m[x][y + 1] == 0) {
						m[x][y + 1] = m[x][y];
						m[x][y] = 0;
					}
				}
			}
		}
	}
};

class Color {
	Color();

	public:
		const static uint16_t bbcolor = 0xB553; // whole board background color

	// converted using http://drakker.org/convert_rgb565.html
	// tiles background color
	static uint16_t bcolor(int value) {
		switch (value) {
			case 0:
				return 0xC5F5;
			case 2:
				return 0xE71A;
			case 4:
				return 0xE6F8;
			case 8:
				return 0xED6E;
			case 16:
				return 0xEC8C;
			case 32:
				return 0xEBCB;
			case 64:
				return 0xEAE7;
			case 128:
				return 0xE66D;
			case 256:
				return 0xE64B;
			case 512:
				return 0xE629;
			case 1024:
				return 0xE607;
			case 2048:
				return 0xE5E5;
			default:
				return 0xE5E5;
		}
	}

	// tiles foreground colors
	static uint16_t fcolor(int value) {
		switch (value) {
			case 0:
				return 0xC5F6;
			case 2:
				return 0x736C;
			case 4:
				return 0x736C;
			default:
				return 0xF79D;
		}
	}
};

// compare current & previous state
bool cmp() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (prevm[i][j] != m[i][j]) {
				return false;
			}
		}
	}

	return true;
}

// update previous state and undo stack
void cp() {
	// reset undo stack
	if (moves_counter >= UNDO_STACK) {
		moves_counter = 0;
		memset(undos, 0, sizeof undos);
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			prevm[i][j] = m[i][j];
			undos[moves_counter][i][j] = m[i][j];
		}
	}
}

class D {
	int w;
	int h;

	public:

		D(int sw, int sh) {};
	bool color = COLOR; // starting color

	void log(String msg) {
		if (!LOG) return;
		if (log_cursor_x >= W - 10) {
			log_cursor_x = 0;
			GO.lcd.fillRect(0, 0, W, 10, BLACK);
		}

		GO.lcd.setCursor(log_cursor_x, 0);
		GO.lcd.setTextSize(1);
		GO.lcd.setTextColor(RED);

		GO.lcd.print(msg);
		log_cursor_x += 6;
	}

	void render() {
		for (int r = 0; r < N; r++) {
			for (int c = 0; c < N; c++) {
				uint16_t bc = Color::bcolor(m[r][c]);
				uint16_t fc = Color::fcolor(m[r][c]);

				int x = BX + OX * c;
				int y = BY + OY * r;

				str(x, y, String(m[r][c]), 3, bc, fc);
			}
		}
	}

	void undo() {
		if (moves_counter == 0) return;
		if (moves_counter >= UNDO_STACK) return;

		moves_counter--;

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				m[i][j] = undos[moves_counter][i][j];
			}
		}

		render();
	}

	void str(int x, int y, String s, int font_size, uint16_t bc, uint16_t fc) {
		GO.lcd.setTextSize(font_size);
		int full_width = GO.lcd.textWidth(s);

		if (s.length() >= 4) {
			font_size -= 1;

			GO.lcd.setTextSize(font_size);
		}

		if (color) {
			GO.lcd.setTextColor(fc);
			GO.lcd.fillRoundRect(x + BO, y + BO, OX - BO, OY - BO, A, bc);
		} else {
			GO.lcd.setTextColor(WHITE, BLACK);
			GO.lcd.fillRect(x, y, OX, OY, BLACK);
		}

		int offset_x;
		int offset_y;

		switch (s.length()) {
			case 1:
				offset_x = 34;
				offset_y = 22;
				break;
			case 2:
				offset_x = 26;
				offset_y = 20;
				break;
			case 3:
				offset_x = 15;
				offset_y = 20;
				break;
			case 4:
				offset_x = 19;
				offset_y = 25;
				break;
		}

		GO.lcd.setCursor(x + offset_x, y + offset_y);

		if (s == "0") {
			GO.lcd.print(" ");
		} else {
			GO.lcd.print(s);
		}
	}

	void setColor(bool c) {
		color = c;
	}

	// update states after actions
	void tick() {
		if (!cmp()) {
			add_random_n();
			render();
			moves_counter++;
		}
		GO.update();
	}


	void add_random_n() {
		int zeros_count = 0;

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				if (m[i][j] == 0) zeros_count++;
			}
		}

		if (zeros_count == 0) {
			S = over;
			return;
		}

		int zeros_idx_r[zeros_count + 1];
		int zeros_idx_c[zeros_count + 1];

		int j = 0;
		for (int i = 0; i < N; i++) {
			for (int k = 0; k < N; k++) {
				if (m[i][k] == 0) {
					zeros_idx_r[j] = i;
					zeros_idx_c[j] = k;
					j++;
				}
			}
		}

		int i = random(j);
		int k = zeros_idx_r[i];
		int c = zeros_idx_c[i];

		int n = r();
		m[k][c] = n;
	}

	void init() {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				m[i][j] = 0;
				prevm[i][j] = 0;
			}
		}

		S = ongoing;
		GO.lcd.clear();
		if (color) GO.lcd.fillRoundRect(0, 0, W, H, A, Color::bbcolor);

		// start with two random numbers
		add_random_n();
		add_random_n();

		render();
	}

	private:
		// center with spaces
		String pad(String s) {
			int size = OX / 6;

			if (s.length() > 2) return s;
			if (s.length() == 1) return pad(" " + s + " ");
			if (s.length() == 2) return pad(" " + s);
			return pad(s + " ");
		}

	// 2 or 4?
	int r() {
		int i = random(10);
		if (i == 0) return 4; // 10% chance of 0
		return 2;
	}

};

D d(10, 10);

void setup() {
	randomSeed(analogRead(0));

	GO.begin();

	GO.lcd.setBrightness(10);

	d.init();
}

void loop() {
	GO.update();

	// update brightness: A + Up/Down 
	if (GO.BtnStart.isPressed() && GO.JOY_Y.wasAxisPressed()) {
		int step = 20;
		if (GO.JOY_Y.wasAxisPressed() == 2) {
			if (brightness + step < 250) brightness += step;
		} else {
			if (brightness - step > 0) brightness -= step;
		}
		GO.lcd.setBrightness(brightness);
		return;
	}

	// toggle colors: A + left
	if (GO.BtnStart.isPressed() && GO.JOY_X.wasAxisPressed()) {
		if (GO.JOY_X.wasAxisPressed() == 2) {
			d.log("color toggle");
			d.setColor(!d.color);
			GO.lcd.clear();
			if (d.color) {
				// redraw background that was removed BY the clear();
				GO.lcd.fillRoundRect(0, 0, W, H, A, Color::bbcolor);
			}
			d.render();
		}
		return;
	}

	// undo: A
	if (GO.BtnA.wasPressed()) {
		d.undo();

		return;
	}

	// restart: B
	if (GO.BtnB.wasPressed()) {
		d.init();

		return;
	}

	// UP
	if (GO.JOY_Y.wasAxisPressed() == 2) {
		d.log("u");

		Move::up();
		d.tick();

		return;
	}

	// DOWN
	if (GO.JOY_Y.wasAxisPressed() == 1) {
		d.log("d");

		Move::down();
		d.tick();

		return;
	}

	// LEFT
	if (GO.JOY_X.wasAxisPressed() == 2) {
		d.log("l");

		Move::left();
		d.tick();

		return;
	}

	// RIGHT
	if (GO.JOY_X.wasAxisPressed() == 1) {
		d.log("r");

		Move::right();
		d.tick();

		return;
	}

	if (!cmp()) cp();
}