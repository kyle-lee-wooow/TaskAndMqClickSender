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
    int addContent;//0不需要  1增加  2替换{}
};

std::vector<ButtonInfo> buttons = {
    {L"启动", L"TYPE:2|COMMAND:START_TASK|TASK_ID:0",0},
    {L"停止", L"TYPE:2|COMMAND:STOP_TASK|TASK_ID:0",0},
    {L"协助Bdsm", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/协助 Bdsm",0},
    {L"跟随Bdsm", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/follow Bdsm",0},
    {L"停止移动", L"TYPE:1|KEY_ID:87",0},
    //{L"离开小队", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run LeaveParty()",0},
    {L"向前奔跑", L"TYPE:1|KEY_ID:144",0}, 
    {L"清除目标", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run  ClearTarget()",0},
    {L"上马下马", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run UseItemByName(\"驯鹿缰绳\")",0},
   
    {L"鞠躬", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/bow",0},
    {L"鼓掌", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/applause",0},
    {L"喝彩", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/applaud",0},
    {L"确定交易", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run AcceptTrade()",0},
    {L"炉石", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run UseItemByName(\"炉石\")",0},

    {L"对话", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/interact",0},
    {L"buff-吃肉", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run UseItemByName(\"魔法甜面包\")",0},
    {L"buff-喝水", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run UseItemByName(\"魔法晶水\")",0},
    {L"buff-甲", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run local c=UnitClass(\"player\") if c==\"牧师\" then CastSpellByName(\"心灵之火\") else if c==\"法师\" then CastSpellByName(\"魔甲术\") end end",0},
    {L"buff-智耐", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run local c=UnitClass(\"player\") if c==\"牧师\" then CastSpellByName(\"真言术：韧\") else if c==\"法师\" then CastSpellByName(\"奥术智慧\") end end",0},
    {L"buff-盾", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run local c=UnitClass(\"player\") if c==\"牧师\" then CastSpellByName(\"真言术：盾\") else if c==\"法师\" then CastSpellByName(\"法力护盾\") end end",0},
    //{L"团队跟随", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run for i=1,GetNumRaidMembers() do if UnitIsUnit(\"player\", \"raid\"..i) then FollowUnit(\"raid\"..(i-1)) break end end",0},
    //{L"小队跟随", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run for i=1,GetNumPartyMembers() do if UnitIsUnit(\"player\", \"party\"..i) then FollowUnit(\"party\"..(i-1)) break end end",0},
    {L"左键点中间", L"TYPE:4|COMMAND:LIFT_CLICK|POSITION:CENTER",0},
    {L"右键点中间", L"TYPE:4|COMMAND:RIGHT_CLICK|POSITION:CENTER",0},
    //{L"接受组队", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/run local f=CreateFrame(\"Frame\") f:RegisterEvent(\"PARTY_INVITE_REQUEST\") f:SetScript(\"OnEvent\",function() AcceptGroup(); StaticPopup_Hide(\"PARTY_INVITE\") end)",0},

    {L"---分割线---", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:1",0},

    {L"选中按键+", L"TYPE:2|COMMAND:CHECK_CHECKBOX|CHECKBOX_ID:",1},
    {L"取消按键+", L"TYPE:2|COMMAND:UNCHECK_CHECKBOX|CHECKBOX_ID:",1},
    {L"通用喊话+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:",1},
    {L"跟随目标+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/follow ",1},
    {L"指定目标+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/target ",1},
    {L"协助队友+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/协助 ",1},
    {L"升为队长+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/pr ",1},
    {L"使用物品+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/RUN UseItemByName(\"{}\")",2},
    {L"释放技能+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/RUN CastSpellByName(\"{}\")",2},
    {L"交互选择+", L"TYPE:2|COMMAND:WOW_SAY|CONTENT:/RUN SelectGossipOption({})",2},

    {L"组合按键+", L"TYPE:3|KEYS_ID:",1}
};
std::map<int, const ButtonInfo*> commandMap;

std::map<int, HWND> ButtonInfoinputControlsEditListHWNDMap;




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