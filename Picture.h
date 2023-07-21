#pragma once

#include "WinInclude.h"
#include "Renderer.h"

#include <fstream>
#include <vector>


class Picture {
 public:
  Picture(Renderer& renderer);
  ~Picture();
  void draw(Renderer& renderer);

 public:
  void createMesh(Renderer& renderer);
  void createShaders(Renderer& renderer);
  void createRenderStates(Renderer& renderer);

  private:
  void loadTexture(Renderer& renderer);
  void setRenderStates(ComPtr<ID3D11DeviceContext> deviceContext);
  void bindShadersAndVertexBuffers(ComPtr<ID3D11DeviceContext> deviceContext);

 private:
  ComPtr<ID3D11Buffer> m_vertexBuffer;
  ComPtr<ID3D11VertexShader> m_vertexShader;
  ComPtr<ID3D11PixelShader> m_pixelShader;
  ComPtr<ID3D11InputLayout> m_inputLayout;
  ComPtr<ID3D11RasterizerState> m_rasterizerState;
  ComPtr<ID3D11DepthStencilState> m_depthState;
  ComPtr<ID3D11BlendState> m_blendState;
  ComPtr<ID3D11ShaderResourceView> m_texture;
};