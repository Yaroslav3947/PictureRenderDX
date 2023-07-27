struct Input {
  float4 position : SV_POSITION;
  float3 color : COLOR;
  float2 texCoord : TEXCOORD;  // Texture coordinates
};

Texture2D<float4> textureSampler : register(t0);
SamplerState textureSamplerState : register(s0);

float4 main(Input input) : SV_TARGET {
  float4 textureColor =
      textureSampler.Sample(textureSamplerState, input.texCoord);
  return float4(input.color * textureColor.rgb, textureColor.a);
}
