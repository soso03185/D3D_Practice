#include "Shared.hlsli"
// 정점 셰이더.

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

PS_INPUT main(VS_INPUT Input)
{
	PS_INPUT ps_input;
	
	// 정점 월드 변환
	ps_input.Pos = mul(Input.Pos, World);
	
	// 카메라 벡터
    float3 viewDir = normalize(ps_input.Pos.xyz - gWorldCameraPosition.xyz);
    ps_input.mViewDir = viewDir;
	
	ps_input.Pos = mul(ps_input.Pos, View);		
	ps_input.Pos = mul(ps_input.Pos, Projection);
	ps_input.Tex = Input.Tex;

	float3 worldNormal = mul(Input.Norm, (float3x3)World);
	worldNormal = normalize(worldNormal);
	
    ps_input.Norm = worldNormal;	
	return ps_input;
};
