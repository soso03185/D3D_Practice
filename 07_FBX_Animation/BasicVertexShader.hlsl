#include "Shared.hlsli"
// 정점 셰이더.

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

PS_INPUT main(VS_INPUT Input)
{
	PS_INPUT output;
	
	// 정점 월드 변환
    output.Pos = mul(Input.Pos, World);
 //   output.Pos.w = 1.f;
    
	// 카메라 벡터
    output.mViewDir = normalize(output.Pos.xyz - gWorldCameraPosition.xyz);    
    output.Norm = normalize( mul(Input.Norm, (float3x3)World) );
    output.TangentWorld = Input.TangentWorld;
    output.Tex = Input.Tex;
    
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
        
    return output;
};