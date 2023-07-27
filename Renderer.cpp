#include "Renderer.h"

Renderer::Renderer(HWND &hwnd) {
  CreateDevices(hwnd);
  CreateRenderTarget();
}

void Renderer::BeginFrame() {
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

void Renderer::EndFrame() {
  // Swap the buffer!
  m_swapChain->Present(1, 0);
}

HRESULT Renderer::ExtractVideoFrame(IMFSample* pSample,
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

HRESULT Renderer::CreateBitmapFromTexture(ComPtr<ID3D11Texture2D> pTexture,
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

void Renderer::CreateDevices(HWND &hwnd) {
  // Define our swap chain
  DXGI_SWAP_CHAIN_DESC swapChainDesc = {0};
  swapChainDesc.BufferCount = 1;
  swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.OutputWindow = hwnd;
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.Windowed = true;

  // Create the swap chain, device and device context
  HRESULT hr = D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
      D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, nullptr,
      &m_deviceContext);

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
}

void Renderer::CreateRenderTarget() {
  ComPtr<ID3D11Texture2D> backBuffer;
  m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                         (void **)backBuffer.GetAddressOf());
  m_device->CreateRenderTargetView(backBuffer.Get(), nullptr,
                                   m_renderTargetView.GetAddressOf());

  backBuffer->GetDesc(&m_backBufferDesc);
}
