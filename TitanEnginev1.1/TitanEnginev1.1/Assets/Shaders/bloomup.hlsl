Texture2D gBloomUp : register(t0);
Texture2D gBloomDown : register(t1);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gBloomInputSampler   : register(s2);
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


float2 Circle(float Start, float Points, float Point)
{
	float Radians = (2.0f * 3.141592f * (1.0f / Points)) * (Start + Point);
	return float2(cos(Radians), sin(Radians));
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
    float BloomUpScale = 1.32f;

    float4 position = pin.PosH;

	int X = floor(position.x);
	int Y = floor(position.y);

	float4 OutColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float DeltaU = 1.0f / RTsize[0];
	float DeltaV = 1.0f / RTsize[1];
    float2 DeltaUV = float2(DeltaU, DeltaV);


	float2 Tex;
	Tex.x = 1.0f * X / RTsize[0];
	Tex.y = 1.0f * Y / RTsize[1];

	float Start = 2.0 / 7.0;
	float4 Color0 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 0.0f));
	float4 Color1 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 1.0f));
	float4 Color2 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 2.0f));
	float4 Color3 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 3.0f));
	float4 Color4 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 4.0f));
	float4 Color5 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 5.0f));
	float4 Color6 = gBloomUp.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 6.0f));
	float4 Color7 = gBloomUp.Sample(gBloomInputSampler, Tex);

	float4 Color8 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 0.0f));
	float4 Color9 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 1.0f));
	float4 Color10 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 2.0f));
	float4 Color11 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 3.0f));
	float4 Color12 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 4.0f));
	float4 Color13 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 5.0f));
	float4 Color14 = gBloomDown.Sample(gBloomInputSampler, Tex + DeltaUV * BloomUpScale * Circle(Start, 7.0, 6.0f));
	float4 Color15 = gBloomDown.Sample(gBloomInputSampler, Tex);


    float BloomWeightScalar = 1.0f / 10.0f;
    float BloomWeightScalar1 = 1.0f / 10.0f;


	float4 BloomWight = float4(BloomWeightScalar, BloomWeightScalar, BloomWeightScalar, 0.0f);
	float4 BloomWight1 = float4(BloomWeightScalar1, BloomWeightScalar1, BloomWeightScalar1, 0.0f);

	OutColor = (Color0 + Color1 + Color2 + Color3 + Color4 + Color5 + Color6 + Color7) * BloomWight +
		(Color8 + Color9 + Color10 + Color11 + Color12 + Color13 + Color14 + Color15) * BloomWight1;
	OutColor.a = 0.0f;


	return OutColor;
}
