
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



Texture2D gSceneColor : register(t0);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6);


float Luminance(float3 InColor)
{
	return dot(InColor, float3(0.3f, 0.59f, 0.11f));
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
    const float BloomThreshold = 1.0f;

    float4 position = pin.PosH;

	int X = floor(position.x);
	int Y = floor(position.y);

    // float2 RenderTargetSize = float2(800, 600);

    float2 RenderTargetSize = RTsize.xy;

	float Width = RenderTargetSize[0] * 0.25f;
	float Height = RenderTargetSize[1] * 0.25f;



	float DeltaU = 1.0f / RenderTargetSize[0];
	float DeltaV = 1.0f / RenderTargetSize[1];

	float2 Tex;
	Tex.x = 1.0f * X / Width;
	Tex.y = 1.0f * Y / Height;

	float4 Color0 = gSceneColor.Sample(gsamAnisotropicWrap, Tex + float2(-DeltaU, -DeltaV));
	float4 Color1 = gSceneColor.Sample(gsamAnisotropicWrap, Tex + float2(+DeltaU, -DeltaV));
	float4 Color2 = gSceneColor.Sample(gsamAnisotropicWrap, Tex + float2(-DeltaU, +DeltaV));
	float4 Color3 = gSceneColor.Sample(gsamAnisotropicWrap, Tex + float2(+DeltaU, +DeltaV));

	float4 AvailableColor = Color0 * 0.25f + Color1 * 0.25f + Color2 * 0.25f + Color3 * 0.25f;
	
	//AvailableColor.rgb = max(AvailableColor.rgb, 0) * OneOverPreExposure;

	float TotalLuminance = Luminance(AvailableColor.rgb);
	float BloomLuminance = TotalLuminance - BloomThreshold;
	float Amount = saturate(BloomLuminance * 0.5f);

	float4 OutColor;
	OutColor.rgb = AvailableColor.rgb;
	OutColor.rgb *= Amount;
	OutColor.a = 0.0f;

    //   if(outColor.x < 1.0f || outColor.y < 1.0f || outColor.z < 1.0f)
    // {
    //     outColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    // }


	return OutColor;
    //return float4(0.5f, 0.6f, 1.0f, 1.0f);
}