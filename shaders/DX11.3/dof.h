#ifndef DOF_H_INCLUDED
#define DOF_H_INCLUDED

uniform	float4	screen_res;
float4 dof_params; // x - near y - focus z - far w - sky distance
float3 dof_kernel; // x,y - resolution pre-scaled z - just kernel size

#ifndef USE_DOF
float3 dof(float3 image, float2 center)
{
	return image;
}
#else //	USE_DOF


float DOFFactor(float depth)
{
	float dist_to_focus = depth - dof_params.y;
	float blur_far = saturate(dist_to_focus / (dof_params.z - dof_params.y));
	float blur_near = saturate(dist_to_focus / (dof_params.x - dof_params.y));
	float blur = blur_near + blur_far;
	return saturate(blur * blur);
};

float calculate_dof_factor(float2 tc)
{
	float depth;
#ifndef USE_MSAA
	depth = s_position.Sample(smp_nofilter, tc).z;
#else
	depth = s_position.Load(int3(tc * pos_decompression_params2.xy, 0), 0).z;
#endif

	[flatten] if (depth <= EPSDEPTH)
		depth = dof_params.w;

	return DOFFactor(depth);
};

float3 dof(float3 color, float2 center)
{
	float blur = calculate_dof_factor(center) * dof_kernel.z;

	static const float2 o[12] = {
		{-0.326212, -0.405810},
		{-0.840144, -0.073580},
		{-0.695914, 0.457137},
		{-0.203345, 0.620716},
		{0.962340, -0.194983},
		{0.473434, -0.480026},
		{0.519456, 0.767022},
		{0.185461, -0.893124},
		{0.507431, 0.064425},
		{0.896420, 0.412458},
		{-0.321940, -0.932615},
		{-0.791559, -0.597710}
	};

	float contrib = 1.h;

	[unroll(12)] for (int i = 0; i < 12; i++)
	{
		float2 tap = center + o[i] * screen_res.zw * blur;
		float3 tap_color = s_image.Sample(smp_rtlinear, tap).rgb;
		color.rgb += tap_color * calculate_dof_factor(tap);
		contrib += calculate_dof_factor(tap);
	}

	return color / contrib;
};
#endif // USE_DOF
#endif // DOF_H_INCLUDED