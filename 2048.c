#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

int gridSize;
int **grid;

void generateGrid() {
    clear();
    printw("\n2048 Game (%dx%d)\n", gridSize, gridSize);
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (grid[i][j] == 0)
                printw("|    ");
            else
                printw("|%3d ", grid[i][j]);
        }
        printw("|\n");
        for (int k = 0; k < gridSize; k++)
            printw("-----");
        printw("-\n");
    }
    refresh();
}

void spawn_tile() {
    int emptyCells[36][2]; // Maximum for 6x6 grid
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
    printw("Press any key to exit...\n");
    refresh();
    getch();
}

void init_game() {
    initscr();
    noecho();
    curs_set(FALSE);
    srand(time(NULL));

    printw("Choose grid size (4: Hard, 5: Medium, 6: Hard): ");
    refresh();
    scanw("%d", &gridSize);
    if (gridSize < 4 || gridSize > 6) {
        printw("Invalid choice, defaulting to 4x4.\n");
        refresh();
        gridSize = 4;
    }

    // Allocate memory for grid
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
    endwin();
}

int main() {
    init_game();
    play_game();
    cleanup_game();
    return 0;
}
