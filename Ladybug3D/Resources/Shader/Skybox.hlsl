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
    output.position = mul(g_ViewProjMatrix, float4(input.position + g_CameraWorldPosition.xyz, 1.0f)).xyzz; // z / w = 1�� �ǵ���(�� �ϴ� ���� �׻� �� ��鿡 �ֵ���) z = w�� ����
    output.localPosition = input.position;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return SkyboxCubeMap.Sample(PointClamp, input.localPosition);
}