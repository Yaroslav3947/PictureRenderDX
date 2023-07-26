#include "Renderer.h"

Renderer::Renderer(HWND &hwnd) {
  createDevice(hwnd);
  createRenderTarget();
}

void Renderer::beginFrame() {
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

void Renderer::endFrame() {
  // Swap the buffer!
  m_swapChain->Present(1, 0);
}

//HRESULT Renderer::ExtractVideoFrame(IMFSample *pSample,
//                                    ID3D11Texture2D **ppVideoTexture) {
//  HRESULT hr = S_OK;
//
//  *ppVideoTexture = nullptr;
//
//  // Get the media buffer from the sample.
//  IMFMediaBuffer *pBuffer = nullptr;
//  hr = pSample->GetBufferByIndex(0, &pBuffer);
//  if (FAILED(hr)) return hr;
//
//  // Get the buffer length and pointer to the raw data.
//  BYTE *pData = nullptr;
//  DWORD dwLength = 0;
//  hr = pBuffer->Lock(&pData, nullptr, &dwLength);
//  if (FAILED(hr)) {
//    pBuffer->Release();
//    return hr;
//  }
//
//  // Get the video frame attributes.
//  UINT32 width = 720, height = 1280, stride;
//  /*hr = MFGetAttributeSize(pSample, MF_MT_FRAME_SIZE, &width, &height);
//  if (FAILED(hr)) {
//    pBuffer->Unlock();
//    pBuffer->Release();
//    return hr;
//  }*/
//
//  // Create a DXGI format compatible with the texture.
//  DXGI_FORMAT format =
//      DXGI_FORMAT_B8G8R8A8_UNORM;  // Assuming 32-bit RGBA format.
//
//  // Describe the texture.
//  D3D11_TEXTURE2D_DESC textureDesc;
//  ZeroMemory(&textureDesc, sizeof(textureDesc));
//  textureDesc.Width = width;
//  textureDesc.Height = height;
//  textureDesc.MipLevels = 1;
//  textureDesc.ArraySize = 1;
//  textureDesc.Format = format;
//  textureDesc.SampleDesc.Count = 1;
//  textureDesc.Usage = D3D11_USAGE_DEFAULT;
//  textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//  textureDesc.CPUAccessFlags = 0;
//  textureDesc.MiscFlags = 0;
//
//  // Create the D3D11 texture.
//  hr = m_device->CreateTexture2D(&textureDesc, nullptr, ppVideoTexture);
//  if (FAILED(hr)) {
//    pBuffer->Unlock();
//    pBuffer->Release();
//    return hr;
//  }
//
//  // Calculate the pitch (row pitch) of the video frame data.
//  stride = width * 4;  // Assuming 32-bit (4 bytes per pixel) RGBA format.
//  if (stride != dwLength / height) {
//    pBuffer->Unlock();
//    pBuffer->Release();
//    (*ppVideoTexture)->Release();
//    *ppVideoTexture = nullptr;
//    return E_INVALIDARG;
//  }
//
//  // Update the texture data.
//  D3D11_BOX destBox;
//  destBox.left = 0;
//  destBox.right = width;
//  destBox.top = 0;
//  destBox.bottom = height;
//  destBox.front = 0;
//  destBox.back = 1;
//
//  m_deviceContext->UpdateSubresource(*ppVideoTexture, 0, &destBox, pData,
//                                         stride, 0);
//
//  // Unlock and release the media buffer.
//  pBuffer->Unlock();
//  pBuffer->Release();
//
//  return S_OK;
//
//}

HRESULT Renderer::ExtractVideoFrame(IMFSample* pSample,
                                    ID3D11Texture2D** ppVideoTexture) {
  if (!pSample || !ppVideoTexture) return E_INVALIDARG;

  ComPtr<IMFMediaBuffer> pBuffer;
  HRESULT hr = pSample->ConvertToContiguousBuffer(pBuffer.GetAddressOf());
  if (FAILED(hr)) return hr;

  BYTE* pData = nullptr;
  DWORD dwMaxLength = 0;
  DWORD dwCurrentLength = 0;
  hr = pBuffer->Lock(&pData, &dwMaxLength, &dwCurrentLength);
  if (FAILED(hr)) return hr;

  UINT32 width = 0, height = 0;
  hr = MFGetAttributeSize(pSample, MF_MT_FRAME_SIZE, &width, &height);
  if (FAILED(hr)) {
    pBuffer->Unlock();
    return hr;
  }

  DXGI_FORMAT format =
      DXGI_FORMAT_B8G8R8A8_UNORM;  

  D3D11_TEXTURE2D_DESC textureDesc;
  ZeroMemory(&textureDesc, sizeof(textureDesc));
  textureDesc.Width = width;
  textureDesc.Height = height;
  textureDesc.MipLevels = 1;
  textureDesc.ArraySize = 1;
  textureDesc.Format = format;
  textureDesc.SampleDesc.Count = 1;
  textureDesc.Usage = D3D11_USAGE_DEFAULT;
  textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  textureDesc.CPUAccessFlags = 0;
  textureDesc.MiscFlags = 0;

  hr = m_device->CreateTexture2D(&textureDesc, nullptr, ppVideoTexture);
  if (FAILED(hr)) {
    pBuffer->Unlock();
    return hr;
  }

  UINT stride = width * 4;  

  D3D11_BOX destBox;
  destBox.left = 0;
  destBox.right = width;
  destBox.top = 0;
  destBox.bottom = height;
  destBox.front = 0;
  destBox.back = 1;

  m_deviceContext->UpdateSubresource(*ppVideoTexture, 0, &destBox, pData,
                                     stride, 0);

  pBuffer->Unlock();
  return S_OK;
}



HRESULT Renderer::RenderVideoFrameToSwapChain(ID3D11Texture2D* pVideoTexture) {
  return E_NOTIMPL;
}


HRESULT Renderer::RenderTextureToWindow(ComPtr<ID3D11Texture2D> pTexture) {
  return E_NOTIMPL;
}

void Renderer::createDevice(HWND &hwnd) {
  // Define our swap chain
  DXGI_SWAP_CHAIN_DESC swapChainDesc = {0};
  swapChainDesc.BufferCount = 1;
  swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.OutputWindow = hwnd;
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.Windowed = true;

  // Create the swap chain, device and device context
  auto result = D3D11CreateDeviceAndSwapChain(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
      D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, nullptr,
      &m_deviceContext);

  // Check for error
  if (result != S_OK) {
    MessageBox(nullptr, "Error creating DX11", "Error", MB_OK);
    exit(0);
  }
}

void Renderer::createRenderTarget() {
  ComPtr<ID3D11Texture2D> backBuffer;
  m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                         (void **)backBuffer.GetAddressOf());
  m_device->CreateRenderTargetView(backBuffer.Get(), nullptr,
                                   m_renderTargetView.GetAddressOf());

  backBuffer->GetDesc(&m_backBufferDesc);
}
