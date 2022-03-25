cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
	float4x4 gWorld; 
	float4x4 gViewProj; 
	float4x4 gLocation;
	float4x4 gRotation;
	float4x4 gScale;
	float1 gTime;
};

Texture2D gDiffuseMap : register(t0);
//Texture2D gNormal : register(t1);

float3 Cameraloc : register(b1);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

cbuffer cbMaterial : register(b2)
{
	float4   gDiffuseAlbedo;
    float3   gFresnelR0;
    float    gRoughness;
}

cbuffer cbShadowPass : register(b3)
{
	float4x4 gLightViewProj;
}



struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
	float4 Normal: NORMAL;
	float2 Texcoord : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 ShadowPosH : POSITION0;
    float4 Color : COLOR;
	float4 Normal: NORMAL;
	float2 Texcoord : TEXCOORD;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	//float4 Normal = mul(vin.Normal, gRotation);
	float3 Pos = vin.PosL;
	float4 PosW = mul(float4(Pos, 1.0f), gWorld);
	vout.PosH = mul(PosW, gLightViewProj);



	vout.Texcoord = vin.Texcoord;
	vout.Color = vin.Color;
	vout.Normal = mul(gRotation, vin.Normal);
	vout.ShadowPosH = vout.PosH;

    return vout;
}

