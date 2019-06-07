cbuffer PerObject : register(b0)
{
	matrix WorldMatrix;
	matrix InverseTransposeWorldMatrix;
	matrix WorldViewProjectionMatrix;
	matrix proj;
	matrix view;
}

struct AppData
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal   : NORMAL;
};

struct VertexShaderOutput
{
	float4 PositionWS   : TEXCOORD1;
	float3 NormalWS     : TEXCOORD2;
	float2 TexCoord     : TEXCOORD0;
	float4 Position     : SV_Position;
};

VertexShaderOutput main(AppData IN)
{
	VertexShaderOutput OUT, OUT1;
	matrix wvp = mul(mul(WorldMatrix, view), proj);
	
	matrix pvw = mul(mul(proj, view), WorldMatrix);
	//OUT.Position = mul(WorldMatrix, float4(IN.Position, 1.0f));
	//OUT.Position = mul(view, OUT.Position);
	//OUT.Position = mul(proj, OUT.Position);

	//OUT.Position = mul(pvw, float4(IN.Position, 1.0f));


	OUT.Position = mul(WorldViewProjectionMatrix, float4(IN.Position, 1.0f));
	//OUT.Position = mul(float4(IN.Position, 1.0f), WorldViewProjectionMatrix);
	//OUT.Position = mul(pvw, float4(IN.Position, 1.0f));
	OUT.PositionWS = mul(WorldMatrix, float4(IN.Position, 1.0f));
	OUT.NormalWS = mul((float3x3)WorldMatrix, IN.Normal); //mul((float3x3)InverseTransposeWorldMatrix, IN.Normal);
	OUT.TexCoord = IN.TexCoord;

	return OUT;
}