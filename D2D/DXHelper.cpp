#include "DXHelper.h"

#include <wincodec.h>

#include "../Window.h"

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

  /// TODO: use settings of pSample for width and height
  desc.Width = 1920;
  desc.Height = 1080;
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

HRESULT DXHelper::RenderBitmapOnWindow(ComPtr<ID2D1Bitmap1> pBitmap) {

  m_renderTarget->BeginDraw();
  m_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
   m_renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Red));

  //D2D1_SIZE_F renderTargetSize = m_renderTarget->GetSize();
  //D2D1_SIZE_F bitmapSize = pBitmap->GetSize();

  //// Calculate the position to draw the bitmap at the center of the
  //// render target.
  //D2D1_POINT_2F upperLeftCorner =
  //    D2D1::Point2F((renderTargetSize.width - bitmapSize.width) / 2.f,
  //                  (renderTargetSize.height - bitmapSize.height) / 2.f);

  //m_renderTarget->DrawBitmap(
  //    pBitmap.Get(), D2D1::RectF(upperLeftCorner.x, upperLeftCorner.y,
  //                               upperLeftCorner.x + bitmapSize.width,
  //                               upperLeftCorner.y + bitmapSize.height));

  m_renderTarget->EndDraw();

  Window::Get().EndFrame();

  return S_OK;
}

void DXHelper::Init() {
  D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_9_1,  D3D_FEATURE_LEVEL_9_2,
                                D3D_FEATURE_LEVEL_9_3,  D3D_FEATURE_LEVEL_10_0,
                                D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0,
                                D3D_FEATURE_LEVEL_11_1};

  UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

  D3D_FEATURE_LEVEL featureLevel;

  HRESULT hr = D3D11CreateDevice(
      nullptr,
      D3D_DRIVER_TYPE_HARDWARE,
      0,
      deviceFlags,
      levels,
      ARRAYSIZE(levels),
      D3D11_SDK_VERSION,
      m_device.GetAddressOf(),
      &featureLevel, 
      m_deviceContext.GetAddressOf()             
  );
  D2D1_FACTORY_OPTIONS options = {};
  hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
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

  D2D1_RENDER_TARGET_PROPERTIES renderTargetProps =
      D2D1::RenderTargetProperties(
          D2D1_RENDER_TARGET_TYPE_HARDWARE,
          D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN,
                            D2D1_ALPHA_MODE_PREMULTIPLIED));

  auto swapChain = Window::Get().GetSwapChain();
  IDXGISurface* dxgiBackbuffer;
  hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackbuffer));

  hr = m_factory->CreateDxgiSurfaceRenderTarget(
      dxgiBackbuffer, 
      &renderTargetProps,
      m_renderTarget.GetAddressOf()
  );
  if (FAILED(hr)) {
    return;
  }
}

