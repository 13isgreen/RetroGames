#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define CELL_SIZE 20
#define GRID_WIDTH (SCREEN_WIDTH / CELL_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT / CELL_SIZE)

// Directions
typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

// Snake structure
typedef struct {
    int x, y;
} Segment;

Segment snake[GRID_WIDTH * GRID_HEIGHT];
int snake_length = 5;
Direction dir = RIGHT;
int food_x, food_y;
int running = 1;

void init_game() {
    for (int i = 0; i < snake_length; i++) {
        snake[i].x = snake_length - i - 1;
        snake[i].y = 0;
    }
    srand(time(NULL));
    food_x = rand() % GRID_WIDTH;
    food_y = rand() % GRID_HEIGHT;
}

void update_game() {
    // Move the snake
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    
    // Update head position
    switch (dir) {
        case UP: snake[0].y--; break;
        case DOWN: snake[0].y++; break;
        case LEFT: snake[0].x--; break;
        case RIGHT: snake[0].x++; break;
    }

    // Check for collisions with walls
    if (snake[0].x < 0 || snake[0].x >= GRID_WIDTH || snake[0].y < 0 || snake[0].y >= GRID_HEIGHT) {
        running = 0;
    }
    
    // Check for collisions with itself
    for (int i = 1; i < snake_length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            running = 0;
        }
    }

    // Check if food is eaten
    if (snake[0].x == food_x && snake[0].y == food_y) {
        snake_length++;
        food_x = rand() % GRID_WIDTH;
        food_y = rand() % GRID_HEIGHT;
    }
}

void render_game(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < snake_length; i++) {
        SDL_Rect rect = { snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect food_rect = { food_x * CELL_SIZE, food_y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
    SDL_RenderFillRect(renderer, &food_rect);

    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    init_game();
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: if (dir != DOWN) dir = UP; break;
                    case SDLK_DOWN: if (dir != UP) dir = DOWN; break;
                    case SDLK_LEFT: if (dir != RIGHT) dir = LEFT; break;
                    case SDLK_RIGHT: if (dir != LEFT) dir = RIGHT; break;
                }
            }
        }
        update_game();
        render_game(renderer);
        SDL_Delay(100);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
