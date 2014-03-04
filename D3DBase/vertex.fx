
struct Light
{
	float3 dir;
	float4 ambient;
	float4 diffuse;
};

cbuffer cbPerFrame
{
	Light light;
};

cbuffer cbPerObject
{
	float4x4 WVP;
};

Texture2D DiffuseMap;
TextureCube SkyMap;

SamplerState TriLinearSample
{
	Filter = MIN_MAG_MIP_LINEAR;
};

struct VS_OUTPUT	//output structure for vertex shader
{
    float4 Pos : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct SKYMAP_VS_OUTPUT	//output structure for skymap vertex shader
{
    float4 Pos : SV_POSITION;
	float3 texCoord : TEXCOORD;
};

// Vertex Shader
VS_OUTPUT VS(float3 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(float4(inPos, 1.0f), WVP);

	output.normal = mul(normal, WVP);

	output.texCoord = inTexCoord;

    return output;		//send color and position to pixel shader
}

SKYMAP_VS_OUTPUT SKYMAP_VS(float3 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
	SKYMAP_VS_OUTPUT output = (SKYMAP_VS_OUTPUT)0;

	output.Pos = mul(float4(inPos, 1.0f), WVP).xyww;

	output.texCoord = inPos;

    return output;		//send color and position to pixel shader
}

// Pixel Shader
float4 PS(VS_OUTPUT input) : SV_Target
{
	input.normal = normalize(input.normal);

	float4 diffuse = DiffuseMap.Sample( TriLinearSample, input.texCoord );

	float3 finalColor;

	finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, input.normal) * light.diffuse * diffuse);

	return float4(finalColor, diffuse.a);
}

float4 SKYMAP_PS(SKYMAP_VS_OUTPUT input) : SV_Target
{
	return SkyMap.Sample(TriLinearSample, input.texCoord);
}

RasterizerState NoCulling
{
	CullMode = None;
};

DepthStencilState LessEqualDSS
{
	DepthFunc = LESS_EQUAL;
};

technique10 Tech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

technique10 SkyMapTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, SKYMAP_VS() ) );
        SetPixelShader( CompileShader( ps_4_0, SKYMAP_PS() ) );

		SetRasterizerState(NoCulling);
		SetDepthStencilState(LessEqualDSS, 0);
    }
}