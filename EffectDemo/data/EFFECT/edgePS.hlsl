sampler ZSampler;
sampler texSampler;

float4 PS(in float4 diffuse : COLOR0, in float2 uv : TEXCOORD0) : COLOR0
{
	float2 ScreenResolution;
	ScreenResolution.x = 640;
	ScreenResolution.y = 480;

	float2 pixelOffset = 1 / ScreenResolution;

	float4 n1 = tex2D(ZSampler, uv + float2(-1, -1) * pixelOffset);
	float4 n2 = tex2D(ZSampler, uv + float2(1, 1) * pixelOffset);
	float4 n3 = tex2D(ZSampler, uv + float2(-1, 1) * pixelOffset);
	float4 n4 = tex2D(ZSampler, uv + float2(1, -1) * pixelOffset);

	float4 diagonalDelta = abs(n1 - n2) + abs(n3 - n4);

	// Zç∑ï™ñ@
	//float depthDelta = diagonalDelta.a;
	//depthDelta = saturate((diagonalDelta - 0.1) * 1.0f);
	//float edgeAmount = saturate(depthDelta) * 1.0f;

	// ñ@ê¸ç∑ï™ñ@
	float normalDelta = dot(diagonalDelta.rgb, 1);
	normalDelta = saturate(normalDelta - 0.5)*10.0f;
	float edgeAmount = saturate(normalDelta)* 1.0f;

	return float4((float3)(1 - edgeAmount), 1.0f) * tex2D(texSampler,uv);
}