#ifndef NOISEMODUL_PERMUTE
#define NOISEMODUL_PERMUTE
float2 mod289(float2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
float3 mod289(float3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
float4 mod289(float4 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
float3 permute(float3 x) { return mod289(((x*34.0)+1.0)*x); }
float4 permute(float4 x) { return mod289(((x*34.0)+1.0)*x); }
#endif