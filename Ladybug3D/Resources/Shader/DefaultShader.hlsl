
cbuffer cb0 : register(b0)
{
    float4x4 g_WorldMatrix;
    float4x4 g_wvpMatrix;
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 tangent : TANGENT;
    float3 color : COLOR;
};


struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 tangent : TANGENT;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    //output.position = mul(float4(input.position, 1.0f), g_WorldMatrix);
    output.position = float4(input.position, 1.0f);
    output.normal = normalize(mul(input.normal, (float3x3) g_WorldMatrix));
    output.tangent = normalize(mul(input.tangent, g_WorldMatrix));
    output.uv = input.uv;
    output.color = float4(input.color, 1.0f);
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
