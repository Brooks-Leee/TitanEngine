
Texture2D gSceneColor : register(t0);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);


cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
	float4x4 gWorld; 
	float4x4 gViewProj; 
	float4x4 gLocation;
	float4x4 gRotation;
	float4x4 gScale;
	
};


float4 RTsize : register(b1);

cbuffer cbMaterial : register(b2)
{
	float4   gDiffuseAlbedo;
    float3   gFresnelR0;
    float    gRoughness;
}

cbuffer cbShadowPass : register(b3)
{
	float4x4 gLightViewProj;
	float4x4 gLightTVP;
}

float4 RenderTargetSize : register(b4);

struct VertexIn
{
	float3 PosL  : POSITION;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = float4(vin.PosL, 0);

	return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
   

    float4 position = pin.PosH;

	int X = floor(position.x);
	int Y = floor(position.y);

    // float2 RenderTargetSize = float2(800, 600);

    float2 RenderTargetSize = RTsize.xy;

	float Width = RenderTargetSize[0];
	float Height = RenderTargetSize[1];



	float DeltaU = 1.0f / RenderTargetSize[0];
	float DeltaV = 1.0f / RenderTargetSize[1];

	float2 Tex;
	Tex.x = 1.0f * X / Width;
	Tex.y = 1.0f * Y / Height;
	
	float4 OutColor;
    OutColor = gSceneColor.Sample(gsamAnisotropicWrap, Tex);



	return OutColor;
    //return float4(0.5f, 0.6f, 1.0f, 1.0f);
}