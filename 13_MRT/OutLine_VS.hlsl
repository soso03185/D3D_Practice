
#include "Shared.hlsli"


PS_INPUT main(VS_INPUT Input)
{
    PS_INPUT output;
     
	// 정점 월드 변환
    output.WorldPos = mul(Input.Pos, World);
    output.Tex = Input.Tex;
    
	// 카메라 벡터
    output.mViewDir = normalize(output.WorldPos.xyz - gWorldCameraPosition.xyz);
    output.Norm = normalize(mul(Input.Norm, (float3x3) World));
    output.Norm += Input.Norm * outlineThickness;
    
    output.Pos = mul(output.WorldPos, View);
    output.Pos = mul(output.Pos, Projection);
    
    return output;
}