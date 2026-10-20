//Lots of the shader stuff is from the module
/*
cbuffer MatrixBuffer : register(b0,space1)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};
*/

struct Input
{
    float3 position : TEXCOORD0;
    float4 colour : TEXCOORD1;
};

struct Output
{
    float4 position : SV_Position;
    float4 colour : TEXCOORD0;
};

Output main(Input input)
{
    Output output;
    float4 position = float4(input.position, 1.f);
    /*
    position = mul(position, worldMatrix);
    position = mul(position, viewMatrix);
    position = mul(position, projectionMatrix);
*/
    output.position = position;
    output.colour = input.colour;

    return output;
}