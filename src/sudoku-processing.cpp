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

    // We are looking for the biggest square like shaped blob 
    for (int i = 0; i < contours.size(); i++) 
    {
        double area = contourArea(contours[i]);
        double perimeter = arcLength(contours[i], true);
        std::vector<Point> approx1;
        approxPolyDP(contours[i], approx1, 0.02*perimeter, true); // Draw shape around contours
        if (area > maxArea && approx1.size() == 4) // Grid has 4 corners 
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

// This function takes as input the 4 points corresponding to 4 corners of the grid
// and it creates a cropped image containing only the grid, top down view
Mat cropGrid(std::vector<Point> corners, Mat sourceIMG)
{
    Point2f src_vertices[4];
    src_vertices[0] = corners[0]; // top left
    src_vertices[1] = corners[1]; // top right
    src_vertices[2] = corners[2]; // bottom left
    src_vertices[3] = corners[3]; // bottom right

    Point2f dst_vertices[4];
    dst_vertices[0] = Point(0, 0); // top left
    dst_vertices[1] = Point(640, 0); // top right
    dst_vertices[2] = Point(0, 640); // bottom left
    dst_vertices[3] = Point(640, 640); // bottom right

    Mat M = getPerspectiveTransform(src_vertices, dst_vertices);
    Mat cropped(640, 640, CV_8UC3);
    warpPerspective(sourceIMG, cropped, M, cropped.size(), INTER_LINEAR, BORDER_CONSTANT);

    return cropped;
}

// Sort points from top left (tl), top right (tr), bottom left (bl), bottom right (br)
std::vector<Point> sortPoints(std::vector<Point> corners)
{
    std::vector<Point> sorted;

    int largestSum = 0, smallestSum = 100000;
    int xSmall = 10000; int xBig = 0;
    int indexTL, indexTR, indexBL, indexBR;

    // Identidy top left and bottom right corners
    for (int i = 0; i < corners.size(); i++)
    {
        int sum = corners[i].x + corners[i].y; // sum up x and y coordinates
        if (sum < smallestSum)
        {
            smallestSum = sum;
            indexTL = i; // top left corner has the smallest sum 
        }
        if (sum > largestSum)
        {
            largestSum = sum;
            indexBR = i; // bottom right has the largest sum 
        }
    }

    // Identify top right and bottom left
    for (int i = 0; i < corners.size(); i++)
    {
        if (i == indexBR || i == indexTL)
        {
            continue;
        }
        if (corners[i].x < xSmall)
        {
            xSmall = corners[i].x; // bottom left has the smallest x coordinate
            indexBL = i;
        }
        if (corners[i].x > xBig)
        {
            xBig = corners[i].x; // top right has the largest x coordinate
            indexTR = i;
        }
    }

    sorted.push_back(corners[indexTL]);
    sorted.push_back(corners[indexTR]);
    sorted.push_back(corners[indexBL]);
    sorted.push_back(corners[indexBR]);

    return sorted;
}