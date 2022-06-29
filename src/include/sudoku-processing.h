#ifndef __SUDOKU_PROCESSING__
#define __SUDOKU_PROCESSING__

#include <opencv2/opencv.hpp>
using namespace cv;

// Detect the sudoku grid
// Returns vector of points of the grid
std::vector<Point> detectGrid(Mat image);

// This function takes as input the 4 points corresponding to 4 corners of the grid
// and it creates a cropped image containing only the grid, top down view
Mat cropGrid(std::vector<Point> corners, Mat sourceIMG);

// Sort points from top left (tl), top right (tr), bottom left (bl), bottom right (br)
// Assumption: only 4 points in vector
std::vector<Point> sortPoints4(std::vector<Point> corners);

// Returns a binary image with the lines of the grid in white and 
// and everything within the lines is black
// This step is necessary for finding the contours of the cells
Mat filterOutDigits(Mat image);

// Detect the contours of the cell and return a vector containing those contours
// Assumption: input image is image obtained through the function filterOutDigits
std::vector< std::vector < Point > > findCells(Mat image, Mat src);

// Sort contours from left to right (used in sortCells function)
bool sortCellsLeftToRight(std::vector<Point> a, std::vector<Point> b);

// Sort the contours of the cells from top left to bottom right
std::vector< std::vector < Point > > sortCells(std::vector< std::vector < Point > > cells);

// Recognize digits with the KNN model and return a 2d vector of ints
// Input image must be image obtained from cropGrid function
// Empty cell = 0
std::vector< std::vector< int > > ImageToVec(Mat src, std::vector< std::vector < Point > > sortedCells, Ptr<ml::KNearest> knn);

// This function creates a mask which will overlay the sudoku grid in the source image (image captured by webcam/camera)
// The vector contoursCells contains all the 81 contours of the cells, the vector solution contains the grid of the solved sudoku
// And the vector original contains the grid of the original sudoku (with empty cells)
Mat createMask(std::vector< std::vector < Point > > contoursCells, std::vector< std::vector< int > > solution, std::vector< std::vector< int > > original);

// This functions overlays the mask (obtained from createMask) on the source image
// To do that it needs the corner coordinates of the grid on the source image
// which is why we need to pass in vector containing these corners
Mat showSolution(std::vector< Point > corners, Mat source, Mat mask);

#endif /*__SUDOKU_PROCESSING__*/