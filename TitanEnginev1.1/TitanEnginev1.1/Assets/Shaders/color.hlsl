// #define WaterRock_RootSig \
// 	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAUOUT), " \
// 	"RootConstants(b0, num32BitConstants = 4), " \
// 	"CBV(b1, visibility = SHADER_VISIBILITY_VERTEX), " \
// 	"DescriptorTable(SRV(t0, numDescriptors = 4), visibility = SHADER_VISIBILITY_PIXEL)," \
// 	"StaticSampler(s0," \
// 		"addressU = TEXTURE_ADDRESS_WRAP," \
// 		"addressV = TEXTURE_ADDRESS_WRAP," \
// 		"addressW = TEXTURE_ADDRESS_WRAP," \
// 		"filter = FILTER_MIN_MAG_MIP_LINEAR),"




cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
	float4x4 gLocation;
	float4x4 gRotation;
	float4x4 gScale;
	float1 gTime;
};

Texture2D gDiffuseMap : register(t0);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

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
    float4 Color : COLOR;
	float4 Normal: NORMAL;
	float2 Texcoord : TEXCOORD;
};



VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	//float4 Normal = mul(vin.Normal, gRotation);
	float3 Pos = vin.PosL;

	vout.PosH = mul(float4(Pos, 1.0f), gWorldViewProj);
	vout.Texcoord = vin.Texcoord;
	vout.Color = vin.Color;
	vout.Normal = mul(gRotation, vin.Normal);
	//vout.Normal = vin.Normal;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.Texcoord);
	pin.Color = pow((pin.Normal * 0.5f + 0.5f), 1/2.2f);
	////pin.Color = (pin.Normal * 0.5f + 0.5f);
	pin.Color = diffuseAlbedo;

    return diffuseAlbedo;
}