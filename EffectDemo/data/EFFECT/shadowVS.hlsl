float4x4 gWVP;
float4x4 gWorld;
float4 LightDiffuse;
float4 LightAmbient;
float4 MatDiffuse;
float4 MatAmbient;
float3 localLightDir;
float3 playerPos;

void VS(in float3 pos:POSITION0,
	in float3 normal : NORMAL0,
	in float2 uv : TEXCOORD0,
	out float4 outPos : POSITION,
	out float4 outColor : COLOR0,
	out float2 outUV : TEXCOORD0,
	out float2 outShadowUV : TEXCOORD1)
{
	outPos = mul(float4(pos, 1.0f), gWVP);
	float light = max(dot(normal, -localLightDir), 0);
	outColor = float4(1.0f, 1.0f, 1.0f, 1.0f);//matDiffuse*LightDiffuse*light;// +MatAmbient*LightAmbient;
	outColor.a = 1.0f;

	outShadowUV.x = -(pos.x - playerPos.x) * 0.005f + 0.5f;
	outShadowUV.y = (pos.z - playerPos.z) * 0.005f + 0.5f;

	outUV = uv;
}