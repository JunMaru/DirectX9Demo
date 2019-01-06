
sampler texSampler;
sampler shadowSampler;

float4 PS(float4 diffuse : COLOR0, float2 uv : TEXCOORD0, float2 shadowUV : TEXCOORD1) : COLOR0
{
	return diffuse * tex2D(texSampler,uv) * tex2D(shadowSampler,shadowUV);
}