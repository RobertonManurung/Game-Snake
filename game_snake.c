#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define GRID_SIZE 10

typedef struct {
    int x;
    int y;
} SnakeSegment;

SnakeSegment snake[GRID_SIZE * GRID_SIZE];
int snakeLength = 1;
int fruitX, fruitY;
int gameRunning = 1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void generateFruit() {
    pthread_mutex_lock(&mutex);
    fruitX = rand() % GRID_SIZE;
    fruitY = rand() % GRID_SIZE;
    pthread_mutex_unlock(&mutex);
}

void displayGrid() {
    system("clear");
    printf("Use 'w', 'a', 's', 'd' to control the snake. Press 'x' to exit.\n\n");

    pthread_mutex_lock(&mutex);

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int isSnake = 0;
            for (int k = 0; k < snakeLength; k++) {
                if (snake[k].x == j && snake[k].y == i) {
                    isSnake = 1;
                    break;
                }
            }

            if (isSnake)
                printf("O ");
            else if (j == fruitX && i == fruitY)
                printf("F ");
            else
                printf(". ");
        }
        printf("\n");
    }

    pthread_mutex_unlock(&mutex);
}

void moveSnake(char direction) {
    pthread_mutex_lock(&mutex);

    switch (direction) {
        case 'w': snake[0].y--; break;
        case 'a': snake[0].x--; break;
        case 's': snake[0].y++; break;
        case 'd': snake[0].x++; break;
    }

    if (snake[0].x == fruitX && snake[0].y == fruitY) {
        snakeLength++;
        generateFruit();
    }

    if (snake[0].x < 0 || snake[0].x >= GRID_SIZE || snake[0].y < 0 || snake[0].y >= GRID_SIZE) {
        gameRunning = 0;
    }

    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            gameRunning = 0;
        }
    }

    pthread_mutex_unlock(&mutex);
}

void* gameThread(void* arg) {
    snake[0].x = GRID_SIZE / 2;
    snake[0].y = GRID_SIZE / 2;

    snakeLength = 1;
    generateFruit();

    while (gameRunning) {
        displayGrid();

        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 500000000;  // Jeda sementara untuk simulasi gerakan ular (500 ms)
        nanosleep(&ts, NULL);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, gameThread, NULL);

    while (1) {
        char ch;
        scanf(" %c", &ch);

        if (ch == 'x') {
            gameRunning = 0;
            break;
        }

        moveSnake(ch);
    }

    pthread_join(thread, NULL);

    return 0;
}
