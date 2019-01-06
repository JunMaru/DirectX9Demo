sampler texSampler;

void PS(in float4 diffuse : COLOR0, in float2 uv : TEXCOORD0, in float4 inDepth : TEXCOORD1,
	out float4 color : COLOR0 ,out float4 color2 : COLOR1)
{
	color = tex2D(texSampler, uv) * diffuse;
	color2 = inDepth;
}