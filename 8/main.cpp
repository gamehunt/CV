#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <utility>

typedef std::vector<std::vector<cv::Point>> contour_wrapper;

#define SQUARE_COLOR   cv::Scalar(255, 0,   0)
#define CIRCLE_COLOR   cv::Scalar(0,   255, 0)
#define ELLIPSE_COLOR  cv::Scalar(255, 0,   255)
#define TRIANGLE_COLOR cv::Scalar(0,   0,   255)
#define CROSS_COLOR    cv::Scalar(255, 255, 0)
#define W_COLOR        cv::Scalar(0,   255, 255)
#define UNK_COLOR      cv::Scalar(0,   0,   0)

std::pair<std::string, cv::Scalar> identify(const std::vector<cv::Point>& contour) {
    std::vector<cv::Point> approx;
    double epsilon = 0.03 * cv::arcLength(contour, true);
    cv::approxPolyDP(contour, approx, epsilon, true);

    bool conv = cv::isContourConvex(approx);
    if(conv) {
        switch(approx.size()) {
            case 3:
                return std::make_pair("TRIANGLE", TRIANGLE_COLOR);
            case 4:
                return std::make_pair("SQUARE", SQUARE_COLOR);
            case 6:
            case 5:
            case 7:
            case 8:
                if(fabs(cv::minAreaRect(approx).angle - 45) < 5) {
                    return std::make_pair("ELLIPSE", ELLIPSE_COLOR);
                } else {
                    return std::make_pair("CIRCLE", CIRCLE_COLOR);
                }
        }
    } else {
        auto rect = cv::boundingRect(approx);
        double aspect = 1.0 * rect.width / rect.height;
        if(aspect > 1.25 || aspect < 0.9) {
            return std::make_pair("W", W_COLOR);
        } else {
            return std::make_pair("CROSS", CROSS_COLOR);
        }
    }

    return std::make_pair("UNKNOWN " + std::to_string(approx.size()) + (conv ? " Conv" : " Non Conv"), UNK_COLOR);
}
 
int main() {
    cv::VideoCapture cap("samples/video.mkv");
    if (!cap.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть видеофайл!" << std::endl;
        return -1;
    }

    int frame_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    cv::Size frameSize(frame_width, frame_height);
    
    cv::VideoWriter writer("output.mp4", cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 30, frameSize, true);
 
    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Ошибка: пустой кадр!" << std::endl;
            break;
        }

        cv::Mat processed;
        cv::cvtColor(frame, processed, cv::COLOR_BGR2GRAY);

        cv::GaussianBlur(processed, processed, cv::Size(5, 5), 1.25);
        cv::Canny(processed, processed, 50, 80);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(processed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for(const auto& contour: contours) { 
            auto [type, color] = identify(contour);
            cv::Moments moments = cv::moments(contour);
            cv::Point center(moments.m10/moments.m00, moments.m01/moments.m00);
            cv::putText(frame, type, center, cv::FONT_HERSHEY_SIMPLEX, 0.6, color, 2);
            cv::drawContours(frame, contour_wrapper(1, contour), -1, color, 1);
        }

        cv::imshow("video", frame);
        writer.write(frame);
        // cv::imshow("video (processed)", processed);
#if 0 
        while(cv::waitKey() != 'c');
#endif

        if((cv::waitKey(30) & 0xEFFFFF) == 27) {
            break;
        }
    }

    writer.release();
    cap.release(); // Освобождаем ресурс камеры
    cv::destroyAllWindows();
 
    return 0;
}
