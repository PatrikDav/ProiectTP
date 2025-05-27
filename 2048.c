#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 700
#define GRID_PADDING 15
#define TILE_CORNER_RADIUS 10
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    int value;
    int x;
    int y;
    int targetX;
    int targetY;
    bool merged;
    bool newTile;
} Tile;

typedef enum {
    MENU,
    GRID_SIZE_SELECTION,
    GAME,
    GAME_OVER
} GameState;

int gridSize = 4;
Tile **grid = NULL;
int score = 0;
bool moved = false;
bool gameOver = false;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;
TTF_Font *bigFont = NULL;


SDL_Color bgColor = {187, 173, 160, 255};
SDL_Color textColor = {119, 110, 101, 255};
SDL_Color textColorLight = {249, 246, 242, 255};
SDL_Color tileColors[] = {
    {205, 193, 180, 255}, // 0
    {238, 228, 218, 255}, // 2
    {237, 224, 200, 255}, // 4
    {242, 177, 121, 255}, // 8
    {245, 149, 99, 255},  // 16
    {246, 124, 95, 255},  // 32
    {246, 94, 59, 255},   // 64
    {237, 207, 114, 255}, // 128
    {237, 204, 97, 255},  // 256
    {237, 200, 80, 255},  // 512
    {237, 197, 63, 255},  // 1024
    {237, 194, 46, 255},  // 2048
    {60, 58, 50, 255}     // >2048
};

void initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        exit(1);
    }

    window = SDL_CreateWindow("2048 Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                             WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 36);
    bigFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 48);

    if (!font || !bigFont) {
        fprintf(stderr, "Failed to load fallback font. Error: %s\n", TTF_GetError());
        // Clean up and exit
        TTF_Quit();
        SDL_Quit();
    }
}

void closeSDL() {
    TTF_CloseFont(font);
    TTF_CloseFont(bigFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

void freeGame() {
    if (grid != NULL) {
        for (int i = 0; i < gridSize; i++) {
            if (grid[i] != NULL) {
                free(grid[i]);
            }
        }
        free(grid);
        grid = NULL;
    }
}

void spawnTile() {
    int emptyCells[gridSize * gridSize][2];
    int emptyCount = 0;

    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            if (grid[y][x].value == 0) {
                emptyCells[emptyCount][0] = y;
                emptyCells[emptyCount][1] = x;
                emptyCount++;
            }
        }
    }

    if (emptyCount > 0) {
        int tilesToSpawn = (gridSize == 4) ? 2 : (gridSize == 5) ? 3 : 4;
        tilesToSpawn = (tilesToSpawn > emptyCount) ? emptyCount : tilesToSpawn;
        
        for (int i = 0; i < tilesToSpawn; i++) {
            int index = rand() % emptyCount;
            int y = emptyCells[index][0];
            int x = emptyCells[index][1];
            grid[y][x].value = (rand() % 10 < 9) ? 2 : 4;
            grid[y][x].newTile = true;
            
            emptyCells[index][0] = emptyCells[emptyCount-1][0];
            emptyCells[index][1] = emptyCells[emptyCount-1][1];
            emptyCount--;
        }
    }
}

void initGame() {
    freeGame();
    
    score = 0;
    gameOver = false;
    moved = false;

    grid = (Tile **)malloc(gridSize * sizeof(Tile *));
    for (int i = 0; i < gridSize; i++) {
        grid[i] = (Tile *)malloc(gridSize * sizeof(Tile));
        for (int j = 0; j < gridSize; j++) {
            grid[i][j].value = 0;
            grid[i][j].x = j;
            grid[i][j].y = i;
            grid[i][j].targetX = j;
            grid[i][j].targetY = i;
            grid[i][j].merged = false;
            grid[i][j].newTile = false;
        }
    }
    spawnTile();
}

bool isGameOver() {
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            if (grid[y][x].value == 0) {
                return false;
            }
        }
    }

    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            int value = grid[y][x].value;
            if ((x < gridSize - 1 && value == grid[y][x + 1].value) ||
                (y < gridSize - 1 && value == grid[y + 1][x].value)) {
                return false;
            }
        }
    }

    return true;
}

void moveTiles(int dx, int dy) {
    moved = false;
    
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            grid[y][x].merged = false;
            grid[y][x].newTile = false;
        }
    }

    if (dx == -1) { //Stanga
        for (int y = 0; y < gridSize; y++) {
            for (int x = 1; x < gridSize; x++) {
                if (grid[y][x].value != 0) {
                    int newX = x;
                    while (newX > 0 && grid[y][newX - 1].value == 0) {
                        newX--;
                    }
                    
                    if (newX > 0 && grid[y][newX - 1].value == grid[y][x].value && !grid[y][newX - 1].merged) {
                        grid[y][newX - 1].value *= 2;
                        score += grid[y][newX - 1].value;
                        grid[y][x].value = 0;
                        grid[y][newX - 1].merged = true;
                        moved = true;
                    } else if (newX != x) {
                        grid[y][newX].value = grid[y][x].value;
                        grid[y][x].value = 0;
                        moved = true;
                    }
                }
            }
        }
    } 
    else if (dx == 1) { //Dreapta
        for (int y = 0; y < gridSize; y++) {
            for (int x = gridSize - 2; x >= 0; x--) {
                if (grid[y][x].value != 0) {
                    int newX = x;
                    while (newX < gridSize - 1 && grid[y][newX + 1].value == 0) {
                        newX++;
                    }
                    
                    if (newX < gridSize - 1 && grid[y][newX + 1].value == grid[y][x].value && !grid[y][newX + 1].merged) {
                        grid[y][newX + 1].value *= 2;
                        score += grid[y][newX + 1].value;
                        grid[y][x].value = 0;
                        grid[y][newX + 1].merged = true;
                        moved = true;
                    } else if (newX != x) {
                        grid[y][newX].value = grid[y][x].value;
                        grid[y][x].value = 0;
                        moved = true;
                    }
                }
            }
        }
    } 
    else if (dy == -1) { //SUS
        for (int x = 0; x < gridSize; x++) {
            for (int y = 1; y < gridSize; y++) {
                if (grid[y][x].value != 0) {
                    int newY = y;
                    while (newY > 0 && grid[newY - 1][x].value == 0) {
                        newY--;
                    }
                    
                    if (newY > 0 && grid[newY - 1][x].value == grid[y][x].value && !grid[newY - 1][x].merged) {
                        grid[newY - 1][x].value *= 2;
                        score += grid[newY - 1][x].value;
                        grid[y][x].value = 0;
                        grid[newY - 1][x].merged = true;
                        moved = true;
                    } else if (newY != y) {
                        grid[newY][x].value = grid[y][x].value;
                        grid[y][x].value = 0;
                        moved = true;
                    }
                }
            }
        }
    } 
    else if (dy == 1) { // JOS
        for (int x = 0; x < gridSize; x++) {
            for (int y = gridSize - 2; y >= 0; y--) {
                if (grid[y][x].value != 0) {
                    int newY = y;
                    while (newY < gridSize - 1 && grid[newY + 1][x].value == 0) {
                        newY++;
                    }
                    
                    if (newY < gridSize - 1 && grid[newY + 1][x].value == grid[y][x].value && !grid[newY + 1][x].merged) {
                        grid[newY + 1][x].value *= 2;
                        score += grid[newY + 1][x].value;
                        grid[y][x].value = 0;
                        grid[newY + 1][x].merged = true;
                        moved = true;
                    } else if (newY != y) {
                        grid[newY][x].value = grid[y][x].value;
                        grid[y][x].value = 0;
                        moved = true;
                    }
                }
            }
        }
    }

    if (moved) {
        spawnTile();
        gameOver = isGameOver();
    }
}

void drawRoundedRect(int x, int y, int w, int h, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    SDL_Rect rects[] = {
        {x + radius, y, w - 2 * radius, h},
        {x, y + radius, radius, h - 2 * radius},
        {x + w - radius, y + radius, radius, h - 2 * radius}
    };
    SDL_RenderFillRects(renderer, rects, 3);
    
    for (int i = 0; i < 360; i++) {
        float angle = i * M_PI / 180.0f;
        int dx = radius * cosf(angle);
        int dy = radius * sinf(angle);
        
        SDL_RenderDrawPoint(renderer, x + radius + dx, y + radius + dy);
        // Top-right corner
        SDL_RenderDrawPoint(renderer, x + w - radius + dx, y + radius + dy);
        // Bottom-left corner
        SDL_RenderDrawPoint(renderer, x + radius + dx, y + h - radius + dy);
        // Bottom-right corner
        SDL_RenderDrawPoint(renderer, x + w - radius + dx, y + h - radius + dy);
    }
}

void renderGame() {
    if (grid == NULL) return;  // Safety check)
    // Clear screen
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderClear(renderer);

    // Calculate tile size based on window size and grid size
    int tileSize = (WINDOW_WIDTH - (gridSize + 1) * GRID_PADDING) / gridSize;
    
    // Draw score
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    SDL_Color scoreColor = {119, 110, 101, 255};
    SDL_Surface* scoreSurface = TTF_RenderText_Blended(font, scoreText, scoreColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = {GRID_PADDING, GRID_PADDING, scoreSurface->w, scoreSurface->h};
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    // Draw grid background
    SDL_Color gridColor = {187, 173, 160, 255};
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            int posX = GRID_PADDING + x * (tileSize + GRID_PADDING);
            int posY = GRID_PADDING * 2 + scoreRect.h + y * (tileSize + GRID_PADDING);
            drawRoundedRect(posX, posY, tileSize, tileSize, TILE_CORNER_RADIUS, gridColor);
        }
    }

    // Draw tiles
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            if (grid[y][x].value > 0) {
                int posX = GRID_PADDING + x * (tileSize + GRID_PADDING);
                int posY = GRID_PADDING * 2 + scoreRect.h + y * (tileSize + GRID_PADDING);
                
                // Determine tile color based on value
                int colorIndex = 0;
                int value = grid[y][x].value;
                if (value >= 2048) colorIndex = 12;
                else if (value >= 1024) colorIndex = 11;
                else if (value >= 512) colorIndex = 10;
                else if (value >= 256) colorIndex = 9;
                else if (value >= 128) colorIndex = 8;
                else if (value >= 64) colorIndex = 7;
                else if (value >= 32) colorIndex = 6;
                else if (value >= 16) colorIndex = 5;
                else if (value >= 8) colorIndex = 4;
                else if (value >= 4) colorIndex = 3;
                else if (value >= 2) colorIndex = 2;
                
                // Draw tile with animation if needed
                drawRoundedRect(posX, posY, tileSize, tileSize, TILE_CORNER_RADIUS, tileColors[colorIndex]);
                
                // Draw tile value
                char valueText[10];
                sprintf(valueText, "%d", grid[y][x].value);
                
                SDL_Color textClr = (value <= 4) ? textColor : textColorLight;
                TTF_Font* useFont = (value < 100) ? bigFont : font;
                
                SDL_Surface* textSurface = TTF_RenderText_Blended(useFont, valueText, textClr);
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                
                SDL_Rect textRect = {
                    posX + (tileSize - textSurface->w) / 2,
                    posY + (tileSize - textSurface->h) / 2,
                    textSurface->w,
                    textSurface->h
                };
                
                SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);
            }
        }
    }

    // Draw game over message if needed
    if (gameOver) {
        SDL_Color gameOverColor = {119, 110, 101, 220};
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, 200);
        SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(renderer, &overlay);
        
        SDL_Surface* gameOverSurface = TTF_RenderText_Blended(bigFont, "Game Over!", gameOverColor);
        SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
        SDL_Rect gameOverRect = {
            (WINDOW_WIDTH - gameOverSurface->w) / 2,
            (WINDOW_HEIGHT - gameOverSurface->h) / 2,
            gameOverSurface->w,
            gameOverSurface->h
        };
        SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
        SDL_FreeSurface(gameOverSurface);
        SDL_DestroyTexture(gameOverTexture);
    }

    SDL_RenderPresent(renderer);
}

void renderButton(int x, int y, int w, int h, const char* text, bool hovered) {
    SDL_Color btnColor = hovered ? (SDL_Color){143, 122, 102, 255} : (SDL_Color){187, 173, 160, 255};
    SDL_Color textClr = {249, 246, 242, 255};
    
    drawRoundedRect(x, y, w, h, TILE_CORNER_RADIUS, btnColor);
    
    SDL_Surface* textSurface = TTF_RenderText_Blended(bigFont, text, textClr);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    
    SDL_Rect textRect = {
        x + (w - textSurface->w) / 2,
        y + (h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };
    
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void renderMenu() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderClear(renderer);

    // Title
    SDL_Color titleColor = {119, 110, 101, 255};
    SDL_Surface* titleSurface = TTF_RenderText_Blended(bigFont, "2048", titleColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {
        (WINDOW_WIDTH - titleSurface->w) / 2,
        WINDOW_HEIGHT / 4,
        titleSurface->w,
        titleSurface->h
    };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Start button
    int btnWidth = 200;
    int btnHeight = 80;
    renderButton((WINDOW_WIDTH - btnWidth)/2, WINDOW_HEIGHT/2, btnWidth, btnHeight, "Start", false);
    
    SDL_RenderPresent(renderer);
}

void renderGridSizeSelection() {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderClear(renderer);

    // Title
    SDL_Color titleColor = {119, 110, 101, 255};
    SDL_Surface* titleSurface = TTF_RenderText_Blended(bigFont, "Select Grid Size", titleColor);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {
        (WINDOW_WIDTH - titleSurface->w) / 2,
        WINDOW_HEIGHT / 4,
        titleSurface->w,
        titleSurface->h
    };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Grid size buttons
    int btnWidth = 120;
    int btnHeight = 80;
    int padding = 20;
    
    renderButton((WINDOW_WIDTH - (3*btnWidth + 2*padding))/2, 
                WINDOW_HEIGHT/2, btnWidth, btnHeight, "4x4", false);
    renderButton((WINDOW_WIDTH - (3*btnWidth + 2*padding))/2 + btnWidth + padding, 
                WINDOW_HEIGHT/2, btnWidth, btnHeight, "5x5", false);
    renderButton((WINDOW_WIDTH - (3*btnWidth + 2*padding))/2 + 2*(btnWidth + padding), 
                WINDOW_HEIGHT/2, btnWidth, btnHeight, "6x6", false);
    
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    initSDL();
    
    GameState state = MENU;
    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                
                if (state == MENU) {
                    // Check if start button was clicked
                    int btnWidth = 200;
                    int btnHeight = 80;
                    SDL_Rect startBtn = {
                        (WINDOW_WIDTH - btnWidth)/2,
                        WINDOW_HEIGHT/2,
                        btnWidth,
                        btnHeight
                    };
                    
                    if (mouseX >= startBtn.x && mouseX <= startBtn.x + startBtn.w &&
                        mouseY >= startBtn.y && mouseY <= startBtn.y + startBtn.h) {
                        state = GRID_SIZE_SELECTION;
                    }
                } 
                else if (state == GRID_SIZE_SELECTION) {
                    // Check grid size buttons
                    int btnWidth = 120;
                    int btnHeight = 80;
                    int padding = 20;
                    int startX = (WINDOW_WIDTH - (3*btnWidth + 2*padding))/2;
                    
                    // 4x4 button
                    SDL_Rect btn4x4 = {startX, WINDOW_HEIGHT/2, btnWidth, btnHeight};
                    if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &btn4x4)) {
                        gridSize = 4;
                        initGame();
                        state = GAME;
                    }
                    
                    // 5x5 button
                    SDL_Rect btn5x5 = {startX + btnWidth + padding, WINDOW_HEIGHT/2, btnWidth, btnHeight};
                    if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &btn5x5)) {
                        gridSize = 5;
                        initGame();
                        state = GAME;
                    }
                    
                    // 6x6 button
                    SDL_Rect btn6x6 = {startX + 2*(btnWidth + padding), WINDOW_HEIGHT/2, btnWidth, btnHeight};
                    if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &btn6x6)) {
                        gridSize = 6;
                        initGame();
                        state = GAME;
                    }
                }
            } 
            else if (e.type == SDL_KEYDOWN && state == GAME && !gameOver) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP: moveTiles(0, -1); break;
                    case SDLK_DOWN: moveTiles(0, 1); break;
                    case SDLK_LEFT: moveTiles(-1, 0); break;
                    case SDLK_RIGHT: moveTiles(1, 0); break;
                    case SDLK_r: 
                        freeGame(); 
                        score = 0;
                        initGame(); 
                        break;
                    case SDLK_ESCAPE: 
                        if (state == GAME || state == GAME_OVER) {
                            freeGame();
                            state = MENU;
                        }
                        break;
                }
            }
        }

        // Render based on current state
        switch (state) {
            case MENU:
                renderMenu();
                break;
            case GRID_SIZE_SELECTION:
                renderGridSizeSelection();
                break;
            case GAME:
                renderGame();
                break;
            case GAME_OVER:
                renderGame(); // Game over is handled within renderGame()
                break;
        }

        SDL_Delay(16);
    }

    freeGame();
    closeSDL();
    return 0;
}