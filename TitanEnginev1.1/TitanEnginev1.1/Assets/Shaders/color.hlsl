
#include "LightUtil.hlsl"


cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
	float4x4 gWorld; 
	float4x4 gViewProj; 
	float4x4 gLocation;
	float4x4 gRotation;
	float4x4 gScale;
	Light gLight;
	float1 gTime;
};

Texture2D gDiffuseMap : register(t0);
Texture2D gShadowMap : register(t1);
Texture2D gNormalMap : register(t2);


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
	float4x4 gLightTVP;
}



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

float CalcShadowFactor(float4 shadowPosH)
{
    // Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;

    // Depth in NDC space.
    float depth = shadowPosH.z;

    uint width, height, numMips;
    gShadowMap.GetDimensions(0, width, height, numMips);

    // Texel size.
    float dx = 1.0f / (float)width;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx,  -dx), float2(0.0f,  -dx), float2(dx,  -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx,  +dx), float2(0.0f,  +dx), float2(dx,  +dx)
    };

    [unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += gShadowMap.SampleCmpLevelZero(gsamShadow,
            shadowPosH.xy + offsets[i], depth).r;
    }

    return percentLit / 9.0f;
}


float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
    float3 normalT = 2.0f * normalMapSample - 1.0f;

    float3 N = unitNormalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
	float4 Normal: NORMAL;
    float4 TangentX : TANGENTX;
	float2 Texcoord : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 ShadowPosH : POSITION0;
    float3 PosW : POSITION1;
    float4 Color : COLOR;
	float3 Normal: NORMAL;
    float3 TangentX : TANGENTX;

	float2 Texcoord : TEXCOORD;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	//float4 Normal = mul(vin.Normal, gRotation);
	float3 Pos = vin.PosL;
	float4 PosW = mul(float4(Pos, 1.0f), gWorld);
    vout.PosW = PosW.xyz;
	vout.PosH = mul(PosW, gViewProj);


	vout.Texcoord = vin.Texcoord;
	vout.Color = vin.Color;

	//vout.Normal = mul(gRotation, vin.Normal);
    vout.Normal = mul(vin.Normal.xyz, (float3x3)gWorld);
    vout.TangentX = mul(vin.TangentX.xyz, (float3x3)gWorld);
	vout.ShadowPosH = mul(PosW, gLightTVP);

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.Texcoord);
    float3 normalMap = gNormalMap.Sample(gsamAnisotropicWrap, pin.Texcoord);

    float shadowFactor = CalcShadowFactor(pin.ShadowPosH);
    float3 toEyeW = normalize(Cameraloc - pin.PosW);

    // need to normalize our normal and tangent ater we transform them to world space
    pin.Normal = normalize(pin.Normal);
    pin.TangentX = normalize(pin.TangentX);


    const float shininess = 1.0f - gRoughness;

    float3 bumpedNormalMap = NormalSampleToWorldSpace(normalMap, pin.Normal, pin.TangentX);

    float3 Fr0 = {0.5f, 0.5f, 0.5f};

    Material mat = { diffuseAlbedo, Fr0, shininess };
    //
    float4 directionLight = ComputeLighting(gLight, mat, bumpedNormalMap, toEyeW, shadowFactor);

    float4 ambient = 0.1 * diffuseAlbedo;

	// normal color
	float4 test = mul(diffuseAlbedo, gDiffuseAlbedo);
   
    //litColor.a = gDiffuseAlbedo.a;

    pin.Color = pow(float4((pin.Normal * 0.5f + 0.5f), 1.0f), 1 / 2.2f);

    float4 litColor = ambient + directionLight;
    return pow(litColor, 1/2.2f);
  	//return ambient * (shadowFactor + 0.1);
    //return pin.Color * (shadowFactor + 0.1);
	//return test;

}


