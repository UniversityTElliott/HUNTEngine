struct Input
{
    float4 Colour : TEXCOORD0;
};

struct Output
{
    float4 Colour : SV_Target0;
};

Output main(Input input)
{
    Output output;
    output.Colour = input.Colour;

    return output;
}