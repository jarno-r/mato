#define _DEFAULT_SOURCE

#include <stdio.h>

#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define MIN_WIDTH 30
#define MIN_HEIGHT 30
#define CENTER_X 15
#define CENTER_Y 15
#define MAX_APPLES 100

#define WORM_SIGN ACS_BLOCK //'+'
#define APPLE_SIGN '@'
#define ASSPLOSION_SIGN 'X'

typedef struct apple apple;
struct apple {
    int x,y;
    apple *next,*prev;
};
static apple apples[MAX_APPLES];

int napples=0;

typedef struct node node;
struct node {
    int x,y;
    node *next,*prev;
};

node *head,*tail;
enum direction {
    UP, DOWN, LEFT, RIGHT
};
enum direction dir;

void start()
{
    node *mato = calloc(1,sizeof(node));
    mato->x = CENTER_X;
    mato->y = CENTER_Y;
    mato->prev = mato->next = NULL;
    refresh();
    head=tail=mato;
}

void loopy()
{
    int i=0;
    while (1) {
        bool grow=false;

        int na=LINES*COLS/300;
        if (na>MAX_APPLES) na=MAX_APPLES;
        if (napples<na) {
            for(int i=napples;i<napples+na;i++) {
                apples[i].x = rand() % COLS;
                apples[i].y = rand() % LINES;
                mvaddch(apples[i].y, apples[i].x, APPLE_SIGN);
            }
            napples+=na;
        }

        for(int i=0;i<napples;i++) {
            if (head->x==apples[i].x && head->y==apples[i].y) {
                grow=true;
                apples[i].x = rand() % COLS;
                apples[i].y = rand() % LINES;
                mvaddch(apples[i].y, apples[i].x, APPLE_SIGN);
            }
        }

        if (head->x<0 || head->x>=COLS || head->y<0 || head->y>=LINES) {
            break;
        }

        int ch = getch();
        if (ch == 'q' || ch==27) {
            break;
        }

        if (ch=='g') grow=true;

        switch(ch) {
            case KEY_UP:
                dir = UP;
                break;
            case KEY_DOWN:
                dir = DOWN;
                break;
            case KEY_LEFT:
                dir = LEFT;
                break;
            case KEY_RIGHT:
                dir = RIGHT;
                break;
        }

        if (!grow) mvaddch(tail->y, tail->x, ' ');
        else {
            tail->next=calloc(1,sizeof(node));
            tail->next->prev=tail;
            tail=tail->next;
            tail->x=tail->prev->x;
            tail->y=tail->prev->y;
        }

        tail->x=head->x;
        tail->y=head->y;
        switch(dir) {
            case UP:
                tail->y=head->y-1;
                break;
            case DOWN:
                tail->y=head->y+1;
                break;
            case LEFT:
                tail->x=head->x-1;
                break;
            case RIGHT:
                tail->x=head->x+1;
                break;
        }

        if (tail!=head) {
            tail->next=head;
            head->prev=tail;
            tail=tail->prev;
            head=head->prev;
            tail->next=NULL;
            head->prev=NULL;
        }

        mvaddch(head->y, head->x, WORM_SIGN);
        refresh();

        for(node *n=head->next;n;n=n->next) {
            if (head->x==n->x && head->y==n->y) {
                return;
            }
        }

        usleep(100000);
    }
}

int main()
{
    setlocale(LC_ALL, "");

    srand(time(NULL));

    WINDOW *win=initscr();
    if (!win) {
        printf("Failed to initialize.\n");
        exit(1);
    }
    atexit((void (*)(void))endwin);

    int x, y;
    getmaxyx(win, y, x);
    if (x < MIN_WIDTH || y < MIN_HEIGHT) {
        printf("Terminal too small (%dx%d). Resize to at least %dx%d\n",x,y,MIN_WIDTH, MIN_HEIGHT);
        exit(1);
    }

    cbreak(); noecho();
    noqiflush();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    clear();
    refresh();

    start();
    loopy();
}
