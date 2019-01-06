sampler texSampler;
sampler normalSampler;

float3 worldCameraPos;
float3 worldLightVec;
float4 g_lightColor;
float4x4 w;

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 normalW, float3 tangentW);

float4 PS(float4 diffuse : COLOR0,float2 uv : TEXCOORD0,float3 posW : TEXCOORD1,
		  float3 tangentW : TEXCOORD2,float3 normalW : TEXCOORD3) : COLOR0
{
	float3 worldLightPos[3] = {{0.5f,0.0f,-0.5f},{-0.5f,0.0f,-0.5f},{0.0f,1.0f,-0.5f}};
	float3 lightColorArray[3] = { {1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f }};
	float3 specColor = float3(1.0f, 1.0f, 1.0f);
	float specPower = 300.0f;

	float3 normal = normalize(normalW);
	float3 toEye = worldCameraPos - posW;
	float distToEye = length(toEye);
	toEye /= distToEye;
	float3 texColor = tex2D(texSampler,uv).rgb;
	float3 bumpNormalW = NormalSampleToWorldSpace(tex2D(normalSampler,uv).xyz, normal,tangentW);

	float3 outColor = float3(0.0f,0.0f,0.0f);
	for (int i = 0; i < 3;i++)
	{
		float3 lightDir = posW - worldLightPos[i];
		lightDir = normalize(lightDir);
		float distanceNum = distance(worldLightPos[i], posW);
		float attn = 0.01f + 0.01f * distanceNum + 0.05f * distanceNum * distanceNum;
		float3 dir = normalize(posW - worldLightPos[i]);
		float3 D = dot(dir, bumpNormalW) * 0.5f + 0.5f;
		D *= D;
		float3 ref = reflect(-lightDir,bumpNormalW);
		float spec = pow(max(dot(ref, toEye), 0.0f), specPower);
		float3 S = spec * specColor;

		outColor += (texColor * D * lightColorArray[i]) + S / attn;
	}

	return float4(outColor, 1.0f);
}

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 normalW, float3 tangentW)
{
	float3 normalT = normalMapSample * 2.0f - 1.0f;
	float3 N = normalW;
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);
	return mul(normalT, TBN);
}
