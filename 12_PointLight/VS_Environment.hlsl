#include "Shared.hlsli"

// ���ؽ� ���̴�

PS_INPUT_ENVIRONMENT main(VS_INPUT input)
{
    PS_INPUT_ENVIRONMENT output = (PS_INPUT_ENVIRONMENT) 0;
    
    float4 pos = mul(input.Pos, World);
    output.TexCoords = pos.xyz;
    float3 pos3 = mul(pos.xyz, (float3x3) View); // ����Ŀ��� ��ġ�� �������
    pos = mul(float4(pos3, 1.0f), Projection);
    output.PositionProj = pos;
    
    return output;
}
