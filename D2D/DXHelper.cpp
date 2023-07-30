#include "DXHelper.h"

DXHelper::DXHelper() { Init(); }

HRESULT DXHelper::ExtractVideoFrame(IMFSample* pSample,
                                    ID3D11Texture2D** ppVideoTexture) {
  HRESULT hr = S_OK;
  ComPtr<IMFMediaBuffer> pBuffer;
  BYTE* pData = nullptr;
  DWORD cbData = 0;

  hr = pSample->ConvertToContiguousBuffer(pBuffer.GetAddressOf());
  if (FAILED(hr)) {
    return hr;
  }

  hr = pBuffer->Lock(&pData, nullptr, &cbData);
  if (FAILED(hr)) {
    return hr;
  }

  D3D11_TEXTURE2D_DESC desc = {};

  /// TODO: make swapChain resizible and use settings of pSample
  desc.Width = 800;
  desc.Height = 600;
  desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = 0;
  desc.ArraySize = 1;
  desc.MipLevels = 1;
  desc.SampleDesc.Count = 1;

  hr = m_device->CreateTexture2D(&desc, nullptr, ppVideoTexture);
  if (FAILED(hr)) {
    pBuffer->Unlock();
    return hr;
  }

  m_deviceContext->UpdateSubresource(*ppVideoTexture, 0, nullptr, pData,
                                     desc.Width * 4, 0);

  pBuffer->Unlock();

  return hr;
}

HRESULT DXHelper::CreateBitmapFromTexture(ComPtr<ID3D11Texture2D> pTexture,
                                          ComPtr<ID2D1Bitmap1> pBitmap) {

  ComPtr<IDXGISurface> dxgiSurface;
  HRESULT hr = pBitmap->GetSurface(dxgiSurface.GetAddressOf());
  if (FAILED(hr)) {
    return hr;
  }

  ComPtr<ID3D11Resource> pResource;
  hr = dxgiSurface.As(&pResource);
  if (FAILED(hr)) {
    return hr;
  }

  m_deviceContext->CopyResource(pResource.Get(), pTexture.Get());

  return S_OK;
}

void DXHelper::Init() {
  D2D1_FACTORY_OPTIONS options = {};
  HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                 __uuidof(ID2D1Factory1), &m_factory);

  ComPtr<IDXGIDevice> dxgiDevice;
  hr = m_device.As(&dxgiDevice);
  if (FAILED(hr)) {
    return;
  }

  hr = m_factory->CreateDevice(dxgiDevice.Get(), &m_D2DDevice);
  if (FAILED(hr)) {
    return;
  }

  hr = m_D2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                                        m_D2DDeviceContext.GetAddressOf());
  if (FAILED(hr)) {
    return;
  }
}

