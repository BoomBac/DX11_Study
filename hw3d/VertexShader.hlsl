
struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Norm : NORMAL;
};


struct VSOut
{
	float4 Pos : SV_POSITION;
	float3 Norm : TEXCOORD0;
	float3 PosW : POSITION;
};
cbuffer CBuf
{
	row_major matrix World;
	row_major matrix WorldInvTranspose;
	row_major matrix View;
	row_major matrix Projection;
};

VSOut VS(VS_INPUT input)
{
	VSOut vso;
	vso.PosW = mul(float4(input.Pos, 1.0f), World);
	vso.Pos = mul(float4(input.Pos,1.0f), World);
	vso.Pos = mul(vso.Pos, View);
	vso.Pos = mul(vso.Pos, Projection);
	vso.Norm = mul(float4(input.Norm, 1.f), World).xyz;
	return vso;
}

