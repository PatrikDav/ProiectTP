#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int grid[4][4] = {0}; // 4X4

void generateGrid() {
    printf("\n2048 Game\n");
    printf("-----------------------------\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (grid[i][j] == 0)
                printf("|    ");
            else
                printf("|%3d ", grid[i][j]);
        }
        printf("|\n-----------------------------\n");
    }
}

void spawn_tile() {
    int emptyCells[16][2];
    int emptyCount = 0;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (grid[i][j] == 0) {
                emptyCells[emptyCount][0] = i;
                emptyCells[emptyCount][1] = j;
                emptyCount++;
            }
        }
    }

    if (emptyCount == 0) return;

    int randIndex = rand() % emptyCount;
    int x = emptyCells[randIndex][0];
    int y = emptyCells[randIndex][1];

    grid[x][y] = (rand() % 10 < 9) ? 2 : 4;
}

void play_game(){
    return;
}

void init_game() {
    srand(time(NULL));
    
    spawn_tile();
    spawn_tile();

    generateGrid();
}

int main() {
    init_game();
    play_game();
    return 0;
}
