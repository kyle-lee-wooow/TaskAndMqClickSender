#pragma once
#include <windows.h>
#include <string>
#include <stdexcept>

class CFG {
private:
    std::string filePath;  // 配置文件路径

public:
    // 构造函数，指定配置文件路径
    explicit CFG(const std::string& path) : filePath(path) {}

    // 读取字符串类型的值
    std::string getString(const std::string& section, const std::string& key, const std::string& defaultValue = "") const {
        char buffer[256] = { 0 };  // 缓冲区，大小根据需求调整
        DWORD result = GetPrivateProfileStringA(
            section.c_str(),
            key.c_str(),
            defaultValue.c_str(),
            buffer,
            sizeof(buffer),
            filePath.c_str()
        );
        return std::string(buffer);
    }

    // 读取整数类型的值
    int getInt(const std::string& section, const std::string& key, int defaultValue = 0) const {
        return GetPrivateProfileIntA(section.c_str(), key.c_str(), defaultValue, filePath.c_str());
    }

    // 设置字符串类型的值
    void setString(const std::string& section, const std::string& key, const std::string& value) const {
        if (!WritePrivateProfileStringA(section.c_str(), key.c_str(), value.c_str(), filePath.c_str())) {
            throw std::runtime_error("Failed to write to INI file.");
        }
    }

    // 设置整数类型的值
    void setInt(const std::string& section, const std::string& key, int value) const {
        setString(section, key, std::to_string(value));
    }

    // 检查文件是否存在
    bool fileExists() const {
        DWORD fileAttr = GetFileAttributesA(filePath.c_str());
        return (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));
    }
};
