#ifndef OGSE_FUNCTIONS_H
#define OGSE_FUNCTIONS_H

#define SKY_WITH_DEPTH					// sky renders with depth to avoid some problems with reflections
#define SKY_DEPTH float(10.f)
#define SKY_EPS float(0.001)
#define FARPLANE float(180.0)

float normalize_depth(float depth)
{
	return (saturate(depth/FARPLANE));
}

static const float2 poisson_disk[12] = {
	float2(0.2636057f, 0.4026764f),
	float2(0.8931927f, 0.3809958f),
	float2(-0.351227f, 0.9287762f),
	float2(-0.4619994f, -0.001132701f),
	float2(0.1122695f, 0.9784672f),
	float2(0.6323467f, -0.2056593f),
	float2(-0.08509368f, -0.2938382f),
	float2(-0.2061059f, 0.3703068f),
	float2(-0.5982098f, -0.5618949f),
	float2(-0.8395036f, 0.4914388f),
	float2(-0.9885221f, 0.02723012f),
	float2(0.1027245f, -0.8669642f),
};

#ifndef SKY_WITH_DEPTH
float is_sky(float depth)		{return step(depth, SKY_EPS);}
float is_not_sky(float depth)	{return step(SKY_EPS, depth);}
#else
float is_sky(float depth)		{return step(abs(depth - SKY_DEPTH), SKY_EPS);}
float is_not_sky(float depth)	{return step(SKY_EPS, abs(depth - SKY_DEPTH));}
#endif

float is_in_range(float3 args)
{
	float mn = (args.x > args.y) ? 1: 0;
	float mx = (args.z > args.x) ? 1: 0;
	return mn*mx;
}

#endif