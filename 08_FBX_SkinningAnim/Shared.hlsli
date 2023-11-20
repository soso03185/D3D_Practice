//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txSpecular : register(t2);
Texture2D txEmissive : register(t3);
Texture2D txOpacity : register(t4);

SamplerState samLinear : register(s0);

cbuffer ConstantBuffer : register(b0)
{
    float gCubeSpecularPower;
    float gCubeAmbient;
    float gpadding[2];    
}

cbuffer BoolBuffer : register(b1)
{
    int UseNormalMap;
    int UseSpecularMap;
    int UseGamma;
    int UseDiffuseMap;     // 16
    int UseEmissiveMap;
    int UseOpacityMap;
    int boolbpadding[2];   // 16
}

cbuffer TransformBuffer : register(b2)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer LightDirBuffer : register(b3)
{
    float4 vLightDir;
    float4 vLightColor;
    float4 gWorldCameraPosition;
    float4 pad[1];
}

cbuffer MatrixPallete : register(b4)
{
    matrix MatrixPalleteArray[128];
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : UV; // 버텍스 쉐이더의 시멘틱은 의미있는 이름로 짓는 게 좋다.
    float3 Norm : NORMAL;
    float3 TangentWorld : TANGENT;

    int4 BlendIndices : BLENDINDICES;
    float4 BlendWeights : BLENDWEIGHTS;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : UV;
    float3 Norm : NORMAL;
    float3 TangentWorld : TANGENT;
    
    float3 mDiffuse : TEXCOORD1;
    float3 mViewDir : TEXCOORD2;
    float3 mReflection : TEXCOORD3;
};