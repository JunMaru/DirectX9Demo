sampler blurSampler;

float4 PS(in float4 diffuse : COLOR0, in float2 uv : TEXCOORD0, in float4 inVelVec : TEXCOORD1) : COLOR0
{
	float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 10; i++)
	{
		outColor += tex2D(blurSampler, uv + inVelVec * i * -0.2f) * 0.1f;
	}

	return outColor;
}