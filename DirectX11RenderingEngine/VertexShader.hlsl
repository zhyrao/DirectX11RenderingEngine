struct VS_OUTPUT
{
	float3 color : Color;
	float4 pos   : SV_Position;
};

cbuffer Cbuf {
	matrix transform;
};

VS_OUTPUT main(float2 pos : Position, float3 color : Color)
{
	VS_OUTPUT vso;
	vso.pos = mul(float4(pos, 0, 1), transform);
	vso.color = color;
	return vso;
}