#pragma once
#include <windows.h>
#include <string>
#include <stdexcept>

class CFG {
private:
    std::string filePath;  // �����ļ�·��

public:
    // ���캯����ָ�������ļ�·��
    explicit CFG(const std::string& path) : filePath(path) {}

    // ��ȡ�ַ������͵�ֵ
    std::string getString(const std::string& section, const std::string& key, const std::string& defaultValue = "") const {
        char buffer[256] = { 0 };  // ����������С�����������
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

    // ��ȡ�������͵�ֵ
    int getInt(const std::string& section, const std::string& key, int defaultValue = 0) const {
        return GetPrivateProfileIntA(section.c_str(), key.c_str(), defaultValue, filePath.c_str());
    }

    // �����ַ������͵�ֵ
    void setString(const std::string& section, const std::string& key, const std::string& value) const {
        if (!WritePrivateProfileStringA(section.c_str(), key.c_str(), value.c_str(), filePath.c_str())) {
            throw std::runtime_error("Failed to write to INI file.");
        }
    }

    // �����������͵�ֵ
    void setInt(const std::string& section, const std::string& key, int value) const {
        setString(section, key, std::to_string(value));
    }

    // ����ļ��Ƿ����
    bool fileExists() const {
        DWORD fileAttr = GetFileAttributesA(filePath.c_str());
        return (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));
    }
};
