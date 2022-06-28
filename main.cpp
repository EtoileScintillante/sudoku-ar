#include "sudoku-processing.h"
#include "sudoku-solver.h"
using namespace cv;

int main(int argc, char* argv[])
{
    Mat src;
    namedWindow("Display window");
    VideoCapture cap(0);

    bool solved = false;
    bool gridDetected = false;
    bool cellsFound = false;
    bool gridConverted = false;

    Mat cropped; // This will be a cropped image of the sudoku grid
    std::vector< std::vector < Point > > contoursCells; // Holds all the 81 cell contours
    std::vector< Point > gridContour; // Holds 4 corners of the grid
    std::vector< std::vector< int > > gridOG; // Original grid
    std::vector< std::vector< int > > gridSOLVED; // Solved grid

    // Taking an image the moment the camera boots up leads to a very low light image
    // on which the grid is nearly invisible
    // To prevent this we create some lag so that the camera can catch enough light
    int lag = 0;

    /// Load data and train KNN model ///
    Ptr<ml::KNearest> knn = cv::ml::KNearest::create();

    Mat sample;
    Mat response,tmp;
    FileStorage Data("model/TrainingData.yml",FileStorage::READ); // Read traing data to a Mat
    Data["data"] >> sample;
    Data.release();

    FileStorage Label("model/LabelData.yml",FileStorage::READ); // Read label data to a Mat
    Label["label"] >> response;
    Label.release();

    knn->train(sample,ml::ROW_SAMPLE, response); // Train with sample and responses
    std::cout << "Training of KNN model completed" << std::endl;

    if (!cap.isOpened()) 
    {
        std::cout << "cannot open camera";
    }

    /// Video loop ///
    while (true) 
    {
        lag++;
        cap >> src;
        if (lag > 30)
        {
            if (solved == false)
            {
                imshow("Display window", src);

                if (gridDetected == false)
                {
                    std::cout << "here" << std::endl;
                    /// Detect grid ///
                    Mat srcCopy = src.clone();
                    gridContour = detectGrid(srcCopy);
                    imwrite("gridContour.png", srcCopy);
                    if (gridContour.size() == 4)
                    {   
                        std::cout << "Grid detected" << std::endl;
                        gridDetected = true;
                    }
                }

                if (gridDetected == true)
                {
                    std::cout << "here1" << std::endl;
                    /// Find cells ///
                    gridContour = sortPoints4(gridContour);
                    cropped = cropGrid(gridContour, src);
                    imwrite("cropped.png", cropped.clone());
                    std::cout << "image cropped" << std::endl;
                    Mat noDigs = filterOutDigits(cropped);
                    imwrite("noDigs.png", noDigs);
                    Mat cellCnts = cropped.clone();
                    contoursCells = findCells(noDigs, cellCnts);
                    imwrite("cellcontours.png", cellCnts);
                    contoursCells = sortCells(contoursCells);
                    std::cout << contoursCells.size() << std::endl;

                    if (contoursCells.size() == 81)
                    {
                        std::cout << "Cells found" << std::endl;
                        cellsFound = true;
                        gridDetected = false;
                    }
                }

                if (cellsFound == true)
                {
                    std::cout << "here2" << std::endl;
                    /// Recognize digits ///
                    std::cout << contoursCells.size() <<std::endl;
                    gridOG = ImageToVec(cropped, contoursCells, knn);
                    if (gridOG.size() == 9)
                    {
                        std::cout << "Image of grid converted to vector" << std::endl;
                        gridConverted = true;
                        cellsFound = false;
                    }
                }
                
                if (gridConverted == true)
                {
                    std::cout << "here3" << std::endl;
                    /// Try to solve sudoku ///
                    if (solve_sudoku(gridOG, 0, 0, gridSOLVED))
                    {
                        std::cout << "Solution found!" << std::endl;
                        print_sudoku(gridSOLVED);
                        solved = true;
                        gridConverted = false;
                    }
                }
            }
            else // If solution has been found
            {
                std::cout << "here4" << std::endl;
                /// Extract joints from grid ///
                Mat points = src.clone();
                std::vector< Point > jointPoints;
                jointPoints = extractJoints(src.clone());

                if (jointPoints.size() == 100) 
                {
                    /// Display solution ///
                    std::vector< std::vector< Point > > pointsSorted = sortPoints100(jointPoints);
                    Mat solutionimg = displaySolution(src, gridSOLVED, gridOG, pointsSorted);
                    imwrite("solution1.png", solutionimg);
                    imshow("Display window", src);
                } 
            }
            
            // Press  ESC on keyboard to exit
            if (waitKey(5) == 27) break;
        }
    }

    cap.release();
    destroyAllWindows();

    return 0;
}