// TaskAndMqClick.cpp : 定义应用程序的入口点。

#include "framework.h"
#include <windows.h>
#include <string>
#include <commctrl.h>
#include <thread>
#include <vector>
#include <string> 
#include <commdlg.h>  // 用于文件选择对话框
#include <sstream>
#include <iostream>
#include <locale>  // 必须包含这个头文件
#include <uxtheme.h>
#include "TaskAndMqClickSender.h"
#include "cfg.h"

#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "comctl32.lib")

#define MAX_LOADSTRING 100
#define ID_LISTBOX 101
#define HOTKEY_ID 102  

#define CHECKBOX_BASE_ID 200  
#define EDIT_B_BASE_ID 300    
#define EDIT_C_BASE_ID 400    



#define TIMER_ID 1
#define INTERVAL_MS 500  // 定时器触发间隔时间

// 现在可以同时执行多少个按键
#define L_TASK_COUNTS 10

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND hListBox;
HWND TaskStartBtn;
HWND TaskStopBtn;
HWND hCheckBoxes[L_TASK_COUNTS];
HWND hEditB[L_TASK_COUNTS];
HWND hEditC[L_TASK_COUNTS];
HWND mainHWnd;
HWND inputs1;

//本地文件配置
std::unique_ptr<CFG> l_config;  // 使用 unique_ptr


// 用来存储每一行的下次执行时间
std::chrono::steady_clock::time_point nextExecutionTimes[L_TASK_COUNTS];

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//单个案件
void sendKeyToListBoxs(int key);
void sendKeyToListBoxsDown(int key);
void sendKeyToListBoxsUp(int key);

HBRUSH hGreenBrush, hRedBrush; // 按钮背景色画刷



bool isTaskRunning = false;  // 任务是否正在执行
std::thread taskThread;      // 任务线程

bool remoteCtrlisTaskRunning = false;  // 远控任务是否正在执行

//处理远程控制事件
void withRemoteCtrlHandler(const std::string& message);


//多个案件
void sendMultipleKeysToListBoxs(const std::wstring& keyCombo);
std::vector<BYTE> ParseKeyCombo(const std::wstring& keyCombo);

//多按键发送
std::vector<std::vector<BYTE>> ParseMultipleKeyCombos(const std::wstring& keyCombos);

INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

//批量登录 
INT_PTR CALLBACK BATCH_LOGINS(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

//选择本地文件
void SelectFile(HWND hEdit);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TASKANDMQCLICK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TASKANDMQCLICK));
    MSG msg;
    l_config = std::make_unique<CFG>("./TaskAndMqClick.ini");  // 智能指针初始化

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }


    return (int)msg.wParam;
}



// 启动任务A的函数
void StartTaskA(HWND hWnd) {
    SetFocus(hWnd);
    if(isTaskRunning == false){
     
        isTaskRunning = true;

        // 强制重绘窗口，触发WM_PAINT消息
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
    }
}

// 停止任务A的函数
void StopTaskA(HWND hWnd) {
    SetFocus(hWnd);
    if (isTaskRunning == true) {
        isTaskRunning = false;
        
        std::fill(std::begin(nextExecutionTimes), std::end(nextExecutionTimes), std::chrono::steady_clock::time_point{});

        // 强制重绘窗口，触发WM_PAINT消息
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
    }
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TASKANDMQCLICK));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TASKANDMQCLICK);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


int setTheme(HWND hWnd) {
    SetWindowTheme(hWnd, L"Explorer", NULL);
    //SetWindowTheme(hWnd, L"Windows", NULL);
    return 1;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    //HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        //CW_USEDEFAULT, 0, 600, 400, nullptr, nullptr, hInstance, nullptr);

    mainHWnd = CreateWindowW(szWindowClass, szTitle,
        WS_OVERLAPPEDWINDOW | WS_TABSTOP,  // 确保支持键盘输入
        CW_USEDEFAULT, CW_USEDEFAULT, 700, 500,
        nullptr, nullptr, hInstance, nullptr);


    if (!mainHWnd)
    {
        return FALSE;
    }

    


    ShowWindow(mainHWnd, nCmdShow);
    UpdateWindow(mainHWnd);

    //美化
    setTheme(mainHWnd);
    setTheme(hListBox);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        // 在窗口右侧增加10行，每行包含：
     // - 勾选框 A
     // - 文字 "每隔"
     // - 输入框 B（只能输入数字，单位：秒）
     // - 文字 "秒执行按键"
     // - 输入框 C（输入键盘按键）

    case WM_CREATE:
    { 

        // 创建启动按钮
        TaskStartBtn = CreateWindowW(L"BUTTON", L"启动按键", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            400, 320, 80, 30, hWnd, (HMENU)1001, hInst, NULL);

        // 创建停止按钮
        TaskStopBtn = CreateWindowW(L"BUTTON", L"停止按键", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            500, 320, 80, 30, hWnd, (HMENU)1002, hInst, NULL);


        //输入框
        inputs1 = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
            20, 20, 150, 30, hWnd, (HMENU)(1003), hInst, NULL);

        // 选中
        TaskStartBtn = CreateWindowW(L"BUTTON", L"选中", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            100, 320, 80, 30, hWnd, (HMENU)1004, hInst, NULL);

        // 取消选中
        TaskStopBtn = CreateWindowW(L"BUTTON", L"取消选中", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            200, 320, 80, 30, hWnd, (HMENU)1005, hInst, NULL);

        // 喊话
        TaskStopBtn = CreateWindowW(L"BUTTON", L"喊话", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            300, 320, 80, 30, hWnd, (HMENU)1006, hInst, NULL);



    }
    break;



    case WM_KEYDOWN:
    {
        sendKeyToListBoxs(wParam);
    }
    break;

    case WM_HOTKEY:
       
        break;

    case WM_LBUTTONDOWN:  // 处理鼠标左键点击事件
    {
         
        break;
    }


    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam); 
        TCHAR buffer[256];
        std::string msg;

        switch (wmId)
        {
        case 1001: // 启动按钮点击
            StartTaskA(hWnd);
            msg = "TYPE:2|COMMAND:START_TASK|TASK_ID:0";
            RemoteCtrl::Conntection::getInstance().publishMessage(msg);
            break;

        case 1002: // 停止按钮点击
            StopTaskA(hWnd);
            msg = "TYPE:2|COMMAND:STOP_TASK|TASK_ID:0";
            RemoteCtrl::Conntection::getInstance().publishMessage(msg);
            break;

        case 1004: // 选中
            GetWindowText(inputs1, buffer, sizeof(buffer) / sizeof(TCHAR));
            msg = "TYPE:2|COMMAND:CHECK_CHECKBOX|CHECKBOX_ID:";
            msg += TCHARToString(buffer);
            RemoteCtrl::Conntection::getInstance().publishMessage(msg);
            break;

        case 1005: // 取消选择
            GetWindowText(inputs1, buffer, sizeof(buffer) / sizeof(TCHAR));
            msg = "TYPE:2|COMMAND:UNCHECK_CHECKBOX|CHECKBOX_ID:";
            msg += TCHARToString(buffer);
            RemoteCtrl::Conntection::getInstance().publishMessage(msg);
            break;

        case 1006: // 喊话
            GetWindowText(inputs1, buffer, sizeof(buffer) / sizeof(TCHAR));
            msg = "TYPE:2|COMMAND:WOW_SAY|CONTENT:";
            msg += TCHARToString(buffer);
            RemoteCtrl::Conntection::getInstance().publishMessage(msg);
            break;

        case ID_REMOTE_CTRL_RUN:{
            if (!l_config->fileExists()) { break; }
            remoteCtrlisTaskRunning = true;
            std::string mqaddr = l_config->getString("mqtt", "SERVER_ADDRESS");
            std::string name = l_config->getString("mqtt", "MQ_NAME");
            std::string MQ_PWD = l_config->getString("mqtt", "MQ_PWD");

            RemoteCtrl::Conntection::getInstance().setMQTTParam(mqaddr, name, MQ_PWD);

            // 在新线程中启动 loading()
            std::thread([]() {
               
                RemoteCtrl::Conntection::getInstance().loading(withRemoteCtrlHandler);
                }).detach();  // 使用 detach 保证线程独立
            break;
        }
        case ID_REMOTE_CTRL_STOP:
            remoteCtrlisTaskRunning = false;
            RemoteCtrl::Conntection::getInstance().stopMqtt();
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;


    case WM_PAINT:
       
        HDC hdc;
        PAINTSTRUCT ps;
        RECT rect;

        // 获取设备上下文
        hdc = BeginPaint(hWnd, &ps);

        // 获取窗口的大小
        GetClientRect(hWnd, &rect);


        // 根据isrun的值绘制圆圈
        if (isTaskRunning)
        {
            // 绘制绿色圆圈
            HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0)); // 绿色
            SelectObject(hdc, greenBrush);
            Ellipse(hdc, 540, 375, 560, 395);

            DeleteObject(greenBrush);
        }
        else
        {
            // 绘制红色圆圈
            HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0)); // 红色
            SelectObject(hdc, redBrush);
            Ellipse(hdc, 540, 375, 560, 395);

            DeleteObject(redBrush);
        }

        // 结束绘制
        EndPaint(hWnd, &ps);
        return 0;

    case WM_DESTROY:
        UnregisterHotKey(hWnd, HOTKEY_ID);
        PostQuitMessage(0);
        break;


    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void sendMultipleKeysToListBoxs(const std::wstring& keyCombos)
{
    std::vector<std::vector<BYTE>> allKeyCombos = ParseMultipleKeyCombos(keyCombos);

    for (const auto& keyCombo : allKeyCombos) // 逐个执行每个按键组合
    {
        if (!keyCombo.empty())
        { 


               // 按下所有按键
            for (BYTE key : keyCombo) {
                sendKeyToListBoxsDown(key);

            } 

    

            // **逆序** 松开所有按键（后按下的先释放）
            for (auto it = keyCombo.rbegin(); it != keyCombo.rend(); ++it) {
                sendKeyToListBoxsUp(*it);
            }
            



            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 短暂延迟防止冲突
        }
    }
}

// 解析多个按键组合
std::vector<std::vector<BYTE>> ParseMultipleKeyCombos(const std::wstring& keyCombos)
{
    std::vector<std::vector<BYTE>> allKeyCombos;
    size_t startPos = 0, endPos = 0;

    while ((endPos = keyCombos.find(L",", startPos)) != std::wstring::npos)
    {
        std::wstring singleCombo = keyCombos.substr(startPos, endPos - startPos);
        allKeyCombos.push_back(ParseKeyCombo(singleCombo)); // 解析每个组合
        startPos = endPos + 1;
    }

    allKeyCombos.push_back(ParseKeyCombo(keyCombos.substr(startPos))); // 解析最后一个
    return allKeyCombos;
}



std::vector<BYTE> ParseKeyCombo(const std::wstring& keyCombo)
{
    std::vector<BYTE> keys;
    std::wistringstream stream(keyCombo);
    std::wstring key;

    while (std::getline(stream, key, L'+')) // 以 '+' 拆分组合键
    {
        if (_wcsicmp(key.c_str(), L"Ctrl") == 0) keys.push_back(VK_CONTROL);
        else if (_wcsicmp(key.c_str(), L"Alt") == 0) keys.push_back(VK_MENU);
        else if (_wcsicmp(key.c_str(), L"Space") == 0) keys.push_back(VK_SPACE);
        else if (_wcsicmp(key.c_str(), L"Shift") == 0) keys.push_back(VK_SHIFT);
        else if (_wcsicmp(key.c_str(), L"Tab") == 0) keys.push_back(VK_TAB);
        else if (_wcsicmp(key.c_str(), L"Enter") == 0) keys.push_back(VK_RETURN);
        else if (_wcsicmp(key.c_str(), L"Esc") == 0) keys.push_back(VK_ESCAPE);
        else if (_wcsicmp(key.c_str(), L"-") == 0) keys.push_back(VK_OEM_MINUS);
        else if (_wcsicmp(key.c_str(), L"+") == 0) keys.push_back(VK_OEM_PLUS);
        else if (_wcsicmp(key.c_str(), L"F1") == 0) keys.push_back(VK_F1);
        else if (_wcsicmp(key.c_str(), L"F2") == 0) keys.push_back(VK_F2);
        else if (_wcsicmp(key.c_str(), L"F3") == 0) keys.push_back(VK_F3);
        else if (_wcsicmp(key.c_str(), L"F4") == 0) keys.push_back(VK_F4);
        else if (_wcsicmp(key.c_str(), L"F5") == 0) keys.push_back(VK_F5);
        else if (_wcsicmp(key.c_str(), L"F6") == 0) keys.push_back(VK_F6);
        else if (_wcsicmp(key.c_str(), L"F7") == 0) keys.push_back(VK_F7);
        else if (_wcsicmp(key.c_str(), L"F8") == 0) keys.push_back(VK_F8);
        else if (_wcsicmp(key.c_str(), L"F9") == 0) keys.push_back(VK_F9);
        else if (_wcsicmp(key.c_str(), L"F10") == 0) keys.push_back(VK_F10);
        else if (_wcsicmp(key.c_str(), L"F11") == 0) keys.push_back(VK_F11);
        else if (_wcsicmp(key.c_str(), L"F12") == 0) keys.push_back(VK_F12);
        else if (key.length() == 1) keys.push_back(VkKeyScan(key[0])); // 普通字符
    }

    return keys;
}


void sendKeyToListBoxsDown(int key)
{
    // 执行按键发送
    if (key != '\0')
    {
        int count = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
        for (int j = 0; j < count; j++)
        {
            WCHAR buffer[100];
            SendMessage(hListBox, LB_GETTEXT, j, (LPARAM)buffer);

            UINT windowID;
            if (swscanf_s(buffer, L"Window ID: 0x%08X", &windowID) == 1)
            {
                HWND targetWindow = (HWND)windowID;
                if (IsWindow(targetWindow))
                {
                    PostMessage(targetWindow, WM_KEYDOWN, (WPARAM)key, 0); 
                }
            }
        }
    }
}


void sendKeyToListBoxsUp(int key)
{
    // 执行按键发送
    if (key != '\0')
    {
        int count = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
        for (int j = 0; j < count; j++)
        {
            WCHAR buffer[100];
            SendMessage(hListBox, LB_GETTEXT, j, (LPARAM)buffer);

            UINT windowID;
            if (swscanf_s(buffer, L"Window ID: 0x%08X", &windowID) == 1)
            {
                HWND targetWindow = (HWND)windowID;
                if (IsWindow(targetWindow))
                {
                   
                    PostMessage(targetWindow, WM_KEYUP, (WPARAM)key, 0);
                }
            }
        }
    }
}

void sendKeyToListBoxs(int key)
{
    // 执行按键发送
    if (key != '\0')
    {
        int count = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
        for (int j = 0; j < count; j++)
        {
            WCHAR buffer[100];
            SendMessage(hListBox, LB_GETTEXT, j, (LPARAM)buffer);

            UINT windowID;
            if (swscanf_s(buffer, L"Window ID: 0x%08X", &windowID) == 1)
            {
                HWND targetWindow = (HWND)windowID;
                if (IsWindow(targetWindow))
                {
                    PostMessage(targetWindow, WM_KEYDOWN, (WPARAM)key, 0);
                    PostMessage(targetWindow, WM_KEYUP, (WPARAM)key, 0);
                }
            }
        }
    }
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

 

// 选择文件并填入输入框
void SelectFile(HWND hEdit)
{
    OPENFILENAME ofn;
    wchar_t szFile[MAX_PATH] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hEdit;
    ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.TXT\0";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrTitle = L"选择文件";

    if (GetOpenFileName(&ofn))
    {
        SetWindowText(hEdit, szFile);  // 将文件路径填充到输入框
    }
}

// mqtt_msg
void withRemoteCtrlHandler(const std::string& message) {
     
}