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
 * Prints the sudoku grid.
 *
 * @param grid 2d vector containing the sudoku grid.
 */
void print_sudoku(std::vector<std::vector< int > > grid);

/**
 * Checks whether it is safe to place a digit in a certain cell.
 * 
 * @param grid 2d vector containing the sudoku grid
 * @param row row number
 * @param col column number
 * @param num inserted digit
 * @return true if cell is a safe place, false otherwhise.
 */
bool checkPlace(std::vector<std::vector< int > > grid, int row, int col, int num);

/**
 * Solves the sudoku puzzle using backtracking algorithm.
 * 
 * @param grid 2d vector containing the sudoku grid
 * @param row row number
 * @param col column number
 * @param num digit we want to insert
 * @return true if solution to sudoku puzzle is found, false otherwhise.
 */
bool solve_sudoku(std::vector<std::vector< int > > grid, int row, int col, std::vector<std::vector< int > > &solvedGrid);

#endif /*__SUDOKU_FUNCTIONS__*/