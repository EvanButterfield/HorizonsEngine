struct vs_input
{
  float3 Pos    : POSITION;
  float3 Normal : NORMAL;
  float2 UV     : TEXCOORD;
  float3 Color  : COLOR;
};

struct ps_input
{
  float4 Pos           : SV_POSITION;
  float3 Normal        : NORMAL;
  float2 UV            : TEXCOORD;
  float4 Color         : COLOR;
  float3 FragPosition  : FRAG_POSITION;
};

cbuffer cbuffer0 : register(b0)
{
  float4x4 M;
  float4x4 TransformM;
  float4 Color;
  float3x3 NormalM;
}

ps_input VS(vs_input Input)
{
  ps_input Output;
  Output.Pos = mul(float4(Input.Pos, 1), M);
  Output.UV = Input.UV;
  Output.Color = float4(Input.Color, 1)*Color;
  Output.Normal = normalize(mul(Input.Normal, NormalM));
  Output.FragPosition = mul(TransformM, float4(Input.Pos, 1)).xyz;
  
  return Output;
}

cbuffer cbuffer1 : register(b1)
{
  float AmbientStrength;
  float3 LightDirection;
  float3 LightColor;
  float Padding;
  float3 CameraPosition;
}

sampler sampler0 : register(s0);

Texture2D<float4> texture0 : register(t0);

float4 PS(ps_input Input) : SV_TARGET
{
  float3 RealLightDirection = normalize(-LightDirection);
  float Diff = max(dot(Input.Normal, LightDirection), 0);
  float3 Diffuse = LightColor*Diff;
  
  float3 Ambient = LightColor*AmbientStrength;
  float3 Light = Diffuse + Ambient;
  
  float3 ViewDirection = normalize(CameraPosition - Input.FragPosition);
  
  float4 Tex = texture0.Sample(sampler0, Input.UV);
  float4 Result = Input.Color * Tex * float4(Light, 1);
  return Result;
}