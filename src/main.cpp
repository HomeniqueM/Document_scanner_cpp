#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "helpers/documentHandler.h"

cv::Mat imgOriginal, imgGray, imgCanny, imgThre,imgBlur,imgDil,imgErode;

cv::Mat preProcessing(cv::Mat img)
{
    cv::Mat imgDilate;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
    cv::cvtColor(img,imgGray,cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(imgGray,imgBlur,cv::Size(3,3),3,0);
    cv::Canny(imgBlur,imgCanny,25,75);

    cv::dilate(imgCanny,imgDilate,kernel);
    //cv::erode(imgDil,imgErode,kernel);
    return imgDilate;
}

int main(int argc, char const *argv[])
{
    std::string path = "imgs/paper.jpg";
    imgOriginal = cv::imread(path);
    resize(imgOriginal, imgOriginal, cv::Size(), 0.5, 0.5);

    // pre processamento
    imgThre = preProcessing(imgOriginal);

    // Get Contours - Assumindo que o maior retângulo representa o documento

    //
    cv::imshow("Image", imgOriginal);
    cv::waitKey(0);
    return 0;
}
