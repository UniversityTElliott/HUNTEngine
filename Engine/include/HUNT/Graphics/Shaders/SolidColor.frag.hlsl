Texture2D HUNTTexture : register(t0, space2);
SamplerState HUNTSampler : register(s0, space2);

struct Input
{
    float4 Colour : TEXCOORD0;
    float2 UV : TEXCOORD1;
};

struct Output
{
    float4 Colour : SV_Target0;
};

Output main(Input input)
{
    Output output;
    
    float4 texColor = HUNTTexture.Sample(HUNTSampler, input.UV);

    output.Colour = texColor * input.Colour;

    return output;
}