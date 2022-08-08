/*                                  
 * sudoku-solver.h
 *
 * Created on: 29 June 2022
 * Author: EtoileScintillante
 */

#ifndef __SUDOKU_FUNCTIONS__
#define __SUDOKU_FUNCTIONS__

#include <vector>
#include <iostream>

/**
 * @brief Prints the sudoku grid.
 *
 * @param grid 2d vector representing the sudoku grid
 */
void printSudoku(std::vector<std::vector< int > > grid);

/**
 * @brief Checks whether it is safe to place a digit in a certain cell.
 * 
 * @param grid 2d vector containing the sudoku grid
 * @param row row number
 * @param col column number
 * @param num inserted digit
 * @return true if cell is a safe place, false otherwhise
 */
bool checkPlace(std::vector<std::vector< int > > grid, int row, int col, int num);

/**
 * @brief Solves the sudoku puzzle using backtracking algorithm.
 * 
 * @param grid 2d vector containing the sudoku grid
 * @param row row number
 * @param col column number
 * @param num digit to insert
 * @param[out] solvedGrid 2d vector containing the solved sudoku
 * @return true if solution to sudoku puzzle is found, false otherwhise
 */
bool solveSudoku(std::vector<std::vector< int > > grid, int row, int col, std::vector<std::vector< int > > &solvedGrid);

#endif /*__SUDOKU_FUNCTIONS__*/