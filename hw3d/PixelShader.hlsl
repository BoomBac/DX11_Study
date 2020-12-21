float4 PS(float3 color : Color) : SV_Target
{
    return float4(color,1.0f);    // Yellow, with Alpha = 1
}