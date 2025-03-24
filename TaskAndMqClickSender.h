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
    {L"��������", L"TYPE:2|COMMAND:START_TASK|TASK_ID:0",0},
    {L"ֹͣ����", L"TYPE:2|COMMAND:STOP_TASK|TASK_ID:0",0},
    {L"Э��Bdsm", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/Э�� Bdsm",0},
    {L"����Bdsm", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/follow Bdsm",0},
    {L"ֹͣ����", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/unfollow",0},
    {L"�뿪С��", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run LeaveParty()",0},
    {L"��ǰ�ƶ�", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run MoveForwardStart()",0},
    {L"ֹͣ�ƶ�", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run MoveForwardStop()",0},
    {L"���Ŀ��", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run  ClearTarget()",0},
    {L"�������", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run local f=CreateFrame(\"Frame\") f:RegisterEvent(\"PARTY_INVITE_REQUEST\") f:SetScript(\"OnEvent\",function() AcceptGroup(); StaticPopup_Hide(\"PARTY_INVITE\") end)",0},

    {L"---------", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:1",0},

    {L"ѡ������+", L"TYPE:2|COMMAND:CHECK_CHECKBOX|CHECKBOX_ID:",1},
    {L"ȡ������+", L"TYPE:2|COMMAND:UNCHECK_CHECKBOX|CHECKBOX_ID:",1},
    {L"ͨ�ú���+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:",1},
    {L"����Ŀ��+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/follow ",1},
    {L"ָ��Ŀ��+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/target ",1},
    {L"Э������+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/Э�� ",1},

    {L"��ϰ���+", L"TYPE:3|KEYS_ID:",1}
};
std::map<int, const ButtonInfo*> commandMap;






std::string WstringToString(const std::wstring& wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed, NULL, NULL);
    str.pop_back(); // ȥ���ַ���ĩβ�Ŀ��ַ�
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
    // ��ȡת������ַ���
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);

    // ����һ���㹻��� buffer �����ת������ַ���
    char* str = new char[len];

    // ִ��ת��
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);

    // ����ת����� std::string
    std::string result(str);

    delete[] str;  // �ǵ��ͷ��ڴ�
    return result;
}


// ��ȡ�ؼ��ı��ĺ���
std::wstring GetEditText(HWND hDlg, int controlID) {
    wchar_t szText[256];  // ������
    GetWindowText(GetDlgItem(hDlg, controlID), szText, sizeof(szText) / sizeof(wchar_t));
    return std::wstring(szText);
}

// ���ߺ������� TCHAR תΪ std::string
//std::string TCHARToString(const TCHAR* buffer) {
//    std::string msg;
//#ifdef UNICODE
//    // ����� Unicode��ת��Ϊ std::wstring ��ת��Ϊ std::string
//    std::wstring wstr(buffer);
//    msg.assign(wstr.begin(), wstr.end());
//#else
//    // ����� ANSI��ֱ�Ӹ�ֵ
//    msg = buffer;
//#endif
//    return msg;
//}

// ���ߺ������� TCHAR תΪ std::string
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