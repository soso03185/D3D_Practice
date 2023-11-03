//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txColor : register(t0);
Texture2D txNormal : register(t1);
Texture2D txSpecular : register(t2);

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

cbuffer BoolBuffer : register(b1)
{   
    int UseNormalMap;
    int UseSpecularMap;
    int UseGamma;
    int boolbpadding[1];
}
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : UV; // 버텍스 쉐이더의 시멘틱은 의미있는 이름로 짓는 게 좋다.
    float3 TangentWorld : TANGENT;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : NORMAL;
    float2 Tex : UV;
    float3 TangentWorld : TANGENT;
  //  float3 BiTangent : BITANGENT;
    
    float3 mDiffuse : TEXCOORD1;
    float3 mViewDir : TEXCOORD2;
    float3 mReflection : TEXCOORD3;
};