#pragma comment(lib, "Msimg32.lib")
#include <iostream>
#include <string>
#include <windows.h>
#include "classi.h"
#include <typeinfo>
#include "funzioni.h"
#include <dwmapi.h>
#include <wingdi.h>
#pragma comment(lib, "Dwmapi.lib")
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")


using namespace std;
menu m;

const wchar_t CLASS_NAME[] = L"Vanish";
HBITMAP hBitmap = NULL;

struct CustomButton {
    RECT rect;
    const wchar_t* text;
    bool hover;
    bool visible;
};

// ID degli elementi
#define BTN_LOGIN_SCREEN 101
#define BTN_FUNZIONE2 102
#define BTN_FUNZIONE3 103
#define BTN_LOGIN      201
#define BTN_CANCEL     202
#define EDIT_USERNAME  301
#define EDIT_PASSWORD  302

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void MostraSchermataLogin(HWND hwnd);
void SetWindowAppearance(HWND hwnd);
void MostraSchermataIniziale(HWND hwnd);
void CenterWindow(HWND hwnd);
void DrawCustomButton(HDC hdc, const CustomButton& button);
void MostraStart(HWND hwnd);
void MostraSchermataLicense(HWND hwnd);

static HWND btnLoginScreen, btnFunzione2, btnFunzione3;
static HWND editUser, editPass;
static HWND licenseUser, licensePass;

bool hoverLogin = false, hoverCancel = false;
bool loginMode = false;
bool loadingMode = false;

void DrawGradientBackground(HDC hdc, RECT rect) {
    TRIVERTEX vertex[2];
    GRADIENT_RECT gRect;

    vertex[0] = { rect.left, rect.top, 0x3200, 0x3200, 0x3200, 0x0000 };

    vertex[1] = { rect.right, rect.bottom, 0x0000, 0x0000, 0x0000, 0x0000 };

    gRect.UpperLeft = 0;
    gRect.LowerRight = 1;

    GradientFill(hdc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
}

void SetWindowAppearance(HWND hwnd) {
    MARGINS margins = { 5, 5, 5, 5 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    BYTE opacity = 220;
    SetLayeredWindowAttributes(hwnd, 0, opacity, LWA_ALPHA);
}

void OpenDebugConsole() {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);

}

void CenterWindow(HWND hwnd) {
    RECT rect;
    GetWindowRect(hwnd, &rect);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int posX = (screenWidth - width) / 2;
    int posY = (screenHeight - height) / 2;

    SetWindowPos(hwnd, NULL, posX, posY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

HWND hProgressBar = NULL;
CustomButton ProgressBar = { {50, 380, 300, 400}, L"", false, false };

void ShowProgressBar(HWND hwnd) {
    if (!hProgressBar) {
        hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, NULL,
            WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
            ProgressBar.rect.left, ProgressBar.rect.top,
            ProgressBar.rect.right - ProgressBar.rect.left,
            ProgressBar.rect.bottom - ProgressBar.rect.top,
            hwnd, NULL, GetModuleHandle(NULL), NULL);

        SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
        SendMessage(hProgressBar, PBM_SETSTEP, (WPARAM)10, 0);

        SendMessage(hProgressBar, PBM_SETBARCOLOR, 0, (LPARAM)RGB(255, 0, 0));

        ProgressBar.visible = true;
    }
}

void UpdateProgressBar(int value) {
    SendMessage(hProgressBar, PBM_SETPOS, value, 0);
}

void HideProgressBar() {
    ProgressBar.visible = false;
    DestroyWindow(hProgressBar);
    hProgressBar = NULL;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    //OpenDebugConsole();
    hosts();
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = NULL;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST, CLASS_NAME, L"Vanish",
        WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 350, 450, NULL, NULL, hInstance, NULL
    );

    MostraStart(hwnd);

    if (hwnd == NULL) {
        return 0;
    }

    SetWindowAppearance(hwnd);
    CenterWindow(hwnd);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


CustomButton Start = { {75, 100, 275, 150}, L"Start", false, false };
CustomButton Logout = { {75, 200, 275, 250}, L"Exit", false, false };


void DrawBackgroundImage(HDC hdc, RECT rect) {
    if (!hBitmap) {
        MessageBox(NULL, L"Errore: hBitmap è NULL!", L"Debug", MB_OK | MB_ICONERROR);
        return;
    }

    HDC hdcMem = CreateCompatibleDC(hdc);
    SelectObject(hdcMem, hBitmap);

    BITMAP bitmap;
    GetObject(hBitmap, sizeof(bitmap), &bitmap);

    SetStretchBltMode(hdc, HALFTONE);

    int imgWidth = rect.right;
    int imgHeight = rect.bottom;

    StretchBlt(hdc, 0, 0, imgWidth, imgHeight,
        hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

    DeleteDC(hdcMem);
}

HFONT CreateModernFont(int size, bool bold = false) {
    return CreateFont(
        size, 0, 0, 0,
        bold ? FW_BOLD : FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI"
    );
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {


        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        DrawGradientBackground(hdc, ps.rcPaint);

        DrawCustomButton(hdc, Start);
        DrawCustomButton(hdc, Logout);
        

        DrawCustomButton(hdc, ProgressBar);


        EndPaint(hwnd, &ps);
        return 0;
    }



    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wParam;
        SetTextColor(hdcEdit, RGB(0, 180, 255));
        SetBkColor(hdcEdit, RGB(30, 30, 30));
        return (LRESULT)CreateSolidBrush(RGB(30, 30, 30));
    }

    case WM_LBUTTONDOWN: {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        

        if (PtInRect(&Start.rect, pt) && Start.visible) {
            ShowProgressBar(hwnd);

            system("cls");
            //-------tempo-----------
            changeSystemTime();

            UpdateProgressBar(10);
            cout << "10%" << endl;
            //-------artefatti-----------
            
            
            system("cls");
            UpdateProgressBar(20);
            cout << "20%" << endl;
            //-------cronologia-----------
            
            discordcache();
            cleandns();
            windef();

            system("cls");
            UpdateProgressBar(40);
            cout << "40%" << endl;
            //-------object-----------

            cleanevtx();
            cleanregedit();
            cleancron();
            amcache();

            system("cls");
            UpdateProgressBar(60);
            cout << "60%" << endl;
            //-------HISTORY-----------

            cleanhistory();
            cleanvarious();
            DPSClean();
            RestartSvc();
            CleanMemory();
            
            system("cls");
            UpdateProgressBar(70);
            cout << "70%" << endl;
            //-------EVASION-----------
            Journal();
            filestemp();
            Shadows();
            DeleteBam();
            Prefetch();
            
            system("cls");
            UpdateProgressBar(80);
            cout << "80%" << endl;
            //-------RESET--------------

            rsttime();
            
            system("cls");
            UpdateProgressBar(90);
            cout << "90%" << endl;

            //-------CLEANUP-----------

            POPJournal();
            Events();
            security();

            system("cls");
            UpdateProgressBar(100);
            cout << "100%" << endl;
            HideProgressBar();
            exit(1);
            break;
        }
        if (Logout.visible && PtInRect(&Logout.rect, pt))
        {
            PostQuitMessage(0);
            return 0;
        }
        break;
        break;
    }

    case WM_NCHITTEST: {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        ScreenToClient(hwnd, &pt);

        CustomButton* buttons[] = { &Start, &Logout };
        for (CustomButton* button : buttons) {
            if (button->visible && PtInRect(&button->rect, pt)) {
                return HTCLIENT;
            }
        }

        return HTCAPTION;
    }

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_MOUSEMOVE: {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        bool hoverChanged = false;

        CustomButton* buttons[] = { &Start, &Logout };
        for (CustomButton* button : buttons) {
            if (button->visible) {
                bool isHovering = PtInRect(&button->rect, pt);
                if (button->hover != isHovering) {
                    button->hover = isHovering;
                    hoverChanged = true;
                }
            }
        }

        if (hoverChanged) {
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;
    }

    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void DrawCustomButton(HDC hdc, const CustomButton& button) {
    if (!button.visible) return;

    HWND hProgressBar = NULL;
    CustomButton ProgressBar = { {50, 380, 300, 400}, L"", false, false };
    bool loadingMode = false;

    COLORREF buttonColor = button.hover ? RGB(165, 165, 165) : RGB(255, 255, 255);
    COLORREF borderColor = RGB(100, 100, 100);

    HPEN hPen = CreatePen(PS_SOLID, 1, borderColor);
    SelectObject(hdc, hPen);

    HFONT hModernFont = CreateModernFont(18, true);
    SelectObject(hdc, hModernFont);

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, button.rect.right - button.rect.left, button.rect.bottom - button.rect.top);
    SelectObject(hdcMem, hBitmap);

    HBRUSH hBrush = CreateSolidBrush(buttonColor);
    SelectObject(hdcMem, hBrush);

    RoundRect(hdcMem, 0, 0, button.rect.right - button.rect.left, button.rect.bottom - button.rect.top, 8, 8);

    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 230, AC_SRC_ALPHA };
    AlphaBlend(hdc, button.rect.left, button.rect.top, button.rect.right - button.rect.left, button.rect.bottom - button.rect.top,
        hdcMem, 0, 0, button.rect.right - button.rect.left, button.rect.bottom - button.rect.top, blend);

    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, TRANSPARENT);

    RECT textRect = button.rect;
    DrawText(hdc, button.text, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    DeleteObject(hBrush);
    DeleteObject(hPen);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
}

void MostraStart(HWND hwnd) {

    Start.visible = true;
    Logout.visible = true;

    InvalidateRect(hwnd, NULL, TRUE);
}

