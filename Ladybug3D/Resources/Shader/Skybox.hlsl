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

PSInput VSMain(VSInput input)
{
	PSInput output;
    output.position = mul(float4(input.position, 1.0f), g_CurWvpMatrix).xyww; // z / w = 1이 되도록(즉 하늘 돔이 항상 면 평면에 있도록) z = w로 설정
    output.localPosition = input.position;
    return output;
}

struct PS_OUTPUT_Deferred
{
    float4 pos : WORLD_POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR;
    float4 light : COLOR1;
    float4 depth : COLOR2;
};

float4 PSMain(PSInput input) : SV_TARGET
{
    Pixel_DeferredOpaque output;
    output.pos = input.inLocalPos * 1000 + CameraPosition;
    output.colorFlag = -1.0f;
    output.normal = float3(-1.0f, -1.0f, -1.0f);
    output.depth = 1.0f;
    output.color = SkyboxCubeMap.Sample(LinearWrap, input.inLocalPos) * SkyBoxColor;
    output.metal = 1.0f;
    output.emission = 1.0f;
    output.roughness = 1.0f;
    return output;
}

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.position = mul(g_CurWvpMatrix, float4(input.position, 1.0f));
    output.normal = normalize(mul(input.normal, (float3x3) g_WorldMatrix));
    output.tangent = normalize(mul(input.tangent, g_WorldMatrix));
    output.uv = input.uv;

    return output;
}

cbuffer ConstantBuffer : register(b0)
{
	float4x4 worldViewProjection;
	float4x4 world;
	float4x4 view;
	float4x4 projection;
	float4x4 sview; //not used
	float4x4 sprojection; //not used
};

VertexToPixel main(VertexInput input)
{
	// Set up output
	VertexToPixel output;

	// Make a view matrix with NO translation
	matrix viewNoMovement = view;
	viewNoMovement._41 = 0;
	viewNoMovement._42 = 0;
	viewNoMovement._43 = 0;

	// Calculate output position
	matrix viewProj = mul(viewNoMovement, projection);
	output.position = mul(float4(input.pos, 1.0f), viewProj);

	// Ensure our polygons are at max depth
	output.position.z = output.position.w;

	// Use the cube's vertex position as a direction in space
	// from the origin (center of the cube)
	output.uvw = input.pos;

	return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
