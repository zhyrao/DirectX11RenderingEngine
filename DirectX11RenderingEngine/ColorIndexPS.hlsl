cbuffer CBuf
{
	float4 face_color[8];
};

float4 main(uint tid: SV_PrimitiveID) : SV_Target
{
	return float4(face_color[(tid / 2) % 8]);
}