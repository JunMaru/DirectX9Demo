float4x4 gWVP;

void VS(in float3 pos:POSITION0,
	in float3 normal : NORMAL0,
	in float2 UV : TEXCOORD0,
	out float4 outPos : POSITION,
	out float4 outColor : COLOR0,
	out float2 outUV : TEXCOORD0,
	out float4 outDepth : TEXCOORD1)
{
	outPos = mul(float4(pos, 1.0f), gWVP);
	outColor = float4(0.0f,0.0f,0.0f,1.0f);
	outColor.a = 1.0f;
}
