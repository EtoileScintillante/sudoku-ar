#include "sudoku-solver.h"

void printSudoku(std::vector<std::vector< int > > grid)
{
    printf("\n");
    int i, j;
    for(i = 0; i < 9; i++)
    {
        for(j = 0; j < 9; j++)
        {
            printf("%i ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

bool checkPlace(std::vector<std::vector< int > > grid, int row, int col, int num)
{
    for (int i = 0; i < 9; i++)
    {
        if (grid[row][i] == num) return false;
    }

    for (int i = 0; i < 9; i++)
    {
        if (grid[i][col] == num) return false;
    }

    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (grid[i + startRow][j + startCol] == num) return false;
        }
    }

    return true; // if the place is safe
}

bool solveSudoku(std::vector<std::vector< int > > grid, int row, int col, std::vector<std::vector< int > > &solvedGrid)
{
    // Return true and print sudoku if we managed to place a num in every empty cell
    if (row == 9 - 1 && col == 9)
    {
        solvedGrid = grid; 
        return true;
    }
    // Move to next row
    if (col == 9)
    {
        row += 1;
        col = 0;
    }
    // If cell is not empty, move to next one
    if (grid[row][col] > 0) 
    {
        return solveSudoku(grid, row, col + 1, solvedGrid);
    }
    // Try to insert a number
    for (int num = 1; num < 9 + 1; num++)
    {
        if (checkPlace(grid, row, col, num))
        {
            grid[row][col] = num; // insert num if place is safe
            if (solveSudoku(grid, row, col + 1, solvedGrid))
            {
                return true;
            }
        }
        grid[row][col] = 0; // when place is not safe, cell remains empty
    }

    return false; // No solution found
}

