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

// Returns number of mipmap levels for specular IBL environment map.
uint querySpecularTextureLevels()
{
    uint width, height, levels;
    txIBL_Specular.GetDimensions(0, width, height, levels);
    return levels;
}


float4 main(PS_INPUT Input) : SV_Target
{
    float3 normal = normalize(Input.Norm);
    float3x3 Wtransform;
    
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
    
    if (UseGamma)
        texAlbedo.rgb = pow(texAlbedo.rgb, 2.2f); // gamma
    
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

	// Specular reflection vector.
    float3 Lr = 2.0 * cosLo * normal + Input.mViewDir;

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
    
    float3 ambientLighting = 0;
    float3 pointAmbientLighting = 0;

    if (UseIBL)
    {
        // Sample diffuse irradiance at normal direction.
		// ǥ���� �޴� �ݱ��� ���� ���⿡�� ���� �������� ���ø��Ѵ�. Lambertian BRDF�� �����Ͽ� ���ԵǾ� �ִ�.
        float3 irradiance = txIBL_Diffuse.Sample(samLinear, normal).rgb;

		// Calculate Fresnel term for ambient lighting.
		// Since we use pre-filtered cubemap(s) and irradiance is coming from many directions 
		// use cosLo instead of angle with light's half-vector (cosLh above).
		// See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
		// �񽺵��� ��������, cos���� �� ������ Ư���Ҽ� ���� �ݱ��� ���� ���⿡�� ���� �������̹Ƿ� 
		// dot(Half,View)�� ��������ʰ�  dot(Normal,View)�� ����Ѵ�.
        float3 F = fresnelSchlick(F0, cosLo);

		// Get diffuse contribution factor (as with direct lighting).
		// ǥ���� ������ ���Ѵ�.
        float3 kd = lerp(1.0 - F, 0.0, Metalness);

		// Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
		// txIBL_Diffuse �ʿ��� Lambertian BRDF�� �����Ͽ� ���ԵǾ� �����Ƿ� PI�� �����ʿ䰡 ����.
        float3 diffuseIBL = kd * texAlbedo * irradiance; // IBL�� diffuse ��

		// Sample pre-filtered specular reflection environment at correct mipmap level.
        uint specularTextureLevels = querySpecularTextureLevels(); // ��ü LOD �Ӹ� ������ 
		// View,Normal �ݻ纤�͸� ����Ͽ� ���ø��Ѵ�. ��ĥ�⿡ ���� ������ �ݻ���� ǥ���ϱ�����  LOD ������ ����ȴ�. 
        float3 specularIrradiance = txIBL_Specular.SampleLevel(samLinear, Lr, Roughness * specularTextureLevels).rgb;

		// Split-sum approximation factors for Cook-Torrance specular BRDF.
		// dot(Normal,View) , roughness�� �ؼ���ǥ�� ���ø��Ͽ� �̸����� x,y���� �����´�.
        float2 specularBRDF = txIBL_SpecularBRDF_LUT.Sample(samClamp, float2(cosLo, Roughness)).rg;

		// Total specular IBL contribution.
        float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

		// Total ambient lighting contribution.
        ambientLighting = (diffuseIBL + specularIBL) * AmbientOcclusion;
    }
           
    if (UseLightIBL)
    {
        float3 rotMatrix = mul(normal, (float3x3)m_TestLocal);

        float3 irradiance = txLightIBL_Diffuse.Sample(samLinear, rotMatrix).rgb;
        // float3 irradiance = txLightIBL_Diffuse.Sample(samLinear, normal).rgb;
        
        float3 F = fresnelSchlick(F0, cosLo);
        float3 kd = lerp(1.0 - F, 0.0, Metalness);
        float3 diffuseIBL = kd * texAlbedo * irradiance; // IBL�� diffuse ��
        uint specularTextureLevels = querySpecularTextureLevels(); // ��ü LOD �Ӹ� ������ 

        float3 specularIrradiance = txLightIBL_Specular.SampleLevel(samLinear, Lr, Roughness * specularTextureLevels).rgb;
        float2 specularBRDF = txLightIBL_SpecularBRDF_LUT.Sample(samClamp, float2(cosLo, Roughness)).rg;
        float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

        pointAmbientLighting = (diffuseIBL + specularIBL) * AmbientOcclusion;
    }
    
    float3 final = directLighting + ambientLighting + pointAmbientLighting * 2 + Emissive;
    float4 finalColor = float4(final, Opacity);
    
    if (UseGamma)
        finalColor.rgb = pow(finalColor.rgb, 1 / 2.2f); // gamma
    return finalColor;
};

//// ĳ���� ��ġ
//float3 characterPosition = ...;

//// ť��� �ؽ�ó ��ǥ
//float3 cubemapTexCoord = normalize(characterPosition - CameraPosition);

//// ī�޶��� �� ����� ����ķ� ȸ�� ��ȯ
//float3 rotatedTexCoord = mul(cubemapTexCoord, (float3x3) inverse(ViewMatrix));

//// ȸ���� �ؽ�ó ��ǥ�� ����Ͽ� ť��� ���ø�
//float3 IBLColor = txIBL.Sample(samLinear, rotatedTexCoord).rgb;

//// ���� ��꿡 IBLColor�� ���


