#include "Window.h"

void CreateMenu(HWND hwnd);
void HandleMenuMessage(HWND hwnd, WPARAM wparam);
void OpenFile(HWND hwnd);

//void Window::CreateWnd() {
//  const size_t height = 600;
//  const size_t width = 800;
//  WNDCLASS wc = {0};
//  wc.style = CS_OWNDC;
//  wc.lpfnWndProc = WinProc;
//  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
//  wc.lpszClassName = "DX11Tutorial";
//  RegisterClass(&wc);
//
//  m_hwnd = CreateWindow("DX11Tutorial", "Render Picture",
//                        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, 100,
//                        100, width, height, nullptr, nullptr, nullptr, nullptr);
//
//  // Create and attach the menu to the window
//  //CreateMenu(m_hwnd);
//}

bool Window::Init() {

  // Window class
  WNDCLASSEXW wcex{};
  wcex.cbSize = sizeof(wcex);
  wcex.style = CS_OWNDC;
  wcex.lpfnWndProc = &WinProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = GetModuleHandleW(nullptr);
  wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = nullptr;
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = L"VideoPlayer";
  wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
  m_wndClass = RegisterClassExW(&wcex);
  if (m_wndClass == 0) {
    return false;
  }
 
  // Place window on current screen
  POINT pos{0, 0};
  GetCursorPos(&pos);
  HMONITOR monitor = MonitorFromPoint(pos, MONITOR_DEFAULTTOPRIMARY);
  MONITORINFO monitorInfo{};
  monitorInfo.cbSize = sizeof(monitorInfo);
  GetMonitorInfoW(monitor, &monitorInfo);

  // Window
  m_hwnd = CreateWindowExW(
      WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW, (LPCWSTR)m_wndClass, L"VideoPlayer",
      WS_OVERLAPPEDWINDOW | WS_VISIBLE, monitorInfo.rcWork.left + 100,
      monitorInfo.rcWork.top + 100, 1920, 1080, nullptr, nullptr,
      wcex.hInstance, nullptr);
  if (m_hwnd == 0) {
    return false;
  }

   CreateMenu(m_hwnd);

  CreateDevices();
  CreateRenderTarget();
  
  return true;
}

void Window::CreateDevices() {

  DXGI_SWAP_CHAIN_DESC desc = {};
  desc.BufferDesc.Width = 0;
  desc.BufferDesc.Height = 0;
  desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  desc.BufferDesc.RefreshRate.Numerator = 0;
  desc.BufferDesc.RefreshRate.Denominator = 0;
  desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  desc.BufferCount = 2;
  desc.OutputWindow = m_hwnd;
  desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  desc.Windowed = true;


  D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_9_1,  D3D_FEATURE_LEVEL_9_2,
                                D3D_FEATURE_LEVEL_9_3,  D3D_FEATURE_LEVEL_10_0,
                                D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0,
                                D3D_FEATURE_LEVEL_11_1};

  UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

  D3D_FEATURE_LEVEL m_featureLevel;

  HRESULT hr = D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags,
      levels, ARRAYSIZE(levels), D3D11_SDK_VERSION, &desc,
      m_swapChain.GetAddressOf(), m_device.GetAddressOf(), &m_featureLevel,
      m_deviceContext.GetAddressOf());

}

void Window::Update() {
  MSG msg = {0};
  while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);

    if (msg.message == WM_QUIT) {
      break;
    }
  }
}

void Window::Shutdown() {
  if (m_hwnd) {
    DestroyWindow(m_hwnd);
  }

  if (m_wndClass) {
    UnregisterClassW((LPCWSTR)m_wndClass, GetModuleHandleW(nullptr));
  }
}

void Window::Resize() {
  RECT cr;
  if (GetClientRect(m_hwnd, &cr)) {
    m_width = cr.right - cr.left;
    m_height = cr.bottom - cr.top;

    m_swapChain->ResizeBuffers(1, m_width, m_height,
                               DXGI_FORMAT_UNKNOWN,
                               DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH |
                                   DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
    m_shouldResize = false;
  }
}

void Window::CreateRenderTarget() {
  ComPtr<ID3D11Texture2D> backBuffer;
  m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                         (void **)backBuffer.GetAddressOf());
  m_device->CreateRenderTargetView(backBuffer.Get(), nullptr,
                                   m_renderTargetView.GetAddressOf());

  backBuffer->GetDesc(&m_backBufferDesc);
}

void Window::BeginFrame() {
  // Bind render target
  m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(),
                                      nullptr);
  // Set viewport
  auto viewport = CD3D11_VIEWPORT(0.f, 0.f, (float)m_backBufferDesc.Width,
                                  (float)m_backBufferDesc.Height);
  m_deviceContext->RSSetViewports(1, &viewport);

  // Set the background color
  float clearColor[] = {.25f, .5f, 1, 1};
  m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
}

void Window::EndFrame() {
  // Swap the buffer!
  m_swapChain->Present(1, 0);
}


void CreateMenu(HWND hwnd) {
  HMENU hMenu = CreateMenu();
  HMENU hSubMenu = CreatePopupMenu();
  AppendMenu(hSubMenu, MF_STRING, 1, "Open File");
  AppendMenu(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hSubMenu), "File");

  // Attach the menu to the window
  SetMenu(hwnd, hMenu);
}

LRESULT CALLBACK Window::WinProc(HWND wnd, UINT msg, WPARAM wParam,
                                           LPARAM lParam) {
  switch (msg) {
    /*case WM_KEYDOWN:
      if (wParam == VK_F11) {
        Get().SetFullscreen(!Get().IsFullscreen());
      }
      break;*/
    case WM_SIZE:
      if (lParam && (HIWORD(lParam) != Get().m_height ||
                     LOWORD(lParam) != Get().m_width)) {
        Get().m_shouldResize = true;
      }
      break;
    case WM_CLOSE:
      Get().m_shouldClose = true;
      return 0;
  }
  return DefWindowProcW(wnd, msg, wParam, lParam);
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

