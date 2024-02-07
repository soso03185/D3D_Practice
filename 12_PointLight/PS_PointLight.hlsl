#include "shared.hlsli"


float4 main(PS_INPUT input) : SV_TARGET
{
    // 정점을 바라보는 방향    input.mViewDir;
    
    float3 normal = normalize(input.Norm);
    float4 normLightDir = normalize(vLightDir);
    
    float3 reflection = reflect(normLightDir.xyz, normal);
    float3 diffuse = saturate(dot(-normLightDir.xyz, normal));

    float3 specular = float3(0.0f, 0.0f, 0.0f);

    if (diffuse.x > 0)
    {
        specular = saturate(dot(reflection, -normalize(input.mViewDir)));        

    }
    
    
    
   // float3 FinalColor = att * pLightIntensity * (PhongD + PhongS);
    //  return float4(FinalColor, 1.f);
    
    return float4(0, 0, 0, 0);
}
