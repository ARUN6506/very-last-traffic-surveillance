#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

#include "HelmetDetector.h"
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>

HelmetDetector::HelmetDetector(const std::string& scriptPath, const std::string& weightsPath)
    : scriptPath_(scriptPath), weightsPath_(weightsPath) {}

std::vector<HelmetDetector::Detection> HelmetDetector::detect(const cv::Mat& frame) {
    const std::string tmpPath = "temp_frame.jpg";
    cv::imwrite(tmpPath, frame);

    std::ostringstream cmdStream;
    cmdStream << "python3 " << scriptPath_ << " " << tmpPath << " " << weightsPath_;
    std::string cmd = cmdStream.str();

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("Failed to run helmet detection script");
    }

    std::string output;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        output += buffer;
    }

    int closeResult = pclose(pipe);
    if (closeResult != 0) {
        std::cerr << "pclose failed with code: " << closeResult << std::endl;
    }

    remove(tmpPath.c_str());

    std::vector<HelmetDetector::Detection> detections;
    try {
        auto jsonDet = nlohmann::json::parse(output);
        for (auto& item : jsonDet) {
            auto b = item["bbox"];
            detections.push_back({
                cv::Rect(b[0].get<int>(), b[1].get<int>(), b[2].get<int>() - b[0].get<int>(), b[3].get<int>() - b[1].get<int>()),
                item["confidence"].get<float>()
                });
        }
    }
    catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
    }

    return detections;
}