struct VS_INPUT
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float3 binormal : BINORMAL0;
};

struct VPS_INOUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 worldPosition : TEXCOORD1;
};

struct LightData
{
    float4 color;
    float4 direction;
};

cbuffer constant : register(b0)
{
    row_major float4x4 world;
    row_major float4x4 view;
    row_major float4x4 proj;
    float4 cameraPosition;
    LightData light;
};



VPS_INOUT vsmain(VS_INPUT input)
{
    VPS_INOUT output = (VPS_INOUT)0;

	//WORLD SPACE
    output.position = mul(input.position, world);
    output.worldPosition = output.position.xyz;

    output.position = mul(output.position, view);
    output.position = mul(output.position, proj);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) world));
    return output;
}

Texture2D Color : register(t0);
sampler ColorSampler : register(s0);

float4 psmain(VPS_INOUT input) : SV_TARGET
{
    float4 color = Color.Sample(ColorSampler, input.texcoord);
    float3 light_direction = normalize(light.direction.xyz);
    float3 directionToCamera = normalize(input.worldPosition - cameraPosition.xyz);

	//AMBIENT LIGHT
    float ka = 8.5;
    float3 ia = float3(0.09, 0.082, 0.082);
    ia *= (color.rgb);

    float3 ambient_light = ka * ia;

	//DIFFUSE LIGHT
    float kd = 0.7;
    float amount_diffuse_light = max(dot(light_direction.xyz, input.normal), 0.0);
    float3 id = light.color.rgb;
    id *= (color.rgb);
    float3 diffuse_light = kd * id * amount_diffuse_light;

	//SPECULAR LIGHT
    float ks = 1.0;
    float3 is = float3(1.0, 1.0, 1.0);
    float3 reflected_light = reflect(light_direction.xyz, input.normal);
    float shininess = 30.0;
    float amount_specular_light = pow(max(0.0, dot(reflected_light, directionToCamera)), shininess);

    float3 specular_light = ks * amount_specular_light * is;

    float3 final_light = ambient_light + diffuse_light + specular_light;

    return float4(final_light, 1.0);
}