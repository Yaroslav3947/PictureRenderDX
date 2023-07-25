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
  if (!pSample  || !ppVideoTexture) return E_INVALIDARG;

  HRESULT hr = S_OK;

  // Step 1: Get the video frame data from the IMFSample
  BYTE* pFrameData = nullptr;
  DWORD frameDataSize = 0;
  hr = pSample->ConvertToContiguousBuffer(&pFrameData);
  if (FAILED(hr)) return hr;

  hr = pSample->GetTotalLength(&frameDataSize);
  if (FAILED(hr)) {
    CoTaskMemFree(pFrameData);
    return hr;
  }

  // Step 2: Create a new ID3D11Texture2D
  D3D11_TEXTURE2D_DESC textureDesc;
  ZeroMemory(&textureDesc, sizeof(textureDesc));
  // Fill in the texture description based on the video frame data
  // (format, width, height, etc.)

  ID3D11Texture2D* pVideoTexture = nullptr;
  hr = m_device->CreateTexture2D(&textureDesc, nullptr, &pVideoTexture);

  CoTaskMemFree(
      pFrameData);  // Release the memory allocated by ConvertToContiguousBuffer

  if (FAILED(hr)) return hr;

  // Step 3: Copy the video frame data to the ID3D11Texture2D
  m_device->GetImmediateContext(&m_deviceContext);

  D3D11_BOX srcBox;
  srcBox.left = 0;
  srcBox.right = textureDesc.Width;
  srcBox.top = 0;
  srcBox.bottom = textureDesc.Height;
  srcBox.front = 0;
  srcBox.back = 1;

  /*m_deviceContext->UpdateSubresource(
      pVideoTexture, 0, nullptr, pFrameData,
      textureDesc.Width * textureDesc.Format->bitsPerPixel / 8, 0);*/

  //m_deviceContext->Release();

  // Step 4: Store the ID3D11Texture2D in the ppVideoTexture pointer.
  *ppVideoTexture = pVideoTexture;

  return hr;
}

HRESULT Renderer::RenderVideoFrameToSwapChain(ID3D11Texture2D* pVideoTexture) {
  // Assuming you have initialized the D3D11Device, DeviceContext, and
  // SwapChain.

  // Get DXGI Surface from the ID3D11Texture2D.
  IDXGISurface* pSurface = nullptr;
  HRESULT hr = pVideoTexture->QueryInterface(IID_PPV_ARGS(&pSurface));
  if (FAILED(hr)) {
    // Handle the error.
    return hr;
  }

  // Create a Direct2D bitmap from the DXGI surface.
  ID2D1Bitmap* pBitmap = nullptr;
  D2D1_BITMAP_PROPERTIES bitmapProperties = D2D1::BitmapProperties();
  hr = pD2D1DeviceContext->CreateBitmapFromDxgiSurface(
      pSurface, &bitmapProperties, &pBitmap);
  pSurface->Release();  // Release the DXGI surface.
  if (FAILED(hr)) {
    // Handle the error.
    return hr;
  }

  // Clear the render target view and render the Direct2D bitmap to the swap
  // chain.
  pD3D11DeviceContext->ClearRenderTargetView(pRenderTargetView, ClearColor);
  pD2D1DeviceContext->BeginDraw();
  pD2D1DeviceContext->DrawBitmap(pBitmap);
  hr = pD2D1DeviceContext->EndDraw();
  pD3D11SwapChain->Present(0, 0);

  // Release the Direct2D bitmap.
  if (pBitmap) {
    pBitmap->Release();
    pBitmap = nullptr;
  }

  return hr;
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
