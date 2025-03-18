#pragma once
 
#include "resource.h"
#include "batch_login.h"
#undef max
#undef min
#include "with_mqtt.h"
#include "MsgParse.h"

std::wstring stringToWstring(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
    return wstr;
}

std::string WcharToString(const wchar_t* wstr)
{
    // 获取转换后的字符数
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);

    // 创建一个足够大的 buffer 来存放转换后的字符串
    char* str = new char[len];

    // 执行转换
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);

    // 返回转换后的 std::string
    std::string result(str);

    delete[] str;  // 记得释放内存
    return result;
}


// 获取控件文本的函数
std::wstring GetEditText(HWND hDlg, int controlID) {
    wchar_t szText[256];  // 缓冲区
    GetWindowText(GetDlgItem(hDlg, controlID), szText, sizeof(szText) / sizeof(wchar_t));
    return std::wstring(szText);
}

// 工具函数：将 TCHAR 转为 std::string
//std::string TCHARToString(const TCHAR* buffer) {
//    std::string msg;
//#ifdef UNICODE
//    // 如果是 Unicode，转换为 std::wstring 再转换为 std::string
//    std::wstring wstr(buffer);
//    msg.assign(wstr.begin(), wstr.end());
//#else
//    // 如果是 ANSI，直接赋值
//    msg = buffer;
//#endif
//    return msg;
//}

// 工具函数：将 TCHAR 转为 std::string
std::string TCHARToString(const TCHAR* buffer) {
#ifdef UNICODE
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
    std::string msg(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, buffer, -1, &msg[0], size_needed, nullptr, nullptr);
    return msg;
#else
    return std::string(buffer);
#endif
}