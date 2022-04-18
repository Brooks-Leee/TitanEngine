
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

    float2 offsets[9] =
        {
            float2(-1, 1),
            float2( 0, 1),
            float2( 1, 1),
            float2(-1, 0),
            float2( 0, 0),
            float2( 1, 0),
            float2(-1,-1),
            float2( 0,-1),
            float2( 1,-1)
        };

    // sobel operator

    float3x3 Sobelx = float3x3( -1, +0, +1,
			                	-2, +0, +2,
			                	-1, +0, +1);

    float3x3 Sobely = float3x3( +1, +2, +1,
				                +0, +0, +0,
				                -1, -2, -1);

    // laplacian operator 

    float3x3 laplacian = float3x3(-1, -1, -1,
					              -1,  8, -1,
					              -1, -1, -1);

    float texsize = 1.0 / 512;
    float4 s0 = gSceneColor.SampleLevel(gsamAnisotropicWrap, Tex + offsets[0] * texsize, 0);
    float4 s1 = gSceneColor.SampleLevel(gsamAnisotropicWrap, Tex + offsets[1] * texsize, 0);
    float4 s2 = gSceneColor.SampleLevel(gsamAnisotropicWrap, Tex + offsets[2] * texsize, 0);
    float4 s3 = gSceneColor.SampleLevel(gsamAnisotropicWrap, Tex + offsets[3] * texsize, 0);
    float4 s4 = gSceneColor.SampleLevel(gsamAnisotropicWrap, Tex + offsets[4] * texsize, 0);
    float4 s5 = gSceneColor.SampleLevel(gsamAnisotropicWrap, Tex + offsets[5] * texsize, 0);
    float4 s6 = gSceneColor.SampleLevel(gsamAnisotropicWrap, Tex + offsets[6] * texsize, 0);
    float4 s7 = gSceneColor.SampleLevel(gsamAnisotropicWrap, Tex + offsets[7] * texsize, 0);
    float4 s8 = gSceneColor.SampleLevel(gsamAnisotropicWrap, Tex + offsets[8] * texsize, 0);

    // sobel 
    float4 ds0x = s0 * Sobelx[0][0];
    float4 ds1x = s1 * Sobelx[0][1];
    float4 ds2x = s2 * Sobelx[0][2];
    float4 ds3x = s3 * Sobelx[1][0];
    float4 ds4x = s4 * Sobelx[1][1];
    float4 ds5x = s5 * Sobelx[1][2];
    float4 ds6x = s6 * Sobelx[2][0];
    float4 ds7x = s7 * Sobelx[2][1];
    float4 ds8x = s8 * Sobelx[2][2];
    float4 GX = ds0x + ds1x + ds2x + ds3x + ds4x + ds5x + ds6x + ds7x + ds8x;

    float4 ds0y = s0 * Sobely[0][0];
    float4 ds1y = s1 * Sobely[0][1];
    float4 ds2y = s2 * Sobely[0][2];
    float4 ds3y = s3 * Sobely[1][0];
    float4 ds4y = s4 * Sobely[1][1];
    float4 ds5y = s5 * Sobely[1][2];
    float4 ds6y = s6 * Sobely[2][0];
    float4 ds7y = s7 * Sobely[2][1];
    float4 ds8y = s8 * Sobely[2][2];
    float4 GY = ds0y + ds1y + ds2y + ds3y + ds4y + ds5y + ds6y + ds7y + ds8y;
    float4 SobelOut;

    SobelOut = saturate(length(sqrt(GX * GX + GY * GY)));

    // laplacian
    float4 fs0 = s0 * laplacian[0][0];
    float4 fs1 = s1 * laplacian[0][1];
    float4 fs2 = s2 * laplacian[0][2];
    float4 fs3 = s3 * laplacian[1][0];
    float4 fs4 = s4 * laplacian[1][1];
    float4 fs5 = s5 * laplacian[1][2];
    float4 fs6 = s6 * laplacian[2][0];
    float4 fs7 = s7 * laplacian[2][1];
    float4 fs8 = s8 * laplacian[2][2];
    float4 lapValue = fs0 + fs1 + fs2 + fs3 + fs4 + fs5 + fs6 + fs7 + fs8;
    float4 LaplacianOut = saturate(1 - length(lapValue));;


	return LaplacianOut;
    //return float4(0.5f, 0.6f, 1.0f, 1.0f);
}