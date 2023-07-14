#include "log.h"
#include <iostream>
#include <fstream>
#include <ctime>


#ifdef _WIN32
#include <direct.h> // Include the appropriate header for Windows
#else
#include <sys/stat.h> // Include the appropriate header for Unix-like systems
#endif


void log(const std::string& message) {
    std::string mainfolder = "logs";

    std::ofstream log_file("log.txt", std::ios_base::app);

#ifdef _WIN32
   int result = _mkdir(mainfolder.c_str()); // Create the Books folder on Windows
#else
    result = mkdir(folderName.c_str(), 0700); // Create the Books folder on Unix-like systems
#endif
    if (log_file.is_open()) {
        std::time_t now = std::time(nullptr);
        std::tm time_info;
        localtime_s(&time_info, &now);
        char timestamp[20];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &time_info);
        log_file << timestamp << " " << message << std::endl;
        log_file.close();
    }
    else {
        std::cerr << "Error: could not open log file" << std::endl;
    }
}