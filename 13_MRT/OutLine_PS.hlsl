#include "Shared.hlsli"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

Texture2D image : register(t0);
 
static float xFilter[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
static float yFilter[9] = { 1, 2, 1, 0, 0, 0, -1, -2, -1 };
 
struct PS_Out
{
    float4 color : SV_TARGET0;
};

PS_Out main(PS_INPUT Input)
{
    float width, height;
    image.GetDimensions(width, height);
 
    float2 tx = float2(outlineThickness / width, 0.0);
    float2 ty = float2(0.0, outlineThickness / height);
 
    float grid[9];
    float3 greyScale = float3(0.299, 0.587, 0.114);
 
    grid[0] = length(dot(image.Sample(samClamp, Input.Tex - tx + ty).xyz, greyScale));
    grid[1] = length(dot(image.Sample(samClamp, Input.Tex + ty).xyz, greyScale));
    grid[2] = length(dot(image.Sample(samClamp, Input.Tex + tx + ty).xyz, greyScale));
    grid[3] = length(dot(image.Sample(samClamp, Input.Tex - tx).xyz, greyScale));
    grid[4] = length(dot(image.Sample(samClamp, Input.Tex).xyz, greyScale));
    grid[5] = length(dot(image.Sample(samClamp, Input.Tex + tx).xyz, greyScale));
    grid[6] = length(dot(image.Sample(samClamp, Input.Tex - tx - ty).xyz, greyScale));
    grid[7] = length(dot(image.Sample(samClamp, Input.Tex - ty).xyz, greyScale));
    grid[8] = length(dot(image.Sample(samClamp, Input.Tex + tx - ty).xyz, greyScale));
    float sx = 0.0f, sy = 0.0f;
    for (int i = 0; i < 9; ++i)
    {
        sx += grid[i] * xFilter[i];
        sy += grid[i] * yFilter[i];
    }
    float dist = sqrt(sx * sx + sy * sy);
    float edge = dist > outlineThreshHold ? 1 : 0;
 
    float4 result = outlineColor;
    result.a = result.a * edge;
 
    PS_Out output;
    output.color = result;
    return output;
}
