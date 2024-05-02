struct VS_INPUT
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float3 binormal : BINORMAL0;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

//Retrive data is c++ version of the constant buffer
cbuffer constant : register(b0)
{
    row_major float4x4 world;
    row_major float4x4 view;
    row_major float4x4 proj;
};


VS_OUTPUT vsmain(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
		
	//WORLD SPACE
    output.position = mul(input.position, world);
	//VIEW SPACE
    output.position = mul(output.position, view);
	//SCREEN SPACE
    output.position = mul(output.position, proj);
    output.texcoord = input.texcoord;
    
    return output;
}

//Retrive Earth texture
Texture2D EarthColor : register(t0);
sampler EarthColorSampler : register(s0);
//Retrive EarthSpecular texture
Texture2D EarthSpecular : register(t1);
//Since there are multiple ways to sample textures, we need to know how to sazzmple. 
//This varaible hold the information on how to sample the texture.
sampler EarthSpecularSampler : register(s1);
//Retrive Clouds texture
Texture2D Clouds : register(t2);
//Since there are multiple ways to sample textures, we need to know how to sample. 
//This varaible hold the information on how to sample the texture.
sampler CloudsSampler : register(s2);
//Retrive EarthNight texture
Texture2D EarthNight : register(t3);
//Since there are multiple ways to sample textures, we need to know how to sample. 
//This varaible hold the information on how to sample the texture.
sampler EarthNightSampler : register(s3);
//Custon dat type to store the position, texcoord, normal, and direction_to_camera 
struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

//Retrive Earth texture
Texture2D Color : register(t0);
sampler ColorSampler : register(s0);

float4 psmain(PS_INPUT input) : SV_TARGET
{
    return float4(Color.Sample(ColorSampler, input.texcoord));
}