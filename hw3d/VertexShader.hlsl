struct VSOut
{
	float3 color : Color;
	float4 pos : SV_Position;
};
cbuffer CBuf
{
	row_major matrix World;
	row_major matrix View;
	row_major matrix Projection;
};
VSOut VS(float3 pos : Position, float3 color : Color)
{
	VSOut vso;
	vso.pos = mul(float4(pos, 1.0f), World);
	vso.pos = mul(vso.pos, View);
	vso.pos = mul(vso.pos, Projection);
//vso.pos = float4(pos, 1.0f);
	vso.color = color;
	return vso;
}

