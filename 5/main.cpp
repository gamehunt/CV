#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
 
int main() {
    cv::Mat image = cv::imread("samples/stop.png");
    if (image.empty()) {
        std::cout << "Could not open or find the image." << std::endl;
        return -1;
    }

    cv::Mat hsvImage;
    cv::cvtColor(image, hsvImage,  cv::COLOR_BGR2HSV);
    // cv::GaussianBlur(hsvImage, hsvImage, cv::Size(9, 9), 2);

    // Определяем диапазон синего цвета (оттенки могут отличаться в зависимости от условий освещения)
    cv::Mat mask, mask1, mask2, mask3;
    // cv::inRange(hsvImage, cv::Scalar(0, 80, 0), cv::Scalar(10, 255, 255), mask);
    cv::inRange(hsvImage, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), mask1);     
    cv::inRange(hsvImage, cv::Scalar(160, 100, 100), cv::Scalar(180, 255, 255), mask2);     
    cv::bitwise_or(mask1, mask2, mask3); 
    // Применяем маску к исходному изображению
    cv::Mat result1, result2;
    // cv::bitwise_and(image, image, result1, mask); 
    cv::bitwise_and(image, image, result2, mask3); 

    // Отображение изображения на экране
    cv::imshow("Image", image);
    // cv::imshow("Mask", mask);
    cv::imshow("Mask", mask3);
    // cv::imshow("Result", result1);
    cv::imshow("Result", result2);

    cv::Mat blurredImage;
    result2.copyTo(blurredImage);
    cv::cvtColor(blurredImage, blurredImage, cv::COLOR_HSV2BGR);
    cv::cvtColor(blurredImage, blurredImage, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(blurredImage, blurredImage, cv::Size(9, 9), 2); 
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(blurredImage, circles, cv::HOUGH_GRADIENT, 1.25, blurredImage.rows, 200, 30);

    for(const auto& circle: circles) {
        cv::Vec3i c = circle;
        cv::Point center(c[0], c[1]);
        int radius = c[2];
        // Рисуем центр окружности
        cv::circle(result2, center, 3, cv::Scalar(0, 255, 0), -1, cv::LINE_AA);
        // Рисуем саму окружность
        cv::circle(result2, center, radius, cv::Scalar(255, 0, 0), 2, cv::LINE_AA);
    }

    cv::imshow("Result", result2);
    // Ожидание, пока пользователь не закроет окно с изображением
    while((cv::waitKey() & 0xEFFFFF) != 27);
 
    return 0;
}
