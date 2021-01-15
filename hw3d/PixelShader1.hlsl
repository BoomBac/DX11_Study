struct PointLight
{
	float4 Color;
	float3 Position;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 Norm : NORMAL;
	float3 PosW : POSITION;
	float2 Tex : TEXCOORD;
};

cbuffer CBuf
{
	row_major matrix World;
	row_major matrix WorldInvTranspose;
	row_major matrix View;
	row_major matrix Projection;
	PointLight plight;
};
Texture2D Texture0;
SamplerState SState;




float4 PS(PS_INPUT input) : SV_Target
{
	float4 OutColor = { 0.f, 0.f, 0.f, 0.f };
	input.Norm = normalize(input.Norm);
	float3 lightVec = plight.Position - input.PosW;
	lightVec = normalize(lightVec);
	float instensity = max(dot(lightVec, input.Norm), 0);
	float dis = distance(input.PosW, plight.Position);
	instensity *= max(min((1 - dis / 100.f), 1.f), 0);
	OutColor = Texture0.Sample(SState, input.Tex); //+ plight.Color * instensity;
	OutColor.a = 0.8f;
	return OutColor;
}
