#include "framework.h"
#include "Lab2.h"
#include <cmath>
#include <cwchar>

#define MAX_LOADSTRING 100

// Глобальні змінні:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
int width = 400;                                // initial window width
int height = 400;                               // initial window height

// ---------------------------------------------------------------------
// Варіант 4: Налаштування функції та діапазону
// ---------------------------------------------------------------------
double TargetFunction(double x) {
    return std::sin(x) * std::cos(2.0 * x);
}

const double X_MIN = -10.0;
const double X_MAX = 10.0;
const int NUM_POINTS = 800;
// ---------------------------------------------------------------------

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2));
    MSG msg;

    // Main message loop:
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

//
// FUNCTION: MyRegisterClass()
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LAB2);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
// FUNCTION: InitInstance(HINSTANCE, int)
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    int cx = GetSystemMetrics(SM_CXSCREEN);
    int cy = GetSystemMetrics(SM_CYSCREEN);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        cx / 2 - width / 2, cy / 2 - height / 2, width, height,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return TRUE;
}

//
// FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_SIZE:
        width = LOWORD(lParam);
        height = HIWORD(lParam);
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        const int margin = 50;
        int graphWidth = width - 2 * margin;
        int graphHeight = height - 2 * margin;

        if (graphWidth > 0 && graphHeight > 0)
        {
            double yMin = TargetFunction(X_MIN);
            double yMax = yMin;
            double dx = (X_MAX - X_MIN) / (NUM_POINTS - 1);

            for (int i = 0; i < NUM_POINTS; ++i)
            {
                double x = X_MIN + i * dx;
                double y = TargetFunction(x);
                if (y < yMin) yMin = y;
                if (y > yMax) yMax = y;
            }

            if (std::abs(yMax - yMin) < 1e-9)
            {
                yMax += 1.0;
                yMin -= 1.0;
            }
            else
            {
                double pad = (yMax - yMin) * 0.05;
                yMin -= pad;
                yMax += pad;
            }

            HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
            HPEN oldPen = (HPEN)SelectObject(hdc, gridPen);

            Rectangle(hdc, margin, margin, margin + graphWidth, margin + graphHeight);

            int zeroX = margin + static_cast<int>((0.0 - X_MIN) / (X_MAX - X_MIN) * graphWidth);
            int zeroY = margin + graphHeight - static_cast<int>((0.0 - yMin) / (yMax - yMin) * graphHeight);

            HPEN axisPen = CreatePen(PS_SOLID, 1, RGB(120, 120, 120));
            SelectObject(hdc, axisPen);

            if (zeroX >= margin && zeroX <= margin + graphWidth)
            {
                MoveToEx(hdc, zeroX, margin, NULL);
                LineTo(hdc, zeroX, margin + graphHeight);
            }
            if (zeroY >= margin && zeroY <= margin + graphHeight)
            {
                MoveToEx(hdc, margin, zeroY, NULL);
                LineTo(hdc, margin + graphWidth, zeroY);
            }
            DeleteObject(axisPen);

            HPEN graphPen = CreatePen(PS_SOLID, 2, RGB(0, 102, 204));
            SelectObject(hdc, graphPen);

            for (int i = 0; i < NUM_POINTS; ++i)
            {
                double xVal = X_MIN + i * dx;
                double yVal = TargetFunction(xVal);

                int px = margin + static_cast<int>((xVal - X_MIN) / (X_MAX - X_MIN) * graphWidth);
                int py = margin + graphHeight - static_cast<int>((yVal - yMin) / (yMax - yMin) * graphHeight);

                if (i == 0)
                    MoveToEx(hdc, px, py, NULL);
                else
                    LineTo(hdc, px, py);
            }

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 0, 0));

            WCHAR buf[64];
            swprintf_s(buf, L"x_min = %.2f", X_MIN);
            TextOutW(hdc, margin, margin + graphHeight + 5, buf, (int)wcslen(buf));

            swprintf_s(buf, L"x_max = %.2f", X_MAX);
            TextOutW(hdc, margin + graphWidth - 80, margin + graphHeight + 5, buf, (int)wcslen(buf));

            swprintf_s(buf, L"y_max = %.2f", yMax);
            TextOutW(hdc, 5, margin, buf, (int)wcslen(buf));

            swprintf_s(buf, L"y_min = %.2f", yMin);
            TextOutW(hdc, 5, margin + graphHeight - 15, buf, (int)wcslen(buf));

            SelectObject(hdc, oldPen);
            DeleteObject(gridPen);
            DeleteObject(graphPen);
        }

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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