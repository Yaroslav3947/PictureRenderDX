#pragma once

#include "Support/WinInclude.h"

class DXHelper {
 public:
  DXHelper();

  ComPtr<ID3D11Device> GetDevice() { 
      return m_device; 
  }
  ComPtr<ID3D11DeviceContext> GetD3DDeviceContext() { 
      return m_deviceContext; 
  }
  ComPtr<ID2D1DeviceContext> GetD2DDeviceContext() {
    return m_D2DDeviceContext;
  }

  public:
  HRESULT ExtractVideoFrame(IMFSample *pSample,
                             ID3D11Texture2D **ppVideoTexture);
  HRESULT CreateBitmapFromTexture(ComPtr<ID3D11Texture2D> pTexture,
                                   ComPtr<ID2D1Bitmap1> pBitmap);

 private:
  void Init();

 private:
  ComPtr<ID3D11Device> m_device;
  ComPtr<ID3D11DeviceContext> m_deviceContext;
  ComPtr<ID2D1DeviceContext> m_deviceContext2D;
  ComPtr<ID3D11RenderTargetView> m_renderTargetView;
  ComPtr<ID2D1Factory1> m_factory;
  ComPtr<ID2D1Device> m_D2DDevice;
  ComPtr<ID2D1DeviceContext> m_D2DDeviceContext;
};