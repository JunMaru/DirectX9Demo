sampler texSampler;

float4 PSWIDTH(in float4 diffuse : COLOR0, in float2 uv : TEXCOORD0) : COLOR0
{
	float4 outColor =  tex2D(texSampler, float2(uv.x - 0.008f, uv.y)) * 0.1f +
						tex2D(texSampler, float2(uv.x - 0.006f, uv.y)) * 0.1f +
						tex2D(texSampler, float2(uv.x - 0.004f, uv.y)) * 0.1f +
						tex2D(texSampler, float2(uv.x - 0.002f, uv.y)) * 0.1f +
						tex2D(texSampler, float2(uv.x, uv.y)) * 0.2f +
						tex2D(texSampler, float2(uv.x + 0.002f, uv.y)) * 0.1f +
						tex2D(texSampler, float2(uv.x + 0.004f, uv.y)) * 0.1f +
						tex2D(texSampler, float2(uv.x + 0.006f, uv.y)) * 0.1f +
						tex2D(texSampler, float2(uv.x + 0.008f, uv.y)) * 0.1f; 

	outColor *= diffuse;

	return outColor;
}

float4 PSHEIGHT(in float4 diffuse : COLOR0, in float2 uv : TEXCOORD0) : COLOR0
{
	float4 outColor =  tex2D(texSampler, float2(uv.x, uv.y - 0.008f)) * 0.1f +
						tex2D(texSampler, float2(uv.x, uv.y - 0.006f)) * 0.1f +
						tex2D(texSampler, float2(uv.x, uv.y - 0.004f)) * 0.1f +
						tex2D(texSampler, float2(uv.x, uv.y - 0.002f)) * 0.1f +
						tex2D(texSampler, float2(uv.x, uv.y)) * 0.2f +
						tex2D(texSampler, float2(uv.x, uv.y + 0.002f)) * 0.1f +
						tex2D(texSampler, float2(uv.x, uv.y + 0.004f)) * 0.1f +
						tex2D(texSampler, float2(uv.x, uv.y + 0.006f)) * 0.1f +
						tex2D(texSampler, float2(uv.x, uv.y + 0.008f)) * 0.1f;

	outColor *= diffuse;

	return outColor;
}