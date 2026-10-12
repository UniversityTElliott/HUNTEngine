struct Input
{
    float3 Position : TEXCOORD0;
    float4 colour : TEXCOORD1;
};

struct Output
{
    float4 Position : SV_Position;
    float4 colour : TEXCOORD0;
};

Output main(Input input)
{
    Output output;
    output.Position = float4(input.Position, 1.0f);
    output.colour = input.colour;

    return output;
}