/*
Atmospheric Scattering and fancy stars

Credits goes to wwwtyro

References:
https://github.com/wwwtyro/glsl-atmosphere

If you want to use this code in your project, just do it but...
don't be a douchebag, and do not remove this header. Thanks.

LVutner 2020
*/

#ifndef ATMOSPHERIC_H
#define ATMOSPHERIC_H
#include "common.h"

#define PI 3.141592

//Atmospheric Scattering 
#define AS_ISTEPS int(4)
#define AS_JSTEPS int(2)
#define AS_SUN_INTENSITY float(40.0)
#define AS_DITHER float(0.0075)

float2 rsi(float3 r0, float3 rd, float sr) {
    // ray-sphere intersection that assumes
    // the sphere is centered at the origin.
    // No intersection when result.x > result.y
    float a = dot(rd, rd);
    float b = 2.0 * dot(rd, r0);
    float c = dot(r0, r0) - (sr * sr);
    float d = (b*b) - 4.0*a*c;
	
    if (d < 0.0)
		return float2(1e5,-1e5);

	return float2(
        (-b - sqrt(d))/(2.0*a),
        (-b + sqrt(d))/(2.0*a)
    );
}

float3 atmosphere(float3 r, float3 r0, float3 pSun, float iSun, float rPlanet, float rAtmos, float3 kRlh, float kMie, float shRlh, float shMie, float g) {
    // Normalize the sun and view directions.
    pSun = normalize(pSun);
    r = normalize(r);

    // Calculate the step size of the primary ray.
    float2 p = rsi(r0, r, rAtmos);
	
    if (p.x > p.y) 
		return float3(0,0,0);
	
    p.y = min(p.y, rsi(r0, r, rPlanet).x);
    float iStepSize = (p.y - p.x) / float(AS_ISTEPS);

    // Initialize the primary ray time.
    float iTime = 0.0;

    // Initialize accumulators for Rayleigh and Mie scattering.
    float3 totalRlh = (0,0,0);
    float3 totalMie = (0,0,0);

    // Initialize optical depth accumulators for the primary ray.
    float iOdRlh = 0.0;
    float iOdMie = 0.0;

    // Calculate the Rayleigh and Mie phases.
    float mu = dot(r, pSun);
    float mumu = mu * mu;
    float gg = g * g;
    float pRlh = 3.0 / (16.0 * PI) * (1.0 + mumu);
    float pMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));

    // Sample the primary ray.
    for (int i = 0; i < AS_ISTEPS; i++) {

        // Calculate the primary ray sample position.
        float3 iPos = r0 + r * (iTime + iStepSize * 0.5);

        // Calculate the height of the sample.
        float iHeight = length(iPos) - rPlanet;

        // Calculate the optical depth of the Rayleigh and Mie scattering for this step.
        float odStepRlh = exp(-iHeight / shRlh) * iStepSize;
        float odStepMie = exp(-iHeight / shMie) * iStepSize;

        // Accumulate optical depth.
        iOdRlh += odStepRlh;
        iOdMie += odStepMie;

        // Calculate the step size of the secondary ray.
        float jStepSize = rsi(iPos, pSun, rAtmos).y / float(AS_JSTEPS);

        // Initialize the secondary ray time.
        float jTime = 0.0;

        // Initialize optical depth accumulators for the secondary ray.
        float jOdRlh = 0.0;
        float jOdMie = 0.0;

        // Sample the secondary ray.
        for (int j = 0; j < AS_JSTEPS; j++) {

            // Calculate the secondary ray sample position.
            float3 jPos = iPos + pSun * (jTime + jStepSize * 0.5);

            // Calculate the height of the sample.
            float jHeight = length(jPos) - rPlanet;

            // Accumulate the optical depth.
            jOdRlh += exp(-jHeight / shRlh) * jStepSize;
            jOdMie += exp(-jHeight / shMie) * jStepSize;

            // Increment the secondary ray time.
            jTime += jStepSize;
        }

        // Calculate attenuation.
        float3 attn = exp(-(kMie * (iOdMie + jOdMie) + kRlh * (iOdRlh + jOdRlh)));

        // Accumulate scattering.
        totalRlh += odStepRlh * attn;
        totalMie += odStepMie * attn;

        // Increment the primary ray time.
        iTime += iStepSize;

    }

    // Calculate and return the final color.
    return iSun * (pRlh * kRlh * totalRlh + pMie * kMie * totalMie);
}

float3 compute_scattering(float3 position)
{
	float3 v2point = position.xyz-eye_position.xyz;	
	
    float3 sky = atmosphere(
        normalize(v2point),				// normalized ray direction
        float3(0,6372e3,0),				// ray origin
        normalize(-L_sun_dir_w.xyz),				// position of the sun
        AS_SUN_INTENSITY,                           // intensity of the sun
        6371e3,                      	   // radius of the planet in meters
        6471e3,                        	 // radius of the atmosphere in meters
        float3(5.5e-6, 13.0e-6, 22.4e-6),// Rayleigh scattering coefficient
        21e-6,                          // Mie scattering coefficient
        8e3,                            // Rayleigh scale height
        1.2e3,                          // Mie scale height
        0.758                           // Mie preferred scattering direction
    );

    // Apply exposure.
    sky = 1.0 - exp(-1.0 * sky);
	
	// Apply dithering
	sky += random(position.xz)*AS_DITHER;
	return sky;
}


float3 compute_stars(float3 img, float3 dir)
{
    //Sample stars texture
    float s_layer_1 = s_stars.Sample(smp_rtlinear, dir.xz*1024.f).r;
    float s_layer_2 = s_stars.Sample(smp_rtlinear, dir.xz*1024.f).g;
    float s_layer_3 = s_stars.Sample(smp_rtlinear, dir.xz*1024.f).b;
    float s_layer_4 = s_stars.Sample(smp_rtlinear, dir.xz*1024.f).a;
    
    //Make them dynamic
	s_layer_2 *= cos(timers.x*0.7f);
	s_layer_3 *= cos(timers.x*0.8f);
 	s_layer_4 *= cos(timers.x*0.9f);
    
    //Compute them!
    float stars_layer = s_layer_1 + s_layer_2 + s_layer_3 + s_layer_4 / 4.f;
    stars_layer = saturate(stars_layer);
    
    //Render them!
	float3 final = stars_layer.xxx;
    return final;
} 

#endif