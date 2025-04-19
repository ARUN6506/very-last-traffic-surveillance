cmake_minimum_required(VERSION 3.20)

project(TrafficSurveillanceSystem)

set(CMAKE_CXX_STANDARD 17)

set(VCPKG_ROOT "C:/vcpkg")

# Set VCPKG toolchain
if(DEFINED ENV{VCPKG_ROOT} AND NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "VCPKG Toolchain File")
    message(STATUS "Using VCPKG toolchain from environment variable: $ENV{VCPKG_ROOT}")
elseif(EXISTS "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" AND NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "VCPKG Toolchain File")
    message(STATUS "Using VCPKG toolchain at default location: C:/vcpkg")
elseif(NOT CMAKE_TOOLCHAIN_FILE)
    message(WARNING "VCPKG toolchain file not found. Please set the VCPKG_ROOT environment variable or CMAKE_TOOLCHAIN_FILE manually.")
endif()


file(GLOB_RECURSE SOURCES
    "src/main.cpp"
    "src/DBManager.cpp"
    "src/HelmetDetector.cpp"
    "src/LicensePlateDetector.cpp"
    "src/NetworkServer.cpp"
    "src/VehicleDetector.cpp"
)

add_executable(TrafficSurveillanceSystem ${SOURCES}) 

# Link with vcpkg packages
find_package(Tesseract REQUIRED)
message(STATUS "Tesseract found: ${Tesseract_LIBRARIES}")
find_package(SQLite3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Set include and source directories
target_include_directories(TrafficSurveillanceSystem PRIVATE 
${OpenCV_INCLUDE_DIRS}
${Tesseract_INCLUDE_DIRS}
${PROJECT_SOURCE_DIR}/include
)

set(Tesseract_LIBRARIES "${VCPKG_ROOT}/installed/x64-windows/lib/tesseract55.lib")



target_link_libraries(TrafficSurveillanceSystem
    PRIVATE
    ${Tesseract_LIBRARIES}
    SQLite::SQLite3
    nlohmann_json::nlohmann_json
    ${OpenCV_LIBS}
    Boost::boost
)