
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

TextureCube SkyboxCubeMap : register(t0);

SamplerState PointClamp : SAMPLER: register(s0);
SamplerState LinearClamp : SAMPLER: register(s1);
SamplerState LinearWrap : SAMPLER: register(s2);
SamplerState LinearMirror : SAMPLER: register(s3);
SamplerState AnisotropicMirror : SAMPLER: register(s4);
SamplerState TrilinearMirror : SAMPLER: register(s5);

