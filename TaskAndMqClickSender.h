#pragma once
 
#include "resource.h"
#include "batch_login.h"
#undef max
#undef min
#include "with_mqtt.h"
#include "MsgParse.h"

struct ButtonInfo {
    std::wstring name;
    std::wstring command;
    int addContent;
};

std::vector<ButtonInfo> buttons = {
    {L"启动按键", L"TYPE:2|COMMAND:START_TASK|TASK_ID:0",0},
    {L"停止按键", L"TYPE:2|COMMAND:STOP_TASK|TASK_ID:0",0},
    {L"协助Bdsm", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/协助 Bdsm",0},
    {L"跟随Bdsm", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/follow Bdsm",0},
    {L"停止跟随", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/unfollow",0},
    {L"离开小队", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run LeaveParty()",0},
    {L"向前移动", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run MoveForwardStart()",0},
    {L"停止移动", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run MoveForwardStop()",0},
    {L"清除目标", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run  ClearTarget()",0},
    {L"接受组队", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run local f=CreateFrame(\"Frame\") f:RegisterEvent(\"PARTY_INVITE_REQUEST\") f:SetScript(\"OnEvent\",function() AcceptGroup(); StaticPopup_Hide(\"PARTY_INVITE\") end)",0},

    {L"---------", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:1",0},

    {L"选中任务+", L"TYPE:2|COMMAND:CHECK_CHECKBOX|CHECKBOX_ID:",1},
    {L"取消任务+", L"TYPE:2|COMMAND:UNCHECK_CHECKBOX|CHECKBOX_ID:",1},
    {L"通用喊话+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:",1},
    {L"跟随目标+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/follow ",1},
    {L"指定目标+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/target ",1},
    {L"协助队友+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/协助 ",1},

    {L"组合按键+", L"TYPE:3|KEYS_ID:",1}
};
std::map<int, const ButtonInfo*> commandMap;






std::string WstringToString(const std::wstring& wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, NULL, NULL);
    str.pop_back(); // 去掉字符串末尾的空字符
    return str;
}

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