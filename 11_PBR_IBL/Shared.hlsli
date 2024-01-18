//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txSpecular : register(t2);
Texture2D txEmissive : register(t3);
Texture2D txOpacity : register(t4);
Texture2D txMetalness : register(t5);
Texture2D txRoughness : register(t6);

TextureCube txEnvironment : register(t7);
TextureCube txIBL_Diffuse : register(t8);
TextureCube txIBL_Specular : register(t9);
Texture2D txIBL_SpecularBRDF_LUT : register(t10);

SamplerState samLinear : register(s0);
SamplerState samClamp : register(s1);

cbuffer IBL_Buffer : register(b0)
{
    int UseIBL = false;              // 4 
    float AmbientOcclusion = 1.0f;   // 4
    float IBL_pad[2];                // 8  ,  16byte
}

cbuffer BoolBuffer : register(b1)
{
    int UseNormalMap;
    int UseSpecularMap;
    int UseGamma;
    int UseDiffuseMap;     // 16
    
    int UseEmissiveMap;
    int UseOpacityMap;
    int UseMetalnessMap;
    int UseRoughnessMap;   // 16
}

cbuffer TransformW : register(b2)
{
    matrix World;
}

cbuffer TransformVP : register(b3)
{
    matrix View;
    matrix Projection;
}

cbuffer LightDirBuffer : register(b4)
{
    float4 vLightDir;
    float4 vLightColor;
    float4 gWorldCameraPosition;
    float4 pad[1];
}

cbuffer MatrixPalette : register(b5)
{
    matrix MatrixPaletteArray[128];
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : UV; // 버텍스 쉐이더의 시멘틱은 의미있는 이름으로 짓는 게 좋다.
    float3 Norm : NORMAL;
    float3 TangentWorld : TANGENT;
    
#ifdef VERTEX_SKINNING 
    int4 BlendIndices : BLENDINDICES;
    float4 BlendWeights : BLENDWEIGHT;
#endif
 
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

struct PS_INPUT_ENVIRONMENT
{
    float4 PositionProj : SV_POSITION;
    float3 TexCoords : TEXCOORD0;
};