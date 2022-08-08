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
 * @brief Detects a sudoku grid on an image.
 * 
 * @param mat image containing a sudoku grid
 * @return vector of points representing the 4 corners of the sudoku grid
 */
std::vector<Point> detectGrid(Mat image);

/**
 * @brief Crops image containing a sudoku grid, so that only the grid is left over.
 * 
 * @param corners vector of points representing the 4 corners of the sudoku grid
 * @param sourceIMG original image containing the sudoku grid
 * @return cropped image of sudoku grid
 */
Mat cropGrid(std::vector<Point> corners, Mat sourceIMG);

/**
 * @brief Sorts a vector of four points from top left to bottom right.
 * 
 * @param corners vector of points representing the 4 corners of a square
 * @return sorted vector of points
 */
std::vector<Point> sortSquarePoints(std::vector<Point> corners);

/**
 * @brief Creates a binary image of the sudoku grid with the digits filtered out,
 * where the lines of the grid are white and the cells are black.
 * 
 * @param image image containing only a sudoku grid
 * @return binary image of the sudoku grid with the digits filtered out
 */
Mat filterOutDigits(Mat image);

/**
 * @brief Finds the contours of the cells of the sudoku grid.
 * This functions also draws the contours of the cells, in case you want to see those.
 * 
 * @param image binary image of the sudoku grid with the digits filtered out
 * @param src image with sudoku grid on which the contours of the cells will be drawn 
 * @return 2d vector of points containing the contours of the cells
 */
std::vector< std::vector < Point > > findCells(Mat image, Mat src);

/**
 * @brief Checks if contour a comes before contour b.
 * It does so by adding a bounding rectangle to both contours.
 * Contour a comes before b if the x coordinate of rectangle a is smaller than the x of rectangle b. 
 *
 * @param a vector of points
 * @param b vector of points
 * @see sortCells
 * @return true if point a comes before point b, false otherwhise
 */
bool sortContours(std::vector<Point> a, std::vector<Point> b);

/**
 * @brief Sorts cells of a sudoku grid from top left to bottom right.
 *
 * @param cells 2d vector of contours of the cells of a sudoku grid (must be 81 cells)
 * @see sortContours
 * @return sorted 2d vector of contours of the cells
 */
std::vector< std::vector < Point > > sortCells(std::vector< std::vector < Point > > cells);

/**
 * @brief Converts an image of a sudoku grid to a 2d vector of integers. 
 * Detection of digits is achieved by using a trained KNN model.
 * Empty cells will be represented by a zero.
 *
 * @param src cropped image of sudoku grid (must be 9x9 grid)
 * @param sortedCells sorted 2d vector of contours of the cells of a sudoku grid
 * @param knn trained K-Nearest Neighbor model to recognize digits 
 * @see cropGrid
 * @see sortCells
 * @return 2d vector of integers representing the sudoku grid
 */
std::vector< std::vector< int > > gridToVector(Mat src, std::vector< std::vector < Point > > sortedCells, Ptr<ml::KNearest> knn);

/**
 * @brief Creates a mask for the sudoku grid. 
 * This mask contains only the digits of cells that were empty in the unsolved sudoku, 
 * since the goal of this mask is to fill in the empty cells of an unsolved sudoku image with digits
 * and creating an image of a solved sudoku. 
 *
 * @param contourCells sorted 2d vector of contours of the cells of a sudoku grid
 * @param solution 2d vector of integers representing the solved sudoku grid
 * @param original 2d vector of integers representing the original/unsolved sudoku grid
 * @return mask image 
 */
Mat createMask(std::vector< std::vector < Point > > contoursCells, std::vector< std::vector< int > > solution, std::vector< std::vector< int > > original);

/**
 * @brief Applies the mask to the source image to create an augmented reality effect.
 *
 * @param corners sorted vector of points representing the 4 corners of the sudoku grid
 * @param source image to which the mask should be applied
 * @param mask image mask 
 * @see createMask
 * @return image source and mask blended together, creating an image of a solved sudoku grid
 */
Mat showSolution(std::vector< Point > corners, Mat source, Mat mask);

#endif /*__SUDOKU_PROCESSING__*/