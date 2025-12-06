#include "framework.h"
#include "complot_raclette.h"
#include <windows.h>
#include <tchar.h>
#include <vector>
#include <commdlg.h>
#define ID_BUTTON_CREER    101
#define ID_BUTTON_QUITTER  102

#define ID_MENU_CHARGER    201
#define ID_MENU_QUITTER    202
#define ID_MENU_APROPOS    203

using namespace std;

// --- Variables globales ---
int g_width = 2100;
int g_height = 1200;
int g_colorIndex = 0;
HBRUSH g_hBrush = NULL;

// Pour le déplacement de la fenêtre
static BOOL g_bDragging = FALSE;
static POINT g_ptOffset;

// Pour le dessin avec la souris
vector<POINT> g_points;
BOOL g_bDrawing = FALSE;

// Handles pour les contrôles
HWND hEdit;




HMENU Mebar;
HMENU MeFile;
HMENU MeHelp;

HBITMAP hBitmap = (HBITMAP)LoadImage(
    NULL, L"C:\\Users\\avaesken\\Pictures\\raclette_1080p.bmp",
    IMAGE_BITMAP, 0, 0,
    LR_LOADFROMFILE | LR_CREATEDIBSECTION

);

void PaintBitmap(HDC hdc, HBITMAP hBmp) {
    HDC hdcMem = CreateCompatibleDC(hdc);

    // Sauvegarde l’ancien bitmap avant de mettre le nouveau
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBmp);

    BITMAP bmp;
    GetObject(hBmp, sizeof(BITMAP), &bmp);

    // Copie le bitmap dans la fenêtre
    BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcMem, 0, 0, SRCCOPY);

    // Restauration de l’ancien bitmap
    SelectObject(hdcMem, hOldBmp);

    DeleteDC(hdcMem);
}

// --- Fenêtre callback ---
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        g_hBrush = CreateSolidBrush(RGB(240, 240, 240));
        SetWindowText(hwnd, _T("Fenêtre initiale"));



        // Créer les contrôles (Niveau 4)
        hEdit = CreateWindowEx(0, _T("EDIT"), _T(""),
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            20, 20, 200, 25, hwnd, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        CreateWindowEx(0, _T("BUTTON"), _T("Créer"),
            WS_CHILD | WS_VISIBLE,
            20, 60, 80, 25, hwnd, (HMENU)ID_BUTTON_CREER, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

        CreateWindowEx(0, _T("BUTTON"), _T("Quitter"),
            WS_CHILD | WS_VISIBLE,
            120, 60, 80, 25, hwnd, (HMENU)ID_BUTTON_QUITTER, ((LPCREATESTRUCT)lParam)->hInstance, NULL);



      

        Mebar = CreateMenu();
        MeFile = CreateMenu();
        MeHelp = CreateMenu();

        AppendMenu(MeFile, MF_STRING, ID_MENU_CHARGER, _T("Charger une image: "));
        AppendMenu(MeFile, MF_STRING, ID_MENU_QUITTER, _T("Quitter. "));
        AppendMenu(MeHelp, MF_STRING, ID_MENU_APROPOS, _T("A propos. "));
    
        AppendMenu(Mebar, MF_POPUP, (UINT_PTR)MeFile, _T("Fichier "));
        AppendMenu(Mebar, MF_POPUP, (UINT_PTR)MeHelp, _T("Aide "));

        SetMenu(hwnd, Mebar);


        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        if (hBitmap) {
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBitmap);

            BITMAP bmp;
            GetObject(hBitmap, sizeof(BITMAP), &bmp);

            // Récupère la taille de la fenêtre
            RECT rc;
            GetClientRect(hwnd, &rc);

            // Étire l’image pour remplir la fenêtre
            SetStretchBltMode(hdc, HALFTONE);
            StretchBlt(
                hdc,
                0, 0, rc.right, rc.bottom,   // destination : toute la fenêtre
                hdcMem,
                0, 0, bmp.bmWidth, bmp.bmHeight, // source : taille originale du BMP
                SRCCOPY
            );

            // Restaure l’ancien bitmap
            SelectObject(hdcMem, hOldBmp);
            DeleteDC(hdcMem);
        }

        EndPaint(hwnd, &ps);
    } break;

    case WM_ERASEBKGND:{
        return 0; 
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam)){
        case ID_BUTTON_CREER:
        {
            
            TCHAR buffer[256];
            GetWindowText(hEdit, buffer, 256);
            MessageBox(hwnd, buffer, _T("Texte Saisi"), MB_OK);
        }
        break;

       case ID_BUTTON_QUITTER:

            PostQuitMessage(0);
            break;

        case ID_MENU_CHARGER: // Charger une image
    {
        OPENFILENAME ofn;
        TCHAR szFile[MAX_PATH] = _T("");

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = _T("Bitmap Files/0*.bmp/0All Files/0*.*/0");
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        if (GetOpenFileName(&ofn)) {
            // Libérer l’ancien bitmap si nécessaire
            if (hBitmap) {
                DeleteObject(hBitmap);
            }

            // Charger le nouveau
            hBitmap = (HBITMAP)LoadImage(
                NULL, ofn.lpstrFile,
                IMAGE_BITMAP, 0, 0,
                LR_LOADFROMFILE | LR_CREATEDIBSECTION
            );

            if (!hBitmap) {
                MessageBox(hwnd, _T("Impossible de charger l'image"), _T("Erreur"), MB_OK);
            }
            else {
                InvalidateRect(hwnd, NULL, TRUE); // redessiner la fenêtre
            }
        }
    }
    break;
        case ID_MENU_APROPOS:
            MessageBox(hwnd, _T("Juste un complot fromager. "), _T("A propos"), MB_OK);
            break;
        }
    }
    break;
        
    
    case WM_DESTROY:
        if (g_hBrush)
            DeleteObject(g_hBrush);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// --- Point d'entrée ---
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = _T("MaFenetre");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        _T("Fenêtre initiale"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        g_width, g_height,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
