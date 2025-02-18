#include <opencv2/opencv.hpp>

int main() {
    // Загружаем изображение
    cv::Mat image = cv::imread("samples/2.jpg");

    // Проверяем, успешно ли загружено изображение
    if (image.empty()) {
        std::cerr << "Ошибка: не удалось загрузить изображение!" << std::endl;
        return -1;
    }

    cv::line(image, cv::Point(0, 0), cv::Point(image.rows, image.cols), cv::Scalar(0, 0, 255), 2.0);
    cv::rectangle(image, cv::Point(125, 120), cv::Point(425, 520), cv::Scalar(255, 0, 0), 2.0);
    cv::circle(image, cv::Point(1.0 * image.rows / 2 - 10, 1.0 * image.cols / 2 - 10), 20.0, cv::Scalar(0, 255, 0), 2);
    cv::ellipse(image, cv::Point(image.rows / 2, image.cols / 2 + 100), cv::Size(50, 25), 45, 0, 360, cv::Scalar(0, 255, 0), 2);
    std::vector<cv::Point> points = { {150, 110}, {250, 210}, {450, 10} };
    cv::polylines(image, points, true, cv::Scalar(0, 0, 255), 2); 
    cv::putText(image, "Test", cv::Point(5, image.cols / 2), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 255, 0), 2);
    
    // Отображаем изображение в окне
    cv::imshow("Hello OpenCV", image);

    // Ожидаем нажатия клавиши перед закрытием окна
    while((cv::waitKey() & 0xEFFFFF) != 27);

    return 0;
}
