float4x4 gWVP;
float4x4 gWorld;
float4 LightDiffuse;
float4 LightAmbient;
float4 MatDiffuse;
float4 MatAmbient;
float3 localLightDir;

void VS(in float3 pos:POSITION0,
	in float3 normal : NORMAL0,
	in float2 UV : TEXCOORD0,
	out float4 outPos : POSITION,
	out float4 outColor : COLOR0,
	out float2 outUV : TEXCOORD0,
	out float4 outDepth : TEXCOORD1)
{
	outPos = mul(float4(pos, 1.0f), gWVP);
	float4 wNormal = mul(float4(normal, 0.0f), gWorld);
	float light = dot(wNormal, -localLightDir);
	light *= 0.5f;
	light += 0.5f;
	outUV.x = light;
	outUV.y = 0.0f;
	outColor = MatDiffuse;
	outColor.a = 1.0f;

	outDepth = wNormal;
	outDepth *= 0.5;
	outDepth += 0.5;
	outDepth.w = outPos.z / outPos.w;
}
