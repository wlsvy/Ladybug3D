//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

cbuffer CB_PerScene : register(b0)
{
    float4x4 g_ViewMatrix;
    float4x4 g_ProjMatrix;
    float4x4 g_ViewProjMatrix;
};
cbuffer CB_PerObject : register(b1)
{
    float4x4 g_WorldMatrix;
    float4x4 g_CurWvpMatrix;
    float4x4 g_PrevWvpMatrix;

};

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
