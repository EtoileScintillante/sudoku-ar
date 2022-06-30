/*                                  
 * sudoku-processing.h
 *
 * Created on: 30 June 2022
 * Author: EtoileScintillante
 */

#ifndef __SUDOKU_PROCESSING__
#define __SUDOKU_PROCESSING__

#include <opencv2/opencv.hpp>
using namespace cv;

/**
 * Detects a sudoku grid on an image.
 * 
 * @param mat image with sudoku grid
 * @return vector of points representing the 4 corners of the sudoku grid.
 */
std::vector<Point> detectGrid(Mat image);

/**
 * Crops image containing a sudoku grid, so that only the grid is left over.
 * 
 * @param corners vector of points representing the 4 corners of the sudoku grid.
 * @param sourceIMG original image containing the sudoku grid
 * @return cropped image of sudoku grid.
 */
Mat cropGrid(std::vector<Point> corners, Mat sourceIMG);

/**
 * Sorts a vector of points from top left to bottom right.
 * 
 * @param corners vector of points representing the 4 corners of the sudoku grid.
 * @return sorted vector of points.
 */
std::vector<Point> sortPoints4(std::vector<Point> corners);

/**
 * Creates a binary image of the sudoku grid with the digits filtered out.
 * 
 * @param image image containing only a sudoku grid.
 * @return binary image of the sudoku grid with the digits filtered out.
 */
Mat filterOutDigits(Mat image);

/**
 * Creates a binary image of the sudoku grid with the digits filtered out.
 * 
 * @param image binary image of the sudoku grid with the digits filtered out (must be 9x9 grid).
 * @param src image with sudoku grid on which the contours of the cells wll be drawn 
 * @return 2d vector of points containing the contours of the cells.
 */
std::vector< std::vector < Point > > findCells(Mat image, Mat src);

/**
 * Check if point a comes before point b (x coordinate of a < x coordinate of b).
 *
 * @param a vector of points
 * @param b vector of points
 * @see sortCells
 * @return true if point a comes before point b, false otherwhise
 */
bool sortCellsLeftToRight(std::vector<Point> a, std::vector<Point> b);

/**
 * Check if point a comes before point b (x coordinate of a < x coordinate of b).
 *
 * @param cells 2d vector of contours of the cells of a sudoku grid (must be 81 cells)
 * @see sortCellsLeftToRight
 * @return sorted 2d vector of contours of the cells of a sudoku grid
 */
std::vector< std::vector < Point > > sortCells(std::vector< std::vector < Point > > cells);

/**
 * Converts an image of a sudoku grid to a 2d vector if integers.
 *
 * @param src cropped image of sudoku grid (must be 9x9 grid)
 * @param sortedCells sorted 2d vector of contours of the cells of a sudoku grid
 * @param knn trained K-Nearest Neighbor model to recognize the digits on the image of the sudoku grid 
 * @see cropGrid
 * @see sortCells
 * @return 2d vector of integers representing the sudoku grid (empty cell = 0)
 */
std::vector< std::vector< int > > ImageToVec(Mat src, std::vector< std::vector < Point > > sortedCells, Ptr<ml::KNearest> knn);

/**
 * Creates a mask for the sudoku grid (containing only the digits of cells that were originally empty).
 *
 * @param contourCells sorted 2d vector of contours of the cells of a sudoku grid
 * @param solution 2d vector of integers representing the solved sudoku grid
 * @param original 2d vector of integers representing the original/unsolved sudoku grid
 * @return mask image (containing only the digits of cells that were originally empty)
 */
Mat createMask(std::vector< std::vector < Point > > contoursCells, std::vector< std::vector< int > > solution, std::vector< std::vector< int > > original);

/**
 * Overlays the mask on the source image to create an augmented reality effect.
 *
 * @param corners sorted vector of points representing the 4 corners of the sudoku grid.
 * @param source image on which we want to overlay the mask
 * @param mask image representing the mask 
 * @return image where the empty cells of the suoku grid contain green digits because of the mask we put over it
 */
Mat showSolution(std::vector< Point > corners, Mat source, Mat mask);

#endif /*__SUDOKU_PROCESSING__*/