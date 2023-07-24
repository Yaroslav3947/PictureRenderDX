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

HRESULT Renderer::ConvertSampleToTexture(IMFSample *pSample,
                                         ComPtr<ID3D11Texture2D> &pTexture) {
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
  m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
  m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());

  backBuffer->GetDesc(&m_backBufferDesc);
}
