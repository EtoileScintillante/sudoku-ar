#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

int main(int argc, char* argv[])
{
    Mat image;

    namedWindow("Display window");

    VideoCapture cap(0);

    if (!cap.isOpened()) 
    {
        std::cout << "cannot open camera";
    }

    while (true) 
    {
        cap >> image;
        imshow("Display window", image);
        
        if (waitKey(10) >= 0)
            break;
    }

    return 0;
}