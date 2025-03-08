#include "CommonRoot.hlsl"
 
struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
    float3 PosL : POSITION;
    float3 PosW : POSITIONW;
    float3 PosH : POSITIONH;
    float3 NormalL : NORMAL;
    float3 NormalW : NORMALW;
    float2 TexC : TEXCOORD;
};

struct GeoOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
    uint PrimID : SV_PrimitiveID;
};


	
void Subdivide(VertexOut inVerts[3], out VertexOut outVerts[6])
{
    VertexOut m[3];
    
	//	Compute	edge	midpoints.
    m[0].PosL = 0.5f * (inVerts[0].PosL + inVerts[1].PosL);
    m[1].PosL = 0.5f * (inVerts[1].PosL + inVerts[2].PosL);
    m[2].PosL = 0.5f * (inVerts[2].PosL + inVerts[0].PosL);
	
	//	Project	onto	unit	sphere
    m[0].PosL = normalize(m[0].PosL);
    m[1].PosL = normalize(m[1].PosL);
    m[2].PosL = normalize(m[2].PosL);
	
	//	Derive	normals.
    m[0].NormalL = m[0].PosL;
    m[1].NormalL = m[1].PosL;
    m[2].NormalL = m[2].PosL;
	
	//	Interpolate	texture	coordinates.
    m[0].TexC = 0.5f * (inVerts[0].TexC + inVerts[1].TexC);
    m[1].TexC = 0.5f * (inVerts[1].TexC + inVerts[2].TexC);
    m[2].TexC = 0.5f * (inVerts[2].TexC + inVerts[0].TexC);
	
    outVerts[0] = inVerts[0];
    outVerts[1] = m[0];
    outVerts[2] = m[2];
    outVerts[3] = m[1];
    outVerts[4] = inVerts[2];
    outVerts[5] = inVerts[1];
};

void OutputSubdivision(VertexOut v[6], inout TriangleStream<GeoOut> triStream, uint primID)
{
    GeoOut gout[6];
	
	[unroll]
    for (int i = 0; i < 6; ++i)
    {
        gout[i].PosW = mul(float4(v[i].PosL, 1.0f), gWorld).xyz;
        gout[i].NormalW = mul(v[i].NormalL, (float3x3)gWorldInvTranspose);
        float4x4 WorldWorldViewProj = mul(gWorld, gViewProj);
        gout[i].PosH = mul(float4(v[i].PosL, 1.0f), WorldWorldViewProj);
        gout[i].TexC = v[i].TexC;
        gout[i].PrimID = primID + i * 3;
    }
			
	[unroll]
    for (int j = 0; j < 5; ++j)
    {
        triStream.Append(gout[j]);
    }
    triStream.RestartStrip();
			
    triStream.Append(gout[1]);
    triStream.Append(gout[5]);
    triStream.Append(gout[3]);
}

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.PosL = vin.PosL;
    vout.NormalL = vin.NormalL;
    vout.TexC = vin.TexC;
    vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.NormalW = mul(float4(vin.NormalL, 1.0f), gWorld);
    vout.PosH = mul(float4(vout.PosW, 1.0f), gViewProj);

    return vout;
}
 
[maxvertexcount(8)]
void GS(triangle VertexOut gin[3], uint primID : SV_PrimitiveID, inout TriangleStream<GeoOut> triStream)
{
    VertexOut v[6];
    Subdivide(gin, v);
    OutputSubdivision(v, triStream, primID);
}

float4 PS(GeoOut pin) : SV_Target
{
    int ColorIndex = pin.PrimID % 32;
    return gBaseColors[ColorIndex];
}