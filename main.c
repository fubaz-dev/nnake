# include <ncurses.h>
# include <stdio.h>
# include <stdlib.h>
# include <inttypes.h>
# include <time.h>
# include <signal.h>
# include <locale.h>
# include <string.h>

# define SPEED 8
# define UPDATE_DELAY 1000000/SPEED
# define S_MAX 512
# define KQ_MAX 24
# define EMPTY_ELEMENT -99

# define DIRECTION_RIGHT 1
# define DIRECTION_LEFT -1
# define DIRECTION_UP    2
# define DIRECTION_DOWN -2

# define SNAKE_BODY_CHARACTER 'o'
# define SNAKE_HEAD_CHARACTER '@'
# define APPLE_COLOR 1
# define SNAKE_COLOR 2
# define WIDTH  20
# define HEIGHT 20

# define EXIT_SUCCESS 0
# define EXIT_FAILURE 1

# define HELP_PAGE "Usage: nnake [OPTION]...\nRetro themed snake game made in ncurses.\n\n  -h, --help               print this help and exit\n  -g, --grayscale          grayscale, no color\n  -bc, --border-character  specify border character\n  -ac, --apple-character   specify apple character\n"

short gs = 0;
wchar_t border_character = '#';
wchar_t apple_character = 'a';

void ctrl_c_handler(int d) {
}

void exit_ncurses(const char *message, const int ec) {
	endwin();
	fprintf(stderr, "%s", message);
	exit(ec);
}

void init_ncurses() {
	initscr();
	if(!gs) {
		if(!has_colors()) {
			printf("WARNING: your terminal does not support color. forcing --grayscale\n");
			gs=1;
		} else {
			start_color();
			use_default_colors();
			init_pair(1, COLOR_RED, -1);
			init_pair(2, COLOR_GREEN, -1);
		}
	}
	noecho();
	curs_set(0);
	nodelay(stdscr, 1);
}

uint64_t get_current_time(struct timespec *t) {
	clock_gettime(CLOCK_MONOTONIC_RAW, t);
	return t->tv_sec*1000000+t->tv_nsec/1000;
}

void kq_append(int e, int kq[KQ_MAX]) {
	int i;
	for(i=0; kq[i]!=EMPTY_ELEMENT; i++);
	kq[i]=e;
}

int get_kq_last(int kq[KQ_MAX]) {
	int i;
	for(i=0; kq[i]!=EMPTY_ELEMENT; i++);
	return kq[i-1];
}

int get_kq_first(int kq[KQ_MAX]) {
	int t, i;
	t=kq[0];
	for(i=0; kq[i]!=EMPTY_ELEMENT; i++) {
		kq[i]=kq[i+1];
	}
	return t;
}

void handle_keypresses(const int k, int kq[KQ_MAX], const int lt) {
	int i;
	if(k == 'q') {
		exit_ncurses("", EXIT_SUCCESS);
	}
	int keys[8]={'d', DIRECTION_RIGHT, 'a', DIRECTION_LEFT, 'w', DIRECTION_UP, 's', DIRECTION_DOWN};
	for(i=0; i<8; i+=2) {
		if(k == keys[i] && lt != -keys[i+1]) {
			kq_append(keys[i+1], kq);
		}
	}
}

void frame_delay(struct timespec *t, const uint64_t lt, int *d, int kq[KQ_MAX]) {
	while(get_current_time(t)<lt+UPDATE_DELAY) {
		handle_keypresses(getch(), kq, kq[0]!=EMPTY_ELEMENT?get_kq_last(kq):*d);
	}
	int td;
	td=get_kq_first(kq);
	*d=(td!=EMPTY_ELEMENT?td:*d);
}

void create_snake(int s[][2]) {
	s[0][0]=10;
	s[0][1]=8;
	s[1][0]=10;
	s[1][1]=9;
	s[2][0]=10;
	s[2][1]=10;
}

void init_s(int s[][2]) {
	int i;
	for(i=0; i<S_MAX; i++) {
		s[i][0]=EMPTY_ELEMENT;
		s[i][1]=EMPTY_ELEMENT;
	}
	create_snake(s);
}

void init_kq(int kq[KQ_MAX]) {
	int i;
	for(i=0; i<KQ_MAX; i++) {
		kq[i]=EMPTY_ELEMENT;
	}
}

void move_snake(int s[][2], const int d) {
	int i;
	for(i=S_MAX; i>0; i--) {
		if(s[i+1][0] == EMPTY_ELEMENT) {
			mvaddch(s[i][1], s[i][0], ' ');
		}
		if(s[i][0] != EMPTY_ELEMENT && i != 0) {
			s[i][0]=s[i-1][0];
			s[i][1]=s[i-1][1];
		}
	}
	s[0][0]+=(d!=DIRECTION_UP&&d!=DIRECTION_DOWN?d:0);
	s[0][1]+=(d==DIRECTION_UP?-1:(d==DIRECTION_DOWN?1:0));
}

void draw_snake(const int s[][2]) {
	int i;
	for(i=0; s[i][0]!=EMPTY_ELEMENT; i++) {
		if(!gs) {
			attron(COLOR_PAIR(SNAKE_COLOR));
		}
		mvaddch(s[i][1], s[i][0], i==0?SNAKE_HEAD_CHARACTER:SNAKE_BODY_CHARACTER);
		attroff(COLOR_PAIR(SNAKE_COLOR));
	}
}

void generate_apple(const int s[][2]) {
	int x, y, i, f;
	while(1) {
		f=1, x=rand()%WIDTH+1, y=rand()%HEIGHT+1;
		for(i=0; s[i][0]!=EMPTY_ELEMENT; i++) {
			if(s[i][0]==x&&s[i][1]==y) {
				f=0;
			}
		}
		if(f) {
			break;
		}
	}
	if(!gs) {
		attron(COLOR_PAIR(APPLE_COLOR));
	}
	mvprintw(y, x, "%lc", apple_character);
	attroff(COLOR_PAIR(APPLE_COLOR));
}

void draw_border() {
	int i;
	for(i=0; i<HEIGHT+1; i++) {
		mvprintw(i, 0, "%lc", border_character);
		mvprintw(i, WIDTH+1, "%lc", border_character);
	}
	for(i=0; i<WIDTH+2; i++) {
		mvprintw(0, i, "%lc", border_character);
		mvprintw(HEIGHT+1, i, "%lc", border_character);
	}
}

void append_body(int s[][2]) {
	int i;
	for(i=0; s[i][0]!=EMPTY_ELEMENT; i++);
	s[i][0]=s[i-1][0];
	s[i][1]=s[i-1][1];
}

void apple_collision(int s[][2]) {
	if(mvinch(s[0][1], s[0][0])!=' ') {
		append_body(s);
		generate_apple(s);
	}
}

int snake_collision(const int s[][2]) {
	int i;
	for(i=1; s[i][0]!=EMPTY_ELEMENT; i++) {
		if(s[i][0]==s[0][0]&&s[i][1]==s[0][1]) {
			return 0;
		}
	}
	return 1;
}

int border_collision(const int s[][2]) {
	if(s[0][0]<1 || s[0][1]<1 || s[0][0]>WIDTH || s[0][1]>HEIGHT) {
		return 0;
	}
	return 1;
}

void loop() {
	int d;
	struct timespec t;
	uint64_t lt;

	d=DIRECTION_UP;

	int s[S_MAX][2];
	int kq[KQ_MAX];

	init_s(s);
	init_kq(kq);

	draw_border();
	generate_apple(s);

	while(1) {
		lt=get_current_time(&t);
		move_snake(s, d);
		if(!snake_collision(s) || !border_collision(s)) {
			break;
		}
		apple_collision(s);
		draw_snake(s);
		frame_delay(&t, lt, &d, kq);
	}
}

char *compose_error_message(const char *fs, const char *a, const char *ss) {
	char *r = malloc(strlen(a)+strlen(fs)+strlen(ss)+1);
	strcpy(r, fs);
	strcat(r, a);
	strcat(r, ss);
	return r;
}

int main(int argc, char *argv[]) {
	srand(time(0));
	if(setlocale(LC_ALL, "") == NULL) {
		printf("WARNING: locale not set\n");
	}
	int i;
	for(i=1; i<argc; i++) {
		if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			exit_ncurses(HELP_PAGE, EXIT_SUCCESS);
		} if(!strcmp(argv[i], "-g") || !strcmp(argv[i], "--grayscale")) {
			gs=1;
		} if(!strcmp(argv[i], "-bc") || !strcmp(argv[i], "--border-character")) {
			if(i+1<argc) {
				if(mbstowcs(&border_character, argv[i+1], 1) == -1) {
					exit_ncurses(compose_error_message("nnnake: mbstowcs: error converting multibyte char '", argv[i+1], "' to wchar_t\n"), EXIT_FAILURE);
				}
			} else {
				exit_ncurses(compose_error_message("nnake: option '", argv[i], "' requires an argument\n"), EXIT_FAILURE);
			}
			i++;
		} if(!strcmp(argv[i], "-ac") || !strcmp(argv[i], "--apple-character")) {
			if(i+1<argc) {
				if(mbstowcs(&apple_character, argv[i+1], 1) == -1) {
					exit_ncurses(compose_error_message("nnnake: mbstowcs: error converting multibyte char '", argv[i+1], "' to wchar_t\n"), EXIT_FAILURE);
				}
			} else {
				exit_ncurses(compose_error_message("nnake: option '", argv[i], "' requires an argument\n"), EXIT_FAILURE);
			}
			i++;
		} else {
			exit_ncurses(compose_error_message("nnake: invalid option '", argv[i], "'\n"), EXIT_FAILURE);
		}
	}
	signal(SIGINT, ctrl_c_handler);
	init_ncurses();
	while(1) {
		clear();
		loop();
	}
}
