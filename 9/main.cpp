#include <opencv2/calib3d.hpp>
#include <opencv2/core/base.hpp>
#include <opencv2/core/cvstd_wrapper.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>

std::vector<cv::DMatch> ratio_test(const std::vector<std::vector<cv::DMatch>>& matches12, double ratio){
    std::vector<cv::DMatch> good_matches;         
    for(int i = 0; i < matches12.size(); i++){  
        if(matches12[i][0].distance < ratio * matches12[i][1].distance)
            good_matches.push_back(matches12[i][0]);
    }
    return good_matches;                  
}

int main() {
    cv::Mat image1 = cv::imread("samples/part1.png");
    cv::Mat image2 = cv::imread("samples/part2.png");
    if (image1.empty() || image2.empty()) {
        std::cout << "Could not open or find the image." << std::endl;
        return -1;
    }

    cv::Ptr<cv::ORB> orb = cv::ORB::create();
    std::vector<cv::KeyPoint> kp1, kp2;
    cv::Mat ds1, ds2;

    orb->detectAndCompute(image1, cv::Mat(), kp1, ds1);
    orb->detectAndCompute(image2, cv::Mat(), kp2, ds2);

    cv::BFMatcher matcher(cv::NORM_HAMMING);
    std::vector<std::vector<cv::DMatch>> matches;
    matcher.knnMatch(ds1, ds2, matches, 2);

    std::vector<cv::DMatch> good_matches = ratio_test(matches, 0.7);

    cv::Mat imgMatches;
    cv::drawMatches(image1, kp1, image2, kp2, good_matches, imgMatches,
                    cv::Scalar::all(-1), cv::Scalar::all(-1),
                    std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    cv::imshow("Совпадения", imgMatches); 

    std::vector<cv::Point2f> points1;
    std::vector<cv::Point2f> points2;

    for( int i = 0; i < good_matches.size(); i++ )
    {
      points1.push_back(kp1[good_matches[i].queryIdx].pt);
      points2.push_back(kp2[good_matches[i].trainIdx].pt);
    }
    cv::Mat H = findHomography(points2, points1, cv::RANSAC);

    cv::Mat result(image1.rows, image1.cols + image2.cols, CV_8UC3);
    cv::Mat warp2;
    warpPerspective(image2, warp2, H, cv::Size(2 * image2.cols, image2.rows));
    cv::Mat half1(result, cv::Rect(0, 0, image1.cols, image1.rows));
    cv::Mat half2(result, cv::Rect(0, 0, warp2.cols, warp2.rows));
    warp2.copyTo(half2);
    image1.copyTo(half1);

    cv::imshow("Панорама", result); 


    // Ожидание, пока пользователь не закроет окно с изображением
    while((cv::waitKey() & 0xEFFFFF) != 27);
 
    return 0;
}
