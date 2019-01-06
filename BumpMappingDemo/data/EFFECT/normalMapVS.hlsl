float4x4 wvp;
float4x4 w;
float4x4 wit;
float3 gLightDir;
float4 gLightColor;

void VS(in float3 inPos : POSITION0,
		in float3 inNormal : NORMAL0,
		in float4 inDiffuse : COLOR0,
		in float2 inUV : TEXCOORD0,
		in float3 inTangent : TANGENT0,
		out float4 outPos : POSITION,
		out float4 outDiffuse : COLOR0,
		out float2 outUV : TEXCOORD0,
		out float3 outPosW : TEXCOORD1,
		out float3 outTangentW : TEXCOORD2,
		out float3 outNormalW : TEXCOORD3)
{
	outPos = mul(float4(inPos.xyz,1.0f), wvp);
	outPosW = mul(float4(inPos.xyz, 1.0f), w);
	outDiffuse = inDiffuse;
	outUV = inUV;
	outTangentW = normalize(mul(inTangent, (float3x3)w));
	outNormalW = normalize(mul(float4(inNormal, 0.0f),wit));
}