#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int gridSize;
int **grid;

void generateGrid() {
    printf("\n2048 Game (%dx%d)\n", gridSize, gridSize);
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (grid[i][j] == 0)
                printf("|    ");
            else
                printf("|%3d ", grid[i][j]);
        }
        printf("|\n");
        for (int k = 0; k < gridSize; k++)
            printf("-----");
        printf("-\n");
    }
}

void spawn_tile() {
    int emptyCells[36][2];
    int emptyCount = 0;

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
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

void play_game() {
    return;
}

void init_game() {
    srand(time(NULL));

    printf("Choose grid size (4: Hard, 5: Medium, 6: Hard): ");
    scanf("%d", &gridSize);
    if (gridSize < 4 || gridSize > 6) {
        printf("Invalid choice, defaulting to 4x4.\n");
        gridSize = 4;
    }

    grid = (int **)malloc(gridSize * sizeof(int *));
    for (int i = 0; i < gridSize; i++) {
        grid[i] = (int *)calloc(gridSize, sizeof(int));
    }

    int tilesToSpawn = (gridSize == 4) ? 2 : (gridSize == 5) ? 3 : 4;
    for (int i = 0; i < tilesToSpawn; i++) {
        spawn_tile();
    }

    generateGrid();
}

void cleanup_game() {
    for (int i = 0; i < gridSize; i++) {
        free(grid[i]);
    }
    free(grid);
}

int main() {
    init_game();
    play_game();
    cleanup_game();
    return 0;
}
