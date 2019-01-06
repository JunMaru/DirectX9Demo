float4x4 gWVP;
float4x4 gOldWVP;
float4x4 gWorld;

void VS(in float3 pos:POSITION0,
	in float3 normal : NORMAL0,
	in float4 col : COLOR0,
	out float4 outCol : COLOR0,
	out float4 outPos : POSITION,
	out float2 outUV : TEXCOORD0,
	out float4 outVelVec : TEXCOORD1)
{
	float4 oldPos = mul(float4(pos, 1.0f), gOldWVP);
	float4 nowPos = mul(float4(pos, 1.0f), gWVP);

	float2 velVec = nowPos.xy / nowPos.w - oldPos.xy / oldPos.w;
	outVelVec.x = velVec.x * 0.5f;
	outVelVec.y = velVec.y * -0.5f;
	
	float4 workNormal;
	workNormal.x = normal.x;
	workNormal.y = normal.y;
	workNormal.z = normal.z;

	workNormal.w = 0.0f;
	workNormal = mul(workNormal, gWVP);

	float fDotAns = dot(workNormal.xy, velVec);

	if (fDotAns <= 0.0f)
	{
		outPos = oldPos;
	}
	else
	{
		outPos = nowPos;
	}

	outUV = nowPos.xy / nowPos.w * 0.5f + 0.5f;
	outUV.y = 1.0f - outUV.y;

	outCol = col;
}
