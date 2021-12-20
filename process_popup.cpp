#include <iostream>
#include <Windows.h>
#include <Commctrl.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <algorithm>
#include <vector>
#include <ctime>


int pop_up_width, pop_up_height;
using namespace std;

int pop_up_padding = 5;
int size_of_font = 18;
HFONT windows_95_font = CreateFont(size_of_font, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("System"));

vector<vector<string>> get_processes_information()
{
    // get list of processes
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    vector<vector<string>> processes_information;
    int total_processes_to_show = 30;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return {};
    }
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);
        return {};
    }
    do
    {
        HANDLE hProcess;
        PROCESS_MEMORY_COUNTERS pmc;
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (hProcess == NULL)
        {
            //do nothing if process is not opened
            continue;
        }
        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
        {
            int memory_kbs = (int)(pmc.WorkingSetSize / 1024);
            string process_name = pe32.szExeFile;
            DWORD process_id = pe32.th32ProcessID;
            vector<string> process_info;          
            process_info.push_back(process_name);
            process_info.push_back(to_string(memory_kbs));
            process_info.push_back(to_string(process_id));
            processes_information.push_back(process_info);
            process_info.clear();

        
        }
        CloseHandle(hProcess);
    } while (Process32Next(hProcessSnap, &pe32));
    CloseHandle(hProcessSnap);

    // sort processes by memory
    sort(processes_information.begin(), processes_information.end(), [](vector<string> a, vector<string> b) { return stoi(a[1]) > stoi(b[1]); });
    //slice this vector to show only top 20 processes
    processes_information.erase(processes_information.begin() + total_processes_to_show, processes_information.end());
    return processes_information;
}



LRESULT WINAPI WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_CREATE:
    {
        break;
    }
    //paint the popup to salmon
    case WM_TIMER:
    {

        int cell_width = 100;
        int column_2_distance = 100;
        int column_3_distance = 200;
        //adds a box
        HDC hdcWindow = GetDC(hWnd);
        RECT rect;
        GetClientRect(hWnd, &rect);
        HBRUSH salmon = CreateSolidBrush(RGB(255, 127, 90));
        COLORREF dark_blue = RGB(20, 0, 80);
        FillRect(hdcWindow, &rect, salmon);
        //font for the text
        SelectObject(hdcWindow, windows_95_font);
        SetTextColor(hdcWindow, dark_blue);
        SetBkMode(hdcWindow, TRANSPARENT);
        //add text
        auto processes_information = get_processes_information();
        // SelectObject(hdcWindow, windows_95_font);
        // SetTextColor(hdcWindow, dark_blue);
        // SetBkMode(hdcWindow, TRANSPARENT);
        // DrawText(hdcWindow, TEXT("Processes"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
        for(int i = 0; i < processes_information.size(); i++)
        {
            int line_height = size_of_font - 2;
            RECT text_rect = { pop_up_padding,  //left
                               (i * (line_height)), //top
                               pop_up_padding + cell_width, //right
                               (i * (line_height)) + line_height}; //bottom
            DrawText(hdcWindow, TEXT(processes_information[i][0].c_str()), -1, &text_rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            // draw the text of memory usage right after the process name
            text_rect.left = pop_up_padding + column_2_distance;
            text_rect.right = pop_up_padding + column_2_distance + cell_width;
            DrawText(hdcWindow, TEXT((processes_information[i][1] + " KB").c_str()), -1, &text_rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            // draw the text of process id right after the memory usage
            text_rect.left = pop_up_padding + column_3_distance;
            text_rect.right = pop_up_padding + column_3_distance + cell_width;
            DrawText(hdcWindow, TEXT(("ID: " + processes_information[i][2]).c_str()), -1, &text_rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        }
        DeleteObject(salmon);
        DeleteObject(hdcWindow);
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdcWindow = BeginPaint(hWnd, &ps);
        RECT rect;
        GetClientRect(hWnd, &rect);
        HBRUSH salmon = CreateSolidBrush(RGB(255, 127, 90));
        COLORREF dark_blue = RGB(20, 0, 80);
        FillRect(hdcWindow, &rect, salmon);
        DeleteObject(salmon);
        DeleteObject(hdcWindow);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_CLOSE:
    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }
        return 0;
    }
    return DefWindowProc(hWnd, Msg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
    int screen_width = GetSystemMetrics(SM_CXSCREEN),
        screen_height = GetSystemMetrics(SM_CYSCREEN);

    int padding = 30;
    int left_padding = padding;
    int top_padding = padding;
    int right_padding = padding;
    int bottom_padding = padding;

    pop_up_width = 400;
    pop_up_height = 400;

    //salmon brush HBRUSH
    HBRUSH salmon = CreateSolidBrush(RGB(255, 127, 90));
    //salmon pen HPEN
    HPEN salmon_pen = CreatePen(PS_SOLID, 1, RGB(255, 127, 90));
    WNDCLASS wndClass = {0, WndProc, 0, 0, hInstance, NULL, LoadCursor(NULL, IDC_ARROW), 0, NULL, "popup_process"};
    if (!RegisterClass(&wndClass))
        return MessageBox(HWND_DESKTOP, "Cannot register class!", NULL, MB_ICONERROR | MB_OK);
    HWND hWnd = CreateWindowEx(WS_EX_TOPMOST, "popup_process", "popup_process", WS_POPUP,
        screen_width - pop_up_width - right_padding, screen_height / 2 - pop_up_height / 2, pop_up_width, pop_up_height, NULL, NULL, hInstance, NULL);
    //start timer
    SetTimer(hWnd, 1, 1000, NULL);

    if (!hWnd)
        return MessageBox(HWND_DESKTOP, "Cannot create window!", NULL, MB_ICONERROR | MB_OK);
    MSG Msg = {0};
    //show & update window
    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);

    while (Msg.message != WM_QUIT)
    {
        if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            DestroyWindow(hWnd);
    }
    return 0;
}
