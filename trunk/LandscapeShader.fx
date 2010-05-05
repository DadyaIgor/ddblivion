//--------------------------------------------------------------------------------------
// File: Tutorial05.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
matrix View;
matrix Projection;

Texture2D datext0;
Texture2D datext1;
Texture2D datext2;
Texture2D datext3;
Texture2D datext4;
Texture2D datext5;
Texture2D datext6;
Texture2D datext7;
Texture2D datext8;

SamplerState samLinear
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
//    MaxLOD = 0;
//    MinLOD = 0;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Normal : NORMAL;
    float4 Color : COLOR0;
    float2 Tex : TEXCOORD0;
    float4 Blend : TEXCOORD1;
    float4 Blend1 : TEXCOORD2;
    float Blend2 : TEXCOORD3;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Normal : NORMAL;
    float4 Color : COLOR0;
    float2 Tex : TEXCOORD0;
    float4 Blend : TEXCOORD1;
    float4 Blend1 : TEXCOORD2;
    float Blend2 : TEXCOORD3;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Normal = input.Normal;
    output.Color = input.Color;
    output.Tex = input.Tex;
    output.Blend = input.Blend;
    output.Blend1 = input.Blend1;
    output.Blend2 = input.Blend2;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    float4 result;
    result=(datext0.Sample(samLinear,input.Tex)*input.Blend.x);
    result+=(datext1.Sample(samLinear,input.Tex)*input.Blend.y);
    result+=(datext2.Sample(samLinear,input.Tex)*input.Blend.z);
    result+=(datext3.Sample(samLinear,input.Tex)*input.Blend.w);
    result+=(datext4.Sample(samLinear,input.Tex)*input.Blend1.x);
    result+=(datext5.Sample(samLinear,input.Tex)*input.Blend1.y);
    result+=(datext6.Sample(samLinear,input.Tex)*input.Blend1.z);
    result+=(datext7.Sample(samLinear,input.Tex)*input.Blend1.w);
    result+=(datext8.Sample(samLinear,input.Tex)*input.Blend2);
    result*=input.Color;
    return result;
}


//--------------------------------------------------------------------------------------
technique10 RenderLand
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}


