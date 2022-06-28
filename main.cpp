#include "sudoku-processing.h"
#include "sudoku-solver.h"
using namespace cv;

int main(int argc, char* argv[])
{
    Mat src;
    namedWindow("Display window");
    VideoCapture cap(0);
    cap.set(10,150);

    bool solved = false;
    bool gridDetected = false;
    bool cellsFound = false;
    bool gridConverted = false;

    Mat cropped;
    std::vector< std::vector < Point > > contoursCells; // Hold all the 81 cells
    std::vector< Point > gridContour; // Holds 4 corners of the grid
    std::vector< std::vector< int > > gridOG; // Original grid
    std::vector< std::vector< int > > gridSOLVED; // Solved grid

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
        cap >> src;
        if (solved == false)
        {
            imshow("Display window", src);

            if (gridDetected == false)
            {
                std::cout << "here" << std::endl;
                /// Detect grid ///
                Mat srcCopy = src.clone();
                gridContour = detectGrid(srcCopy);
                if (gridContour.size() == 4)
                {   
                    imwrite("grid.png", srcCopy);
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
                std::vector< std::vector < Point > > contoursCells = findCells(noDigs, cellCnts);
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
                displaySolution(src, gridSOLVED, gridOG, pointsSorted);
                imshow("Display window", src);
            } 
        }
        
        if (waitKey(10) >= 0)
            break;
    }

    return 0;
}