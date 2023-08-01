#pragma once

#include <cstdlib>

#include "../Support/WinInclude.h"
#include "../Window.h"

class DXHelper {
 public:
  DXHelper();

  ComPtr<ID3D11Device> GetDevice() { return m_device; }
  ComPtr<ID3D11DeviceContext> GetD3DDeviceContext() { return m_deviceContext; }
  ComPtr<ID2D1DeviceContext> GetD2DDeviceContext() {
    return m_D2DDeviceContext;
  }

 public:
  void RenderBitmapOnWindow(ComPtr<ID2D1Bitmap> bitmap);
  ComPtr<ID2D1Bitmap> CreateBitmapFromVideoSample(IMFSample *pSample);

 private:
  void Init();

 private:
  ComPtr<ID3D11Device> m_device;
  ComPtr<ID3D11DeviceContext> m_deviceContext;

  ComPtr<ID2D1Factory1> m_factory;
  ComPtr<ID2D1Device> m_D2DDevice;
  ComPtr<ID2D1DeviceContext> m_D2DDeviceContext;
  ComPtr<ID2D1RenderTarget> m_renderTarget;
};