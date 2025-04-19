#pragma once
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <string>

class LicensePlateDetector {
public:
    explicit LicensePlateDetector(const std::string& tessdatapath = "C:\\tesseract\\tessdata");
    std::string detectPlate(const cv::Mat& frame);

private:
    tesseract::TessBaseAPI tess_;
    cv::Mat preprocess(const cv::Mat& roi);
};