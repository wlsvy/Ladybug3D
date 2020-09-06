
struct VSInput
{
    float4 position : POSITION;
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

    output.position = input.position;
    output.normal = input.normal;
    output.uv = input.uv;
    output.tangent = input.tangent;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
