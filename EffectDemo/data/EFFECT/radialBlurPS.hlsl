sampler radialBlurSampler;

float2 center;
float blurPow;
float TU;
float TV;

float4 PS(in float3 pos:POSITION0, in float2 uv : TEXCOORD0) : COLOR0
{
	float4 color[10];
	float2 dir = center - uv;
	float len = length(dir);

	dir = normalize(dir) * float2(TU, TV);
	dir *= blurPow * len;

	color[0] = tex2D(radialBlurSampler, uv) * 0.19f;
	color[1] = tex2D(radialBlurSampler, uv + dir) * 0.17f;
	color[2] = tex2D(radialBlurSampler, uv + dir * 2.0f) * 0.15f;
	color[3] = tex2D(radialBlurSampler, uv + dir * 3.0f) * 0.13f;
	color[4] = tex2D(radialBlurSampler, uv + dir * 4.0f) * 0.11f;
	color[5] = tex2D(radialBlurSampler, uv + dir * 5.0f) * 0.09f;
	color[6] = tex2D(radialBlurSampler, uv + dir * 6.0f) * 0.07f;
	color[7] = tex2D(radialBlurSampler, uv + dir * 7.0f) * 0.05f;
	color[8] = tex2D(radialBlurSampler, uv + dir * 8.0f) * 0.03f;
	color[9] = tex2D(radialBlurSampler, uv + dir * 9.0f) * 0.01f;

	return (color[0] + color[1] + color[2] + color[3] + color[4] +
			color[5] + color[6] + color[7] + color[8] + color[9]);


}