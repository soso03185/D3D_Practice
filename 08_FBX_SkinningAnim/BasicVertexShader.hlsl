#include "Shared.hlsli"
// ���� ���̴�.

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

PS_INPUT main(VS_INPUT Input)
{
	PS_INPUT output;
	
	// ���� ���� ��ȯ
    output.Pos = mul(Input.Pos, World);
 //   output.Pos.w = 1.f;
    
	// ī�޶� ����
    output.mViewDir = normalize(output.Pos.xyz - gWorldCameraPosition.xyz);    
    output.Norm = normalize( mul(Input.Norm, (float3x3)World) );
    output.TangentWorld = Input.TangentWorld;
    output.Tex = Input.Tex;
    
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
        
    return output;
};