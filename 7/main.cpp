#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
 
int main() {
    cv::VideoCapture cap("samples/video.mp4");
    if (!cap.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть видеофайл!" << std::endl;
        return -1;
    }
 
    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Ошибка: пустой кадр!" << std::endl;
            break;
        }


        cv::Mat processed;
        cv::cvtColor(frame, processed, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(processed, processed, cv::Size(7, 7), 1.5);
        cv::Canny(processed, processed, 50, 150);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(processed, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

        for(const auto& contour: contours) { 
            if(cv::contourArea(contour) < 100) {
                continue;
            }

            std::vector<cv::Point> approx;
            double epsilon = 0.01 * cv::arcLength(contour, true);
            cv::approxPolyDP(contour, approx, epsilon, true);

            if(approx.size() == 4) {
                cv::Moments moments = cv::moments(contour);
                cv::Point center(moments.m10/moments.m00, moments.m01/moments.m00);
                cv::drawContours(frame,std::vector<std::vector<cv::Point>>(1, contour), -1, cv::Scalar(0, 255, 0), 2);
                cv::putText(frame, "SQUARE", center, cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 255, 0), 2);
                break;
            }
        }

        cv::imshow("video", frame);

        if((cv::waitKey(30) & 0xEFFFFF) == 27) {
            break;
        }
    }
 
 
    return 0;
}
