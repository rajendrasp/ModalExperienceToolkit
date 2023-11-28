#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.system.h>
#include <winrt/windows.ui.xaml.hosting.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
#include <winrt/windows.ui.xaml.controls.h>
#include <winrt/Windows.ui.xaml.media.h>

using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::Foundation::Numerics;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND _hWnd;
HWND _childhWnd;
HINSTANCE _hInstance;

// Stored instance handle for use in Win32 API calls such as FindResource
HINSTANCE hInst;

#define ID_BUTTON 101

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    _hInstance = hInstance;

    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(hPrevInstance);
    WNDCLASSEX wcex;

    const wchar_t szWindowClass[] = L"MainWindowClass";

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            L"Call to RegisterClassEx failed!",
            L"Windows Desktop Guided Tour",
            NULL);

        return 1;
    }

    hInst = hInstance;

    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        L"MainWindowClass",
        L"Learn to Program Windows 32 app",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1000, 500,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            L"Call to CreateWindow failed!",
            L"Windows Desktop Guided Tour",
            NULL);

        return 1;
    }

    // Get the dimensions of the main window
    RECT rect;
    GetClientRect(hWnd, &rect);
    int buttonWidth = 100;
    int buttonHeight = 30;

    // Calculate the position to center the button
    int x = (rect.right - rect.left - buttonWidth) / 2;
    int y = (rect.bottom - rect.top - buttonHeight) / 2;


    HWND hwndButton = CreateWindow(
        L"Button",  // Predefined class; Unicode assumed 
        L"Click here",      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        x,         // x position 
        y,         // y position 
        buttonWidth,        // Button width
        buttonHeight,        // Button height
        hWnd,     // Parent window
        (HMENU)ID_BUTTON,       // callback for button clicked
        (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.

    if (!hwndButton)
    {
        MessageBox(NULL,
            L"Call to create button failed!",
            L"Windows Desktop Guided Tour",
            NULL);

        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        EnableWindow(GetParent(hwnd), TRUE);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    LPCWSTR greeting = L"Hello, Windows desktop!";
    LPCWSTR buttonClicked = L"Hi! You clicked me.";
    LPCWSTR buttonClickEvent = L"Button Click Event";
    UNREFERENCED_PARAMETER(buttonClickEvent);
    UNREFERENCED_PARAMETER(buttonClicked);
    static HWND hwndChild = NULL;
    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        TextOut(hdc,
            5, 5,
            greeting, lstrlen(greeting));
        // End application-specific layout section.

        EndPaint(hWnd, &ps);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BUTTON) {
            //MessageBox(hWnd, buttonClicked, buttonClickEvent, MB_OK);
            if (hwndChild == NULL)
            {
                // Register the child window class
                WNDCLASS wcChild = {};
                wcChild.lpfnWndProc = ChildWndProc;
                wcChild.hInstance = GetModuleHandle(NULL);
                wcChild.lpszClassName = L"ChildWindowClass";
                wcChild.hCursor = LoadCursor(NULL, IDC_ARROW);

                if (!RegisterClass(&wcChild))
                {
                    MessageBox(hWnd, L"Child Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
                    return 0;
                }

                // Create the child window
                hwndChild = CreateWindow(
                    L"ChildWindowClass", // Class name for the child window
                    L"Child Window",     // Window title
                    WS_OVERLAPPEDWINDOW, // Window style
                    CW_USEDEFAULT,       // x position
                    CW_USEDEFAULT,       // y position
                    800,                 // Window width
                    800,                 // Window height
                    hWnd,               // Parent window handle
                    NULL,               // Menu
                    GetModuleHandle(NULL),
                    NULL);              // Additional application data

                if (hwndChild == NULL)
                {
                    MessageBox(hWnd, L"Child window creation failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
                }
                else
                {
                    // Begin XAML Island section.

                    // The call to winrt::init_apartment initializes COM; by default, in a multithreaded apartment.
                    winrt::init_apartment(apartment_type::single_threaded);

                    // Initialize the XAML framework's core window for the current thread.
                    WindowsXamlManager winxamlmanager = WindowsXamlManager::InitializeForCurrentThread();

                    // This DesktopWindowXamlSource is the object that enables a non-UWP desktop application 
                    // to host WinRT XAML controls in any UI element that is associated with a window handle (HWND).
                    DesktopWindowXamlSource desktopSource;

                    // Get handle to the core window.
                    auto interop = desktopSource.as<IDesktopWindowXamlSourceNative>();

                    // Parent the DesktopWindowXamlSource object to the current window.
                    check_hresult(interop->AttachToWindow(hwndChild));

                    // This HWND will be the window handler for the XAML Island: A child window that contains XAML.  
                    HWND hWndXamlIsland = nullptr;

                    // Get the new child window's HWND. 
                    interop->get_WindowHandle(&hWndXamlIsland);

                    // Update the XAML Island window size because initially it is 0,0.
                    SetWindowPos(hWndXamlIsland, 0, 200, 100, 800, 200, SWP_SHOWWINDOW);

                    // Create the XAML content.
                    Windows::UI::Xaml::Controls::StackPanel xamlContainer;
                    xamlContainer.Background(Windows::UI::Xaml::Media::SolidColorBrush{ Windows::UI::Colors::LightGray() });

                    Windows::UI::Xaml::Controls::TextBlock tb;
                    tb.Text(L"Hello World from Xaml Islands!");
                    tb.VerticalAlignment(Windows::UI::Xaml::VerticalAlignment::Center);
                    tb.HorizontalAlignment(Windows::UI::Xaml::HorizontalAlignment::Center);
                    tb.FontSize(48);

                    xamlContainer.Children().Append(tb);
                    xamlContainer.UpdateLayout();
                    desktopSource.Content(xamlContainer);

                    // End XAML Island section.

                    // Disable the parent window when the child window is created
                    EnableWindow(hWnd, FALSE);
                    // Show the child window
                    ShowWindow(hwndChild, SW_SHOWNORMAL);
                    UpdateWindow(hwndChild);
                }
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}
