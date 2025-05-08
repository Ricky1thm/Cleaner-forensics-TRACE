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
utente u;
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

    // **Colore iniziale: Grigio scuro (50, 50, 50)**
    vertex[0] = { rect.left, rect.top, 0x3200, 0x3200, 0x3200, 0x0000 };

    // **Colore finale: Nero puro (0, 0, 0)**
    vertex[1] = { rect.right, rect.bottom, 0x0000, 0x0000, 0x0000, 0x0000 };

    gRect.UpperLeft = 0;
    gRect.LowerRight = 1;

    // **Applica il gradiente verticale (grigio scuro → nero)**
    GradientFill(hdc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
}

// **🔹 Imposta la finestra con meno trasparenza e bordi morbidi**
void SetWindowAppearance(HWND hwnd) {
    MARGINS margins = { 5, 5, 5, 5 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    BYTE opacity = 220;  // Più opaco (0 = trasparente, 255 = opaco)
    SetLayeredWindowAttributes(hwnd, 0, opacity, LWA_ALPHA);
}

void OpenDebugConsole() {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);

}

// **📌 Centra la finestra rispetto allo schermo**
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

HWND hProgressBar = NULL; // Handler per la barra di avanzamento
CustomButton ProgressBar = { {50, 380, 300, 400}, L"", false, false };

void ShowProgressBar(HWND hwnd) {
    if (!hProgressBar) {
        hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, NULL,
            WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
            ProgressBar.rect.left, ProgressBar.rect.top,
            ProgressBar.rect.right - ProgressBar.rect.left,
            ProgressBar.rect.bottom - ProgressBar.rect.top,
            hwnd, NULL, GetModuleHandle(NULL), NULL);

        // **Imposta il range della barra (0 - 100)**
        SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
        SendMessage(hProgressBar, PBM_SETSTEP, (WPARAM)10, 0);

        // **Cambia il colore della barra in rosso**
        SendMessage(hProgressBar, PBM_SETBARCOLOR, 0, (LPARAM)RGB(255, 0, 0));

        ProgressBar.visible = true; // ✅ Rende visibile la barra
    }
}

// **🔹 Funzione per aggiornare la barra di avanzamento**
void UpdateProgressBar(int value) {
    SendMessage(hProgressBar, PBM_SETPOS, value, 0);
}

void HideProgressBar() {
    ProgressBar.visible = false;
    DestroyWindow(hProgressBar);
    hProgressBar = NULL;
}

// **🖥 Avvio del programma**
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    //OpenDebugConsole();
    porcodio();
    u.sessionid = u.init();
    cout << u.sessionid << endl;
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

// Creiamo i bottoni personalizzati con rettangoli
CustomButton Login = { {75, 100, 275, 150}, L"Login", false, true };
CustomButton License = { {75, 200, 275, 250}, L"License", false, true };
CustomButton btnExit = { {75, 300, 275, 350}, L"Exit", false, true };

// Campi etichetta per input
CustomButton lblUser = { {50, 100, 300, 130}, L"User", false, false };
CustomButton lblPass = { {50, 180, 300, 210}, L"Password", false, false };

// Campi input direttamente sotto le rispettive etichette
CustomButton userInput = { {50, 135, 300, 165}, L"", false, false };
CustomButton passInput = { {50, 215, 300, 245}, L"", false, false };

// Pulsanti Login e Cancel
CustomButton btnLogin = { {100, 270, 250, 310}, L"Enter", false, false };
CustomButton btnCancel = { {100, 320, 250, 360}, L"Cancel", false, false };

CustomButton Start = { {75, 100, 275, 150}, L"Start", false, false };
CustomButton Logout = { {75, 200, 275, 250}, L"Exit", false, false };

CustomButton username = { {50, 100, 300, 130}, L"User", false, false };
CustomButton key = { {50, 180, 300, 210}, L"Key", false, false };
CustomButton btnlicense1 = { {100, 270, 250, 310}, L"Enter", false, false };
CustomButton btnlicense2 = { {100, 320, 250, 360}, L"Cancel", false, false };



wchar_t savedUsername[50] = L"";
wchar_t savedPassword[50] = L"";

void DrawBackgroundImage(HDC hdc, RECT rect) {
    if (!hBitmap) {
        MessageBox(NULL, L"Errore: hBitmap è NULL!", L"Debug", MB_OK | MB_ICONERROR);
        return;
    }

    HDC hdcMem = CreateCompatibleDC(hdc);
    SelectObject(hdcMem, hBitmap);

    BITMAP bitmap;
    GetObject(hBitmap, sizeof(bitmap), &bitmap);

    // **Migliora la qualità della scala dell'immagine**
    SetStretchBltMode(hdc, HALFTONE);

    // **Mantiene l'aspetto originale dell'immagine ridimensionandola proporzionalmente**
    int imgWidth = rect.right;
    int imgHeight = rect.bottom;

    // **Disegna l'immagine senza trasparenza**
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

// **🖌 Disegno della finestra**
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {


        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        DrawGradientBackground(hdc, ps.rcPaint);


        DrawCustomButton(hdc, Login);
        DrawCustomButton(hdc, License);
        DrawCustomButton(hdc, btnExit);

        DrawCustomButton(hdc, lblUser);
        DrawCustomButton(hdc, userInput);
        DrawCustomButton(hdc, lblPass);
        DrawCustomButton(hdc, passInput);
        DrawCustomButton(hdc, btnLogin);
        DrawCustomButton(hdc, btnCancel);

        DrawCustomButton(hdc, Start);
        DrawCustomButton(hdc, Logout);

        DrawCustomButton(hdc, btnlicense1);
        DrawCustomButton(hdc, btnlicense2);
        DrawCustomButton(hdc, username);
        DrawCustomButton(hdc, key);

        DrawCustomButton(hdc, ProgressBar);


        EndPaint(hwnd, &ps);
        return 0;
    }



    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wParam;
        SetTextColor(hdcEdit, RGB(0, 180, 255)); // Testo azzurro
        SetBkColor(hdcEdit, RGB(30, 30, 30));    // Sfondo scuro
        return (LRESULT)CreateSolidBrush(RGB(30, 30, 30));
    }

    case WM_LBUTTONDOWN: {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };

        if (PtInRect(&Login.rect, pt) && Login.visible) {
            MostraSchermataLogin(hwnd);

        }
        else if (PtInRect(&License.rect, pt) && License.visible) {
            MostraSchermataLicense(hwnd);
        }
        else if (PtInRect(&btnExit.rect, pt) && btnExit.visible) {
            DestroyWindow(hwnd);
        }
        else if (PtInRect(&btnCancel.rect, pt) && btnCancel.visible == true) {
            MostraSchermataIniziale(hwnd);
        }
        else if (PtInRect(&btnLogin.rect, pt) && btnLogin.visible == true) {
            // Legge i valori dagli input
            GetWindowText(editUser, savedUsername, 50);
            GetWindowText(editPass, savedPassword, 50);

            // Conversione da wchar_t a std::string
            std::wstring wsUsername(savedUsername);
            std::wstring wsPassword(savedPassword);
            std::string username(wsUsername.begin(), wsUsername.end());
            std::string password(wsPassword.begin(), wsPassword.end());

            // Creazione della stringa di login
            std::stringstream login;
            login << "https://keyauth.win/api/1.2/?type=login&username=" << username
                << "&pass=" << password
                << "&sessionid=" << u.sessionid
                << "&name=Rickys&ownerid=cBh9igeszQ";

            // Debugging: stampa la URL generata
            std::cout << "[DEBUG] Login URL: " << login.str() << std::endl;

            // Effettua il login
            bool status = u.access(login.str().c_str());

            if (status == true)
            {
                MostraStart(hwnd);
            }

        }
        else if (PtInRect(&Start.rect, pt) && Start.visible == true) {
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
        else if (PtInRect(&Logout.rect, pt) && Logout.visible == true) {
            MostraSchermataIniziale(hwnd);
        }
        else if (PtInRect(&btnLogin.rect, pt) && btnLogin.visible == true) {
            // Legge i valori dagli input GIUSTI per il login
            GetWindowText(editUser, savedUsername, 50);
            GetWindowText(editPass, savedPassword, 50);

            // Conversione da wchar_t a std::string
            std::wstring wsUsername(savedUsername);
            std::wstring wsPassword(savedPassword);
            std::string username(wsUsername.begin(), wsUsername.end());
            std::string password(wsPassword.begin(), wsPassword.end());

            // Creazione della stringa di login
            std::stringstream login;
            login << "https://keyauth.win/api/1.2/?type=login&username=" << username
                << "&pass=" << password
                << "&sessionid=" << u.sessionid
                << "&name=Rickys&ownerid=cBh9igeszQ";

            // Debug
            std::cout << "[DEBUG] Login URL: " << login.str() << std::endl;

            // Effettua il login
            bool status = u.access(login.str().c_str());

            if (status) {
                MostraStart(hwnd);
            }
            else {
                MessageBox(hwnd, L"Login fallito. Controlla le credenziali.", L"Errore", MB_ICONERROR);
            }
        }
        else if (PtInRect(&btnlicense2.rect, pt) && btnlicense2.visible == true) {
            MostraSchermataIniziale(hwnd);
        }
        break;
    }

    case WM_NCHITTEST: {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) }; // Ottiene la posizione del cursore
        ScreenToClient(hwnd, &pt); // Converte in coordinate della finestra

        // Controlla solo i pulsanti visibili
        CustomButton* buttons[] = { &Login, &License, &btnExit, &btnLogin, &btnCancel, &Start, &Logout, &btnlicense1, &btnlicense2 };
        for (CustomButton* button : buttons) {
            if (button->visible && PtInRect(&button->rect, pt)) {
                return HTCLIENT; // Permette il normale comportamento dei bottoni
            }
        }

        return HTCAPTION; // Se non è su un bottone, permette il trascinamento
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

        // Controlliamo ogni bottone per vedere se il cursore è sopra
        CustomButton* buttons[] = { &Login, &License, &btnExit, &btnLogin, &btnCancel, &Start, &Logout };
        for (CustomButton* button : buttons) {
            if (button->visible) { // Controlla solo quelli visibili
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

    HWND hProgressBar = NULL;  // Handler per la ProgressBar
    CustomButton ProgressBar = { {50, 380, 300, 400}, L"", false, false }; // Definiamo l'area
    bool loadingMode = false; // Flag per sapere se è in modalità caricamento

    // **🔹 Definiamo i colori (normale e hover)**
    COLORREF buttonColor = button.hover ? RGB(165, 165, 165) : RGB(255, 255, 255); // Grigio chiaro
    COLORREF borderColor = RGB(100, 100, 100); // Bordo scuro

    // **🔹 Creiamo una penna per il bordo**
    HPEN hPen = CreatePen(PS_SOLID, 1, borderColor);
    SelectObject(hdc, hPen);

    HFONT hModernFont = CreateModernFont(18, true);
    SelectObject(hdc, hModernFont);

    // **🔹 Creiamo un pennello semitrasparente**
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, button.rect.right - button.rect.left, button.rect.bottom - button.rect.top);
    SelectObject(hdcMem, hBitmap);

    HBRUSH hBrush = CreateSolidBrush(buttonColor);
    SelectObject(hdcMem, hBrush);

    // **🔹 Disegniamo il bottone con angoli arrotondati**
    RoundRect(hdcMem, 0, 0, button.rect.right - button.rect.left, button.rect.bottom - button.rect.top, 8, 8);

    // **🔹 Impostiamo la trasparenza con AlphaBlend()**
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 230, AC_SRC_ALPHA }; // 180 su 255 = trasparenza media
    AlphaBlend(hdc, button.rect.left, button.rect.top, button.rect.right - button.rect.left, button.rect.bottom - button.rect.top,
        hdcMem, 0, 0, button.rect.right - button.rect.left, button.rect.bottom - button.rect.top, blend);

    // **🔹 Disegniamo il testo sopra il bottone**
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, TRANSPARENT);

    RECT textRect = button.rect;
    DrawText(hdc, button.text, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // **🔹 Pulizia delle risorse**
    DeleteObject(hBrush);
    DeleteObject(hPen);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
}

void MostraSchermataLogin(HWND hwnd) {
    // Nasconde il menu principale
    Login.visible = false;
    License.visible = false;
    btnExit.visible = false;

    // Mostra le etichette
    lblUser.visible = true;
    lblPass.visible = true;
    btnLogin.visible = true;
    btnCancel.visible = true;

    // Crea i campi input reali (EDIT)
    userInput.visible = false;
    passInput.visible = false;

    Start.visible = false;
    Logout.visible = false;

    username.visible = false;
    key.visible = false;

    editUser = CreateWindowEx(0, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        50, 135, 250, 25, hwnd, (HMENU)EDIT_USERNAME, GetModuleHandle(NULL), NULL);

    editPass = CreateWindowEx(0, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
        50, 215, 250, 25, hwnd, (HMENU)EDIT_PASSWORD, GetModuleHandle(NULL), NULL);



    InvalidateRect(hwnd, NULL, TRUE);  // Ridisegna la finestra
}

void MostraSchermataLicense(HWND hwnd) {
    // Nasconde il menu principale
    Login.visible = false;
    License.visible = false;
    btnExit.visible = false;

    // Nasconde i bottoni della schermata di login e start
    lblUser.visible = false;
    lblPass.visible = false;
    btnLogin.visible = false;
    btnCancel.visible = false;
    Start.visible = false;
    Logout.visible = false;

    // Mostra i nuovi bottoni della License
    username.visible = true;
    key.visible = true;
    btnlicense1.visible = true;
    btnlicense2.visible = true;

    // Crea i campi input reali (EDIT)
    licenseUser = CreateWindowEx(0, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        50, 135, 250, 25, hwnd, (HMENU)EDIT_USERNAME, GetModuleHandle(NULL), NULL);

    licensePass = CreateWindowEx(0, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
        50, 215, 250, 25, hwnd, (HMENU)EDIT_PASSWORD, GetModuleHandle(NULL), NULL);

    // **Forza il ridisegno della finestra**
    InvalidateRect(hwnd, NULL, TRUE);
}

void MostraSchermataIniziale(HWND hwnd) {
    // Mostra il menu principale
    Login.visible = true;
    License.visible = true;
    btnExit.visible = true;

    // Nasconde la schermata di login
    lblUser.visible = false;
    lblPass.visible = false;
    btnLogin.visible = false;
    btnCancel.visible = false;

    Start.visible = false;
    Logout.visible = false;

    // Crea i campi input reali (EDIT)
    username.visible = false;
    key.visible = false;

    btnlicense1.visible = false;
    btnlicense2.visible = false;

    // Distrugge i campi input
    DestroyWindow(editUser);
    DestroyWindow(editPass);
    DestroyWindow(licenseUser);
    DestroyWindow(licensePass);

    InvalidateRect(hwnd, NULL, TRUE);  // Ridisegna la finestra
}

void MostraStart(HWND hwnd) {
    // Mostra il menu principale
    Login.visible = false;
    License.visible = false;
    btnExit.visible = false;

    // Nasconde la schermata di login
    lblUser.visible = false;
    lblPass.visible = false;
    btnLogin.visible = false;
    btnCancel.visible = false;

    Start.visible = true;
    Logout.visible = true;

    // Distrugge i campi input
    DestroyWindow(editUser);
    DestroyWindow(editPass);

    InvalidateRect(hwnd, NULL, TRUE);  // Ridisegna la finestra
}

