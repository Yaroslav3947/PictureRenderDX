struct Input {
  float2 position : POSITION;
  float3 color : COLOR;
  float2 texCoord : TEXCOORD0;  // Texture coordinates
};

struct Output {
  float4 position : SV_POSITION;
  float3 color : COLOR;
  float2 texCoord : TEXCOORD0;  // Texture coordinates
};

Output main(Input input) {
  Output output;

  output.position = float4(input.position.x, input.position.y, 0, 1);
  output.color = input.color;
  output.texCoord = input.texCoord;  // Pass the texture coordinates through

  return output;
}
