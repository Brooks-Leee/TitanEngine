
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
	float gTime;
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
	//vout.Normal = mul(vin.Normal, gWorldViewProj);
	// Transform to homogeneous clip space.

	float3 Pos = vin.PosL;
	Pos.z += sin(gTime%10)* 1000;

	vout.PosH = mul(float4(Pos, 1.0f), gWorldViewProj);

	// Just pass vertex color into the pixel shader.
   // vout.Color = (vout.Normal*0.5f+0.5f);
    vout.Color = (vin.Normal*0.5f+0.5f);
    //vout.Color = vin.Color;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return pin.Color;
}