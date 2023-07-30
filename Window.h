#pragma once

#include "Support/WinInclude.h"

class Window {
 public:

  bool Init();
  void Update();
  void Shutdown();
  void Resize();
  void SetFullscreen(bool enabled);
    
  void InitMenu();

  void CreateDevices();
  void CreateRenderTarget();

  void BeginFrame();
  void EndFrame();

  inline bool ShouldClose() const { return m_shouldClose; }

  inline bool ShouldResize() const { return m_shouldResize; }

  inline bool IsFullscreen() const { return m_isFullscreen; }

  inline HWND GetWindow() { return m_hwnd; }
  inline ComPtr<ID3D11Device> GetDevice() { return m_device; }

 private:
  bool GetBuffers();
  void ReleaseBuffers();
  static LRESULT CALLBACK WinProc(HWND wnd, UINT msg, WPARAM wParam,
                                  LPARAM lParam);
 private:
  HWND m_hwnd;
  ATOM m_wndClass = 0;

  bool m_shouldClose = false;
  bool m_shouldResize = false;
  bool m_isFullscreen = false;

  UINT m_width = 1920;
  UINT m_height = 1080;

  
  ComPtr<IDXGISwapChain> m_swapChain;
  ComPtr<ID3D11Device> m_device;
  ComPtr<ID3D11DeviceContext> m_deviceContext;
  ComPtr<ID3D11RenderTargetView> m_renderTargetView;
  D3D11_TEXTURE2D_DESC m_backBufferDesc;

  // singleton
 public:
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  inline static Window& Get() {
    static Window instance;
    return instance;
  }

 private:
  Window() = default;
};