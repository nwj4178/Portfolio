#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

#define FOREGROUND_RED      0x0004
#define FOREGROUND_GREEN    0x0002
#define FOREGROUND_BLUE     0x0001
#define FOREGROUND_YELLOW   (FOREGROUND_RED | FOREGROUND_GREEN)
#define FOREGROUND_CYAN     (FOREGROUND_GREEN | FOREGROUND_BLUE)
#define FOREGROUND_MAGENTA  (FOREGROUND_RED | FOREGROUND_BLUE)
#define FOREGROUND_ORANGE   (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY)

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define SPACE 32	
#define p 112
#define P 80
#define ESC 27
#define false 0
#define true 1

#define ACTIVE_BLOCK -2
#define CEILLING -1
#define EMPTY 0
#define WALL 1
#define INACTIVE_BLOCK 2
#define MAIN_X 11
#define MAIN_Y 23
#define MAIN_X_ADJ 3 
#define MAIN_Y_ADJ 1
#define STATUS_X_ADJ MAIN_X_ADJ + MAIN_X + 1 
#define ROTATE_KICK 100

int conditionSatisfied;
int game_mode = 0;
int STATUS_Y_GOAL;
int STATUS_Y_LEVEL;
int STATUS_Y_SCORE;

int blocks[7][4][4][4] = {
	{{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},
	 {0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0}},
	{{0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0},
	 {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0}},
	{{0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0},
	 {0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0}},
	{{0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0},
	 {0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0}},
	{{0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0},{0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0},
	 {0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0}},
	{{0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0},
	 {0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0},{0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0}},
	{{0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,1,0,0,1,0,0},
	 {0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0},{0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0}}
};

int timernumber = 0;
int b_type;
int b_rotation;
int b_type_next;
int main_org[MAIN_Y][MAIN_X];
int main_cpy[MAIN_Y][MAIN_X];
int bx, by;
int key;
int speed;
int level;
int level_goal;
int cnt;
int score;
int last_score = 0;
int best_score = 0;
int new_block_on = 0;
int crush_on = 0;
int level_up_on = 0;
int space_key_on = 0;

void title(void);
void title1(void);
void title2(void);
void timer(void);
void reset(void);
void reset_main(void);
void reset_main_cpy(void);
void draw_map(void);
void draw_main(void);
void new_block(void);
void check_key(void);
void drop_block(void);
int check_crush(int bx, int by, int rotation);
void move_block(int dir);
void check_line(void);
void check_level_up(void);
void check_game_over(void);
void checkCondition(void);
void pause(void);

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void clearScreen() {
	printf("\033[2J");
	printf("\033[H");
}

void gotoxy(int x, int y) {
	COORD pos = { 2 * x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE;

void setcursortype(CURSOR_TYPE c) {
	CONSOLE_CURSOR_INFO CurInfo;
	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

int main() {
	int i;
	srand((unsigned)time(NULL));
	setcursortype(NOCURSOR);
	title();
	reset();

	while (1) {
		for (i = 0; i < 5; i++) {
			check_key();
			draw_main();
			Sleep(speed);
			timer();
			if (crush_on && check_crush(bx, by + 1, b_rotation) == false) Sleep(0);

			if (space_key_on == 1) {
				space_key_on = 0;
				break;
			}
		}
		drop_block();
		check_level_up();
		check_game_over();
		checkCondition();
		if (new_block_on == 1) new_block();
	}
}

void title(void) {
	int x = 5;
	int y = 4;
	timernumber = 0;

	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	printf("\n\n\n     --------------------------------------------------------\n");
	printf("     --------------------------------------------------------\n");
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
	printf("     @@@@@@@@@    @@@@@@@   @@@@@@@@  @@@@@@@@    @    @@@@@@\n         @        @            @      @     @     @    @\n         @        @@@@@@@      @      @@@@@@      @    @@@@@@\n         @        @            @      @     @     @         @\n         @        @@@@@@@      @      @      @    @    @@@@@@\n");
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	printf("     --------------------------------------------------------\n");
	printf("     --------------------------------------------------------\n\n\n");
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	printf("     시작하려면 엔터 키를 누르십시오\n\n\n");
	printf("     제작자를 보려면 C를 누르십시오\n\n\n");
	printf("     게임을 종료하려면 ESC를 누르십시오\n\n\n");
	printf("     게임 가이드는 게임을 시작하고 확인할 수 있습니다\n\n\n");

	int choice;
	while (1) {
		if (_kbhit()) {
			choice = _getch();
			if (choice == 13) {
				clearScreen();
				system("cls");
				title2();
				break;
			}
			else if (choice == 27) {
				exit(0);
			}
			else if (choice == 'C' || choice == 'c') {
				clearScreen();
				system("cls");
				title1();
				break;
			}
		}
	}
}

void title1(void) {
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	printf("#### \n");
	printf("######         ###     ###                                                                  ### \n");
	printf("##     ##      ###     ###    ###      ##########        ########       ###            ####        ### \n");
	printf("##      ##             ###  ###     ###         ###    ####      ####       ###          ##     ##      ###    ### \n");
	printf("##       ##    ###     #####        ###         ###    ###        ###     #########      ##       ##     ###  ### \n");
	printf("##       ##    ###     ####         ###         ###    ###                #########     ##         ##    ##### \n");
	printf("##      ##     ###     ###          ############       ###                    ###        ##         ##    #### \n");
	printf("##     ##      ###     ###          ###                ####       ###        ###         ##        ##     ### \n");
	printf("######         ###     ###          ###                ####     ####        ###          ##    ##       ### \n");
	printf("####           ###     ###          ############        ########          ###            ######        ### \n\n\n\n");

	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	printf("     노원종\n     한기승\n     한인하");
	printf("\n\n\n     메인화면으로 가려면 엔터 키를 누르십시오");

	int choice;
	while (1) {
		if (_kbhit()) {
			choice = _getch();
			if (choice == 13) {
				clearScreen();
				system("cls");
				title();
				break;
			}
		}
	}
}

void title2(void) {
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	printf("\n\n\n");
	printf("           일반     모드 :일반 모드는 시간제한이 없고 게임오버 되지 않는 이상 점수를 계속 누적할 수 있습니다\n");
	printf("           타임어택 모드 :목표 점수가 있고 빠르게 점수에 도달해야 하는 방식입니다 (목표점수:10000)\n");
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	printf("\n\n\n\n           A는 일반, B는 타임어택입니다\n\n                     메인화면으로 가려면 ESC를 누르십시오");

	while (1) {
		if (_kbhit()) {
			char key = _getch();
			if (key == 'A' || key == 'a' || key == 'B' || key == 'b' || key == 'd' || key == 'D') {
				game_mode = key;
				break;
			}
			else if (key == 27) {
				clearScreen();
				title();
				return;
			}
		}
	}
	clearScreen();
}

void reset(void) {
	FILE* file = fopen("score.dat", "rt");
	if (file == 0) { best_score = 0; }
	else {
		fscanf(file, "%d", &best_score);
		fclose(file);
	}

	level = 1;
	score = 0;
	level_goal = 1000;
	key = 0;
	crush_on = 0;
	cnt = 0;
	speed = 100;

	system("cls");
	reset_main();
	draw_map();
	draw_main();
	b_type_next = rand() % 7;
	new_block();
}

void reset_main(void) {
	int i, j;

	for (i = 0; i < MAIN_Y; i++) {
		for (j = 0; j < MAIN_X; j++) {
			main_org[i][j] = 0;
			main_cpy[i][j] = 100;
		}
	}
	for (j = 1; j < MAIN_X; j++) {
		main_org[3][j] = CEILLING;
	}
	for (i = 1; i < MAIN_Y - 1; i++) {
		main_org[i][0] = WALL;
		main_org[i][MAIN_X - 1] = WALL;
	}
	for (j = 0; j < MAIN_X; j++) {
		main_org[MAIN_Y - 1][j] = WALL;
	}
}

void reset_main_cpy(void) {
	int i, j;
	for (i = 0; i < MAIN_Y; i++) {
		for (j = 0; j < MAIN_X; j++) {
			main_cpy[i][j] = 100;
		}
	}
}

void timer(void) {
	int y = 3;
	gotoxy(STATUS_X_ADJ, y + 20);
	printf("걸린 시간: %d초\n", timernumber / 9);
	timernumber++;
}

void draw_map(void) {
	int y = 3;
	gotoxy(STATUS_X_ADJ, STATUS_Y_LEVEL = y); printf(" LEVEL : %5d", level);
	gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL = y + 1); printf(" GOAL   : %5d", 10 - cnt);
	gotoxy(STATUS_X_ADJ, y + 2); printf("+-  N E X T  -+ ");
	gotoxy(STATUS_X_ADJ, y + 3); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 4); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 5); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 6); printf("|             | ");
	gotoxy(STATUS_X_ADJ, y + 7); printf("+-- -  -  - --+ ");
	gotoxy(STATUS_X_ADJ, y + 8); printf(" YOUR SCORE :");
	gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE = y + 9); printf("        %6d", score);
	gotoxy(STATUS_X_ADJ, y + 10); printf(" LAST SCORE :");
	gotoxy(STATUS_X_ADJ, y + 11); printf("        %6d", last_score);
	gotoxy(STATUS_X_ADJ, y + 12); printf(" BEST SCORE :");
	gotoxy(STATUS_X_ADJ, y + 13); printf("        %6d", best_score);
	gotoxy(STATUS_X_ADJ, y + 15); printf("  △   : 모양 바꾸기            SPACE : 끝까지 내리기");
	gotoxy(STATUS_X_ADJ, y + 16); printf("◁  ▷ : 좌로 / 우로            P   : 일시정지 (일시정지 후 아무 키를");
	gotoxy(STATUS_X_ADJ, y + 17); printf("  ▽   : 한칸 아래로 내리기                       누르면 게임 재시작)");
	gotoxy(STATUS_X_ADJ, y + 18); printf("일시정지 상태에서 P를 한번 더 누르면 메인화면으로 이동");

	if (game_mode == 'A' || game_mode == 'a') {
		gotoxy(STATUS_X_ADJ + 5, y + 23); printf("현재 모드는 일반 모드입니다.\n");
	}
	else if (game_mode == 'B' || game_mode == 'b') {
		gotoxy(STATUS_X_ADJ + 5, y + 23); printf("현재 모드는 타임어택 모드입니다. (목표점수:10000)\n");
	}
	else if (game_mode == 'D' || game_mode == 'd') {
		gotoxy(STATUS_X_ADJ + 5, y + 23); printf("현재 모드는 타임어택 모드입니다. (목표점수:100)\n");
	}
}

void setConsoleTextColor(int color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

void draw_main(void) {
	int i, j;
	int block_colors[7] = { FOREGROUND_CYAN, FOREGROUND_YELLOW, FOREGROUND_MAGENTA, FOREGROUND_GREEN, FOREGROUND_RED, FOREGROUND_BLUE, FOREGROUND_ORANGE };

	for (j = 1; j < MAIN_X - 1; j++) {
		if (main_org[3][j] == EMPTY) main_org[3][j] = CEILLING;
	}

	for (i = 0; i < MAIN_Y; i++) {
		for (j = 0; j < MAIN_X; j++) {
			if (main_cpy[i][j] != main_org[i][j]) {
				gotoxy(MAIN_X_ADJ + j, MAIN_Y_ADJ + i);
				switch (main_org[i][j]) {
				case EMPTY:
					printf("  ");
					break;
				case CEILLING:
					printf(". ");
					break;
				case WALL:
					printf("▩");
					break;
				case INACTIVE_BLOCK:
					setConsoleTextColor(block_colors[b_type]);
					printf("■");
					setConsoleTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					break;
				case ACTIVE_BLOCK:
					setConsoleTextColor(block_colors[b_type_next]);
					printf("■");
					setConsoleTextColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
					break;
				}
			}
		}
	}

	for (i = 0; i < MAIN_Y; i++) {
		for (j = 0; j < MAIN_X; j++) {
			main_cpy[i][j] = main_org[i][j];
		}
	}
}

void new_block(void) {
	int i, j;
	bx = (MAIN_X / 2) - 1;
	by = 0;
	b_type = b_type_next;
	b_type_next = rand() % 7;
	b_rotation = 0;
	new_block_on = 0;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = ACTIVE_BLOCK;
		}
	}

	for (i = 1; i < 3; i++) {
		for (j = 0; j < 4; j++) {
			if (blocks[b_type_next][0][i][j] == 1) {
				gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
				printf("■");
			}
			else {
				gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
				printf("  ");
			}
		}
	}
}

void check_key(void) {
	key = 0;
	if (_kbhit()) {
		key = _getch();
		if (key == 224) {
			do { key = _getch(); } while (key == 224);
			switch (key) {
			case LEFT:
				if (check_crush(bx - 1, by, b_rotation) == true) move_block(LEFT);
				break;
			case RIGHT:
				if (check_crush(bx + 1, by, b_rotation) == true) move_block(RIGHT);
				break;
			case DOWN:
				if (check_crush(bx, by + 1, b_rotation) == true) move_block(DOWN);
				break;
			case UP:
				if (check_crush(bx, by, (b_rotation + 1) % 4) == true) move_block(UP);
				else if (crush_on == 1 && check_crush(bx, by - 1, (b_rotation + 1) % 4) == true) move_block(ROTATE_KICK);
			}
		}
		else {
			switch (key) {
			case SPACE:
				space_key_on = 1;
				while (crush_on == 0) {
					drop_block();
					score += level;
					gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE); printf("        %6d", score);
				}
				break;
			case P:
			case p:
				pause();
				break;
			}
		}
	}
	while (_kbhit()) _getch();
}

void drop_block(void) {
	int i, j;
	if (crush_on && check_crush(bx, by + 1, b_rotation) == true) crush_on = 0;
	if (crush_on && check_crush(bx, by + 1, b_rotation) == false) {
		for (i = 0; i < MAIN_Y; i++) {
			for (j = 0; j < MAIN_X; j++) {
				if (main_org[i][j] == ACTIVE_BLOCK) main_org[i][j] = INACTIVE_BLOCK;
			}
		}
		crush_on = 0;
		check_line();
		new_block_on = 1;
		return;
	}
	if (check_crush(bx, by + 1, b_rotation) == true) move_block(DOWN);
	if (check_crush(bx, by + 1, b_rotation) == false) crush_on++;
}

int check_crush(int bx, int by, int b_rotation) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (blocks[b_type][b_rotation][i][j] == 1 && main_org[by + i][bx + j] > 0) return false;
		}
	}
	return true;
}

void move_block(int dir) {
	int i, j;
	switch (dir) {
	case LEFT:
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j - 1] = ACTIVE_BLOCK;
			}
		}
		bx--;
		break;

	case RIGHT:
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j + 1] = ACTIVE_BLOCK;
			}
		}
		bx++;
		break;

	case DOWN:
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i + 1][bx + j] = ACTIVE_BLOCK;
			}
		}
		by++;
		break;

	case UP:
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		b_rotation = (b_rotation + 1) % 4;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = ACTIVE_BLOCK;
			}
		}
		break;

	case ROTATE_KICK:
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i][bx + j] = EMPTY;
			}
		}
		b_rotation = (b_rotation + 1) % 4;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type][b_rotation][i][j] == 1) main_org[by + i - 1][bx + j] = ACTIVE_BLOCK;
			}
		}
		by--;
		break;
	}
}

void check_line(void) {
	int i, j, k, l;
	int block_amount;

	for (i = MAIN_Y - 2; i > 3;) {
		block_amount = 0;
		for (j = 1; j < MAIN_X - 1; j++) {
			if (main_org[i][j] > 0) block_amount++;
		}
		if (block_amount == MAIN_X - 2) {
			if (level_up_on == 0) {
				score += 100 * level;
				cnt++;
			}
			for (k = i; k > 1; k--) {
				for (l = 1; l < MAIN_X - 1; l++) {
					if (main_org[k - 1][l] != CEILLING) main_org[k][l] = main_org[k - 1][l];
					if (main_org[k - 1][l] == CEILLING) main_org[k][l] = EMPTY;
				}
			}
		}
		else i--;
	}
}

void check_level_up(void) {
	if (cnt >= 10) {
		draw_main();
		level_up_on = 1;
		level += 1;
		cnt = 0;

		Sleep(100);
		check_line();

		switch (level) {
		case 2: speed = 50; break;
		case 3: speed = 25; break;
		case 4: speed = 10; break;
		case 5: speed = 5; break;
		case 6: speed = 4; break;
		case 7: speed = 3; break;
		case 8: speed = 2; break;
		case 9: speed = 1; break;
		case 10: speed = 0; break;
		}
		level_up_on = 0;

		gotoxy(STATUS_X_ADJ, STATUS_Y_LEVEL); printf(" LEVEL : %5d", level);
		gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL); printf(" GOAL   : %5d", 10 - cnt);
	}
}

void check_game_over(void) {
	int x = 5;
	int y = 5;

	if ((game_mode == 'A' || game_mode == 'a') && conditionSatisfied) {
		gotoxy(x, y + 0); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
		gotoxy(x, y + 1); printf("▤                                    ▤");
		gotoxy(x, y + 2); printf("▤  +-----------------------+         ▤");
		gotoxy(x, y + 3); printf("▤  |        게임오버       |         ▤");
		gotoxy(x, y + 4); printf("▤  +-----------------------+         ▤");
		gotoxy(x, y + 5); printf("▤   YOUR SCORE: %6d               ▤", score);
		gotoxy(x, y + 6); printf("▤                                    ▤");
		gotoxy(x, y + 7); printf("▤  아무 키나 눌러서 재시작 합니다    ▤");
		gotoxy(x, y + 8); printf("▤                                    ▤");
		gotoxy(x, y + 9); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
		last_score = score;

		if (score > best_score) {
			FILE* file = fopen("score.dat", "wt");
			gotoxy(x, y + 6); printf("▤   ★★★ BEST SCORE! ★★★   ▤  ");
			if (file == 0) {
				gotoxy(0, 0);
				printf("FILE ERROR: SYSTEM CANNOT WRITE BEST SCORE ON \"SCORE.DAT\"");
			}
			else {
				fprintf(file, "%d", score);
				fclose(file);
			}
		}
		Sleep(1000);
		while (_kbhit()) _getch();
		key = _getch();
		reset();
		timernumber = 0;
	}
	else if ((game_mode == 'B' || game_mode == 'b') && (score >= 10000 || conditionSatisfied)) {
		gotoxy(x, y + 0); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
		gotoxy(x, y + 1); printf("▤                                    ▤");
		gotoxy(x, y + 2); printf("▤  +-----------------------+         ▤");
		gotoxy(x, y + 3); printf("▤  |        게임종료       |         ▤");
		gotoxy(x, y + 4); printf("▤  +-----------------------+         ▤");
		gotoxy(x, y + 5); printf("▤   YOUR SCORE: %6d               ▤", score);
		gotoxy(x, y + 6); printf("▤                                    ▤");
		gotoxy(x, y + 7); printf("▤  아무 키나 눌러서 재시작 합니다    ▤");
		gotoxy(x, y + 8); printf("▤                                    ▤");
		gotoxy(x, y + 9); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
		last_score = score;

		if (score > best_score) {
			FILE* file = fopen("score.dat", "wt");
			gotoxy(x, y + 6); printf("▤   ★★★ BEST SCORE! ★★★   ▤  ");
			if (file == 0) {
				gotoxy(0, 0);
				printf("FILE ERROR: SYSTEM CANNOT WRITE BEST SCORE ON \"SCORE.DAT\"");
			}
			else {
				fprintf(file, "%d", score);
				fclose(file);
			}
		}
		Sleep(1000);
		while (_kbhit()) _getch();
		key = _getch();
		reset();
		timernumber = 0;
	}
	else if ((game_mode == 'D' || game_mode == 'd') && (score >= 100 || conditionSatisfied)) {
		gotoxy(x, y + 0); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
		gotoxy(x, y + 1); printf("▤                                    ▤");
		gotoxy(x, y + 2); printf("▤  +-----------------------+         ▤");
		gotoxy(x, y + 3); printf("▤  |        게임종료       |         ▤");
		gotoxy(x, y + 4); printf("▤  +-----------------------+         ▤");
		gotoxy(x, y + 5); printf("▤   YOUR SCORE: %6d               ▤", score);
		gotoxy(x, y + 6); printf("▤                                    ▤");
		gotoxy(x, y + 7); printf("▤  아무 키나 눌러서 재시작 합니다    ▤");
		gotoxy(x, y + 8); printf("▤                                    ▤");
		gotoxy(x, y + 9); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
		last_score = score;

		if (score > best_score) {
			FILE* file = fopen("score.dat", "wt");
			gotoxy(x, y + 6); printf("▤   ★★★ BEST SCORE! ★★★   ▤  ");
			if (file == 0) {
				gotoxy(0, 0);
				printf("FILE ERROR: SYSTEM CANNOT WRITE BEST SCORE ON \"SCORE.DAT\"");
			}
			else {
				fprintf(file, "%d", score);
				fclose(file);
			}
		}
		Sleep(1000);
		while (_kbhit()) _getch();
		key = _getch();
		reset();
		timernumber = 0;
	}
}

void checkCondition() {
	for (int i = 1; i < MAIN_X - 2; i++) {
		if (main_org[3][i] > 0) {
			conditionSatisfied = 1;
			return;
		}
	}
	conditionSatisfied = 0;
}

void pause(void) {
	int i, j;
	int choice = _getch();

	if (choice == 'P' || choice == 'p') {
		system("cls");
		title();
		reset();
	}
	else {
		draw_main();
		draw_map();

		for (i = 1; i < 3; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[b_type_next][0][i][j] == 1) {
					gotoxy(MAIN_X + MAIN_X_ADJ + 3 + j, i + 6);
					printf("■");
				}
				else {
					gotoxy(MAIN_X + MAIN_X_ADJ + 3 + j, i + 6);
					printf("  ");
				}
			}
		}
	}
}