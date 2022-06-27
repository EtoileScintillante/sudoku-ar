#include "sudoku-processing.h"

int main(int argc, char* argv[])
{
    /// LOAD DATA AND TRAIN KNN MODEL ///
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

    /// LOAD SOURCE IMAGE ///
    if (argc != 2) {
        std::cout << "Usage: ./exe path-to-sudoku-image" << std::endl;
    }
    std::string path = argv[1];
    Mat src = imread(path, 1);

    /// DETECT GRID ///
    Mat srcCopy = src.clone();
    std::vector<Point> gridContour;
    gridContour = detectGrid(srcCopy);
    
    imshow("Grid contours", srcCopy);
    waitKey(0);

    /// CROP IMAGE ///
    gridContour = sortPoints(gridContour);
    Mat cropped = cropGrid(gridContour, src);
    imshow("Cropped image", cropped);
    waitKey(0);

    /// FILTER OUT DIGITS ///
    Mat noDigs = filterOutDigits(cropped);
    imshow("Filtered out digits", noDigs);
    waitKey(0);

    /// FIND CELL CONTOURS AND SORT THEM ///
    Mat cellCnts = cropped.clone();
    std::vector< std::vector < Point > > contoursCells = findCells(noDigs, cellCnts);
    imshow("Cell contours", cellCnts);
    waitKey(0);

    contoursCells = sortCells(contoursCells);

    /// RECOGNIZE DIGITS AND TURN GRID INTO 2D VECTOR ///
    std::vector< std::vector< int > > gridOG;
    gridOG = ImageToVec(cropped, contoursCells, knn);

    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            std::cout << gridOG[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}