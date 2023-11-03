#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------


float4 main(PS_INPUT Input) : SV_Target
{
   float3 diffuse = saturate(Input.mDiffuse) * vLightColor;
  // return float4(diffuse, 1);
   return txDiffuse.Sample(samLinear, Input.Tex) * float4(diffuse, 1);
}

