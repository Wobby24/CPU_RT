#pragma once
#include <string>
#include <fstream>

namespace Aero::RT::Utils {
    // Helper function to read the next non-comment, non-empty line
    std::string getNextValidPPMLine(std::ifstream& file) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            size_t firstChar = line.find_first_not_of(" \t\r\n");
            if (firstChar == std::string::npos) continue; 

            if (line[firstChar] == '#') {
                continue; 
            }

            return line; 
        }
        return ""; 
    }

    std::string getNextValidPPMLine(std::fstream& file) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;

            size_t firstChar = line.find_first_not_of(" \t\r\n");
            if (firstChar == std::string::npos) continue;

            if (line[firstChar] == '#') {
                continue;
            }

            return line;
        }
        return "";
    }
}