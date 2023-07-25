#include "Window.h"
#include "App.h"
//#include "VideoPlayer.h"



LRESULT CALLBACK WinProc(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam);
void CreateMenu(HWND hwnd);
void HandleMenuMessage(HWND hwnd, WPARAM wparam);
void OpenFile(HWND hwnd);

Window::Window() {
  this->CreateWnd();


  /*if (FAILED(hr)) {
    return;
  }*/
}

void Window::CreateWnd() {
  const size_t height = 600;
  const size_t width = 800;
  WNDCLASS wc = {0};
  wc.style = CS_OWNDC;
  wc.lpfnWndProc = WinProc;
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.lpszClassName = "DX11Tutorial";
  RegisterClass(&wc);

  m_hwnd = CreateWindow("DX11Tutorial", "Render Picture",
                        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, 100,
                        100, width, height, nullptr, nullptr, nullptr, nullptr);

  // Create and attach the menu to the window
  //CreateMenu(m_hwnd);
}

void CreateMenu(HWND hwnd) {
  HMENU hMenu = CreateMenu();
  HMENU hSubMenu = CreatePopupMenu();
  AppendMenu(hSubMenu, MF_STRING, 1, "Open File");
  AppendMenu(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hSubMenu), "File");

  // Attach the menu to the window
  SetMenu(hwnd, hMenu);
}

LRESULT CALLBACK WinProc(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) {
  switch (msg) {
    case WM_DESTROY:
    case WM_CLOSE:
      PostQuitMessage(0);
      return 0;

    case WM_COMMAND:
      HandleMenuMessage(handle, wparam);
      return 0;
  }

  return DefWindowProc(handle, msg, wparam, lparam);
}

void HandleMenuMessage(HWND hwnd, WPARAM wparam) {
  switch (LOWORD(wparam)) {
    case 1:  // Open File option
        ///TODO: app enum
      OpenFile(hwnd);
      break;

    default:
      break;
  }
}

void OpenFile(HWND hwnd) {
  OPENFILENAMEW ofn;
  wchar_t szFileName[MAX_PATH] = {0};  

  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";  
                                                
  ofn.lpstrFile = szFileName;  
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

  if (GetOpenFileNameW(&ofn)) {
    
  }
}

