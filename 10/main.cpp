#include <filesystem>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
namespace fs = std::filesystem;

struct SampleData {
    std::string name;
    cv::Mat image;
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
};

std::vector<cv::DMatch> ratio_test(const std::vector<std::vector<cv::DMatch>>& matches12, double ratio){
    std::vector<cv::DMatch> good_matches;         
    for(int i = 0; i < matches12.size(); i++){  
        if(matches12[i][0].distance < ratio * matches12[i][1].distance)
            good_matches.push_back(matches12[i][0]);
    }
    std::sort(good_matches.begin(), good_matches.end(), [](const cv::DMatch& a, const cv::DMatch& b) { return a.distance < b.distance; });
    good_matches.resize(5);
    return good_matches;                  
}

int main() {
    std::vector<SampleData> samples;

    cv::Ptr<cv::SIFT> orb = cv::SIFT::create();

    std::string path("./samples/cards");
    for (auto &p : fs::recursive_directory_iterator(path))
    {
        if (p.path().extension() == ".png") {
            auto path = p.path().string();
            SampleData data;
            data.name = p.path().stem().string();
            data.image = cv::imread(path);
            if(data.image.empty()) {
                std::cout << "Failed to parse sample: " << path << std::endl;
                continue;
            }
            orb->detectAndCompute(data.image, cv::Mat(), data.keypoints, data.descriptors);
            samples.push_back(data);

            std::cout << "Parsed sample: " << path << std::endl;
        }
    }

    cv::Mat target = cv::imread("./samples/target.png");
    if(target.empty()) {
        std::cout << "Failed to parse target" << std::endl;
    }

    cv::Mat targetGray;
    cv::cvtColor(target, targetGray, cv::COLOR_BGR2GRAY);

    std::vector<cv::KeyPoint> targetKeypoints;
    cv::Mat targetDescriptors;

    orb->detectAndCompute(target, cv::Mat(), targetKeypoints, targetDescriptors);

    cv::Mat blurred;
    cv::GaussianBlur(targetGray, blurred, cv::Size(7, 7), 2);

    // Выделение границ с помощью метода Canny
    cv::Mat edges;
    cv::Canny(blurred, edges, 50, 200);
    // cv::threshold(blurred, edges, 100, 255, cv::THRESH_BINARY);

    // Обнаружение контуров на основе бинарного изображения границ
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::BFMatcher matcher;
    cv::Mat imgMatches = target.clone();
    cv::drawContours(imgMatches, contours, -1, cv::Scalar(0, 255, 0), 3);

    for(auto sample: samples) {
        std::vector<std::vector<cv::DMatch>> matches;
        matcher.knnMatch(targetDescriptors, sample.descriptors, matches, 2);
        auto good_matches = ratio_test(matches, 0.5);
        std::vector<cv::Point2f> points;
        for( int i = 0; i < good_matches.size(); i++ )
        {
            points.push_back(targetKeypoints[good_matches[i].queryIdx].pt);
        }

        // if(sample.name == "5 kresti") {
        //     for(auto p : points) {
        //         cv::circle(imgMatches, p, 5, cv::Scalar(0, 0, 255));
        //     }
        // }
        cv::putText(imgMatches, sample.name, points[0], cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
    }

    cv::imshow("Matches", imgMatches); 

 
    // Ожидание, пока пользователь не закроет окно с изображением

    while((cv::waitKey() & 0xEFFFFF) != 27);
 
    return 0;
}
