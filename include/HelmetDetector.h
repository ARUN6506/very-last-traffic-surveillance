#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class HelmetDetector {
public:
    struct Detection {
        cv::Rect bbox;
        float confidence;
    };

    HelmetDetector(const std::string& scriptPath, const std::string& weightsPath);
    std::vector<Detection> detect(const cv::Mat& frame);

private:
    std::string scriptPath_;
    std::string weightsPath_;
};