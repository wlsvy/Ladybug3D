
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

    output.position = float4(input.position * 0.25f - float3(0.0f, 0.0f, 3.0f), 0.0f);
    output.position = float4(input.position, 0.0f);
    output.normal = input.normal;
    output.uv = input.uv;
    output.tangent = input.tangent;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 0.0f, 1.0f);
}
