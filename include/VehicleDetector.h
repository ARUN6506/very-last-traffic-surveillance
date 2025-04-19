#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include<string>

struct Detection {
	cv::Rect bbox;
	int classId;
	float confidence;
};
class VehicleDetector
{
public:
	VehicleDetector(const std::string& scriptPath);
	std::vector<Detection> detect(const cv::Mat& frame);
	int getClassId(const std::string& className) const;

private:
	std::string scriptPath_;
	cv::dnn::Net net_;
	std::vector<std::string> classNames_;
	float confThreshold_, nmsThreshold_;
};
