#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

cv::Mat imgOriginal, imgGray, imgCanny, imgThre, imgBlur, imgDil, imgErode, imgWarp,imgCrop;

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
                // drawContours(imgOriginal, conPoly, i, cv::Scalar(0, 255, 0), 2);
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

/**
 * @brief Dado um array de ponts garante que os pontos estão organizados no array de
 *        forma a representar a os pontos da direita para a esquerda de cima para baixo
 *
 * @param points
 * @return std::vector<cv::Point>
 */
std::vector<cv::Point> reorder(std::vector<cv::Point> points)
{
    std::vector<cv::Point> newPoints;
    std::vector<int> sumPoints, subPoints;

    for (cv::Point point : points)
    {
        sumPoints.push_back(point.x + point.y);
        subPoints.push_back(point.x - point.y);
    }
    // Pega o index do menor elemento do vector de soma
    newPoints.push_back(points[std::min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); // Index 0
    // Pega o index do menor elemento do vector de soma
    newPoints.push_back(points[std::max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); // Index 1
    // Pega o index do maior elemento do vector de soma
    newPoints.push_back(points[std::min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); // Index 2
    // Pega o index do maior elemento do vector de soma
    newPoints.push_back(points[std::max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); // Index 3

    return newPoints;
}
cv::Mat get_warp(cv::Mat image, std::vector<cv::Point> points, float width, float height)
{
    cv::Mat result, matrix;
    cv::Point2f src[4] = {points[0], points[1], points[2], points[3]};
    cv::Point2f mask[4] = {{0.0f, 0.0f}, {width, 0.0f}, {0.0f, height}, {width, height}};

    matrix = cv::getPerspectiveTransform(src, mask);
    cv::warpPerspective(image, result, matrix, cv::Point(width, height));

    return result;
}

int main(int argc, char const *argv[])
{
    int cropval = 10;
    float width = 420, height = 596;
    std::string path = "imgs/paper.jpg";
    std::vector<cv::Point> initialPoints, docPoints;

    imgOriginal = cv::imread(path);
    resize(imgOriginal, imgOriginal, cv::Size(), 0.5, 0.5);

    // pre processamento
    imgThre = preProcessing(imgOriginal);

    // Get Contours - Assumindo que o maior retângulo representa o documento
    initialPoints = get_contours(imgThre);
    // draw_points(initialPoints, cv::Scalar(0, 0, 255));
    docPoints = reorder(initialPoints);
    // draw_points(docPoints, cv::Scalar(255, 0, 255));

    // Warp
    imgWarp = get_warp(imgOriginal, docPoints, width, height);

    // Crop
    cv::Rect rect(cropval, cropval, width - (2 * cropval), height - (2 * cropval));
    imgCrop = imgWarp(rect);

    cv::imshow("Image", imgOriginal);
    cv::imshow("Image Dilate", imgThre);
    cv::imshow("Image Warp", imgWarp);
    cv::imshow("Image Crop", imgCrop);
    cv::waitKey(0);
    return 0;
}
