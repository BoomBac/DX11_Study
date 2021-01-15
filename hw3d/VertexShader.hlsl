
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
struct PointLight
{
	float4 Color;
	float4 Position;
};
cbuffer CBuf
{
	row_major matrix World;
	row_major matrix WorldInvTranspose;
	row_major matrix View;
	row_major matrix Projection;
	PointLight plight;
	row_major matrix scaling;
};

VSOut VS(VS_INPUT input)
{
	VSOut vso;
	vso.PosW = mul(float4(input.Pos, 1.0f), World);
	vso.Pos = mul(float4(input.Pos,1.0f), World);
	vso.Pos = mul(vso.Pos, View);
	vso.Pos = mul(vso.Pos, Projection);
	vso.Norm = mul(float4(input.Norm, 1.f), World).xyz;
	vso.Tex = input.Tex; //mul(scaling, float4(input.Tex, 1.0f, 1.0f));
	return vso;
}

