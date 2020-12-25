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
	float3 Position;
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
	row_major matrix WorldInvTranspose;
	row_major matrix View;
	row_major matrix Projection;
	PointLight plight;
};


float4 PS(PS_INPUT input) : SV_Target
{
	float4 OutColor = { 0.f, 0.f, 0.f, 0.f };
	input.Norm = normalize(input.Norm);
	float3 lightVec = plight.Position - input.PosW;
	lightVec = normalize(lightVec);
	float instensity =max(dot(lightVec, input.Norm), 0);
	float dis = distance(input.PosW, plight.Position);
	instensity *= max(min((1 - dis / 70.f), 1.f), 0);
	OutColor = plight.Color * instensity;
	return OutColor;
}
