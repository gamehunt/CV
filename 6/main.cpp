#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#define EPS 700

int main() {
    cv::Mat image = cv::imread("samples/3.png");
    if (image.empty()) {
        std::cout << "Could not open or find the image." << std::endl;
        return -1;
    }

    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
    hsv += cv::Scalar(0, 100, 0);

    cv::Mat channels[3];
    cv::split(hsv, channels);

    // Преобразование изображения в оттенки серого
    cv::Mat gray = channels[2];
    // Применение Гауссова сглаживания для уменьшеhough linesния шума
    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 1.5);

    // Выделение границ с помощью метода Canny
    cv::Mat edges;
    // cv::Canny(blurred, edges, 50, 150);
    cv::threshold(blurred, edges, 100, 255, cv::THRESH_BINARY);

    // Обнаружение контуров на основе бинарного изображения границ
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // Визуализация результатов
    cv::Mat contourOutput(image.rows, image.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::drawContours(contourOutput, contours, -1, cv::Scalar(0, 255, 0), 1);

    for(const auto& contour: contours) {
        std::string type;

        cv::Moments moments = cv::moments(contour);
        cv::Point center(moments.m10/moments.m00, moments.m01/moments.m00);

        std::vector<cv::Point> approx;
        double epsilon = 0.03 * cv::arcLength(contour, true);
        cv::approxPolyDP(contour, approx, epsilon, true);

        int size = approx.size();
        if(size == 3) {
            type = "Triangle";
        } else if(size == 4) {
            type = "Square";
        } else if (size > 4) {
            type = "Circle";
        } else {
            type = "Unknown";
        }

        cv::putText(contourOutput, type, center, cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 255, 0));
        // cv::putText(contourOutput, std::to_string(size), center + cv::Point(0, 50), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 0, 255));
    }

    cv::imshow("Исходное изображение", image);
    cv::imshow("Результат", contourOutput);
 
    // Ожидание, пока пользователь не закроет окно с изображением
    while((cv::waitKey() & 0xEFFFFF) != 27);
 
    return 0;
}
