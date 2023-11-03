#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------


float4 main(PS_INPUT Input) : SV_Target
{
    float3 normal = normalize(Input.Norm);
    float3 vTangent = normalize(Input.TangentWorld);
    float3 vBiTangent = cross(normal, vTangent);
    
    if (UseNormalMap)
    {
        float3 vNormalTangentSpace = txNormal.Sample(samLinear, Input.Tex).rgb * 2.0f - 1.0f;
        float3x3 WorldTransform = float3x3(vTangent, vBiTangent, normal);
        normal = mul(vNormalTangentSpace, WorldTransform);
        normal = normalize(normal);
    }
   
    float4 texColor = txColor.Sample(samLinear, Input.Tex);
    if (UseGamma)
        texColor.rgb = pow(texColor.rgb, 2.2f); // gamma
    
    float4 normLightDir = normalize(vLightDir);
    float3 diffuse = saturate(dot(-normLightDir.xyz, normal)) * texColor;
    float3 reflection = reflect(normLightDir.xyz, normal);
    float3 viewDir = normalize(Input.mViewDir);

    float3 specular = float3(0.0f, 0.0f, 0.0f);
    float3 specularMap = txSpecular.Sample(samLinear, Input.Tex);              

    if (diffuse.x > 0)
    {
        specular = saturate(dot(reflection, -viewDir));
        specular = pow(specular, gCubeSpecularPower);
        
        if (UseSpecularMap)
        {
            specular *= specularMap.rgb;
        }
    }
    
    /*
     *  ���ݻ籤(diffuse)�� �������� �ʴ� ǥ��(diffuse.x < 0)���� ���� ���� �����Ƿ�
     *  ���ݻ籤�� ������ �� ���� ������ ��꿡�� �����Ѵ�.
     *
     *  pow �Լ��� �ŵ������� ���� ��� ���� �����ϸ� ���ݻ籤�� ������ Ÿ��Ʈ������.
     */      
    
    float3 ambient = gCubeAmbient * texColor;
    
    float4 finalColor = float4(ambient + diffuse + specular, 1) * vLightColor;
    
    if (UseGamma)
        finalColor.rgb = pow(finalColor.rgb, 1 / 2.2f); // gamma
    
    return finalColor;
};
