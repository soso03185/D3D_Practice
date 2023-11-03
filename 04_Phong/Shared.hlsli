//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    
    float4 vLightDir;
    float4 vLightColor;
    float4 gWorldCameraPosition;

    float gCubeSpecularPower;
    float gCubeAmbient;
    float gpadding[2];
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : UV; // 버텍스 쉐이더의 시멘틱은 의미있는 이름로 짓는 게 좋다.
    // float3 mDiffuse : UV1;
    // float3 mViewDir : UV2;
    // float3 mReflection : UV3;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : NORMAL;

    float2 Tex : TEXCOORD0;
    float3 mDiffuse : TEXCOORD1;
    float3 mViewDir : TEXCOORD2;
    float3 mReflection : TEXCOORD3;
};