#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

class IniReader {
public:
    IniReader(const std::string& filename) {
        loadFile(filename);
    }

    std::string getValue(const std::string& section, const std::string& key) {
        // 查找特定部分和键的值
        if (config.find(section) != config.end()) {
            auto& sectionMap = config[section];
            if (sectionMap.find(key) != sectionMap.end()) {
                return sectionMap[key];
            }
        }
        return "";
    }

private:
    std::map<std::string, std::map<std::string, std::string>> config;

    void loadFile(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        std::string currentSection;

        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        while (getline(file, line)) {
            // 去掉行两端的空格
            trim(line);

            // 忽略空行和注释行
            if (line.empty() || line[0] == ';') {
                continue;
            }

            // 检测是否是一个部分 [Section]
            if (line[0] == '[') {
                currentSection = parseSection(line);
            }
            else {
                // 处理键值对
                std::pair<std::string, std::string> keyValue = parseKeyValue(line);
                if (!currentSection.empty()) {
                    config[currentSection][keyValue.first] = keyValue.second;
                }
            }
        }

        file.close();
    }

    void trim(std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        size_t last = str.find_last_not_of(" \t\r\n");
        if (first == std::string::npos || last == std::string::npos)
            str = "";
        else
            str = str.substr(first, (last - first + 1));
    }

    std::string parseSection(const std::string& line) {
        size_t first = line.find_first_of("[");
        size_t last = line.find_last_of("]");
        if (first != std::string::npos && last != std::string::npos && first < last) {
            return line.substr(first + 1, last - first - 1);
        }
        return "";
    }

    std::pair<std::string, std::string> parseKeyValue(const std::string& line) {
        size_t pos = line.find("=");
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            trim(key);
            trim(value);
            return { key, value };
        }
        return { "", "" };
    }
};

//int sdftest() {
//    IniReader reader("config.ini");
//
//    // 读取并打印某个部分的某个键的值
//    std::string value = reader.getValue("Database", "host");
//    std::cout << "Database host: " << value << std::endl;
//
//    value = reader.getValue("Database", "port");
//    std::cout << "Database port: " << value << std::endl;
//
//    return 0;
//}
