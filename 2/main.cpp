#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

#define SPLIT_SIZE 2

int main(int argc, const char** argv) {
    int splitSize = SPLIT_SIZE;

    if(argc >= 2) {
        splitSize = std::atoi(argv[1]);
        if(!splitSize) {
            splitSize = SPLIT_SIZE;
        }
    }

    // Загружаем изображение
    cv::Mat image = cv::imread("samples/2.jpg");
    cv::Mat result = cv::Mat::zeros(image.size(), image.type());

    // Проверяем, успешно ли загружено изображение
    if (image.empty()) {
        std::cerr << "Ошибка: не удалось загрузить изображение!" << std::endl;
        return -1;
    }


    cv::Size imageSize = image.size();
    const int sideSegments = splitSize;
    const int segmentWidth  = imageSize.width  / sideSegments;
    const int segmentHeight = imageSize.height / sideSegments;

    std::vector<cv::Mat> parts;
    for(int i = 0; i < sideSegments; i++) {
        for(int j = 0; j < sideSegments; j++) {
            const int id = i * sideSegments + j;
            cv::Mat mask = cv::Mat::zeros(image.size(), CV_8UC1);
            cv::rectangle(mask, cv::Point(j * segmentWidth, i * segmentHeight), 
                                cv::Point(j * segmentWidth + segmentWidth, i * segmentHeight + segmentHeight), 
                                cv::Scalar(255), -1);
            cv::Mat part;
            cv::bitwise_and(image, image, part, mask);
            switch(id % 4) {
                case 1:
                    cv::bitwise_not(part, part);
                    break;
                case 2:
                    cv::cvtColor(part, part, cv::COLOR_BGR2GRAY);
                    cv::cvtColor(part, part, cv::COLOR_GRAY2BGR);
                    break;
                case 3:
                    part.setTo(cv::Scalar(0, 255, 0));
                    break;
            }
            cv::Mat roi = result(cv::Rect(0, 0, imageSize.width, imageSize.height));
            part.copyTo(roi, mask);
        }
    }

    // Отображаем изображение в окне
    cv::imshow(std::to_string(splitSize), result);
    cv::imwrite(std::to_string(splitSize)+"_split.png", result);

    // Ожидаем нажатия клавиши перед закрытием окна
    while((cv::waitKey() & 0xEFFFFF) != 27);

    return 0;
}
