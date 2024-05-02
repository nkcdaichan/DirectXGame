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
struct TerrainData
{
    float4 size;
    float heightMapSize;
};

cbuffer constant : register(b0)
{
    row_major float4x4 world;
    row_major float4x4 view;
    row_major float4x4 proj;
    float4 cameraPosition;
    LightData light;
    TerrainData terrain;
};

Texture2D HeightMap : register(t0);
sampler HeightMapSampler : register(s0);

Texture2D GroundMap : register(t1);
sampler GroundMapSampler : register(s1);

Texture2D CliffMap : register(t2);
sampler CliffMapSampler : register(s2);


VPS_INOUT vsmain(VS_INPUT input)
{
    VPS_INOUT output = (VPS_INOUT) 0;
    float texelSize = 1.0 / terrain.heightMapSize; //Calculating the size of a texel
    
    
    float height = HeightMap.SampleLevel(HeightMapSampler, input.texcoord, 0).r;
    output.position = mul(float4(
        input.position.x * terrain.size.x, //Scaling terrain by modify the position of each the vertex
        height * terrain.size.y, //Scaling terrain by modify the position of each the vertex// This one brings in the height from the height map on to the tarrain
        input.position.z * terrain.size.z, //Scaling terrain by modify the position of each the vertex
        1), 
        world);//Mult to get the vertex is world pos
    output.worldPosition = output.position.xyz;

    output.position = mul(output.position, view);//transform vertex into view space
    output.position = mul(output.position, proj); //transform vertex into screen proj
    output.texcoord = input.texcoord;
    
    // fetch the height value at specific texture coordinates relative to input.texcoord.
    float t = HeightMap.SampleLevel(HeightMapSampler, float2(input.texcoord.x, input.texcoord.y - texelSize), 0).r;
    float b = HeightMap.SampleLevel(HeightMapSampler, float2(input.texcoord.x, input.texcoord.y + texelSize), 0).r;
    float l = HeightMap.SampleLevel(HeightMapSampler, float2(input.texcoord.x - texelSize, input.texcoord.y), 0).r;
    float r = HeightMap.SampleLevel(HeightMapSampler, float2(input.texcoord.x + texelSize, input.texcoord.y), 0).r;
    //Get neihboring 
    t *= terrain.size.y;//top neihboring vertex
    b *= terrain.size.y; //bottom neihboring vertex
    l *= terrain.size.y; //left neihboring vertex
    r *= terrain.size.y; //right neihboring vertex
    
    float3 normal;
    normal.x = (r - l) * 0.5; // Scales the difference bettween the bottom and top. Calculate the change in height in the x-direction
    normal.y = 1.0; // Fixed 
    normal.z = (b - t) * 0.5; // Calculate the change in height in the y-direction

    output.normal = normalize(mul(normal, (float3x3) world));
    
    return output;
}


float4 psmain(VPS_INOUT input) : SV_TARGET
{
    float4 ground = GroundMap.Sample(GroundMapSampler, input.texcoord * 100.0f);
    float4 cliff = CliffMap.Sample(CliffMapSampler, input.texcoord * 60.0f);
    
    float4 color = ground;
    float angle = abs(dot(input.normal, float3(0, 1, 0)));
    
    float minAngle = 0.5;
    float maxAngle = 1;
    if (angle >= minAngle && angle <= maxAngle)
        color = lerp(cliff, color, (angle - minAngle) * (1.0 / (maxAngle - minAngle)));
    
    if (angle < minAngle)
        color = cliff;
    
    
    float3 light_direction = normalize(light.direction.xyz);
    float3 directionToCamera = normalize(input.worldPosition - cameraPosition.xyz);

	//AMBIENT LIGHT
    float ka = 4;
    float3 ia = float3(0.09, 0.09, 0.09);
    ia *= (color.rgb);
    float3 ambient_light = ka * ia;

	//DIFFUSE LIGHT
    float kd = 0.7;
    float amount_diffuse_light = max(dot(light_direction.xyz, input.normal), 0.0);
    float3 id = light.color.rgb;
    id *= (color.rgb);
    float3 diffuse_light = kd * id * amount_diffuse_light;

	//SPECULAR LIGHT
    float ks = 0.0;
    float3 is = float3(1.0, 1.0, 1.0);
    float3 reflected_light = reflect(light_direction.xyz, input.normal);
    float shininess = 30.0;
    float amount_specular_light = pow(max(0.0, dot(reflected_light, directionToCamera)), shininess);

    float3 specular_light = ks * amount_specular_light * is;

    float3 final_light = ambient_light + diffuse_light + specular_light;

    return float4(final_light, 1.0);
}