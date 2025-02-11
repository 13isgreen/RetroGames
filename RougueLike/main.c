#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TILE_SIZE 40
#define MAP_WIDTH 20
#define MAP_HEIGHT 15
#define ENEMY_COUNT 3
#define LOOT_COUNT 3

typedef enum { NONE, SWORD, POTION } ItemType;

// Player struct
typedef struct {
    int x, y;
    int hp;
    int attack;
} Player;

Player player = {1, 1, 10, 2};

// Enemy struct
typedef struct {
    int x, y;
    int hp;
    int attack;
    int alive;
} Enemy;

Enemy enemies[ENEMY_COUNT];

// Loot struct
typedef struct {
    int x, y;
    ItemType type;
    int exists;
} Loot;

Loot loot[LOOT_COUNT];

// Dungeon map (1 = wall, 0 = floor)
int map[MAP_HEIGHT][MAP_WIDTH];

// SDL Variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// Check if position is occupied by an enemy
int isEnemyAt(int x, int y) {
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (enemies[i].alive && enemies[i].x == x && enemies[i].y == y) {
            return 1;
        }
    }
    return 0;
}

// Initialize SDL with error handling
void initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("[ERROR] SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow("SDL2 Roguelike", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("[ERROR] Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("[ERROR] Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
}

// Close SDL safely
void closeSDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Generate procedural dungeon
void generateDungeon() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = (rand() % 5 == 0) ? 1 : 0; // 20% chance to be a wall
        }
    }
    map[1][1] = 0; // Ensure player spawn area is open
}

// Spawn loot in random locations
void generateLoot() {
    for (int i = 0; i < LOOT_COUNT; i++) {
        do {
            loot[i].x = rand() % (MAP_WIDTH - 2) + 1;
            loot[i].y = rand() % (MAP_HEIGHT - 2) + 1;
        } while (map[loot[i].y][loot[i].x] == 1); // Ensure it's not in a wall
        loot[i].type = (rand() % 2) ? SWORD : POTION;
        loot[i].exists = 1;
    }
}

// Render game
void renderGame() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            SDL_SetRenderDrawColor(renderer, map[y][x] == 1 ? 100 : 50, 100, 100, 255);
            SDL_Rect tile = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            SDL_RenderFillRect(renderer, &tile);
        }
    }

    for (int i = 0; i < LOOT_COUNT; i++) {
        if (loot[i].exists) {
            SDL_SetRenderDrawColor(renderer, loot[i].type == SWORD ? 255 : 0, 255, 0, 255);
            SDL_Rect item = {loot[i].x * TILE_SIZE, loot[i].y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            SDL_RenderFillRect(renderer, &item);
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (enemies[i].alive) {
            SDL_Rect enemy = {enemies[i].x * TILE_SIZE, enemies[i].y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            SDL_RenderFillRect(renderer, &enemy);
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect playerRect = {player.x * TILE_SIZE, player.y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
    SDL_RenderFillRect(renderer, &playerRect);

    SDL_RenderPresent(renderer);
}

// Move player with collision detection
void movePlayer(int dx, int dy) {
    int newX = player.x + dx;
    int newY = player.y + dy;

    if (map[newY][newX] == 0 && !isEnemyAt(newX, newY)) { // Check for walls and enemies
        player.x = newX;
        player.y = newY;
    }
}

// Enemy AI - Chase the player
void moveEnemies() {
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (!enemies[i].alive) continue;

        int dx = (player.x > enemies[i].x) ? 1 : (player.x < enemies[i].x) ? -1 : 0;
        int dy = (player.y > enemies[i].y) ? 1 : (player.y < enemies[i].y) ? -1 : 0;

        int newX = enemies[i].x + dx;
        int newY = enemies[i].y + dy;

        if (map[newY][newX] == 0 && !isEnemyAt(newX, newY) && !(newX == player.x && newY == player.y)) {
            enemies[i].x = newX;
            enemies[i].y = newY;
        }

        // Attack player if adjacent
        if (abs(player.x - enemies[i].x) <= 1 && abs(player.y - enemies[i].y) <= 1) {
            player.hp -= enemies[i].attack;
            if (player.hp <= 0) {
                printf("Game Over!\n");
                closeSDL();
                exit(0);
            }
        }
    }
}

// Main function
int main() {
    srand(time(NULL));
    initSDL();
    generateDungeon();
    generateLoot();

    for (int i = 0; i < ENEMY_COUNT; i++) {
        do {
            enemies[i].x = rand() % (MAP_WIDTH - 2) + 1;
            enemies[i].y = rand() % (MAP_HEIGHT - 2) + 1;
        } while (map[enemies[i].y][enemies[i].x] == 1); // Ensure enemy isn't in a wall
        enemies[i].hp = 5;
        enemies[i].attack = 1;
        enemies[i].alive = 1;
    }

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: movePlayer(0, -1); break;
                    case SDLK_DOWN: movePlayer(0, 1); break;
                    case SDLK_LEFT: movePlayer(-1, 0); break;
                    case SDLK_RIGHT: movePlayer(1, 0); break;
                }
            }
        }
        moveEnemies();
        renderGame();
        SDL_Delay(200);
    }

    closeSDL();
    return 0;
}
