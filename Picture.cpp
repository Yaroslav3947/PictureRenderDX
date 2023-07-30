//#include "Picture.h"
//
// struct Vertex {
//   float x, y;
//   float r, g, b;
//   float u, v;  // Texture coordinates
// };
//
// Vertex vertices[] = {
//     {-0.5f, -0.5f, 1, 1, 1, 0, 1},
//     {-0.5f, 0.5f, 1, 1, 1, 0, 0}, 
//     {0.5f, -0.5f, 1, 1, 1, 1, 1},
//     {-0.5f, 0.5f, 1, 1, 1, 0, 0},  
//     {0.5f, 0.5f, 1, 1, 1, 1, 0},  
//     {0.5f, -0.5f, 1, 1, 1, 1, 1}  
// };
//
//Picture::Picture(Renderer& renderer) {
//  createMesh(renderer);
//  createShaders(renderer);
//  createRenderStates(renderer);
//  loadTexture(renderer);
//}
//
//Picture::~Picture() {}
//
//void Picture::draw(Renderer& renderer) {
//  auto deviceContext = renderer.GetD3DDeviceContext();
//
//  deviceContext->PSSetShaderResources(0, 1, m_texture.GetAddressOf());
//
//  // Set render states
//  setRenderStates(deviceContext);
//
//  // Bind the shaders and vertex buffer
//  bindShadersAndVertexBuffers(deviceContext);
//
//  // Draw
//  deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//  deviceContext->Draw(6, 0);
//}
//
//void Picture::createMesh(Renderer& renderer) {
//  auto vertexBufferDesc =
//      CD3D11_BUFFER_DESC(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
//  D3D11_SUBRESOURCE_DATA vertexData = {0};
//  vertexData.pSysMem = vertices;
//
//  renderer.GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData,
//                                     m_vertexBuffer.GetAddressOf());
//}
//
//void Picture::createShaders(Renderer& renderer) {
//  // Create shaders
//  std::ifstream vsFile("TriangleVertexShader.cso", std::ios::binary);
//  std::ifstream psFile("TrianglePixelShader.cso", std::ios::binary);
//
//  std::vector<char> vsData = {std::istreambuf_iterator<char>(vsFile),
//                              std::istreambuf_iterator<char>()};
//  std::vector<char> psData = {std::istreambuf_iterator<char>(psFile),
//                              std::istreambuf_iterator<char>()};
//
//  renderer.GetDevice()->CreateVertexShader(
//      vsData.data(), vsData.size(), nullptr, m_vertexShader.GetAddressOf());
//  renderer.GetDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr,
//                                          m_pixelShader.GetAddressOf());
//
//  // Create input layout
//  D3D11_INPUT_ELEMENT_DESC layout[] = {
//      {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
//       D3D11_INPUT_PER_VERTEX_DATA, 0},
//      {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
//       D3D11_INPUT_PER_VERTEX_DATA, 0},
//      {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
//       D3D11_INPUT_PER_VERTEX_DATA, 0},
//  };
//
//  renderer.GetDevice()->CreateInputLayout(
//      layout, 3, vsData.data(), vsData.size(), m_inputLayout.GetAddressOf());
//}
//
//void Picture::createRenderStates(Renderer& renderer) {
//  // Rasterizer state
//  auto rasterizerDesc =
//      CD3D11_RASTERIZER_DESC(D3D11_FILL_SOLID, D3D11_CULL_NONE, false, 0, 0, 0,
//                             0, false, false, false);
//  renderer.GetDevice()->CreateRasterizerState(&rasterizerDesc,
//                                              m_rasterizerState.GetAddressOf());
//
//  // Blend state
//  auto blendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
//  renderer.GetDevice()->CreateBlendState(&blendDesc,
//                                         m_blendState.GetAddressOf());
//
//  // Depth state
//  auto depthDesc = CD3D11_DEPTH_STENCIL_DESC(
//      FALSE, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_LESS, FALSE,
//      D3D11_DEFAULT_STENCIL_READ_MASK, D3D11_DEFAULT_STENCIL_WRITE_MASK,
//      D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP,
//      D3D11_COMPARISON_ALWAYS, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP,
//      D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS);
//  renderer.GetDevice()->CreateDepthStencilState(&depthDesc,
//                                                m_depthState.GetAddressOf());
//}
//
//void Picture::loadTexture(Renderer& renderer) {
//  HRESULT hr = DirectX::CreateWICTextureFromFile(renderer.GetDevice().Get(),
//                                                 L"Image.jpg", nullptr,
//                                                 m_texture.GetAddressOf());
//
//  if (FAILED(hr)) {
//    LPWSTR errorMessage;
//    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
//                       FORMAT_MESSAGE_IGNORE_INSERTS,
//                   NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//                   (LPWSTR)&errorMessage, 0, NULL);
//    MessageBoxW(NULL, errorMessage, L"Error", MB_ICONERROR | MB_OK);
//    LocalFree(errorMessage);
//  }
//}
//
//void Picture::setRenderStates(ComPtr<ID3D11DeviceContext> deviceContext) {
//  deviceContext->RSSetState(m_rasterizerState.Get());
//  deviceContext->OMSetBlendState(m_blendState.Get(), NULL, 0xffffffff);
//  deviceContext->OMSetDepthStencilState(m_depthState.Get(), 1);
//}
//
//void Picture::bindShadersAndVertexBuffers(
//    ComPtr<ID3D11DeviceContext> deviceContext) {
//  deviceContext->IASetInputLayout(m_inputLayout.Get());
//  deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
//  deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
//
//  UINT stride = sizeof(Vertex);
//  UINT offset = 0;
//  deviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(),
//                                    &stride, &offset);
//}
