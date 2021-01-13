struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD;
};
struct VSOut
{
	float4 Pos : SV_POSITION;
	float3 Norm : NORMAL;
	float3 PosW : POSITION;
	float2 Tex : TEXCOORD;
};

cbuffer CBuf : register(b0)
{
	row_major matrix World;
	row_major matrix WorldInvTranspose;
	row_major matrix View;
	row_major matrix Projection;
};

VSOut VS( VS_INPUT input ) 
{
	VSOut vso;
	vso.PosW = mul(float4(input.Pos, 1.0f), World);
	vso.Pos = mul(float4(input.Pos, 1.0f), World);
	vso.Pos = mul(vso.Pos, View);
	vso.Pos = mul(vso.Pos, Projection);
	vso.Norm = mul(float4(input.Norm, 1.f), World).xyz;
	return vso;
}