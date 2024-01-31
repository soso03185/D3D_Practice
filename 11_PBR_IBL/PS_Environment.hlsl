#include "Shared.hlsli"

float4 main(PS_INPUT_ENVIRONMENT input) : SV_Target
{    
    float4 nullReturn = 0;
    
    if (UseIBL)
        return txEnvironment.Sample(samLinear, input.TexCoords);    
    if (UseLightIBL)
        return txLightEnvironment.Sample(samLinear, input.TexCoords);
    
    return nullReturn;
}