#include "Shared.hlsli"
// ���� ���̴�.

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

PS_INPUT main(VS_INPUT Input)
{
	PS_INPUT output;
	    
    // �ִ� 4�������� ���� ������. x y z w (�̸��� �߿����� ����)
    float4x4 matWorld;
    matWorld =  mul(Input.BlendWeights.x, MatrixPaletteArray[Input.BlendIndices.x]);
    matWorld += mul(Input.BlendWeights.y, MatrixPaletteArray[Input.BlendIndices.y]);
    matWorld += mul(Input.BlendWeights.z, MatrixPaletteArray[Input.BlendIndices.z]);
    matWorld += mul(Input.BlendWeights.w, MatrixPaletteArray[Input.BlendIndices.w]);        
    //// else  matWorld = World;
    
	// ���� ���� ��ȯ
    output.Pos = mul(Input.Pos, matWorld);
    output.Tex = Input.Tex;
    
	// ī�޶� ����
    output.mViewDir     = normalize(output.Pos.xyz - gWorldCameraPosition.xyz);    
    output.Norm         = normalize(mul(Input.Norm, (float3x3) matWorld));
    output.TangentWorld = normalize(mul(Input.TangentWorld, (float3x3) matWorld));
    
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    return output;
};