#include "Include/Common.hlsli"



struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 tangent : TANGENT;
};


struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 tangent : TANGENT;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.position = mul(g_CurWvpMatrix, float4(input.position, 1.0f));
    output.normal = normalize(mul(input.normal, (float3x3) g_WorldMatrix));
    output.tangent = normalize(mul(input.tangent, g_WorldMatrix));
    output.uv = input.uv;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
