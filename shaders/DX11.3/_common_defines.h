#ifndef	common_defines_h_included
#define	common_defines_h_included

float4 various_consts; //engine import
float4 various_consts_2; //engine import
float4 test_dev_floats; //engine import
static const float4 dev_floats 		= test_dev_floats;

//////////////////////////////////////////////////////////////
// Water Refl
float4 water_refl_params_1; //engine import
float4 water_refl_params_2; //engine import

static const bool w_refl_enabled 		= water_refl_params_1.x == 0;
static const bool w_refl_objects 		= water_refl_params_1.y == 1;
static const bool w_refl_moon_sun 		= water_refl_params_1.z == 1;

static const float w_refl_env_power 	= water_refl_params_2.x;
static const float w_refl_intencity 	= water_refl_params_2.y;
static const float w_refl_range 		= water_refl_params_2.z;
static const float w_refl_obj_itter 	= water_refl_params_2.w;

#define SW_FOAM_THICKNESS float (0.035)			// толщина пены, плавающей на поверхности воды

#define JITTER_ROADS_MR 						// края дорожки проходят через шум

#define USE_SUNMASK								// тени от облаков

//////////////////////////////////////////////////////////////
//
static const float alt_wet_hud 			= various_consts.x == 1;; // вкл/выкл нормальное намокание худа при дожде

#define WET_HUD_COUNT (0.9)						// сила намокания худа

#define DETAIL_AO_COUNT (1.0)					// фикс SSAO и HBAO. Лучше не трогать


//////////////////////////////////////////////////////////////
//Дефолтные фишки ЛА

#define USE_GRASS_WAVE							// вкл/выкл волны по траве
#define GRASS_WAVE_FREQ 	float(0.7)			// частота появления волн
#define GRASS_WAVE_POWER 	float(3.0)			// яркость волн
#define CGIM									// Обновленные шейдеры

//////////////////////////////////////////////////////////////
// Visor Overlay and Refraction / Reflection

static const bool rain_drops_enabled 	= various_consts.y == 1;
static const bool visor_enabled 		= various_consts.z == 1;
static const bool visor_shadowing 		= various_consts.w == 1;

#define VIS_XFACTOR float(2.0)
#define VIS_YFACTOR float(1.3)
#define VIS_CRUNCHINTENSITY float (0.4)	// Intensity of the peripheral/radial crunch effect in the visor
#define VIS_MIRRORINTENSITY float(0.18)		// Intensity of the mirroring effect in the visor
#define VIS_RADIALCOEFF float(1.0)			// Increment power of radial effect towards periphery (1.0 = linearly incrementing)
#define VIS_BREAK float(1.4)				// Was 0.9
#define VIS_MIRRORMINDIST float (2.50)		// Objects nearer than this values won't be mirror. Use 2.5 to prevent the player's weapon to be mirrored too much.
#define VIS_SCREENRATIO float(0.625)		// Screen Ratio; use 0.75 for 4:3 (normalscreen) and 0.625 for 16:10 (widescreen) resolution
#define VIS_SATURATION float(0.8)			// Level of color saturation before applying visor mirror color (if other than (1,1,1)). 0 is fully desaturated, 1 is original color
#define VIS_SAT_SUN_COEF float(0.7)			// How much influence sun lighting has on the visor mirror color saturation.
#define VIS_CONTRAST float(1.2)				// Contrast ratio of mirrored color (1.0 = normal, 0.0 = none).
#define VIS_COLOR float4(1,1,1,1)			// Color filter applied to visor ("sunglass effect") (RGBA)
#define VIS_COLORCOEFF float(2.0)			// Power of color filter of mirrored object (1.0 = linear, 2.0 = square)

// visor border settings
  #define VIS_CENTER float2(0.0,0.0)		// Y-axix center of border black out.
  #define VIS_BLACKASPECT float2(1.5,0.1)		// Aspect ratio of border black out.
  #define VIS_BLACKRADIUSMIN float2(0,0)	// Screen radius at which border black out begins.
  #define VIS_BLACKRADIUSMAX float2(0,0)	// Screen radius at which border black out is complete.

//////////////////////////////////////////////////////////////
// Visor Water Drops

float4 rain_drops_and_visor;

// Basic Settings
#define VSD_MIX 1.0f							// Amount/mix between original and modified image
#define VSD_ASPECT float(1.5)					// screen aspect ratio (change if not using wide-screen), makes drops round or not

// Advanced Settings
#define VSD_RADIUS float(0.046)					// primary drops pinch / magnification reference radius
#define VSD_INNERRADIUS float(0.005)			// secondary drops pinch / magnification  reference radius
#define VSD_STRENGTH float(100.0)				// intensity of the pinch / magnification effect inside the drops
#define VSD_DEFORM float(0.0080)	// 00350	// drops deformation amount 
#define VSD_SMOOTH 0.5f							// smoothness of drops rendering (not sure...) - looks awful, leave it at 0.0 for the moment
#define VSD_LIGHT 0.30f //0.30f							// Amount of lighting impact. Values 0-1. 0=no lighting, 1=full lighting.
#define VSD_FIXLIGHT float3(.5, .5, 1.)			// fixed light source position to illuminate drops

// Turbulence Settings (make drops render irregularily / more "natural")
#define VSD_TURBCOF 0.4 //0.33 //0.39					// Amount of drop turbulence
#define VSD_TURBSHIFT float4(0.35,1.00,0,1)		// Position shift for drop turbulence
#define VSD_TURBTIME sin(0.08/3) // 0.1			// Time shift for drop turbulence
#define VSD_TURBSIZE 15.f						// Size of drop turbulence 

static const float rain_density_factor_1	= rain_drops_and_visor.x;
static const float rain_drops_time_val_1	= rain_drops_and_visor.y; // changes the drops spread patern for first layer
static const float rain_density_factor_2	= rain_drops_and_visor.z;
static const float rain_drops_time_val_2	= rain_drops_and_visor.w; // changes the drops spread patern for second layer

// Wave Warp Settings (required to spread drops over the screen unorderedly, do not change normally)

#define VSD_WSIZE 32.f		// 32-256			// Wave size (amount of drops spreading)
#define VSD_WVAL1 0.64f		// 0-1				// Wave value 1 (just some value)
#define VSD_WVAL2 0.32f		// 0-1				// Wave value 2 (just some value)
#define VSD_WVAL3 0.192f		// 0-1			// Wave value 3 (just some value)
#define VSD_WVAL4 7.f			// 0-10			// Wave value 4 (just some value)

//////////////////////////////////////////////////////////////////////////////////////////
#define def_gloss       float(2.f /255.f)
#define def_aref        float(130.f/255.f)
#define def_dbumph      float(0.333f)
#define def_virtualh    float(0.05f)              // 5cm
#define def_distort     float(0.05f)             // we get -0.5 .. 0.5 range, this is -512 .. 512 for 1024, so scale it
#define def_hdr         float(9.h)         		// hight luminance range float(3.h)
#define def_hdr_clip	float(0.75h)        		//
#define def_lum_hrange	float(0.7h)	// hight luminance range, for build bloom
#define	LUMINANCE_VECTOR	float3(0.3f, 0.38f, 0.22f)

//////////////////////////////////////////////////////////////////////////////////////////
static const float bloom_type 			= various_consts_2.x;
static const bool  build_style_gloss 	= various_consts_2.y;

#ifndef SMAP_size
#define SMAP_size        1024
#endif

#endif
