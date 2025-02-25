#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
 
int main() {
    cv::Mat image = cv::imread("samples/2.jpg");
    if (image.empty()) {
        std::cout << "Could not open or find the image." << std::endl;
        return -1;
    }
 
    // Отображение изображения на экране
    cv::imshow("Sample Image", image);
 
    // Ожидание, пока пользователь не закроет окно с изображением
    while((cv::waitKey() & 0xEFFFFF) != 27);
 
    return 0;
}
