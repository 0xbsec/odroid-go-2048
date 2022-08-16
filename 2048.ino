#include <odroid_go.h>

#define W            320     // screen width
#define H            240     // screen height
#define COLOR 		 false 	 // show colors or keep it w & b
#define UNDO_STACK   100      // how many undos to save FIFO


/*
	TODO;
		- add undo
		- score
		- highscore
		- extract 4 into a global var
		- add log messages (moves count and the prev move)
		- add menu to enable and disable COLOR
		- add brightness controll shortcut
*/

enum status {
	ongoing,
	won,
	over,
};

status s = ongoing;

int m[4][4];
int prevm[4][4];
int score = 0;
int undos[UNDO_STACK][4][4];
int moves_counter;

int r() {
	int i = random(10);
	if (i == 0) return 4; // 10% chance of 0
	return 2;
}

void add_random_n() {
	int zeros_count = 0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
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
	for (int i = 0; i < 4; i++) {
		for (int k = 0; k < 4; k++) {
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

void move_up() {
    for (int i = 0; i < 3; i++) {
        for (int x = 1; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                if (m[x - 1][y] == 0) {
                    m[x - 1][y] = m[x][y];
                    m[x][y] = 0;
                }
            }
        }
    }
    for (int x = 1; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (m[x - 1][y] == m[x][y]) {
                m[x - 1][y] += m[x][y];
                m[x][y] = 0;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int x = 1; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                if (m[x - 1][y] == 0) {
                    m[x - 1][y] = m[x][y];
                    m[x][y] = 0;
                }
            }
        }
    }
}

void move_down() {
    for (int i = 0; i < 3; i++) {
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 4; y++) {
                if (m[x + 1][y] == 0) {
                    m[x + 1][y] = m[x][y];
                    m[x][y] = 0;
                }
            }
        }
    }
    for (int x = 2; x >= 0; x--) {
        for (int y = 0; y < 4; y++) {
            if (m[x + 1][y] == m[x][y]) {
                m[x + 1][y] += m[x][y];
                m[x][y] = 0;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 4; y++) {
                if (m[x + 1][y] == 0) {
                    m[x + 1][y] = m[x][y];
                    m[x][y] = 0;
                }
            }
        }
    }
}

void move_left() {
    for (int i = 0; i < 3; i++) {
        for (int x = 0; x < 4; x++) {
            for (int y = 1; y < 4; y++) {
                if (m[x][y - 1] == 0) {
                    m[x][y - 1] = m[x][y];
                    m[x][y] = 0;
                }
            }
        }
    }
    for (int x = 0; x < 4; x++) {
        for (int y = 1; y < 4; y++) {
            if (m[x][y - 1] == m[x][y]) {
                m[x][y - 1] += m[x][y];
                m[x][y] = 0;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int x = 0; x < 4; x++) {
            for (int y = 1; y < 4; y++) {
                if (m[x][y - 1] == 0) {
                    m[x][y - 1] = m[x][y];
                    m[x][y] = 0;
                }
            }
        }
    }
}

void move_right() {
    for (int i = 0; i < 3; i++) {
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 3; y++) {
                if (m[x][y + 1] == 0) {
                    m[x][y + 1] = m[x][y];
                    m[x][y] = 0;
                }
            }
        }
    }
    for (int x = 0; x < 4; x++) {
        for (int y = 2; y >= 0; y--) {
            if (m[x][y + 1] == m[x][y]) {
                m[x][y + 1] += m[x][y];
                m[x][y] = 0;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 3; y++) {
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
    static RGB bcolor(int value) {
		if (s == ongoing) {
			switch (value) {
				case 0: return {205 , 193, 181};
				case 2: return {238 , 228, 218};
				case 4: return {237, 224, 200};
				case 8: return {242, 177, 121};
				case 16: return {245, 149,  99};
				case 32: return { 246, 124,  95};
				case 64: return { 246,  94,  59};
				case 128: return { 237, 207, 114};
				case 256: return { 237, 204,  97};
				case 512: return { 237, 200,  80};
				case 1024: return { 237, 197,  63};
				case 2048: return { 237, 194,  46};
				default: return { 60,  58,  50};
			}
		} else if (s == over) {
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
		}
    }


	static RGB fcolor(int value) {
		if (s == ongoing) {
			switch (value) {
				case 0: return RGB {205, 193, 181};
				case 2: return RGB {118, 110, 101};
				case 4: return RGB {118, 110, 101};
				default: return RGB {249, 246, 242};
			}
		} else if (s == over) {
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
		}
	}
};

class D {
	int w; // screen width
	int h; // screen height
	int bt = 12; // border thickness

	public:
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
		GO.lcd.setTextColor(WHITE);
		str(w - s.length() * 6 - 5, h - 10, s);
	}

	void border() {
		// uint16_t beige = rgb(250, 248, 239); 
		// uint16_t beige = rgb(255, 255, 255); 
		uint16_t beige = BLACK; 

		// GO.lcd.fillScreen(beige);
		GO.lcd.setTextColor(WHITE);

		// x, y, w, h, color
		// top
		GO.lcd.fillRect(0, 0, w, bt, beige);

		// left
		GO.lcd.fillRect(0, 0, bt, h, beige);

		// right
		GO.lcd.fillRect(w - bt, 0, bt, h, beige);

		// bottom
		GO.lcd.fillRect(bt, h - bt, w - 2*bt, bt, beige);

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
		int bx = 40; // body start offset x
		int by = 50; // body start offset y

		int ox = 70; // next element offset x
		int oy = 50; // next element offset y
		
		for(int r = 0; r < 4; r++) {
			for (int c = 0; c < 4; c++) {
				RGB bc = Color::bcolor(m[r][c]);			
				RGB fc = Color::fcolor(m[r][c]);

				int x = bx + ox * c;
				int y = by + oy * r;

				if (COLOR) GO.lcd.fillRect(x - ox/2, y - oy/2, x + ox/2, y + oy/2, rgb(bc));

				str(x, y, String(m[r][c]), 3, rgb(bc), rgb(fc));
			}
		}
	}

	uint16_t rgb(int r, int g, int b) {
		return (r << 16) | (g << 8) | b;
	}

	uint16_t rgb(RGB c) {
		return (c.r << 16) | (c.g << 8) | c.b;
	}

	void str(int x, int y, String s) {
		GO.lcd.setCursor(x, y);
		GO.lcd.setTextSize(1);
		GO.lcd.print(s);
	}

	void str(int x, int y, String s, int font_size, uint16_t bc, uint16_t fc) {
		GO.lcd.setCursor(x, y);
		if (s.length() >= 4) {
			font_size -= 1;
		}
		GO.lcd.setTextSize(font_size);
		if (COLOR) {
			GO.lcd.setTextColor(fc, bc);
		} else {
			GO.lcd.setTextColor(WHITE, BLACK);
		}
		// clear prev printed blocks
		// GO.lcd.fillRect(x, y, GO.lcd.textWidth(s), 30, BLACK);
		if (s == "0") {
			GO.lcd.print(pad(" "));
		} else {
			GO.lcd.print(pad(s));
		}
	}

	String pad(String s) {
		if (s.length() > 2) return s;
		return pad(s + " ");
	}
};

D d(W, H);

void setup() {
	GO.begin();
	randomSeed(analogRead(0));

	const int w = GO.lcd.width();
	const int h = GO.lcd.height();

	init();
}

void init() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m[i][j] = 0;
		}
	}
	s = ongoing;
	GO.lcd.clear();

	// start with two random numbers
	add_random_n();
	add_random_n();

	d.render();
	d.sig();
}
 
bool cmp() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (prevm[i][j] != m[i][j]) {
				return false;
			}
		}
	}

	return true;
}

bool cp() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			prevm[i][j] = m[i][j];
			undos[moves_counter][i][j] = m[i][j];
		}
	}

	// update undo stack
}


void undo() {
	if (moves_counter == 0) return;
	moves_counter--;

	if (moves_counter >= UNDO_STACK) return;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m[i][j] = undos[moves_counter][i][j];
		}
	}

	d.render();
}

void loop() {
	while (1) {
		if (GO.BtnA.wasPressed()) {
			undo();
		}

		if (GO.BtnB.wasPressed()) {
			init();
		}

		if (GO.JOY_Y.wasAxisPressed() == 2) {
            // UP
            move_up();
			if (!cmp()) {
				add_random_n();
				d.render();
				moves_counter++;
			}
        } else if (GO.JOY_Y.wasAxisPressed() == 1) {
            // DOWN
            move_down();
			if (!cmp()) {
				add_random_n();
				d.render();
				moves_counter++;
			}
        } else if (GO.JOY_X.wasAxisPressed() == 2) {
            // LEFT
            move_left();
			if (!cmp()) {
				add_random_n();
				d.render();
				moves_counter++;
			}
        } else if (GO.JOY_X.wasAxisPressed() == 1) {
            // RIGHT
            move_right();
			if (!cmp()) {
				add_random_n();
				d.render();
				moves_counter++;
			}
        }

		cp();

		GO.update();
	}
}

