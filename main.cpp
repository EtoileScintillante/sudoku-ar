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
    std::cout << "\033[1;33m Training KNN model...\033[0m\n";

    Mat sample;
    Mat response,tmp;
    FileStorage Data("model/TrainingData.yml",FileStorage::READ); // Read traing data to a Mat
    Data["data"] >> sample;
    Data.release();

    FileStorage Label("model/LabelData.yml",FileStorage::READ); // Read label data to a Mat
    Label["label"] >> response;
    Label.release();

    knn->train(sample,ml::ROW_SAMPLE, response); // Train with sample and responses
    std::cout << "\033[1;32m Training of KNN model completed! \033[0m\n";

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
                    std::cout << "\033[1;33m Trying to detect grid...\033[0m\n";
                    Mat srcCopy = src.clone();
                    gridContour = detectGrid(srcCopy);
                    //imwrite("gridContour.png", srcCopy); // Save image
                    if (gridContour.size() == 4)
                    {   
                        std::cout << "\033[1;32m Grid detected!\033[0m\n";
                        gridDetected = true;
                    }
                }

                if (gridDetected == true)
                {
                    /// Find cells ///
                    std::cout << "\033[1;33m Trying to detect cells of the grid...\033[0m\n";
                    gridContour = sortPoints4(gridContour);
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
                        std::cout << "\033[1;32m Cells found!\033[0m\n";
                        cellsFound = true;
                    }
                }

                if (cellsFound == true)
                {
                    /// Recognize digits ///
                    std::cout << "\033[1;33m Trying to recognize digits...\033[0m\n";
                    gridOG = ImageToVec(cropped, contoursCells, knn);
                    if (gridOG.size() == 9)
                    {
                        std::cout << "\033[1;32m Image of grid converted to 2d vector!\033[0m\n";
                        gridConverted = true;
                    }
                }
                
                if (gridConverted == true)
                {
                    /// Try to solve sudoku ///
                    std::cout << "\033[1;33m Trying to solve the sudoku...\033[0m\n";
                    if (solve_sudoku(gridOG, 0, 0, gridSOLVED))
                    {
                        std::cout << "\033[1;32m Solution found!\033[0m\n";
                        print_sudoku(gridSOLVED);
                        solved = true;
                    }
                }
            }
            else // If solution has been found
            {
                /*  For the mask to work, we need to constantly find the contours of the grid,
                since the grid may move a little while capturing the video
                Holding it still is impossible anyway right? And it is fun if the digits move along with the grid
                That gives the augmented reality feeling :) */
                Mat srcClone = src.clone();
                gridContour = detectGrid(srcClone);
                gridContour = sortPoints4(gridContour);
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