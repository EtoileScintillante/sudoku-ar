#include "sudoku-processing.h"
#include "sudoku-solver.h"
using namespace cv;

/* Note: the following code contains print statements with ANSI colour codes.
Not all terminals support this. */

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
    std::vector< Point > gridContour; // Holds the 4 corners of the grid
    std::vector< std::vector< int > > gridOG; // Original grid
    std::vector< std::vector< int > > gridSOLVED; // Solved grid

    /* Taking an image the moment the camera opens leads to a very low light image
    on which the grid is nearly invisible.
    To prevent this we create some lag so that the camera can catch enough light */
    int lag = 0;

    /// Load data and train KNN model ///
    Ptr<ml::KNearest> knn = cv::ml::KNearest::create();
    std::cout << "\033[1;33mTraining KNN model...\033[0m\n";

    Mat sample;
    Mat response,tmp;
    FileStorage Data("model/TrainingData.yml",FileStorage::READ); // Read traing data to a Mat
    Data["data"] >> sample;
    Data.release();

    FileStorage Label("model/LabelData.yml",FileStorage::READ); // Read label data to a Mat
    Label["label"] >> response;
    Label.release();

    knn->train(sample,ml::ROW_SAMPLE, response); // Train with sample and responses
    std::cout << "\033[1;32mTraining of KNN model completed! \033[0m\n";

    if (!cap.isOpened()) 
    {
        std::cout << "cannot open camera";
    }

    /// Video loop ///
    while (true) 
    {
        lag++;
        cap >> src;
        imshow("Display window", src);

        if (lag > 30)
        {
            if (solved == false)
            {

                if (gridDetected == false)
                {
                    /// Detect grid ///
                    std::cout << "\033[1;33mTrying to detect grid...\033[0m\n";
                    Mat srcCopy = src.clone();
                    gridContour = detectGrid(srcCopy);
                    //imwrite("gridContour.png", srcCopy); // Save image
                    if (gridContour.size() == 4)
                    {   
                        std::cout << "\033[1;32mGrid detected!\033[0m\n";
                        gridDetected = true;
                    }
                }

                if (gridDetected == true)
                {
                    /// Find cells ///
                    std::cout << "\033[1;33mTrying to detect cells of the grid...\033[0m\n";
                    gridContour = sortSquarePoints(gridContour);
                    cropped = cropGrid(gridContour, src);
                    //imwrite("cropped.png", cropped.clone());  // Save image
                    Mat noDigs = filterOutDigits(cropped);
                    //imwrite("noDigits.png", noDigs);  // Save image
                    Mat cellCnts = cropped.clone();
                    contoursCells = findCells(noDigs, cellCnts);
                    //imwrite("cellContours.png", cellCnts);  // Save image
                    contoursCells = sortCells(contoursCells);

                    if (contoursCells.size() == 81)
                    {
                        std::cout << "\033[1;32mCells found!\033[0m\n";
                        cellsFound = true;
                    }
                }

                if (cellsFound == true)
                {
                    /// Recognize digits ///
                    std::cout << "\033[1;33mTrying to recognize digits...\033[0m\n";
                    gridOG = gridToVector(cropped, contoursCells, knn);
                    if (gridOG.size() == 9)
                    {
                        std::cout << "\033[1;32mDigits recognized!\033[0m\n";
                        gridConverted = true;
                    }
                }
                
                if (gridConverted == true)
                {
                    /// Try to solve sudoku ///
                    std::cout << "\033[1;33mTrying to solve the sudoku...\033[0m\n";
                    if (solveSudoku(gridOG, 0, 0, gridSOLVED))
                    {
                        std::cout << "\033[1;32mSolution found!\033[0m\n";
                        printSudoku(gridSOLVED);
                        solved = true;
                    }
                }
            }
            else // If solution has been found
            {
                /*  For the AR effect to work we need to find the contours of the grid again and again
                because if the grid moves while capturing the video, the position of the mask must change too
                so that it can move along with the grid, creating the AR effect */
                Mat srcClone = src.clone();
                gridContour = detectGrid(srcClone);
                gridContour = sortSquarePoints(gridContour);
                Mat mask = createMask(contoursCells, gridSOLVED, gridOG);
                Mat result = showSolution(gridContour, src, mask);
                //imwrite("result.png", result);  // Save image
                imshow("Display window", result);
            }
        }

        // Press  ESC on keyboard to exit
        if (waitKey(5) == 27) break;
    }

    cap.release();
    
    return 0;
}