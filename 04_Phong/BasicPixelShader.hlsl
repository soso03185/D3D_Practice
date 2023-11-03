#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------


float4 main(PS_INPUT Input) : SV_Target
{
    float3 normal = normalize(Input.Norm);
    float3 diffuse = saturate(dot(-vLightDir.xyz, normal));
    float3 reflection = reflect(vLightDir.xyz, normal);
   	float3 viewDir = normalize(Input.mViewDir);
    
    /*
     *  �ٽ� normalize �� ���ִ� ����
     *      ���� �Ǹ鼭 ���� ����Ǿ��� �� �ֱ� �����̴�.
     */
    
    float3 specular = float3(0.0f, 0.0f, 0.0f);

	if (diffuse.x > 0)
	{
		specular = saturate(dot(reflection, -viewDir));
        specular = pow(specular, gCubeSpecularPower);
    }
    
    /*
     *  ���ݻ籤(diffuse)�� �������� �ʴ� ǥ��(diffuse.x < 0)���� ���� ���� �����Ƿ�
     *  ���ݻ籤�� ������ �� ���� ������ ��꿡�� �����Ѵ�.
     *
     *  pow �Լ��� �ŵ������� ���� ��� ���� �����ϸ� ���ݻ籤�� ������ Ÿ��Ʈ������.
     */
     
    return txDiffuse.Sample(samLinear, Input.Tex) * float4(gCubeAmbient + diffuse + specular, 1) * vLightColor;
};
