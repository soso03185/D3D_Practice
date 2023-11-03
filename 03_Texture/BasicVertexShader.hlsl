#include "Shared.hlsli"
// ¡§¡° ºŒ¿Ã¥ı.

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

// Lambert
PS_INPUT main(VS_INPUT Input)
{
    PS_INPUT ps_input;
    ps_input.Pos = mul(Input.Pos, World);
    ps_input.Pos = mul(ps_input.Pos, View);
    ps_input.Pos = mul(ps_input.Pos, Projection);
    ps_input.Tex = Input.Tex;

    float3 lightDirection = vLightDir.xyz;
    lightDirection = normalize(lightDirection);

    float3 worldNormal = mul(Input.Norm, (float3x3)World);
    worldNormal = normalize(worldNormal);

    ps_input.mDiffuse = dot(-lightDirection, worldNormal);

    return ps_input;
}