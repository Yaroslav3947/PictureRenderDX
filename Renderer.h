#pragma once

#include "WinInclude.h"

#include "Window.h"

class Renderer {
 public:
  Renderer(Window& window);

  void beginFrame();
  void endFrame();

  ComPtr<ID3D11Device> getDevice() { return m_device; }
  ComPtr<ID3D11DeviceContext> getDeviceContext() { return m_deviceContext; }

 private:
  void createDevice(Window& window);
  void createRenderTarget();

 private:
  ComPtr<IDXGISwapChain> m_swapChain;
  ComPtr<ID3D11Device> m_device;
  ComPtr<ID3D11DeviceContext> m_deviceContext;

  // Render target
  ComPtr<ID3D11RenderTargetView> m_renderTargetView;
  D3D11_TEXTURE2D_DESC m_backBufferDesc;
};