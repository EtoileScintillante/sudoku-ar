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

// Returns a binary image with the lines of the grid in white and 
// and everything within the lines is black
// This step is necessary for finding the contours of the cells
Mat filterOutDigits(Mat image)
{   
    Mat noDigits;
    cvtColor(image, noDigits, COLOR_BGR2GRAY);
    adaptiveThreshold(noDigits, noDigits, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 57, 5);
    
    std::vector< std::vector < Point > > contours; 
    std::vector<Vec4i> hierarchy;
    findContours(noDigits, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    for (int i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area < 1000) // Find contours of digits
        {
            drawContours(noDigits, contours, i, Scalar(0,0,0), -1); // Filter digits out
        }
    }

    // Fix horizontal and vertical lines (this makes the lines clearer)
    Mat dst;
    Mat verticalKernel = getStructuringElement(MORPH_RECT, Size(1,5));
    morphologyEx(noDigits, dst, MORPH_CLOSE, verticalKernel, Point(-1,-1), 9);
    Mat horizontalKernel = getStructuringElement(MORPH_RECT, Size(5,1));
    morphologyEx(noDigits, dst, MORPH_CLOSE, horizontalKernel, Point(-1,-1), 4);

    return dst;
}

// Detect the contours of the cell and return a vector containing those contours
// Also draws the cell contours on src image
// Assumption: input image is image obtained through the function filterOutDigits
std::vector< std::vector < Point > > findCells(Mat image, Mat src)
{
    // Invert the binary image and find contours of the cells of the grid
    Mat invertedIMG;
    bitwise_not(image, invertedIMG);
    std::vector< std::vector < Point > > contours; 
    std::vector<cv::Vec4i> hierarchy;
    findContours(invertedIMG, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    // The images of the sudokus all have cells with an area between 3000 and 5000
    // These Points will be stored in a separate vector
    std::vector< std::vector < Point > > contoursCells;
    for (int i = 0; i < contours.size(); i++)
    {
        double area = contourArea(contours[i]);
        if (area > 3000 && area < 5000)
        {
            contoursCells.push_back(contours[i]);
            drawContours(src, contours, i, Scalar(0,255,0), 2);
        }
    }

    return contoursCells;
}

// Sort contours from left to right (used in sortCells function)
bool sortCellsLeftToRight(std::vector<Point> a, std::vector<Point> b)
{
    Rect r1 = boundingRect(a);
    Rect r2 = boundingRect(b);
    return (r1.x < r2.x);
}

// Sort the contours of the cells from top left to bottom right
std::vector< std::vector < Point > > sortCells(std::vector< std::vector < Point > > cells)
{
    // By reversing the vector, the first 9 vector<Point> belong to the cells on the top row
    // the 9 vector<Point> after that belong to the cells on row beneath the top row, and so on.
    // However, the vectors are not yet sorted from left to right
    std::reverse(cells.begin(), cells.end());

    // Sort the cells from left to right
    std::vector< std::vector < Point > > sortedCells;
    std::vector< std::vector < Point > > temp;
    int row = 0;
    for (int i = 0; i < 9; i++)
    {
        // Use temp vector to store one row (9 vector<Point>) and sort these
        temp.clear();
        for (int j = 0; j < 9; j++)
        {
            temp.push_back(cells[row+j]);
        }
        std::sort(temp.begin(), temp.end(), sortCellsLeftToRight);
        // Push sorted vector<Point> back to sortedCells
        for (int k = 0; k < 9; k++)
        {
            sortedCells.push_back(temp[k]);
        }
        row += 9; // Move to next row

    }

    return sortedCells;
}

// Recognize digits with the KNN model and return a 2d vector of ints
// Input image must be image obtained from cropGrid function
// Empty cell = 0
std::vector< std::vector< int > > ImageToVec(Mat src, std::vector< std::vector < Point > > sortedCells, Ptr<ml::KNearest> knn)
{
    // Prep image
    Mat thresh;
    cvtColor(src, thresh, COLOR_BGR2GRAY);
    adaptiveThreshold(thresh, thresh, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 57, 5);
    imshow("test", thresh);
    waitKey(0);

    // Create vector to store the sudoku grid
    std::vector< std::vector<int > > grid;
    std::vector< int > gridRow;
    int rowCount = 0;

    // Iterate over sukdoku grid and extract digits using the KNN
    for (int i = 0; i < 9; i++)
    {
        for (int k = 0; k < 9; k++)
        {
            Rect r = boundingRect(sortedCells[k + rowCount]);
            Mat ROI = thresh(r); // Create image of individual cell
            std::vector< std::vector < Point > > ROIcontours;
            int num = 0;
            findContours(ROI, ROIcontours, RETR_TREE, CHAIN_APPROX_SIMPLE); // Find contours of cell image
            for (int j = 0; j < ROIcontours.size(); j++)
            {
                double area = contourArea(ROIcontours[j]);
                if (area > 220) // If there is a digit in the cell
                {
                    Rect r1= boundingRect(ROIcontours[j]); // Create rect around the digit
                    Mat ROI1 = ROI(r1); // Create image of the digit
                    Mat temp1, temp2; 
                    resize(ROI1,temp1, Size(10,10), 0,0,INTER_LINEAR );
                    temp1.convertTo(temp2,CV_32FC1);
                    std::vector< float > response;
                    knn->findNearest(temp2.reshape(1,1), 1, response); // Recognize digit
                    num = response[0];
                }
                
            }
            gridRow.push_back(num);
        }
        grid.push_back(gridRow); // Push back row into grid
        gridRow.clear(); // Clear row vector 
        rowCount+=9; // Move to next row
    }

    return grid;
}