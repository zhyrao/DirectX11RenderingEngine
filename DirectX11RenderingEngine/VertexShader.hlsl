struct VS_OUTPUT
{
	float3 color : Color;
	float4 pos   : SV_Position;
};

VS_OUTPUT main(float2 pos : Position, float3 color : Color)
{
	VS_OUTPUT vso;
	vso.pos = float4(pos, 0, 1);
	vso.color = color;
	return vso;
}