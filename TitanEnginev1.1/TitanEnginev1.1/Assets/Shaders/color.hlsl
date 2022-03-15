
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
	float4x4 gLocation;
	float4x4 gRotation;
	float4x4 gScale;
	float1 gTime;
};

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
	float4 Normal: NORMAL;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
	float4 Normal: NORMAL;
};



VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	//float4 Normal = mul(vin.Normal, gRotation);
	float3 Pos = vin.PosL;

	vout.PosH = mul(float4(Pos, 1.0f), gWorldViewProj);

	vout.Normal = mul(gRotation, vin.Normal);
	//vout.Normal = vin.Normal;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{

	pin.Color = pow((pin.Normal * 0.5f + 0.5f), 1/2.2f);
	//pin.Color = (pin.Normal * 0.5f + 0.5f);

    return pin.Color;
}