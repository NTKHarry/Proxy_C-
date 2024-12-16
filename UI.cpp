#include <iostream>
#include <windows.h>
#include <unordered_map>
#include <string>
using namespace std;
// 
HWND hTextbox3, hSearchBar, hButtonAdd, hButtonStart, hButtonStop;

// Khai báo unordered_map để lưu các dòng đã được thêm vào
unordered_map<string, bool> addedStrings;

//   
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_COMMAND: {
        // Kiểm tra nếu nút Add (ID của hButtonAdd) được nhấn
        if ((HWND)lp == hButtonAdd) {
            // Lấy văn bản từ ô tìm kiếm (hSearchBar)
            int len = GetWindowTextLength(hSearchBar) + 1;
            char *text = new char[len];
            GetWindowTextA(hSearchBar, text, len);

            // Kiểm tra xem văn bản đã tồn tại trong unordered_map chưa
            string textStr = text;
            if (addedStrings.find(textStr) == addedStrings.end()) {
                // Nếu chưa tồn tại, thêm vào unordered_map và vào hTextbox3
                addedStrings[textStr] = true;

                // Lấy văn bản hiện tại trong ô hTextbox3
                int currentTextLength = GetWindowTextLength(hTextbox3) + 1;
                char *currentText = new char[currentTextLength];
                GetWindowTextA(hTextbox3, currentText, currentTextLength);

                // Tạo chuỗi mới, nối văn bản hiện tại và văn bản mới
                string newText = currentText;
                newText += "\r\n";  // Thêm dấu xuống dòng
                newText += textStr;   // Thêm văn bản từ ô tìm kiếm

                // Đưa văn bản mới vào ô textbox bên phải (hTextbox3)
                SetWindowTextA(hTextbox3, newText.c_str());

                // Giải phóng bộ nhớ đã cấp phát
                delete[] text;
                delete[] currentText;
            }
            else {
                // Nếu văn bản đã tồn tại trong unordered_map, không làm gì cả
                cout << "The path was existed!" << endl;
                delete[] text;
            }
        }

        if ((HWND)lp == hButtonStart) {
            const char* cppPath = ".\\proxy_server.cpp";
            const char* exePath = ".\\proxy_server.exe";
            
            // Compile file cpp thành file exe
            string compileCommand = "g++ -o " + (string)exePath + " " + (string)cppPath + "-lws2_32";
            STARTUPINFOA si = { sizeof(si) };
            PROCESS_INFORMATION pi;

            // Chạy lệnh compile
            if (CreateProcessA(
                NULL, // No module name (use command line)
                (LPSTR)compileCommand.c_str(), // Command line
                NULL, // Process handle not inheritable
                NULL, // Thread handle not inheritable
                FALSE, // Set handle inheritance to FALSE
                CREATE_NO_WINDOW, // No creation flags
                NULL, // Use parent's environment block
                NULL, // Use parent's starting directory
                &si, // Pointer to STARTUPINFO structure
                &pi // Pointer to PROCESS_INFORMATION structure
            )) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            // Chạy file exe
            STARTUPINFOA siExe = { sizeof(STARTUPINFOA) };
            PROCESS_INFORMATION piExe;

            if (CreateProcessA(
                NULL, // No module name (use command line)
                (LPSTR)exePath, // Command line
                NULL, // Process handle not inheritable
                NULL, // Thread handle not inheritable
                FALSE, // Set handle inheritance to FALSE
                0, // No creation flags
                NULL, // Use parent's environment block
                NULL, // Use parent's starting directory
                &siExe, // Pointer to STARTUPINFO structure
                &piExe // Pointer to PROCESS_INFORMATION structure
            )) {
                CloseHandle(piExe.hProcess);
                CloseHandle(piExe.hThread);
            }
            else {
                MessageBoxA(hwnd, "Cannot run the exe file", "Error", MB_OK | MB_ICONERROR);
            }
        } 
        else {
            MessageBoxA(hwnd, "Cannot compile the cpp file", "Error", MB_OK | MB_ICONERROR);
        }
        break;
    }

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

int main() {
    const wchar_t *g_szClassName = L"myWindowClass";

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = g_szClassName;
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowW(
        g_szClassName, L"Window with Textboxes and Buttons", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 500, NULL, NULL, wc.hInstance, NULL);

    if (hwnd == NULL) {
        return 0;
    }

    // Lấy kích thước cửa sổ để tính toán vị trí các ô
    RECT rect;
    GetClientRect(hwnd, &rect);

    // Định nghĩa vị trí và kích thước các textbox
    int leftWidth = rect.right * 5 / 10; // 50% chiều rộng cửa sổ cho phần bên trái
    int leftHeight = rect.bottom / 3;    // Chiều cao mỗi textbox là 1/3 chiều cao của cửa sổ
    int rightWidth = 200;                // Chiều rộng cố định cho ô bên phải (textbox 3)

    // Chiều cao của ô bên phải được căn chỉnh sao cho đáy dưới của nó ngang hàng với đáy dưới của 2 ô bên trái
    int rightHeight = leftHeight * 2; // Chiều cao của ô bên phải = 2 lần chiều cao của ô bên trái

    // Tạo 2 ô textbox ở phần bên trái
    HWND hTextbox1 = CreateWindowW(L"EDIT", L"",
                                   WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY,
                                   30, 50, leftWidth - 40, leftHeight - 20, hwnd, (HMENU)1, wc.hInstance, NULL);

    HWND hTextbox2 = CreateWindowW(L"EDIT", L"",
                                   WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY,
                                   30, 50 + leftHeight, leftWidth - 40, leftHeight - 20, hwnd, (HMENU)2, wc.hInstance, NULL);

    // Tạo 1 ô textbox ở phần bên phải, với chiều ngang 200 và chiều cao đã điều chỉnh
    hTextbox3 = CreateWindowW(L"EDIT", L"",
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY,
                              leftWidth + 20, 50, 180, rightHeight - 20, hwnd, (HMENU)3, wc.hInstance, NULL);

    // Tạo một textbox nằm ngang (giống search bar) ngay trên nút Add
    int searchBarHeight = 30; // Chiều cao của search bar
    hSearchBar = CreateWindowW(L"EDIT", L"Search...",
                               WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE,
                               (rect.right - 300) / 2, rect.bottom - 115, 300, searchBarHeight, hwnd, NULL, wc.hInstance, NULL);

    // Đẩy nút Add sang góc phải
    hButtonAdd = CreateWindowW(L"BUTTON", L"Add",
                               WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_BORDER,
                               rect.right - 120, rect.bottom - 75, 100, 30, hwnd, (HMENU)100, wc.hInstance, NULL);

    // Tạo màu nền xám cho cửa sổ
    HBRUSH hbrGray = CreateSolidBrush(RGB(211, 211, 211)); // Màu xám nhạt
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hbrGray);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
