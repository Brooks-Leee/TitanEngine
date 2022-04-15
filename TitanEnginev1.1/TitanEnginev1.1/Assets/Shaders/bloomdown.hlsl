

Texture2D gBloomInput : register(t0);

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
    float BloomDownScale = 2.5f;

    float4 position = pin.PosH;

	int X = floor(position.x);
	int Y = floor(position.y);

	float2 RenderTargetSize = RTsize.xy;

    //float2 RenderTargetSize = float2(200, 150);

	float Width = RenderTargetSize[0] * 0.5f;
	float Height = RenderTargetSize[1] * 0.5f;



	float DeltaU = 1.0f / RenderTargetSize[0];
	float DeltaV = 1.0f / RenderTargetSize[1];
    float2 DeltaUV = float2(DeltaU, DeltaV);


	float2 Tex;
	Tex.x = 1.0f * X / Width;
	Tex.y = 1.0f * Y / Height;

	float StartRaduas = 2.0f / 14.0f;
	float4 Color0 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 0.0f));
	float4 Color1 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 1.0f));
	float4 Color2 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 2.0f));
	float4 Color3 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 3.0f));
	float4 Color4 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 4.0f));
	float4 Color5 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 5.0f));
	float4 Color6 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 6.0f));
	float4 Color7 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 7.0f));
	float4 Color8 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 8.0f));
	float4 Color9 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 9.0f));
	float4 Color10 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 10.0f));
	float4 Color11 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 11.0f));
	float4 Color12 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 12.0f));
	float4 Color13 = gBloomInput.Sample(gBloomInputSampler, Tex + DeltaUV * BloomDownScale * Circle(StartRaduas, 14.0f, 13.0f));

	float4 Color = gBloomInput.Sample(gBloomInputSampler, Tex);

	float Weight = 1.0f / 15.0f;
	
	Color = Weight * (Color + Color0 + Color1 + Color2 + Color3
					+ Color4 + Color5 + Color6 + Color7
					+ Color8 + Color9 + Color10 + Color11 + Color12 + Color13);

	return Color;
}
