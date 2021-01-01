#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

int randInt;


int keyhit (void) {
    //takes in an key input
    struct timeval tv;
    fd_set read_fd;

    tv.tv_sec=0;
    tv.tv_usec=0;
    FD_ZERO(&read_fd);
    FD_SET(0,&read_fd);

    if(select(1, &read_fd, NULL, NULL, &tv)==-1)
        return 0;
    if(FD_ISSET(0, &read_fd))
        return 1;
    return 0;
}



void moveSnake(int *snake, int X, int Y, int speed, int snakeLength) {
    int oldX = 0;
    int oldY = 0;
    int curX = *snake;
    int curY = *(snake+1);
    int newX = curX + X;
    int newY = curY + Y;

    *(snake) = newX;
    *(snake+1) = newY;
    mvprintw(newY, newX, "@");

    for (size_t i = 2; i < snakeLength*2; i += 2) {
        oldX = curX;
        oldY = curY;
        curX = *(snake+i);
        curY = *(snake+i+1);
        newX = oldX;
        newY = oldY;
        *(snake+i) = newX;
        *(snake+i+1) = newY;

        mvprintw(newY, newX, "O");
    }

    if (Y != 0) {
        usleep(500000/speed);
    }

    usleep(1000000/speed);
}

int collision(int *snake, int snakeLength) {
    //returns 1 if snake collides with snake pit or if it collides with its own tail
    int x = *snake;
    int y = *(snake+1);

    if (x <= 0 || x >= COLS) {
        return 1;
    }
    else if (y <= 0 || y >= LINES) {
        return 1;
    }

    for (size_t i = 2; i < snakeLength*2+2; i+=2) {
        if (x == *(snake+i) && y == *(snake+i+1)) {
            return 1;
        }
    }
  return 0;
}

void snakeFood(int* foodX, int* foodY, int *consume) {
    //spawns snake food at random location
    if (*consume) {
        srand(time(0)); //use current time as seed for random generator
        *foodX = (rand() % COLS-1)+1;
        *foodY = (rand() % LINES-1)+1;
        *consume = 0;
  }

  mvprintw(*foodY, *foodX,"*");
}



void consumed(int *snake, int foodX, int foodY, int *consumedFood, int *snakeLength, int *speed) {
    //when the snake eats the food it grows by 1 and speed increases by 1
    int x = *snake;
    int y = *(snake+1);
    if (x == foodX && y == foodY) {
    *consumedFood = 1;
    *snakeLength += 1;
    int snakeLen = *snakeLength;
    int lastX = *(snake + snakeLen*2-2);
    int lastY = *(snake + snakeLen*2-2+1);
    *(snake + snakeLen*2) = lastX;
    *(snake + snakeLen*2+1) = lastY;
    speed++;
    }
}

void trophy(int *trophyX, int *trophyY, int *consume){
    // spawns trophy it increases snake by a random int from 1 to 9 and increases speed by the same value
    if(*consume){
        srand(time(0));
        randInt = (rand()%9)+1;
        *trophyX = (rand()% COLS-1)+1;
        *trophyY = (rand()% LINES-1)+1;
        *consume = 0;
    }
    char *trophyValue;
    sprintf(trophyValue, "%d", randInt);
    mvprintw(*trophyY, *trophyX, trophyValue);
    
}

int victory(int *snakeLength){
    //returns 1 if victory occurs. victory achieved if snake reaches the size of half the perimeter
    int halfPerimeter = (2*(COLS+LINES))/4;
    if( *snakeLength >= halfPerimeter){
        return 1;
    }
    return 0;
}

void trophyConsumed(int *snake, int trophyX, int trophyY, int *consumedT, int *snakeLength, int *speed){
    //increases tail and speed by trophy
    int x = *snake;
    int y = *(snake+1);

    if(x==trophyX && y==trophyY){
        *consumedT = 1;
        *snakeLength += randInt;
        int snakeLen = *snakeLength;
        int prevX = *(snake + snakeLen*2-2);
        int prevY = *(snake + snakeLen*2-2+1);
        *(snake + snakeLen*2) = prevX;
        *(snake + snakeLen*2+1) = prevY;
        speed += randInt;
    }
}

int main() {
    int perimeter = ((2*(COLS+LINES))/2)+1;
    int snake[100][2];
    for (size_t i = 0; i < 100; i++) {
        for (size_t j = 0; j < 2; j++) {
            snake[i][j] = 0;
        }
    }
    srand(time(0));             //random spawn points for snake
    int randX = (rand()%20);
    int randY = (rand()%20);

    snake[0][0] = randX;
    snake[0][1] = randY;
    int keyPressed = 0;
    int X = 1;
    int Y = 0;
    int speed = 15;
    int foodX = 0;          //food cordinates
    int foodY = 0;
    int foodConsumed = 1;
    int trophyX = 1;        //trophy cordinates
    int trophyY = 1;
    int consume = 1;
    int snakeLength = 3;    //starting size of 3



    initscr();
    curs_set(false);
    noecho();

    while (!collision(&snake[0][0], snakeLength)) {
        erase();

        snakeFood(&foodX, &foodY, &foodConsumed);
        trophy(&trophyX, &trophyY, &consume);
        moveSnake(&snake[0][0], X, Y, speed, snakeLength);
        consumed(&snake[0][0], foodX, foodY, &foodConsumed, &snakeLength, &speed);
        trophyConsumed(&snake[0][0], trophyX, trophyY, &consume, &snakeLength, &speed);

        refresh();
        if(victory(&snakeLength))
            break;

        if (keyhit()) {
        keyPressed = getch();
        keypad(stdscr,TRUE);    // enables working with the arrow keys


        if (keyPressed == KEY_UP && !(Y == 1 && X == 0)) {
            Y = -1;
            X = 0;
        }
        if (keyPressed == KEY_DOWN && !(Y == -1 && X == 0)) {
            Y = 1;
            X = 0;
        }
        if (keyPressed == KEY_LEFT && !(Y == 0 && X == 1)) {
            Y = 0;
            X = -1;
        }
        if (keyPressed == KEY_RIGHT && !(Y == 0 && X == -1)) {
            Y = 0;
            X = 1;
        }
    }


  }
    if(victory(&snakeLength)){
        erase();
        mvprintw(LINES/2, COLS/4, "Vicotry!! - Your score was: %d", snakeLength);
        refresh();
        getch();
        endwin();
        return 0;
    }
    erase();
    mvprintw(LINES/2,COLS/4,"Game Over! - Your score was: %d", snakeLength);
    refresh();
    getch();
	endwin();
	return 0;
}