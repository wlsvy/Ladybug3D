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
	float3 localPosition : LOCAL_POSITION;
};

struct PS_OUTPUT_Deferred
{
    float4 pos : WORLD_POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR;
    float4 light : COLOR1;
    float4 depth : COLOR2;
};



PSInput VSMain(VSInput input)
{
	PSInput output;
    float4x4 wvpMatrix = g_CameraWorldMatrix * g_ViewProjMatrix;
    output.position = mul(wvpMatrix, float4(input.position, 1.0f)).xyww; // z / w = 1이 되도록(즉 하늘 돔이 항상 면 평면에 있도록) z = w로 설정
    output.localPosition = input.position;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    //return float4(0.2f, 1.0f, 1.0f, 1.0f);

    //return SkyboxCubeMap.Sample(PointClamp, input.localPosition);
    //float3 col = SkyboxCubeMap.Sample(PointClamp, input.localPosition);
    //return float4(col, 1.0f);
    return float4(0.4f, 0.4f, 0.7f, 1.0f);

    /*Pixel_DeferredOpaque output;
    output.pos = input.inLocalPos * 1000 + CameraPosition;
    output.colorFlag = -1.0f;
    output.normal = float3(-1.0f, -1.0f, -1.0f);
    output.depth = 1.0f;
    output.color = SkyboxCubeMap.Sample(LinearWrap, input.inLocalPos) * SkyBoxColor;
    output.metal = 1.0f;
    output.emission = 1.0f;
    output.roughness = 1.0f;
    return output;*/
}