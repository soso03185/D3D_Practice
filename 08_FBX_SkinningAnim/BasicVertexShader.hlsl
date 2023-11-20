#include "Shared.hlsli"
// 정점 셰이더.

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

PS_INPUT main(VS_INPUT Input)
{
	PS_INPUT output;
	
    float4x4 matWorldBlended;
    matWorldBlended = mul(Input.BlendWeights.x, MatrixPalleteArray[Input.BlendIndices.x]);
    matWorldBlended += mul(Input.BlendWeights.y, MatrixPalleteArray[Input.BlendIndices.y]);
    matWorldBlended += mul(Input.BlendWeights.z, MatrixPalleteArray[Input.BlendIndices.z]);
    matWorldBlended += mul(Input.BlendWeights.w, MatrixPalleteArray[Input.BlendIndices.w]);

    output.Pos = mul(Input.Pos, matWorldBlended);

    // 정점 월드 변환
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    output.Tex = Input.Tex;

	// 카메라 벡터
    output.mViewDir = normalize(output.Pos.xyz - gWorldCameraPosition.xyz);    
    output.Norm = normalize( mul(Input.Norm, (float3x3)matWorldBlended) );
    output.TangentWorld = normalize(mul(Input.TangentWorld, (float3x3)matWorldBlended));
    
    return output;
};