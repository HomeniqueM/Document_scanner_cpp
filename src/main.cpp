#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

cv::Mat imgOriginal, imgGray, imgCanny, imgThre, imgBlur, imgDil, imgErode;

cv::Mat preProcessing(cv::Mat img)
{
    cv::Mat imgDilate;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(imgGray, imgBlur, cv::Size(3, 3), 3, 0);
    cv::Canny(imgBlur, imgCanny, 25, 75);

    cv::dilate(imgCanny, imgDilate, kernel);
    // cv::erode(imgDil,imgErode,kernel);
    return imgDilate;
}
std::vector<cv::Point> get_contours(cv::Mat image)
{
    int contoursSize, area, largestArea = 0;
    float perimeter;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<cv::Point> result;

    cv::findContours(image, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    contoursSize = contours.size();
    std::vector<std::vector<cv::Point>> conPoly(contoursSize);
    std::vector<cv::Rect> boundRect(contoursSize);

    for (int i = 0; i < contoursSize; i++)
    {
        area = cv::contourArea(contours[i]);

        if (area > largestArea)
        {
            perimeter = cv::arcLength(contours[i], true);
            cv::approxPolyDP(contours[i], conPoly[i], 0.02 * perimeter, true);

            if (conPoly[i].size() == 4)
            {
                largestArea = area;
                result = {conPoly[i][0],
                          conPoly[i][1],
                          conPoly[i][2],
                          conPoly[i][3]};
                drawContours(imgOriginal, conPoly, i, cv::Scalar(0, 255, 0), 2);
            }
        }
    }

    return result;
}

void draw_points(std::vector<cv::Point> points, cv::Scalar color)
{

    for (int i = 0; i < points.size(); i++)
    {
        cv::circle(imgOriginal, points[i], 10, color, cv::FILLED);
        putText(imgOriginal, std::to_string(i), points[i], cv::FONT_HERSHEY_PLAIN, 5, color, 2);
    }
}

int main(int argc, char const *argv[])
{
    std::string path = "imgs/paper.jpg";
    std::vector<cv::Point> initialPonts;
    imgOriginal = cv::imread(path);
    resize(imgOriginal, imgOriginal, cv::Size(), 0.5, 0.5);

    // pre processamento
    imgThre = preProcessing(imgOriginal);

    // Get Contours - Assumindo que o maior retângulo representa o documento
    initialPonts = get_contours(imgThre);
    draw_points(initialPonts, cv::Scalar(0, 0, 255));

    //
    cv::imshow("Image", imgOriginal);
    cv::imshow("Image Dilate", imgThre);
    cv::waitKey(0);
    return 0;
}
