#include <odroid_go.h>

#define W            320     // screen width
#define H            240     // screen height
#define UNDO_STACK   20      // how many undos to save
#define N			 4  	 // size of grid
#define LOG          false    // enable logging
#define COLOR        true	 // show colors or keep it w & b
#define bx 0 // body start offset x
#define by 0 // body start offset y
#define	ox 79 // next element offset x 
#define oy 59 // next element offset y
#define A  6 // round angle
#define bo 6 // border offset

int log_cursor_x = 0;

/*
	TODO;
		- score
		- check left right sudden move
		- highscore
		- add moves counter
		- add confetti when 2048
*/

enum status {
	ongoing,
	won,
	over,
};

status s = ongoing;

int m[N][N];
int prevm[N][N];
int score = 0;
int undos[UNDO_STACK][N][N];
int moves_counter;
int brightness = 125;

int r() {
	int i = random(10);
	if (i == 0) return 4; // 10% chance of 0
	return 2;
}

void add_random_n() {
	int zeros_count = 0;

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (m[i][j] == 0) zeros_count++;
		}
	}

	if (zeros_count == 0) {
		s = over;
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

// source for the movements: https://gist.github.com/josephg/7745028 
void move_up() {
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

void move_down() {
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

void move_left() {
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

void move_right() {
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


struct RGB {
  byte r;
  byte g;
  byte b;
};

class Color {
	Color();

	public:


	const static uint16_t bbcolor = 0xB553; // whole board background color

	// converted using http://drakker.org/convert_rgb565.html
    static uint16_t bcolor(int value) {
		/*if (s == ongoing) {*/
			switch (value) {
				case 0: return 0xC5F5;
				case 2: return 0xE71A;
				case 4: return 0xE6F8;
				case 8: return 0xED6E;
				case 16: return 0xEC8C;
				case 32: return 0xEBCB;
				case 64: return 0xEAE7;
				case 128: return 0xE66D;
				case 256: return 0xE64B;
				case 512: return 0xE629;
				case 1024: return 0xE607;
				case 2048: return 0xE5E5;
				default: return 0xE5E5;
			}
		/*} else if (s == over) {
			switch (value) {
				case 2: return { 238, 228, 219};
				case 4: return { 238, 227, 214};
				case 8: return { 239, 214, 194};
				case 16: return { 240, 206, 188};
				case 32: return { 240, 199, 186};
				case 64: return { 240, 191, 178};
				case 128: return { 238, 222, 192};
				case 256: return { 238, 221, 188};
				case 512: return { 238, 220, 184};
				case 1024: return { 238, 219, 180};
				case 2048: return { 238, 218, 177};
				default: return { 190, 181, 173};
			}
		} else if (s == won) {
			switch (value) {
				case 0: return { 220, 193, 122};
				case 2: return { 237, 211, 141};
				case 4: return { 236, 209, 132};
				case 8: return { 238, 185,  94};
				case 16: return { 239, 171,  84};
				case 32: return { 240, 159,  81};
				case 64: return { 240, 144,  65};
				case 128: return { 236, 200,  92};
				case 256: return { 236, 198,  84};
				case 512: return { 236, 196,  77};
				case 1024: return { 236, 195,  70};
				case 2048: return { 236, 193,  64};
				default: return { 148, 126,  57};
			}
		}*/
    }


	static uint16_t fcolor(int value) {
		/*if (s == ongoing) {*/
			switch (value) {
				case 0: return 0xC5F6;
				case 2: return 0x736C;
				case 4: return 0x736C;
				default: return 0xF79D;
			}
		/*} else if (s == over) {
			switch (value) {
				case 2: return { 206, 195, 187};
                default: return { 241, 233, 226};
			}
		} else if (s == won) {
			switch (value) {
				case 0: return { 220, 193, 122};
				case 2: return { 177, 152,  82};
				case 4: return { 177, 152,  82};
				default: return { 242, 220, 153};
			}
		}*/
	}
};

class D {
	int w; // screen width
	int h; // screen height
	int bt = 12; // border thickness

	public:
	bool color = COLOR; // starting color
	D(int sw, int sh) {
		w = sw;
		h = sh;
	}

	void render() {
		// border();	
		// header();
		body();

		// sig();
	}

	void sig() {
		String s = String("with <3 mhasbini.com");

		if (color) {
			GO.lcd.setTextColor(BLACK);
		} else {
			GO.lcd.setTextColor(WHITE);
		}

		str(w - s.length() * 6 - 5, h - 10, s);
	}

	void log(String msg) {
		if (!LOG) return;
		if (log_cursor_x >= W - 10) {
			log_cursor_x = 0;
			GO.lcd.fillRect(0, 0, w, 10, BLACK);
		}

		GO.lcd.setCursor(log_cursor_x, 0);
		GO.lcd.setTextSize(1);
		GO.lcd.setTextColor(RED);

		// str(0, 0, msg);
		GO.lcd.print(msg);
		log_cursor_x += 6;
	}

	void header() {
		int bw = 40;
		int bh = 30;

		int hx = w - bw * 3;
		int hy = bt + 5;

		uint16_t color = rgb(187, 173, 160);
		GO.lcd.fillRect(hx, hy, bw, bh, color);
		GO.lcd.fillRect(hx + bw + 10, hy, bw, bh, color);
	}

	void body() {
		for(int r = 0; r < N; r++) {
			for (int c = 0; c < N; c++) {
				uint16_t bc = Color::bcolor(m[r][c]);			
				uint16_t fc = Color::fcolor(m[r][c]);

				int x = bx + ox * c;
				int y = by + oy * r;

				str(x, y, String(m[r][c]), 3, bc, fc);
			}
		}
	}

	uint16_t rgb(int r, int g, int b) {
		// return (r << 16) | (g << 8) | b;
		// check http://drakker.org/convert_rgb565.html
		return ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
	}

	uint16_t rgb(RGB c) {
		return rgb(c.r, c.g, c.b);
	}

	void str(int x, int y, String s) {
		GO.lcd.setCursor(x, y);
		GO.lcd.setTextSize(1);
		GO.lcd.print(s);
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
			GO.lcd.fillRoundRect(x + bo, y + bo, ox - bo, oy - bo, A, bc);
			// GO.lcd.fillRoundRect(x, y - 20, ox, oy, 90, bc);
		} else {
			GO.lcd.setTextColor(WHITE, BLACK);
			GO.lcd.fillRect(x, y, ox, oy, BLACK);
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

	String pad(String s) {
		// center with spaces
		int size = ox / 6;
		if (s.length() > 2) return s;
		if (s.length() == 1) return pad(" " + s + " ");
		if (s.length() == 2) return pad(" " + s);
		return pad(s + " ");
	}

	void setColor(bool c) {
		color = c;
	}
};

D d(W, H);

void setup() {
	GO.begin();
	randomSeed(analogRead(0));

    GO.lcd.setBrightness(10);

	init();
}

void init() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			m[i][j] = 0;
			prevm[i][j] = 0;
		}
	}

	s = ongoing;
	GO.lcd.clear();
	if (d.color) GO.lcd.fillRoundRect(0, 0, W, H, A, Color::bbcolor);

	// start with two random numbers
	add_random_n();
	add_random_n();

	d.render();
	// d.sig();
}
 
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


void undo() {
	if (moves_counter == 0) return;
	if (moves_counter >= UNDO_STACK) return;

	moves_counter--;

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			m[i][j] = undos[moves_counter][i][j];
		}
	}

	d.render();
}

void loop() {
	GO.update();

	// update brightness
	if (GO.BtnStart.isPressed() && GO.JOY_Y.wasAxisPressed()) {
		int step = 20;
		if (GO.JOY_Y.wasAxisPressed() == 2) {
			if (brightness + step < 250)  brightness += step;
		} else {
			if (brightness - step > 0) brightness -= step;
		}
		GO.lcd.setBrightness(brightness);
		return;
	}

	// toggle colors
	if (GO.BtnStart.isPressed() && GO.JOY_X.wasAxisPressed()) {
		if (GO.JOY_X.wasAxisPressed() == 2) {
			d.log("color toggle");
			d.setColor(!d.color);
			GO.lcd.clear();
			if (d.color) {
				// redraw background that was removed by the clear();
				GO.lcd.fillRoundRect(0, 0, W, H, A, Color::bbcolor);
			}
			d.render();
		} 
		return;
	}

	if (GO.BtnA.wasPressed()) {
		undo();
		// m[0][0] = 512;
		return;
	}

	if (GO.BtnB.wasPressed()) {
		init();
		return;
	}

	if (GO.JOY_Y.wasAxisPressed() == 2) {
		// UP
		d.log("u");
		move_up();
		if (!cmp()) {
			add_random_n();
			d.render();
			moves_counter++;
		}
		GO.update();
		return;
	}

	if (GO.JOY_Y.wasAxisPressed() == 1) {
		// DOWN
		d.log("d");
		move_down();
		if (!cmp()) {
			add_random_n();
			d.render();
			moves_counter++;
		}
		GO.update();
		return;
	}

	if (GO.JOY_X.wasAxisPressed() == 2) {
		// LEFT
		d.log("l");
		move_left();
		if (!cmp()) {
			add_random_n();
			d.render();
			moves_counter++;
		}
		GO.update();
		return;
	}

	if (GO.JOY_X.wasAxisPressed() == 1) {
		// RIGHT
		d.log("r");
		move_right();
		if (!cmp()) {
			add_random_n();
			d.render();
			moves_counter++;
		}
		GO.update();
		return;
	}

	if (!cmp()) cp();
}

