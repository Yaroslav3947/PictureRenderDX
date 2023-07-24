#pragma once

#include "WinInclude.h"

#include "Window.h"

class Renderer {
 public:
  Renderer(HWND &hwnd);

  void beginFrame();
  void endFrame();

  ComPtr<ID3D11Device> getDevice() { return m_device; }
  ComPtr<ID3D11DeviceContext> getDeviceContext() { return m_deviceContext; }

  public:
  HRESULT
  ConvertSampleToTexture(IMFSample *pSample, ComPtr<ID3D11Texture2D> &pTexture);
  HRESULT
  RenderTextureToWindow(ComPtr<ID3D11Texture2D> pTexture);

 private:
  void createDevice(HWND &hwnd);
  void createRenderTarget();

 private:
  ComPtr<IDXGISwapChain> m_swapChain;
  ComPtr<ID3D11Device> m_device;
  ComPtr<ID3D11DeviceContext> m_deviceContext;

  // Render target
  ComPtr<ID3D11RenderTargetView> m_renderTargetView;
  D3D11_TEXTURE2D_DESC m_backBufferDesc;
};