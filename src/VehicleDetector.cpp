#include "VehicleDetector.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

VehicleDetector::VehicleDetector(const std::string& scriptPath)
    : scriptPath_(scriptPath) {}

std::vector<Detection> VehicleDetector::detect(const cv::Mat& frame) {
    static int frameCounter = 0;
    std::string imagePath = "temp_frame_" + std::to_string(frameCounter++) + ".jpg";
    cv::imwrite(imagePath, frame);

    std::string command = "python \"" + scriptPath_ + "\" \"" + imagePath + "\"";
    std::string result;
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) return {};

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }
    _pclose(pipe);

    std::vector<Detection> detections;
    std::istringstream iss(result);
    std::string line;
    while (std::getline(iss, line)) {
        int x, y, w, h, cls;
        float conf;
        if (sscanf(line.c_str(), "%d %d %d %d %d %f", &x, &y, &w, &h, &cls, &conf) == 6) {
            detections.push_back({ cv::Rect(x, y, w, h), cls, conf });
        }
    }
    return detections;
}

int VehicleDetector::getClassId(const std::string& className) const {
    if (className == "motorbike") return 0; // This should match the Python label index
    return -1;
}