#ifndef __SUDOKU_FUNCTIONS__
#define __SUDOKU_FUNCTIONS__

#include <vector>
#include <iostream>

void print_sudoku(std::vector<std::vector< int > > s);

bool checkPlace(std::vector<std::vector< int > > s, int row, int col, int num);

bool solve(std::vector<std::vector< int > > s, int row, int col, std::vector<std::vector< int > > &solvedGrid);

#endif /*__SUDOKU_FUNCTIONS__*/