#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

static const float PI = 3.141592;
static const float Epsilon = 0.00001;

// Constant normal incidence Fresnel factor for all dielectrics.
static const float3 Fdielectric = 0.04;

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}



float4 main(PS_INPUT Input) : SV_Target
{
    float3 normal = normalize(Input.Norm);
    
    if (UseNormalMap)
    {
        float3 vTangent = normalize(Input.TangentWorld);
        float3 vBiTangent = cross(normal, vTangent);    
        float3 vNormalTangentSpace = txNormal.Sample(samLinear, Input.Tex).rgb * 2.0f - 1.0f;
        
        float3x3 WorldTransform = float3x3(vTangent, vBiTangent, normal);
        normal = mul(vNormalTangentSpace, WorldTransform);
        normal = normalize(normal);
    }
    
    float4 normLightDir = normalize(vLightDir);
    float3 texAlbedo = txDiffuse.Sample(samLinear, Input.Tex).rgb;
    
    if (UseGamma)  texAlbedo.rgb = pow(texAlbedo.rgb, 2.2f); // gamma
    
    float Metalness = 0.0f;
    float Roughness = Epsilon;
    float Opacity = 1.0f;
    float3 Emissive = 0.0f;
    
    if (UseEmissiveMap)
        Emissive = txEmissive.Sample(samLinear, Input.Tex).rgb;
    if (UseMetalnessMap)
        Metalness = txMetalness.Sample(samLinear, Input.Tex).r;
    if (UseRoughnessMap)
        Roughness = txRoughness.Sample(samLinear, Input.Tex).r;
    if (UseOpacityMap)
        Opacity = txOpacity.Sample(samLinear, Input.Tex).a;
              
    //--------------------------------------------------------------------------
    //? PBR
    //
    
    // ���� Input.mViewDir �� ī�޶󿡼� ���ؽ� ����.    
    
    // ǥ�� ���� ����� ������ �� ���� ������ �����Դϴ�.
    float cosLo = max(0.0, dot(normal, -Input.mViewDir));

	// ���� �Ի翡���� ������ �ݻ���(�ݼ��� ��� �˺��� ���� ���)
    float3 F0 = lerp(Fdielectric, texAlbedo, Metalness);
    
	// Li�� Lo ������ ���� ����.
    float3 Lh = normalize(-normLightDir.rgb - Input.mViewDir);

	// ǥ�� ������ �پ��� ����Ʈ ���� ������ ������ ����մϴ�.
    float cosLi = max(0.0, dot(normal, -normLightDir.rgb));
    float cosLh = max(0.0, dot(normal, Lh));

    float3 F = fresnelSchlick(F0, max(0.0, dot(Lh, -Input.mViewDir)));
    float D = ndfGGX(cosLh, max(0.1f, Roughness));
    float G = gaSchlickGGX(cosLi, cosLo, Roughness);
    
	// Cook-Torrance specular microfacet BRDF.
    float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

	// Ȯ�� ����� ���� ���� ��ü�� ���� ���� �� �����Ǳ� ������ �߻��մϴ�.
    // �ݸ鿡 �ݼ��� �������� �ݻ��ϰų� ����ϹǷ� Ȯ�� �⿩���� �׻� 0�Դϴ�.
    // �������� �����Ϸ��� ������ ���� �� �ݼӼ��� ������� Ȯ�� BRDF �⿩���� �����ؾ� �մϴ�.
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), Metalness);

	// ����Ʈ Ȯ�� BRDF.
    // ���� ���ϰ� ���� �� ��� ������ ���� 1/PI ������ ũ�⸦ �������� �ʽ��ϴ�.
    float3 diffuseBRDF = kd * texAlbedo;

	// Total contribution for this light.
    float3 directLighting = (diffuseBRDF + specularBRDF) * vLightColor.rgb * cosLi;
        
    // --------------------------------------------------------------------------
    //? IBL
    //
    
    float3 ambientLighting = 0;

    if (UseIBL)
    {
        
    }
    
    float3 final = directLighting + ambientLighting + Emissive;
    float4 finalColor = float4(final, Opacity);
    
    if (UseGamma)
        finalColor.rgb = pow(finalColor.rgb, 1 / 2.2f); // gamma
    return finalColor;
};
