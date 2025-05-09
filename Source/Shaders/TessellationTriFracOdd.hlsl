#include "CommonRoot.hlsl"
#include "BarycentricInterpolate.hlsl"


static const float MaxTessFactor = 12.0f;



struct VertexIn
{
	float3 PosL    : POSITION;
};

struct VertexOut
{
	float3 PosL    : POSITION;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosL = vin.PosL;

	return vout;
}
 
struct PatchTess
{
	float EdgeTess[3]   : SV_TessFactor;
	float InsideTess[1] : SV_InsideTessFactor;
};

PatchTess ConstantHS(InputPatch<VertexOut, 3> patch, uint patchID : SV_PrimitiveID)
{
	PatchTess pt;
	
	float3 centerL = 0.3333333f*(patch[0].PosL + patch[1].PosL + patch[2].PosL);
	float3 centerW = mul(float4(centerL, 1.0f), gWorld).xyz;
	
	float d = distance(centerW, gEyePosW);

	// Tessellate the patch based on distance from the eye such that
	// the tessellation is 0 if d >= d1 and 64 if d <= d0.  The interval
	// [d0, d1] defines the range we tessellate in.
	
	const float d0 = 5.0f;
	const float d1 = 300.0f;
    float tess = MaxTessFactor * saturate((d1 - d) / (d1 - d0));

	// Uniformly tessellate the patch.

	pt.EdgeTess[0] = tess;
	pt.EdgeTess[1] = tess;
	pt.EdgeTess[2] = tess;
	
	pt.InsideTess[0] = tess;
	
	return pt;
}

struct HullOut
{
	float3 PosL : POSITION;
    float4 Color : COLOR;
};

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(MaxTessFactor)]
HullOut HS(InputPatch<VertexOut, 3> p, 
           uint i : SV_OutputControlPointID,
           uint patchId : SV_PrimitiveID)
{
	HullOut hout;
	
	hout.PosL = p[i].PosL;
	
    int ColorIndex = patchId % 32;
	
    hout.Color = gBaseColors[ColorIndex];
	
	return hout;
}

struct DomainOut
{
	float4 PosH : SV_POSITION;
    float4 Color : COLOR;
};

// The domain shader is called for every vertex created by the tessellator.  
// It is like the vertex shader after tessellation.
[domain("tri")]
DomainOut DS(PatchTess patchTess, float3 barycentricCoords : SV_DomainLocation, const OutputPatch<HullOut, 3> patch)
{
	DomainOut dout;
	
    float3 p = BarycentricInterpolate(patch[0].PosL, patch[1].PosL, patch[2].PosL, barycentricCoords);
    p = normalize(p);
	
	float4 posW = mul(float4(p, 1.0f), gWorld);
	dout.PosH = mul(posW, gViewProj);
	
    dout.Color = BarycentricInterpolate(patch[0].Color, patch[1].Color, patch[2].Color, barycentricCoords);
	
	return dout;
}

float4 PS(DomainOut pin) : SV_Target
{
    return pin.Color;
}
