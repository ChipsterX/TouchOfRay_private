#include "common.h"
#include "rain_drops.h"
// Original shader: https://www.shadertoy.com/view/ltffzl
//Done by BigWIngs (thanks man!). Modified, and ported by LVutner (2019)
// Перевод в движок OldSerpski Stalker

float2 mod289(float2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
float3 mod289(float3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
float4 mod289(float4 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
float3 permute(float3 x) { return mod289(((x*34.0)+1.0)*x); }
float4 permute(float4 x) { return mod289(((x*34.0)+1.0)*x); }
/// <summary>
/// 2D Noise by Ian McEwan, Ashima Arts.
/// <summary>
float snoise_2D (float2 v)
{
    const float4 C = float4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                        -0.577350269189626, // -1.0 + 2.0 * C.x
                        0.024390243902439); // 1.0 / 41.0

    // First corner
    float2 i  = floor(v + dot(v, C.yy) );
    float2 x0 = v -   i + dot(i, C.xx);

    // Other corners
    float2 i1;
    i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    float4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    float3 p = permute( permute( i.y + float3(0.0, i1.y, 1.0 ))
        + i.x + float3(0.0, i1.x, 1.0 ));

    float3 m = max(0.5 - float3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;

    // Gradients: 41 points uniformly over a line, mapped onto a diamond.
    // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

    float3 x = 2.0 * frac(p * C.www) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;

    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt( a0*a0 + h*h );
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

    // Compute final noise value at P
    float3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

float2 l_jh2(float2 f, float4 s, float l)
{
	float2 x = s.xy, V = s.zw;
	float y = snoise_2D(f * float2(DROPS_TURBSIZE, DROPS_TURBSIZE))*.5;
	float4 r = float4(y, y, y, 1);
	r.xy = float2(r.x + r.z/4, r.y + r.x/2);
	r -= 1.5;
	r *= l;
	return saturate(f + (x + V) *r.xy);
}


float3 N13(float p) {
   // from DAVE HOSKINS
   float3 p3 = frac(float3(p,p,p) * float3(.1031,.11369,.13787));
   p3 += dot(p3, p3.yzx + 19.19);
   return frac(float3((p3.x + p3.y)*p3.z, (p3.x+p3.z)*p3.y, (p3.y+p3.z)*p3.x));
}

float N(float t) {
    return frac(sin(t*12345.564)*7658.76);
}

float Saw(float b, float t) {
    return smoothstep(0., b, t)*smoothstep(1., b, t);
}

float2 DropLayer2(float2 uv, float t) 
{
    uv.y = 1. - uv.y;  
    float2 UV = uv;
    
    uv.y += t*0.75;
    float2 a = float2(9., 2.);
    float2 grid = a*2.;
    float2 id = floor(uv*grid);
    
    float colShift = N(id.x); 
    uv.y += colShift;
    
    id = floor(uv*grid);
    float3 n = N13(id.x*35.2+id.y*2376.1);
    float2 st = frac(uv*grid)-float2(.5, 0);
    
    float x = n.x-.5;
    
    float y = UV.y;
    float wiggle = sin(y+sin(y));
    x += wiggle*(.5-abs(x))*(n.z-.5);
    x *= .7;
    float ti = frac(t+n.z);
    y = (Saw(.85, ti)-.5)*.9+.5;
    float2 p = float2(x, y);
    
    float d = length((st-p)*a.yx);
    
    float mainDrop = smoothstep(.5, .0, d);
    
    float r = sqrt(smoothstep(1., y, st.y));
    float cd = abs(st.x-x);
    float trail = smoothstep(.23*r, .15*r*r, cd);
    float trailFront = smoothstep(-.02, .02, st.y-y);
    trail *= trailFront*r*r;
    
    y = UV.y;
    float trail2 = smoothstep(.2*r, .0, cd);
    float droplets = max(0., (sin(y*(1.-y)*120.)-st.y))*trail2*trailFront*n.z;
    y = frac(y*1.)+(st.y-.5); //1 is fucking trail 
    float dd = length(st-float2(x, y));
    droplets = smoothstep(.3, 0., dd);
    float m = mainDrop+droplets*r*trailFront;

    return float2(m, trail);
}

float StaticDrops(float2 uv, float t) 
{
	uv *= 40.;
    
    float2 id = floor(uv);
    uv = frac(uv)-.5;
    float3 n = N13(id.x*107.45+id.y*3543.654);
    float2 p = (n.xy-.5)*.7;
    float d = length(uv-p);
    
    float fade = Saw(.025, frac(t+n.z));
    float c = smoothstep(0.5, 0., d)*frac(n.z*10.)*fade;
    return c;
}

float2 Drops(float2 uv, float t, float l0, float l1, float l2) 
{
    float s = StaticDrops(lerp(uv, l_jh2(uv, DROPS_TURBSHIFT, DROPS_TURBTIME), DROPS_TURBCOF), t)*l0; 
    float2 m1 = DropLayer2(lerp(uv, l_jh2(uv, DROPS_TURBSHIFT, DROPS_TURBTIME), DROPS_TURBCOF), t)*l1;
    float2 m2 = DropLayer2(lerp(uv, l_jh2(uv, DROPS_TURBSHIFT, DROPS_TURBTIME), DROPS_TURBCOF)*1.75, t)*l2;
    
    float c = s+m1.x+m2.x;
    c = smoothstep(.3, 1., c);
    
    return float2(c, max(m1.y*l0, m2.y*l1));
}

float4 main(GasmaskStruct I) : COLOR
{
    float rainAmount = drops_control.x;
	//float2 uv = (I.tc0 - 0.5f) * (-1) + 0.5;
	float2 uv = I.tc0;
    float2 UV = uv;

    float T = drops_control.z*(timers.x + rainAmount * 2.);    

    float t = T*.2;

    float staticDrops = smoothstep(-.5, 4., rainAmount)*8.f;
    float layer1 = smoothstep(.25, .75, rainAmount);
    float layer2 = smoothstep(.0, .5, rainAmount);

    float2 c = Drops(uv, t, staticDrops, layer1, layer2);

    float2 e = float2(.00010, 0.); //fycking uv
    float cx = Drops(uv+e, t, staticDrops, layer1, layer2).x;
    float cy = Drops(uv+e.yx, t, staticDrops, layer1, layer2).x;
    float2 n = float2(cx-c.x, cy-c.x);


    float3 col = tex2D(s_image, UV + n);
	float4 clear_img = tex2D(s_image, I.tc0);    
	float4 mask;
#ifdef ALPHA_MASKING	
	if (mask_control.x == 1)
	{		
	mask_tex_diffuse = tex2D(s_mask_1,I.tc0);
	}
	else if (mask_control.x == 2)
	{
	mask_tex_diffuse = tex2D(s_mask_2,I.tc0);
	}
	else if (mask_control.x == 3)
	{
	mask_tex_diffuse = tex2D(s_mask_3,I.tc0);
	}
	else if (mask_control.x == 4)
	{
	mask_tex_diffuse = tex2D(s_mask_4,I.tc0);
	}
	else if (mask_control.x == 5)
	{
	mask_tex_diffuse = tex2D(s_mask_5,I.tc0);
	}
	else if (mask_control.x == 6)
	{		
	mask_tex_diffuse = tex2D(s_mask_6,I.tc0);
	}
	else if (mask_control.x == 7)
	{
	mask_tex_diffuse = tex2D(s_mask_7,I.tc0);
	}
	else if (mask_control.x == 8)
	{
	mask_tex_diffuse = tex2D(s_mask_8,I.tc0);
	}
	else if (mask_control.x == 9)
	{
	mask_tex_diffuse = tex2D(s_mask_9,I.tc0);
	}
	else if (mask_control.x == 10)
	{		
	mask_tex_diffuse = tex2D(s_mask_10,I.tc0);
	}	
    
	col = lerp(col, clear_img, mask.a);
#endif	
	return float4(col, 1.);
}
