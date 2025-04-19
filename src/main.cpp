#include <opencv2/opencv.hpp>
#include <boost/asio.hpp>
#include "LicensePlateDetector.h"
#include "NetworkServer.h"
#include "VehicleDetector.h"
#include "HelmetDetector.h"
#include <nlohmann/json.hpp>
#include "DBManager.h"
#include <iostream>
#include <thread>
#include <ctime>
#include <filesystem>

int main(int argc, char** argv) {
    std::string videoPath = (argc > 1 ? argv[1] : "test_video.mp4");
    std::string dbPath = (argc > 2 ? argv[2] : "surveillance.db");
    std::string place = (argc > 3 ? argv[3] : "unlnownLocation");
    std::string userId = (argc > 4 ? argv[4] : "UnknownUser");
    int port = (argc > 5 ? std::stoi(argv[5]) : 9000);

    DBManager db(dbPath);
    if (!db.init()) return -1;

    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video file\n";
        return -1;
    }

    VehicleDetector vdet("python_scripts/vehicle_detector.py");
    HelmetDetector hdet("python_scripts/helmet_detect.py","models/Helmet-Detection-main/weights/best.pt");
    LicensePlateDetector lpd;
    int points = 100;
    int motorbikeClass = vdet.getClassId("motorbike");

    boost::asio::io_context io_context;
    NetworkServer server(io_context, port);
    std::thread netThread([&] { io_context.run(); });

    cv::Mat frame;
    int frameCount = 0;

    while (cap.read(frame)) {
        io_context.poll();
        bool violation = false;
        std::string plateNum;

        for (auto& d : vdet.detect(frame)) {
            if (d.classId == motorbikeClass) {
                cv::Mat roi = frame(d.bbox & cv::Rect(0, 0, frame.cols, frame.rows));
                auto helmets = hdet.detect(roi);
                if (helmets.empty()) {
                    violation = true;

                    // Save the ROI as an image to pass to OCR
                    std::string roiPath = "detected_plates/plate_" + std::to_string(frameCount++) + ".jpg";
                    std::filesystem::create_directories("detected_plates");
                    cv::imwrite(roiPath, roi);

                    // Call Python OCR detector
                    plateNum = lpd.detectPlate(roi);

                    // Draw on frame
                    cv::rectangle(frame, d.bbox, cv::Scalar(0, 0, 255), 2);
                    cv::putText(frame, "No Helmet!", { d.bbox.x, d.bbox.y - 10 },
                        cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
                }
            }
        }

        if (violation) {
            points -= 10;
            std::time_t t = std::time(nullptr);
            char buf[64];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
            std::string timestamp(buf);

            db.addViolation(userId, timestamp, place, plateNum, points);

            nlohmann::json j = {
                {"user", userId},
                {"time", timestamp},
                {"place", place},
                {"plate", plateNum},
                {"points", points}
            };
            server.sendMessage(j.dump());

            std::cout << "Violation: " << j.dump() << std::endl;
        }

        cv::imshow("Surveillance", frame);
        if (cv::waitKey(1) == 27) break;
    }

    io_context.stop();
    netThread.join();
    return 0;
}