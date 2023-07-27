#pragma once

#include "Support/WinInclude.h"

class Renderer {
 public:
  Renderer(HWND &hwnd);

  void BeginFrame();
  void EndFrame();

  ComPtr<ID3D11Device> GetDevice() { return m_device; }
  ComPtr<ID3D11DeviceContext> GetD3DDeviceContext() { return m_deviceContext; }
  ComPtr<ID2D1DeviceContext> GetD2DDeviceContext() {
    return m_D2DDeviceContext;
  }

  public:
  HRESULT ExtractVideoFrame(IMFSample *pSample,
                             ID3D11Texture2D **ppVideoTexture);
   HRESULT CreateBitmapFromTexture(ComPtr<ID3D11Texture2D> pTexture,
                                   ComPtr<ID2D1Bitmap1> pBitmap);

 private:
  void CreateDevices(HWND &hwnd);
  void CreateRenderTarget();

 private:
  ComPtr<IDXGISwapChain> m_swapChain;
  ComPtr<ID3D11Device> m_device;
  ComPtr<ID3D11DeviceContext> m_deviceContext;
  ComPtr<ID2D1DeviceContext> m_deviceContext2D;

  // Render target
  ComPtr<ID3D11RenderTargetView> m_renderTargetView;

  D3D11_TEXTURE2D_DESC m_backBufferDesc;

  ComPtr<ID2D1Factory1> m_factory;
  ComPtr<ID2D1Device> m_D2DDevice;
  ComPtr<ID2D1DeviceContext> m_D2DDeviceContext;
};