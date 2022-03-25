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
	float4x4 gWorld; 
	float4x4 gViewProj; 
	float4x4 gLocation;
	float4x4 gRotation;
	float4x4 gScale;
	float1 gTime;
};

Texture2D gDiffuseMap : register(t0);
//Texture2D gNormal : register(t1);
Texture2D gShadowMap : register(t1);


float3 Cameraloc : register(b1);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);


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


float CalcShadow(float4 shadowPosH)
{
    // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;

    // Depth in NDC space.
    float depth = shadowPosH.z;

    uint width, height, numMips;
    gShadowMap.GetDimensions(0, width, height, numMips);
	float2 pixelPos = shadowPosH.xy * width;
	float depthInMap = gShadowMap.Load(int3(pixelPos, 0)).r;
	return depth > depthInMap ? 0 : 1;

}

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	//float4 Normal = mul(vin.Normal, gRotation);
	float3 Pos = vin.PosL;
	float4 PosW = mul(float4(Pos, 1.0f), gWorld);
	vout.PosH = mul(PosW, gViewProj);

	//vout.PosH = mul(float4(Pos, 1.0f), gWorldViewProj);

	vout.Texcoord = vin.Texcoord;
	vout.Color = vin.Color;
	vout.Normal = mul(gRotation, vin.Normal);

	vout.ShadowPosH = mul(PosW, gLightViewProj);

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.Texcoord);
 
	float4 AmbientLight = {1.0f, 0.8f, 1.0f, 1.0f};
	float4 ambient = diffuseAlbedo;

  //  float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
    float shadowFactor = CalcShadow(pin.ShadowPosH);

	float4 colorshadow = {0 , 0 ,0, 0};
	// normal color
	pin.Color = pow((pin.Normal * 0.5f + 0.5f), 1/2.2f);
	float4 test = mul(diffuseAlbedo, gDiffuseAlbedo);
	if(shadowFactor == 1)
	{
		ambient = 0;
	}
  	return ambient;
    //return pin.Color;
	//return test;

}