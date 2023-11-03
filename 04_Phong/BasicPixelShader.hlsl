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
     *  다시 normalize 를 해주는 이유
     *      보간 되면서 값이 변경되었을 수 있기 때문이다.
     */
    
    float3 specular = float3(0.0f, 0.0f, 0.0f);

	if (diffuse.x > 0)
	{
		specular = saturate(dot(reflection, -viewDir));
        specular = pow(specular, gCubeSpecularPower);
    }
    
    /*
     *  난반사광(diffuse)이 존재하지 않는 표면(diffuse.x < 0)에는 빛이 닿지 않으므로
     *  정반사광이 존재할 수 없기 때문에 계산에서 제외한다.
     *
     *  pow 함수의 거듭제곱에 들어가는 상수 값을 조절하면 정반사광의 범위가 타이트해진다.
     */
     
    return txDiffuse.Sample(samLinear, Input.Tex) * float4(gCubeAmbient + diffuse + specular, 1) * vLightColor;
};
