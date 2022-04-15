
Texture2D gSceneColor : register(t0);
Texture2D gSunMergeColor : register(t1);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gBloomInputSampler   : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gSceneColorSampler  : register(s4);
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

float3 ACESToneMapping(float3 color, float adapted_lum)
{
	const float A = 2.51f;
	const float B = 0.03f;
	const float C = 2.43f;
	const float D = 0.59f;
	const float E = 0.14f;

	color *= adapted_lum;
	return (color * (A * color + B)) / (color * (C * color + D) + E);
}



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

	float2 RenderTargetSize = {800, 600};


    int X = floor(position.x);
	int Y = floor(position.y);

	float2 Tex;
	Tex.x = 1.0f * X / RenderTargetSize[0];
	Tex.y = 1.0f * Y / RenderTargetSize[1];

	float4 SceneColor = gSceneColor.Sample(gSceneColorSampler, Tex);
	float4 BloomColor = gSunMergeColor.Sample(gSceneColorSampler, Tex);

	half3 LinearColor = SceneColor.rgb + BloomColor.rgb;
	
	float4 OutColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	OutColor.rgb = ACESToneMapping(LinearColor, 1.0f);
	OutColor.a = SceneColor.a;

	return OutColor;

}


