#include "sudoku-processing.h"

// Detect the sudoku grid
// Returns vector of points of the grid
std::vector<Point> detectGrid(Mat image)
{
    Mat gray, thresh;
    cvtColor(image, gray, COLOR_BGR2GRAY);
    adaptiveThreshold(gray, thresh, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 57, 5);

    std::vector< std::vector < Point > > contours; 
    std::vector<Vec4i> hierarchy;
    std::vector<Point> approx;
    findContours(thresh, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    double maxArea = 0;
    int contourIndex = 0;
    for (int i = 0; i < contours.size(); i++) 
    {
        double area = contourArea(contours[i]);
        double perimeter = arcLength(contours[i], true);
        std::vector<Point> approx1;
        approxPolyDP(contours[i], approx1, 0.02*perimeter, true);
        if (area > maxArea && approx1.size() == 4) 
        {
            maxArea = area;
            approx = approx1;
            contourIndex = i;
        }
    
    }

    // Draw contours of the grid
    drawContours(image, contours, contourIndex, Scalar(0,255,0), 2, 8);

    return approx;
}