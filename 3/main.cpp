#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
 
int main() {
    cv::Mat image = cv::imread("samples/2.jpg");
    if (image.empty()) {
        std::cout << "Could not open or find the image." << std::endl;
        return -1;
    }

    cv::Mat hsvImage;
    cv::Mat labImage;
    cv::Mat yuvImage;
    cv::Mat xyzImage;
    cv::Mat grayImage;

    cv::cvtColor(image, hsvImage,  cv::COLOR_BGR2HSV);
    cv::cvtColor(image, labImage,  cv::COLOR_BGR2Lab);
    cv::cvtColor(image, yuvImage,  cv::COLOR_BGR2YUV);
    cv::cvtColor(image, xyzImage,  cv::COLOR_BGR2XYZ);
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
 
    // Отображение изображения на экране
    cv::imshow("Original Image", image);
    cv::imshow("HSV Image", hsvImage);
    cv::imshow("LAB Image", labImage);
    cv::imshow("YUV Image", yuvImage);
    cv::imshow("XYZ Image", xyzImage);
    cv::imshow("Gray Image", grayImage);

    cv::GaussianBlur(grayImage, grayImage, cv::Size(9, 9), 2); 

    cv::imshow("Blurred Gray Image", grayImage);

    cv::Mat tmp;
    grayImage.copyTo(tmp);

    cv::Mat edges;
    cv::Canny(grayImage, edges, 25, 100);

    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI / 180, 100);

    cv::cvtColor(grayImage, grayImage, cv::COLOR_GRAY2BGR);

    for(const auto& line : lines) {
        float rho = line[0];
        float theta = line[1];
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        // Вычисляем две точки, достаточно далеко лежащие вдоль линии
        cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
        cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
        cv::line(grayImage, pt1, pt2, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
    }

    cv::imshow("Gray Image Lines", grayImage);

    grayImage = tmp;

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(grayImage, circles, cv::HOUGH_GRADIENT, 1, grayImage.rows / 6.5, 100, 30);

    cv::cvtColor(grayImage, grayImage, cv::COLOR_GRAY2BGR);

    for(const auto& circle: circles) {
        cv::Vec3i c = circle;
        cv::Point center(c[0], c[1]);
        int radius = c[2];
        // Рисуем центр окружности
        cv::circle(grayImage, center, 3, cv::Scalar(0, 255, 0), -1, cv::LINE_AA);
        // Рисуем саму окружность
        cv::circle(grayImage, center, radius, cv::Scalar(255, 0, 0), 2, cv::LINE_AA);
    }

    cv::imshow("Gray Image Circles", grayImage);
 
    // Ожидание, пока пользователь не закроет окно с изображением
    while((cv::waitKey() & 0xEFFFFF) != 27);
 
    return 0;
}
