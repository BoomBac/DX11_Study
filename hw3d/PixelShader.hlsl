//struct PointLight
//{
//	float4 Ambient;
//	float4 Diffuse;
//	float4 Specular;
//	float4 Position;
//	float Range;
//	float4 Att;
//};
//struct Material
//{

//	float4 Ambient;
//	float4 Diffuse;
//	float4 Specular; 
//	float4 Reflect;
//};

struct PointLight
{
	float4 Color;
	float4 Position;
};

	struct PS_INPUT
	{
		float4 Pos : SV_POSITION;
		float3 Norm : TEXCOORD0;
		float3 PosW : POSITION;
};

cbuffer CBuf
{
	row_major matrix World;
	row_major matrix View;
	row_major matrix Projection;
	PointLight plight;
};


float4 PS(PS_INPUT input) : SV_Target
{
	float4 OutColor = { 1.f, 1.f, 1.f, 1.f };
	//float dis = distance(float4(input.PosW, 1.0f), plight.Position);

	float dis = distance(float4(input.PosW, 1.0f), plight.Position);
	if (dis < 70.f)
	{
		OutColor = plight.Color;
		return OutColor;
	}
	return OutColor;
}
