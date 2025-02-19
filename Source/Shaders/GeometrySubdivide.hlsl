// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
#include "LightingUtil.hlsl"

Texture2DArray gTreeMapArray : register(t0);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gTexTransform;
};

// Constant data that varies per material.
cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
    float4 gAmbientLight;

    float4 gFogColor;
    float gFogStart;
    float gFogRange;
    float2 cbPerObjectPad2;

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light gLights[MaxLights];
};

cbuffer cbMaterial : register(b2)
{
    float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float gRoughness;
    float4x4 gMatTransform;
};
 
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
 
[maxvertexcount(9)]
void GS(triangle VertexOut gin[3],
        uint primID : SV_PrimitiveID,
        inout TriangleStream<GeoOut> output)
{
    float3 CenterL = (gin[0].PosL.xyz + gin[1].PosL.xyz + gin[2].PosL.xyz) / 3.0f;
 
    float3 CenterW = mul(float4(CenterL, 1.0f), gWorld).xyz;
    float3 CenterNormalL = (gin[0].NormalL.xyz + gin[1].NormalL.xyz + gin[2].NormalL.xyz) / 3.0f;
    
    float3 centerNormalW = mul(float4(CenterNormalL, 0.0f), gWorld).xyz;
    float4 CenterH = mul(float4(CenterW, 1.0f), gViewProj);
    float2 CenteTexC = (gin[0].TexC.xy + gin[1].TexC.xy + gin[2].TexC.xy) / 3.0f;

    GeoOut gout0;
    gout0.NormalW = gin[0].NormalW;
    gout0.PosH = float4(gin[0].PosH, 1.0f);
    gout0.PosW    = gin[0].PosW;
    gout0.TexC    = gin[0].TexC;
    gout0.PrimID  = primID;
    
    GeoOut gout1;
    gout1.NormalW = gin[1].NormalW;
    gout1.PosH = float4(gin[1].PosH, 1.0f);
    gout1.PosW    = gin[1].PosW;
    gout1.TexC    = gin[1].TexC;
    gout1.PrimID  = primID;
    
    GeoOut gout2;
    gout2.NormalW = gin[2].NormalW;
    gout2.PosH = float4(gin[2].PosH, 1.0f);
    gout2.PosW    = gin[2].PosW;
    gout2.TexC    = gin[2].TexC;
    gout2.PrimID  = primID;
    
    GeoOut goutCenter;
    goutCenter.NormalW = centerNormalW;
    goutCenter.PosH = CenterH;
    goutCenter.PosW = CenterW;
    goutCenter.TexC = CenteTexC;
    goutCenter.PrimID = primID;
    
    output.Append(gout0);
    output.Append(gout1);
    output.Append(goutCenter);
    output.RestartStrip();

    output.Append(gout1);
    output.Append(gout2);
    output.Append(goutCenter);
    output.RestartStrip();

    output.Append(gout2);
    output.Append(gout0);
    output.Append(goutCenter);
    output.RestartStrip();
}

float4 PS(GeoOut pin) : SV_Target
{
    float3 uvw = float3(pin.TexC, pin.PrimID % 3);
    float4 diffuseAlbedo = gTreeMapArray.Sample(gsamAnisotropicWrap, uvw) * gDiffuseAlbedo;
	
#ifdef ALPHA_TEST
	// Discard pixel if texture alpha < 0.1.  We do this test as soon 
	// as possible in the shader so that we can potentially exit the
	// shader early, thereby skipping the rest of the shader code.
	clip(diffuseAlbedo.a - 0.1f);
#endif

    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = gEyePosW - pin.PosW;
    float distToEye = length(toEyeW);
    toEyeW /= distToEye; // normalize

    // Light terms.
    float4 ambient = gAmbientLight * diffuseAlbedo;

    const float shininess = 1.0f - gRoughness;
    Material mat = { diffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

#ifdef FOG
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
	litColor = lerp(litColor, gFogColor, fogAmount);
#endif

    // Common convention to take alpha from diffuse albedo.
    litColor.a = diffuseAlbedo.a;

    return litColor;
}