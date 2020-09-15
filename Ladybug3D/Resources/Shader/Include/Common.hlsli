#ifndef _COMMON_HLSLI__
#define _COMMON_HLSLI__

cbuffer CB_PerScene : register(b0)
{
    float4x4 g_ViewMatrix;
    float4x4 g_ProjMatrix;
    float4x4 g_ViewProjMatrix;

    float4x4 g_CameraWorldMatrix;
    float4 g_CameraWorldPosition;
    float4 pad[3];

};
cbuffer CB_PerObject : register(b1)
{
    float4x4 g_WorldMatrix;
    float4x4 g_CurWvpMatrix;
    float4x4 g_PrevWvpMatrix;

};

Texture2D SampleTexture : register(t0);
TextureCube SkyboxCubeMap : register(t1);

SamplerState PointClamp : register(s0);
SamplerState LinearClamp : register(s1);
SamplerState LinearWrap : register(s2);
SamplerState LinearMirror : register(s3);
SamplerState AnisotropicMirror : register(s4);
SamplerState TrilinearMirror : register(s5);


//SamplerState MeshTextureSampler
//{
//    Filter = MIN_MAG_MIP_LINEAR;
//    AddressU = Wrap;
//    AddressV = Wrap;
//};

#endif