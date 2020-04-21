#include "stdafx.h"
#include "xrRender_console.h"
#include "xrCore/xr_token.h"
#include "xrEngine/XR_IOConsole.h"
#include "xrEngine/xr_ioc_cmd.h"
#if defined(USE_DX10) || defined(USE_DX11) 
#include "xrRender/xrRenderDX10/StateManager/dx10SamplerStateCache.h"
#endif 

int ps_r2_gasmask_effect_enable = 0;
int ps_r2_vignette = 0;
int ps_render_volumetric_fog = 1;
int ps_r2_rain_drops = 1;
int ps_r2_rain_drops_effect = 0;
int ps_r__LightSleepFrames = 10;
int ps_r__tf_Anisotropic = 16;
int ps_r2_dhemi_count = 5; 
int ps_r2_wait_sleep = 0;
int ps_r3_dyn_wet_surf_sm_res = 1024; 
int ps_r__detail_radius = 75;
int ps_r1_SoftwareSkinning = 0; 
int ps_r3_rain_4p = 0;
int ps_r3_dyn_wet_surf_opt = 1;
int	ps_r3_backbuffers_count	= 2;
int	render_nightvision = 0;
int ps_mt_loading_textures = 1;
int psSkeletonUpdate = 32;

//sweetfx
BOOL ps_r3_technicolor = FALSE;
BOOL ps_r3_hud_rain_drops = FALSE;
BOOL ps_r3_hud_visor_effect	= FALSE;
BOOL ps_r3_adv_wet_hud = TRUE;
BOOL ps_r3_hud_visor_shadowing = FALSE;

float debug_f_h = 0.f;
float debug_f_d = 0.f;
float debug_f_maxd = 0.f;
float debug_f_mixd = 0.f;

float ps_r3_vibrance_val = 0.0f;

float ps_volumetric_intensity = 0.1f;
float ps_volumetric_distance = 1.f;
float ps_volumetric_quality = 0.35f;

float ps_r__Detail_l_ambient = 0.9f;
float ps_r__Detail_l_aniso = 0.25f;
float ps_r__Detail_density = 0.3f;
float ps_r__Detail_rainbow_hemi = 0.75f;
float ps_r__Tree_w_rot = 10.0f;
float ps_r__Tree_w_speed = 1.00f;
float ps_r__Tree_w_amp = 0.005f;
float ps_r2_rain_drops_diff = 0.f;
float render_gamma = 0.f;
float ps_r2_gasmask = 0.f;
float ps_r__Tree_SBC = 1.5f;
float ps_r__WallmarkTTL = 50.f;
float ps_r__WallmarkSHIFT = 0.0001f;
float ps_r__WallmarkSHIFT_V = 0.0001f;
float ps_r__LOD = 0.75f;

// Base factor values
float ps_r__GLOD_ssa_start		= 256.f;
float ps_r__GLOD_ssa_end			= 64.f;
float ps_r__ssaDISCARD			= 3.5f;
float ps_r__ssaHZBvsTEX			= 96.f;
// Distance factor values
float ps_r__geomLodSpriteDistF_	= 2.0f;
float ps_r__geomDiscardDistF_		= 2.0f;
float ps_r__geomLodDistF_			= 2.0f;
float ps_r__geomNTextureDistF_	= 2.0f;
float ps_r__geomDTextureDistF_	= 2.0f;

float ps_lod_filtr_dx11 = -2.5f;
float ps_lod_filtr_dx9 = -0.5f;




float ps_r1_pps_u = 0.f;
float ps_r1_pps_v = 0.f;
float ps_r2_ssaLOD_A = 96.f;
float ps_r2_ssaLOD_B = 64.f;
float ps_r2_df_parallax_h = 0.02f;
float ps_r2_df_parallax_range = 75.f;

float ps_r2_steep_parallax_h = 0.013f;
float ps_r2_steep_parallax_distance = 100.0f;
float ps_r2_steep_parallax_samples = 100.0f;
float ps_r2_steep_parallax_samples_min = 5.0f;

float ps_r2_tonemap_middlegray = 0.45f; 
float ps_r2_tonemap_adaptation = 1.f; 
float ps_r2_tonemap_low_lum = 0.0001f; 
float ps_r2_tonemap_amount = 0.7f; 
float ps_r2_ls_bloom_kernel_g = 3.f;
float ps_r2_ls_bloom_kernel_b = .7f; 
float ps_r2_ls_bloom_speed = 100.f; 
float ps_r2_ls_bloom_kernel_scale = .7f; 
float ps_r2_ls_dsm_kernel = .7f; 
float ps_r2_ls_psm_kernel = .7f; 
float ps_r2_ls_ssm_kernel = .7f; 
float ps_r2_ls_bloom_threshold = .00001f; 
float ps_r2_mblur = 0.2f; 
float ps_r2_ls_depth_scale = 1.00001f; 
float ps_r2_ls_depth_bias = -0.0003f;
float ps_r2_ls_squality = 1.0f; 
float ps_r2_sun_tsm_projection = 0.3f; 
float ps_r2_sun_tsm_bias = -0.01f; 
float ps_r2_sun_near = 20.f; 
float ps_r2_sun_near_border = 0.75f; 
float ps_r2_sun_depth_far_scale = 1.00000f;
float ps_r2_sun_depth_far_bias = -0.00002f; 
float ps_r2_sun_depth_near_scale = 1.0000f; 
float ps_r2_sun_depth_near_bias = 0.00001f;

//r2_sun ambient and hemi
float ps_r2_sun_lumscale = 1.35f; 
const float ps_r2_sun_lumscale_amb = 1.f;
const float ps_r2_sun_lumscale_hemi = 0.9f; 


float ps_r2_zfill = 0.25f; 
float ps_r2_dhemi_sky_scale = 0.08f; 
float ps_r2_dhemi_light_scale = 0.2f;
float ps_r2_dhemi_light_flow = 0.1f;
float ps_r2_lt_smooth = 1.f; 
float ps_r2_slight_fade = 0.5f; 
float dm_fade = 47.f;
float ps_r2_dof_kernel_size = 2.0f; 
float ps_r3_dyn_wet_surf_near = 5.f; 
float ps_r3_dyn_wet_surf_far = 30.f; 
float ps_r2_gloss_factor = 1.0f;
float dm_current_fade = 47.f; 
float ps_current_detail_density = 0.6f;
float ps_current_detail_scale = 1.f;
float ps_r2_ss_sunshafts_length = 1.f;
float ps_r2_ss_sunshafts_radius = 1.f;
float ps_r2_particle_distance = 75.0f;

u32 ps_r_bloom_type = 3;
xr_token qbloom_type_token[] = {
	{ "st_opt_default", 1 },
	{ "st_opt_2218", 2 },
	{ "st_opt_csky", 3 },
	{ 0, 0 }
};

u32 ps_r_type_aa = 1;
xr_token token_aa[] = {
    { "disable_AA", 0 },
    { "FXAA", 1 },
    { "DLAA", 2 },
    { nullptr, 0 }
};

u32 ps_Preset = 2;
const xr_token qpreset_token[] = {
    {"Minimum", 0},
    {"Low", 1},
    {"Default", 2},
    {"High", 3},
    {"Extreme", 4},
    {nullptr, 0}
};

u32 ps_r_sun_shafts = 2;
const xr_token qsun_shafts_token[] = {
    {"st_opt_off", 0},
    {"st_opt_low", 1},
    {"st_opt_medium", 2},
    {"st_opt_high", 3},
    {nullptr, 0}
};

u32 ps_sunshafts_mode = 0;
xr_token sunshafts_mode_token[] = {
    { "volumetric", 0 },
    { "screen_space", 1 },
    { nullptr, 0 }
};

//oldSerpski stalker
u32 ps_mHToken = 2;
xr_token monitors_hertz_tokens[] = {
    {"50Hz", 0},
    {"59Hz", 1},
    {"60Hz", 2},
    {"74Hz", 3},
    {"75Hz", 4},
    {"90Hz", 5},
    {"100Hz", 6},
    {"120Hz", 7},
    {"144Hz", 8},
    {"165Hz", 9},
    {"240Hz", 10},
    {"244Hz", 11},
    { nullptr, 0 }
};

u32 ps_r_ao_quality = 3;
const xr_token qao_quality_token[] = {
    {"st_opt_off", 0},
    {"st_opt_low", 1},
    {"st_opt_medium", 2},
    {"st_opt_high", 3},
    {"st_opt_ultra", 4},
    {nullptr, 0}
};

u32 ps_r_sun_quality = 1; 
const xr_token qsun_quality_token[] = {
    {"st_opt_low", 0},
    {"st_opt_medium", 1},
    {"st_opt_high", 2},
#if defined(USE_DX11)
    {"st_opt_ultra", 3},
    {"st_opt_extreme", 4},
#endif 
    {nullptr, 0}
};

u32 ps_r3_msaa = 0; 
const xr_token qmsaa_token[] = {
    {"st_opt_off", 0},
    {"2x", 1},
    {"4x", 2},
#if defined(USE_DX11)
    {"8x", 3},
#endif
    {nullptr, 0}
};

#if !(defined(DISABLE_SSAO_SYSTEM_R2) || defined(DISABLE_SSAO_SYSTEM_R3) || defined(DISABLE_SSAO_SYSTEM_R4))
u32 ps_r_ssao_mode = 1;
const xr_token qssao_mode_token[] = {
    {"disabled", 0},
    {"default", 1},
    {"hdao", 2},
    {"hbao", 3},
    {nullptr, 0}
};

u32 ps_r_ssao = 3;
const xr_token qssao_token[] = {
{"st_opt_off", 0},
    {"st_opt_low", 1},
    {"st_opt_medium", 2},
    {"st_opt_high", 3},
#if defined(USE_DX11)
    {"st_opt_ultra", 4},
#endif
    {nullptr, 0}
};

class CCC_SSAO_Mode : public CCC_Token
{
public:
    CCC_SSAO_Mode(LPCSTR N, u32* V, const xr_token* T) : CCC_Token(N, V, T){};

    virtual void Execute(LPCSTR args)
    {
        CCC_Token::Execute(args);

        switch (*value)
        {
        case 0:
        {
            ps_r_ssao = 0;
            ps_r2_ls_flags_ext.set(R2FLAGEXT_SSAO_HBAO, 0);
            ps_r2_ls_flags_ext.set(R2FLAGEXT_SSAO_HDAO, 0);
            break;
        }
        case 1:
        {
            if (ps_r_ssao == 0)
            {
                ps_r_ssao = 1;
            }
            ps_r2_ls_flags_ext.set(R2FLAGEXT_SSAO_HBAO, 0);
            ps_r2_ls_flags_ext.set(R2FLAGEXT_SSAO_HDAO, 0);
            ps_r2_ls_flags_ext.set(R2FLAGEXT_SSAO_HALF_DATA, 0);
            break;
        }
        case 2:
        {
            if (ps_r_ssao == 0)
            {
                ps_r_ssao = 1;
            }
            ps_r2_ls_flags_ext.set(R2FLAGEXT_SSAO_HBAO, 0);
            ps_r2_ls_flags_ext.set(R2FLAGEXT_SSAO_HDAO, 1);
            ps_r2_ls_flags_ext.set(R2FLAGEXT_SSAO_OPT_DATA, 0);
            ps_r2_ls_flags_ext.set(R2FLAGEXT_SSAO_HALF_DATA, 0);
            break;
        }
        case 3:
        {
            if (ps_r_ssao == 0)
            {
                ps_r_ssao = 1;
            }
            ps_r2_ls_flags_ext.set(R2FLAGEXT_SSAO_HBAO, 1);
            ps_r2_ls_flags_ext.set(R2FLAGEXT_SSAO_HDAO, 0);
            ps_r2_ls_flags_ext.set(R2FLAGEXT_SSAO_OPT_DATA, 1);
            break;
        }
        }
    }
};

#endif

u32 ps_r3_msaa_atest = 0; 
const xr_token qmsaa__atest_token[] = {
    {"st_opt_off", 0},
    {"st_opt_atest_msaa_dx10_0", 1},
    {"st_opt_atest_msaa_dx10_1", 2},
    {nullptr, 0}
};

u32 ps_r3_minmax_sm = 3; 
const xr_token qminmax_sm_token[] = {
    {"off", 0},
    {"on", 1},
    {"auto", 2},
    {"autodetect", 3},
    {nullptr, 0}
};

u32 ps_r2_smap_size = 1536; 
const xr_token qsmap_size_token[] = {
	{ "SM_1", 1032 },
	{ "SM_2", 1536 },
	{ "SM_3", 2048 },
    { "SM_4", 2560 },
    { "SM_5", 3072 },
    { "SM_6", 3584 },
    { "SM_7", 4096 },
    { nullptr, 0 }
};

xr_token ext_quality_token[] = {
    {"qt_off", 0},
    {"qt_low", 1},
    {"qt_medium", 2},
    {"qt_high", 3},
    {"qt_extreme", 4},
    {nullptr, 0}
};

u32 optTessQuality_ = 0;
xr_token qtess_quality_token[] =
{
    { "st_tess_low",			0 },
    { "st_tess_med",			1 },
    { "st_tess_optimum",		2 },
    { "st_tess_overneeded",		3 },
    { 0, 0 }
};

extern float r__dtex_range;
extern float OLES_SUN_LIMIT_27_01_07;
const float ps_r2_dof_sky = 30.f;     // const float: distance to sky

BOOL ps_clear_models_on_unload = 0;     // Alundaio
BOOL ps_no_scale_on_fade = 0;           // Alundaio
BOOL ps_use_precompiled_shaders = 0;    // Alundaio
BOOL ps_grass_shadow = 0;               // Alundaio

Fvector ps_r__Tree_Wave = { .1f, .01f, .11f };
Fvector ps_r2_drops_control = { 0.000000f, 0.000000f, 0.000000f};
Fvector3 ps_r2_dof = Fvector3().set(-1.25f, 1.4f, 600.f);

Ivector ps_r2_details_opt = { 60, 90, 120 };

Flags32 ps_common_flags = { 0 };
Flags32 ps_actor_shadow_flags = { 0 };

Flags32 ps_r2_ls_flags_ext = 
{ 
#if !(defined(DISABLE_SSAO_SYSTEM_R2) || defined(DISABLE_SSAO_SYSTEM_R3) || defined(DISABLE_SSAO_SYSTEM_R4))
	R2FLAGEXT_SSAO_HALF_DATA |
#endif
	R2FLAGEXT_ENABLE_TESSELLATION | 
	R2FLAGEXT_SUN_FLARES | 
	R2FLAGEXT_DOF_WEATHER 
};

Flags32 ps_r2_ls_flags = {
    R2FLAG_SUN| 
    R2FLAG_EXP_DONT_TEST_UNSHADOWED | 
    R2FLAG_USE_NVSTENCIL | 
    R2FLAG_EXP_SPLIT_SCENE | 
    R2FLAG_EXP_MT_CALC |
    R3FLAG_DYN_WET_SURF | 
    R3FLAG_VOLUMETRIC_SMOKE | 
    R2FLAG_DETAIL_BUMP | 
    R2FLAG_DOF | 
    R2FLAG_SOFT_PARTICLES | 
    R2FLAG_SOFT_WATER |
    R2FLAG_STEEP_PARALLAX | 
    R2FLAG_SUN_FOCUS | 
    R2FLAG_SUN_TSM | 
    R2FLAG_TONEMAP | 
    R2FLAG_VOLUMETRIC_LIGHTS |
    R2FLAG_SSR
}; 

// управление радиусом отрисовки травы
u32 dm_size = 24;
u32 dm_cache1_line = 12; // dm_size*2/dm_cache1_count
u32 dm_cache_line = 49; // dm_size+1+dm_size
u32 dm_cache_size = 2401; // dm_cache_line*dm_cache_line

u32 dm_current_size = 24;
u32 dm_current_cache1_line = 12; // dm_current_size*2/dm_cache1_count
u32 dm_current_cache_line = 49; // dm_current_size+1+dm_current_size
u32 dm_current_cache_size = 2401; // dm_current_cache_line*dm_current_cache_line
u32 ps_steep_parallax = 0;

// AVO: detail draw radius
class CCC_detail_radius : public CCC_Integer
{
public:
    void apply()
    {
        dm_current_size = iFloor((float)ps_r__detail_radius / 4) * 2;
        dm_current_cache1_line = dm_current_size * 2 / 4; 
        dm_current_cache_line = dm_current_size + 1 + dm_current_size;
        dm_current_cache_size = dm_current_cache_line * dm_current_cache_line;
        dm_current_fade = float(2 * dm_current_size) - .5f;
    }

    CCC_detail_radius(LPCSTR N, int* V, int _min = 0, int _max = 999) : CCC_Integer(N, V, _min, _max){};

    void Execute(LPCSTR args) override
    {
        CCC_Integer::Execute(args);
        apply();
    }

    void Status(TStatus& S) override { CCC_Integer::Status(S); }
};
//-AVO

class CCC_DetailsOpt : public CCC_IVector3
{
public:
    CCC_DetailsOpt(LPCSTR N, Ivector* V, const Ivector _min, const Ivector _max) : CCC_IVector3(N, V, _min, _max) {}
    virtual void Execute(LPCSTR args)
    {
        Ivector v;
        if ((3 != sscanf(args, "%d,%d,%d", &v.x, &v.y, &v.z)) && (3 != sscanf(args, "(%d, %d, %d)", &v.x, &v.y, &v.z)))
        {
            InvalidSyntax();
        }
        else if (v.x > v.y || v.y > v.z)
        {
            InvalidSyntax();
        }
        else
        {
            CCC_IVector3::Execute(args);
        }
    }
};

class CCC_tf_Aniso : public CCC_Integer
{
public:
    void apply()
    {
        if (nullptr == HW.pDevice)
            return;
        int val = *value;
        clamp(val, 1, 16);
#if defined(USE_DX10) || defined(USE_DX11)
        SSManager.SetMaxAnisotropy(val);
#else
        for (u32 i = 0; i < HW.Caps.raster.dwStages; i++)
            CHK_DX(HW.pDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, val));
#endif // USE_DX10
    }
    CCC_tf_Aniso(LPCSTR N, int* v) : CCC_Integer(N, v, 1, 16){};
    virtual void Execute(LPCSTR args)
    {
        CCC_Integer::Execute(args);
        apply();
    }
    virtual void Status(TStatus& S)
    {
        CCC_Integer::Status(S);
        apply();
    }
};

class CCC_tf_MipBias : public CCC_Float
{
public:
    void apply()
    {
        if (nullptr == HW.pDevice)
            return;

#if defined(USE_DX10) || defined(USE_DX11)
		SSManager.SetMipLODBias(*value);
#else 
        for (u32 i = 0; i < HW.Caps.raster.dwStages; i++)
            CHK_DX(HW.pDevice->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)value)));
#endif 
    }

    CCC_tf_MipBias(LPCSTR N, float* v) : CCC_Float(N, v, -3.f, +3.f) {}
    virtual void Execute(LPCSTR args)
    {
        CCC_Float::Execute(args);
        apply();
    }
    virtual void Status(TStatus& S)
    {
        CCC_Float::Status(S);
        apply();
    }
};

class CCC_R2GM : public CCC_Float
{
public:
    CCC_R2GM(LPCSTR N, float* v) : CCC_Float(N, v, 0.f, 4.f) { *v = 0; };
    virtual void Execute(LPCSTR args)
    {
        if (0 == xr_strcmp(args, "on"))
        {
            ps_r2_ls_flags.set(R2FLAG_GLOBALMATERIAL, TRUE);
        }
        else if (0 == xr_strcmp(args, "off"))
        {
            ps_r2_ls_flags.set(R2FLAG_GLOBALMATERIAL, FALSE);
        }
        else
        {
            CCC_Float::Execute(args);
            if (ps_r2_ls_flags.test(R2FLAG_GLOBALMATERIAL))
            {
                static LPCSTR name[4] = {"oren", "blin", "phong", "metal"};
                float mid = *value;
                int m0 = iFloor(mid) % 4;
                int m1 = (m0 + 1) % 4;
                float frc = mid - float(iFloor(mid));
                Msg("* material set to [%s]-[%s], with lerp of [%f]", name[m0], name[m1], frc);
            }
        }
    }
};

class CCC_Screenshot : public IConsole_Command
{
public:
    CCC_Screenshot(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        if (GEnv.isDedicatedServer)
            return;

        string_path name;
        name[0] = 0;
        sscanf(args, "%s", name);
        LPCSTR image = xr_strlen(name) ? name : 0;
        GEnv.Render->Screenshot(IRender::SM_NORMAL, image);
    }
};

class CCC_RestoreQuadIBData : public IConsole_Command
{
public:
    CCC_RestoreQuadIBData(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR) { RCache.RestoreQuadIBData(); }
};

class CCC_ModelPoolStat : public IConsole_Command
{
public:
    CCC_ModelPoolStat(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR) { RImplementation.Models->dump(); }
};

//-----------------------------------------------------------------------
class CCC_Preset : public CCC_Token
{
public:
    CCC_Preset(LPCSTR N, u32* V, const xr_token* T) : CCC_Token(N, V, T){};

    virtual void Execute(LPCSTR args)
    {
        CCC_Token::Execute(args);
        string_path _cfg;
        string_path cmd;

        switch (*value)
        {
        case 0: 
            xr_strcpy(_cfg, "renderers\\minimum.ltx"); break;
        case 1: 
            xr_strcpy(_cfg, "renderers\\low.ltx"); break;
        case 2: 
            xr_strcpy(_cfg, "renderers\\default.ltx"); break;
        case 3: 
            xr_strcpy(_cfg, "renderers\\high.ltx"); break;
        case 4: 
            xr_strcpy(_cfg, "renderers\\extreme.ltx"); break;
        }
        FS.update_path(_cfg, "$game_config$", _cfg);
        strconcat(sizeof(cmd), cmd, "cfg_load", " ", _cfg);
        Console->Execute(cmd);
    }
};

class CCC_memory_stats : public IConsole_Command
{
public:
    CCC_memory_stats(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = true; };
    virtual void Execute(LPCSTR)
    {
        u32 m_base = 0;
        u32 c_base = 0;
        u32 m_lmaps = 0;
        u32 c_lmaps = 0;

        RImplementation.ResourcesGetMemoryUsage(m_base, c_base, m_lmaps, c_lmaps);

        Msg("memory usage  mb \t \t video    \t managed      \t system \n");

        const float MiB = 1024 * 1024; // XXX: use it as common enum value (like in X-Ray 2.0)
        const u32* mem_usage = HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_vertex];

        float vb_video = mem_usage[D3DPOOL_DEFAULT] / MiB;
        float vb_managed = mem_usage[D3DPOOL_MANAGED] / MiB;
        float vb_system = mem_usage[D3DPOOL_SYSTEMMEM] / MiB;
        Msg("vertex buffer      \t \t %f \t %f \t %f ", vb_video, vb_managed, vb_system);

        float ib_video = mem_usage[D3DPOOL_DEFAULT] / MiB;
        float ib_managed = mem_usage[D3DPOOL_MANAGED] / MiB;
        float ib_system = mem_usage[D3DPOOL_SYSTEMMEM] / MiB;
        Msg("index buffer      \t \t %f \t %f \t %f ", ib_video, ib_managed, ib_system);

        float textures_managed = (m_base + m_lmaps) / MiB;
        Msg("textures          \t \t %f \t %f \t %f ", 0.f, textures_managed, 0.f);

        mem_usage = HW.stats_manager.memory_usage_summary[enum_stats_buffer_type_rtarget];
        float rt_video = mem_usage[D3DPOOL_DEFAULT] / MiB;
        float rt_managed = mem_usage[D3DPOOL_MANAGED] / MiB;
        float rt_system = mem_usage[D3DPOOL_SYSTEMMEM] / MiB;
        Msg("R-Targets         \t \t %f \t %f \t %f ", rt_video, rt_managed, rt_system);

        Msg("\nTotal             \t \t %f \t %f \t %f ", vb_video + ib_video + rt_video,
            textures_managed + vb_managed + ib_managed + rt_managed, vb_system + ib_system + rt_system);
    }
};

#include "r__pixel_calculator.h"
class CCC_BuildSSA : public IConsole_Command
{
public:
    CCC_BuildSSA(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR)
    {
#if !defined(USE_DX11)
        //  TODO: DX10: Implement pixel calculator
        r_pixel_calculator c;
        c.run();
#endif //   USE_DX10
    }
};

class CCC_DofFar : public CCC_Float
{
public:
    CCC_DofFar(LPCSTR N, float* V, float _min = 0.0f, float _max = 10000.0f) : CCC_Float(N, V, _min, _max) {}
    virtual void Execute(LPCSTR args)
    {
        float v = float(atof(args));

        if (v < ps_r2_dof.y + 0.1f)
        {
            char pBuf[256];
            _snprintf(pBuf, sizeof(pBuf) / sizeof(pBuf[0]), "float value greater or equal to r2_dof_focus+0.1");
            Msg("~ Invalid syntax in call to '%s'", cName);
            Msg("~ Valid arguments: %s", pBuf);
            Console->Execute("r2_dof_focus");
        }
        else
        {
            CCC_Float::Execute(args);
            if (g_pGamePersistent)
                g_pGamePersistent->SetBaseDof(ps_r2_dof);
        }
    }

    //  CCC_Dof should save all data as well as load from config
    virtual void Save(IWriter*) { ; }
};

class CCC_DofNear : public CCC_Float
{
public:
    CCC_DofNear(LPCSTR N, float* V, float _min = 0.0f, float _max = 10000.0f) : CCC_Float(N, V, _min, _max) {}
    virtual void Execute(LPCSTR args)
    {
        float v = float(atof(args));

        if (v > ps_r2_dof.y - 0.1f)
        {
            char pBuf[256];
            _snprintf(pBuf, sizeof(pBuf) / sizeof(pBuf[0]), "float value less or equal to r2_dof_focus-0.1");
            Msg("~ Invalid syntax in call to '%s'", cName);
            Msg("~ Valid arguments: %s", pBuf);
            Console->Execute("r2_dof_focus");
        }
        else
        {
            CCC_Float::Execute(args);
            if (g_pGamePersistent)
                g_pGamePersistent->SetBaseDof(ps_r2_dof);
        }
    }

    // CCC_Dof should save all data as well as load from config
    virtual void Save(IWriter* /*F*/) { ; }
};

class CCC_DofFocus : public CCC_Float
{
public:
    CCC_DofFocus(LPCSTR N, float* V, float _min = 0.0f, float _max = 10000.0f) : CCC_Float(N, V, _min, _max) {}
    virtual void Execute(LPCSTR args)
    {
        float v = float(atof(args));

        if (v > ps_r2_dof.z - 0.1f)
        {
            char pBuf[256];
            _snprintf(pBuf, sizeof(pBuf) / sizeof(pBuf[0]), "float value less or equal to r2_dof_far-0.1");
            Msg("~ Invalid syntax in call to '%s'", cName);
            Msg("~ Valid arguments: %s", pBuf);
            Console->Execute("r2_dof_far");
        }
        else if (v < ps_r2_dof.x + 0.1f)
        {
            char pBuf[256];
            _snprintf(pBuf, sizeof(pBuf) / sizeof(pBuf[0]), "float value greater or equal to r2_dof_far-0.1");
            Msg("~ Invalid syntax in call to '%s'", cName);
            Msg("~ Valid arguments: %s", pBuf);
            Console->Execute("r2_dof_near");
        }
        else
        {
            CCC_Float::Execute(args);
            if (g_pGamePersistent)
                g_pGamePersistent->SetBaseDof(ps_r2_dof);
        }
    }

    //  CCC_Dof should save all data as well as load from config
    virtual void Save(IWriter*) { ; }
};

class CCC_Dof : public CCC_Vector3
{
public:
    CCC_Dof(LPCSTR N, Fvector* V, const Fvector _min, const Fvector _max) : CCC_Vector3(N, V, _min, _max) { ; }
    virtual void Execute(LPCSTR args)
    {
        Fvector v;
        if (3 != sscanf(args, "%f,%f,%f", &v.x, &v.y, &v.z))
            InvalidSyntax();
        else if ((v.x > v.y - 0.1f) || (v.z < v.y + 0.1f))
        {
            InvalidSyntax();
            Msg("x <= y - 0.1");
            Msg("y <= z - 0.1");
        }
        else
        {
            CCC_Vector3::Execute(args);
            if (g_pGamePersistent)
                g_pGamePersistent->SetBaseDof(ps_r2_dof);
        }
    }
    virtual void Status(TStatus& S) { xr_sprintf(S, "%f,%f,%f", value->x, value->y, value->z); }
    virtual void Info(TInfo& I)
    {
        xr_sprintf(I, "vector3 in range [%f,%f,%f]-[%f,%f,%f]", min.x, min.y, min.z, max.x, max.y, max.z);
    }
};

class CCC_DumpResources : public IConsole_Command
{
public:
    CCC_DumpResources(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR)
    {
        RImplementation.Models->dump();
        RImplementation.Resources->Dump(false);
    }
};

class CCC_SunshaftsIntensity : public CCC_Float
{
public:
    CCC_SunshaftsIntensity(LPCSTR N, float* V, float _min, float _max) : CCC_Float(N, V, _min, _max) {}
    virtual void Save(IWriter*) { ; }
};

void xrRender_initconsole()
{
    Fvector tw_min, tw_max;
    Ivector dopt_min, dopt_max;
    dopt_min.set(1, 1, 1);
    dopt_max.set(390, 390, 390);

    CMD1(CCC_Screenshot,                        "screenshot");
    CMD1(CCC_memory_stats,                      "render_memory_stats");
    CMD1(CCC_RestoreQuadIBData,                 "r_restore_quad_ib_data");                      //  Igor: just to test bug with rain/particles corruption


    CMD2(CCC_tf_Aniso,                          "r__tf_aniso",                                  &ps_r__tf_Anisotropic); 
    CMD2(CCC_tf_MipBias,                        "lod_filtr_dx11",                               &ps_lod_filtr_dx11);
    CMD2(CCC_tf_MipBias,                        "lod_filtr_dx9",                                &ps_lod_filtr_dx9);

    CMD3(CCC_Preset,                            "_preset",                                      &ps_Preset, 				qpreset_token);
    CMD3(CCC_Mask,                              "r2_tonemap",                                   &ps_r2_ls_flags,            R2FLAG_TONEMAP);
    CMD3(CCC_Mask,                              "r2_ls_bloom_fast",                             &ps_r2_ls_flags,            R2FLAG_FASTBLOOM);
    CMD3(CCC_Mask,                              "r2_zfill",                                     &ps_r2_ls_flags,            R2FLAG_ZFILL);
    CMD3(CCC_Mask,                              "r2_allow_r1_lights",                           &ps_r2_ls_flags,            R2FLAG_R1LIGHTS);
    CMD3(CCC_Mask,                              "r__actor_shadow",                              &ps_actor_shadow_flags,     RFLAG_ACTOR_SHADOW);         
    CMD3(CCC_Mask,                              "r2_sun",                                       &ps_r2_ls_flags,            R2FLAG_SUN);
    CMD3(CCC_Mask,                              "r2_sun_details",                               &ps_r2_ls_flags,            R2FLAG_SUN_DETAILS);
    CMD3(CCC_Mask,                              "r2_sun_focus",                                 &ps_r2_ls_flags,            R2FLAG_SUN_FOCUS);
    CMD3(CCC_Mask,                              "r2_exp_donttest_shad",                         &ps_r2_ls_flags,            R2FLAG_EXP_DONT_TEST_SHADOWED);
    CMD3(CCC_Mask,                              "r2_sun_tsm",                                   &ps_r2_ls_flags,            R2FLAG_SUN_TSM);
    CMD3(CCC_Mask,                              "r2_dof_enable",                                &ps_r2_ls_flags,            R2FLAG_DOF);
    // skyloader: use dof values from weather or console
    CMD3(CCC_Mask,                              "r2_dof_weather",                               &ps_r2_ls_flags_ext,        R2FLAGEXT_DOF_WEATHER); 
    //
    CMD3(CCC_Mask,                              "r2_volumetric_lights",                         &ps_r2_ls_flags,            R2FLAG_VOLUMETRIC_LIGHTS);
    CMD3(CCC_Token,                             "r2_sun_shafts",                                &ps_r_sun_shafts,           qsun_shafts_token);
	
#if !(defined(DISABLE_SSAO_SYSTEM_R2) || defined(DISABLE_SSAO_SYSTEM_R3) || defined(DISABLE_SSAO_SYSTEM_R4))	
	CMD3(CCC_SSAO_Mode,                         "r2_ssao_mode",                                 &ps_r_ssao_mode,            qssao_mode_token);
    CMD3(CCC_Token,                             "r2_ssao",                                      &ps_r_ssao,                 qssao_token);
    CMD3(CCC_Mask,                              "r2_ssao_blur",                                 &ps_r2_ls_flags_ext,        R2FLAGEXT_SSAO_BLUR);            
    CMD3(CCC_Mask,                              "r2_ssao_opt_data",                             &ps_r2_ls_flags_ext,        R2FLAGEXT_SSAO_OPT_DATA);       
    CMD3(CCC_Mask,                              "r2_ssao_half_data",                            &ps_r2_ls_flags_ext,        R2FLAGEXT_SSAO_HALF_DATA);          
    CMD3(CCC_Mask,                              "r2_ssao_hbao",                                 &ps_r2_ls_flags_ext,        R2FLAGEXT_SSAO_HBAO);              
    CMD3(CCC_Mask,                              "r2_ssao_hdao",                                 &ps_r2_ls_flags_ext,        R2FLAGEXT_SSAO_HDAO);
#endif	

	CMD3(CCC_Mask,								"r2_ao",										&ps_r2_ls_flags,			R2FLAG_AMBIENT_OCCLUSION);
    CMD3(CCC_Token,                             "r2_ao_quality",                                &ps_r_ao_quality,			qao_quality_token);            
    CMD3(CCC_Mask,                              "r2_ssr",                              			&ps_r2_ls_flags,            R2FLAG_SSR);	
    CMD3(CCC_Mask,                              "r4_enable_tessellation",                       &ps_r2_ls_flags_ext,        R2FLAGEXT_ENABLE_TESSELLATION);      
    CMD3(CCC_Mask,                              "r4_wireframe",                                 &ps_r2_ls_flags_ext,        R2FLAGEXT_WIREFRAME);                
    CMD3(CCC_Mask,                              "r2_steep_parallax",                            &ps_r2_ls_flags,            R2FLAG_STEEP_PARALLAX);
    CMD3(CCC_Mask,                              "r2_detail_bump",                               &ps_r2_ls_flags,            R2FLAG_DETAIL_BUMP);
    CMD3(CCC_Mask,                              "r2_soft_water",                                &ps_r2_ls_flags,            R2FLAG_SOFT_WATER);
    CMD3(CCC_Mask,                              "r2_soft_particles",                            &ps_r2_ls_flags,            R2FLAG_SOFT_PARTICLES);
    CMD3(CCC_Token,                             "r2_sunshafts_mode",                            &ps_sunshafts_mode,         sunshafts_mode_token);
    CMD3(CCC_Token,                             "r2_sun_quality",                               &ps_r_sun_quality,          qsun_quality_token);
    CMD3(CCC_Token,                             "r2_smap_size",                                 &ps_r2_smap_size,           qsmap_size_token);
    CMD3(CCC_Token,                             "r3_msaa",                                      &ps_r3_msaa,                qmsaa_token);
    CMD3(CCC_Mask,                              "r3_use_dx10_1",                                &ps_r2_ls_flags,            (u32)R3FLAG_USE_DX10_1);
    CMD3(CCC_Token,                             "r3_msaa_alphatest",                            &ps_r3_msaa_atest,          qmsaa__atest_token);
    CMD3(CCC_Token,                             "r3_minmax_sm",                                 &ps_r3_minmax_sm,           qminmax_sm_token);
    CMD3(CCC_Mask,                              "r3_dynamic_wet_surfaces",                      &ps_r2_ls_flags,            R3FLAG_DYN_WET_SURF);
    CMD3(CCC_Mask,                              "r3_volumetric_smoke",                          &ps_r2_ls_flags,            R3FLAG_VOLUMETRIC_SMOKE);
	CMD3(CCC_Mask,								"r2_sun_flares",								&ps_r2_ls_flags_ext,		R2FLAGEXT_SUN_FLARES);
	CMD3(CCC_Token,								"r2_bloom_type",								&ps_r_bloom_type,			qbloom_type_token);
	CMD3(CCC_Mask,								"r2_gloss_build_2218",							&ps_r2_ls_flags_ext,		R2FLAGEXT_GLOSS_BUILD_2218);
    CMD3(CCC_Mask,                              "r2_mblur_enable",                              &ps_r2_ls_flags,            R2FLAG_MBLUR);
    CMD3(CCC_Token,                             "r4_tess_quality",                              &optTessQuality_,           qtess_quality_token);
    CMD3(CCC_Token,                             "monitor_hertz_token",                          &ps_mHToken,                monitors_hertz_tokens);
    CMD3(CCC_Token,                             "render_type_aa",                               &ps_r_type_aa,              token_aa);

    CMD4(CCC_Integer,                           "mt_loading_textures",                          &ps_mt_loading_textures,    0, 1);
    CMD4(CCC_Integer,                           "rs_skeleton_update",                           &psSkeletonUpdate,          2, 128);
    CMD4(CCC_Float,                             "r__dtex_range",                                &r__dtex_range,             5, 175);
    CMD4(CCC_Float,                             "r__wallmark_ttl",                              &ps_r__WallmarkTTL,         1.0f, 10.f * 60.f);
    CMD4(CCC_Integer,                           "r__supersample",                               &ps_r__Supersample,         1, 8);

    {
        tw_min.set(-10000, -10000, 0);
        tw_max.set(10000, 10000, 10000);
        CMD4(CCC_Dof,                           "r2_dof",                                       &ps_r2_dof,                 tw_min, tw_max);
        CMD4(CCC_DofNear,                       "r2_dof_near",                                  &ps_r2_dof.x,               tw_min.x, tw_max.x);
        CMD4(CCC_DofFocus,                      "r2_dof_focus",                                 &ps_r2_dof.y,               tw_min.y, tw_max.y);
        CMD4(CCC_DofFar,                        "r2_dof_far",                                   &ps_r2_dof.z,               tw_min.z, tw_max.z);
    }
	
	CMD4(CCC_Float,                             "r__geometry_lod",                              &ps_r__LOD,                     0.1f, 3.f); 
	CMD4(CCC_Float,								"r__lod_sprite_dist_f",							&ps_r__geomLodSpriteDistF_,		0.1f, 2.0f);
	CMD4(CCC_Float,								"r__geom_quality_dist_f",						&ps_r__geomLodDistF_,			0.1f, 2.0f);
	CMD4(CCC_Float,								"r__geom_discard_dist_f",						&ps_r__geomDiscardDistF_,		0.1f, 2.0f);
	CMD4(CCC_Float,								"r__dtexture_dist_f",							&ps_r__geomDTextureDistF_,		0.1f, 2.0f);
	CMD4(CCC_Float,								"r__ntexture_dist_f",							&ps_r__geomNTextureDistF_,		0.1f, 2.0f);

    CMD4(CCC_Float,                             "r2_steep_parallax_h",                          &ps_r2_steep_parallax_h,        .005f, 0.05f);
    CMD4(CCC_Float,                             "r2_steep_parallax_distance",                   &ps_r2_steep_parallax_distance, 4.0f, 100.0f);
    CMD4(CCC_Float,                             "r2_steep_parallax_samples",                    &ps_r2_steep_parallax_samples,  5.0f, 100.0f);
    CMD4(CCC_Float,                             "r2_steep_parallax_samples_min",                &ps_r2_steep_parallax_samples_min, 5.0f, 100.0f);

    CMD4(CCC_Float,                             "r__detail_density",                            &ps_current_detail_density, 0.3f, 1.0f);                          //AVO: extended from 0.2f to 0.04f 
	CMD4(CCC_detail_radius,                     "r__detail_radius",                             &ps_r__detail_radius,       50, 200); 
    CMD4(CCC_Float,                             "r__detail_scale",                              &ps_current_detail_scale,   0.3f, 1.2f);  




    CMD4(CCC_Float,                             "r1_pps_u",                                     &ps_r1_pps_u,               -1.f, +1.f);
    CMD4(CCC_Float,                             "r1_pps_v",                                     &ps_r1_pps_v,               -1.f, +1.f);


	CMD4(CCC_Float,								"r2_particle_distance",							&ps_r2_particle_distance,	35.f, 1000.f);

    // Software Skinning
    // 0 - disabled (renderer can override)
    // 1 - enabled
    // 2 - forced hardware skinning (renderer can not override)
    CMD4(CCC_Integer,                           "r1_software_skinning",                         &ps_r1_SoftwareSkinning,    0, 2);

    CMD4(CCC_Float,                             "r2_ssa_lod_a",                                 &ps_r2_ssaLOD_A,            16, 96);
    CMD4(CCC_Float,                             "r2_ssa_lod_b",                                 &ps_r2_ssaLOD_B,            32, 64);	
	CMD4(CCC_Float,                             "r2_rain_drops_diff",                           &ps_r2_rain_drops_diff,     0.f, 3.f);
	CMD4(CCC_Float,                             "render_gamma",                           		&render_gamma,     			0.f, 3.f);
	CMD4(CCC_Float,                             "r2_gasmask",                                   &ps_r2_gasmask,             0.f, 1.1f);
    CMD4(CCC_Float,                             "r2_tonemap_middlegray",                        &ps_r2_tonemap_middlegray,  0.0f, 2.0f);
    CMD4(CCC_Float,                             "r2_tonemap_adaptation",                        &ps_r2_tonemap_adaptation,  0.01f, 10.0f);
    CMD4(CCC_Float,                             "r2_tonemap_lowlum",                            &ps_r2_tonemap_low_lum,     0.0001f, 1.0f);
    CMD4(CCC_Float,                             "r2_tonemap_amount",                            &ps_r2_tonemap_amount,      0.0000f, 1.0f);
    CMD4(CCC_Float,                             "r2_ls_bloom_kernel_scale",                     &ps_r2_ls_bloom_kernel_scale, 0.5f, 2.f);
    CMD4(CCC_Float,                             "r2_ls_bloom_kernel_g",                         &ps_r2_ls_bloom_kernel_g,   1.f, 7.f);
    CMD4(CCC_Float,                             "r2_ls_bloom_kernel_b",                         &ps_r2_ls_bloom_kernel_b,   0.01f, 1.f);
    CMD4(CCC_Float,                             "r2_ls_bloom_threshold",                        &ps_r2_ls_bloom_threshold,  0.f, 1.f);
    CMD4(CCC_Float,                             "r2_ls_bloom_speed",                            &ps_r2_ls_bloom_speed,      0.f, 100.f);
    CMD4(CCC_Float,                             "r2_ls_dsm_kernel",                             &ps_r2_ls_dsm_kernel,       .1f, 3.f);
    CMD4(CCC_Float,                             "r2_ls_psm_kernel",                             &ps_r2_ls_psm_kernel,       .1f, 3.f);
    CMD4(CCC_Float,                             "r2_ls_ssm_kernel",                             &ps_r2_ls_ssm_kernel,       .1f, 3.f);
    CMD4(CCC_Float,                             "r2_ls_squality",                               &ps_r2_ls_squality,         .5f, 1.f);
    CMD4(CCC_Float,                             "r2_zfill_depth",                               &ps_r2_zfill,               .001f, .5f);
    CMD4(CCC_Float,                             "r2_gloss_factor",                              &ps_r2_gloss_factor,        .0f, 10.f);
    CMD4(CCC_Float,                             "r2_sun_tsm_proj",                              &ps_r2_sun_tsm_projection,  .001f, 0.8f);
    CMD4(CCC_Float,                             "r2_sun_tsm_bias",                              &ps_r2_sun_tsm_bias,        -0.5, +0.5);
    CMD4(CCC_Float,                             "r2_sun_near",                                  &ps_r2_sun_near,            1.f, 150.f);                          // AVO: extended from 50.f to 150.f

    CMD4(CCC_Float,                             "r2_sun_far",                                   &OLES_SUN_LIMIT_27_01_07,    51.f, 180.f);
    CMD4(CCC_Float,                             "r2_volum_intensity",                           &ps_volumetric_intensity,    -1.f, 1.f);
    CMD4(CCC_Float,                             "r2_volum_distance",                            &ps_volumetric_distance,     0.f, 2.f);
    CMD4(CCC_Float,                             "r2_volum_quality",                             &ps_volumetric_quality,      0.f, 1.f);

    CMD4(CCC_Float,                             "r2_sun_near_border",                           &ps_r2_sun_near_border,       .5f, 1.0f);
    CMD4(CCC_Float,                             "r2_sun_depth_far_scale",                       &ps_r2_sun_depth_far_scale,  -1.5, 1.5);
    CMD4(CCC_Float,                             "r2_sun_depth_far_bias",                        &ps_r2_sun_depth_far_bias,  -0.5, +0.5);
    CMD4(CCC_Float,                             "r2_sun_depth_near_scale",                      &ps_r2_sun_depth_near_scale, 0, 1.5);
    CMD4(CCC_Float,                             "r2_sun_depth_near_bias",                       &ps_r2_sun_depth_near_bias, -0.5, +0.5);
    CMD4(CCC_Float,                             "r2_sun_lumscale",                              &ps_r2_sun_lumscale,           0, +3.0);
    //LVutner 
    CMD4(CCC_Integer,                           "r_nv",                                         &render_nightvision,         0, 3);
    CMD4(CCC_Float,                             "r2_mblur",                                     &ps_r2_mblur,                0.15f, 1.0f);
    CMD4(CCC_Integer,                           "r2_wait_sleep",                                &ps_r2_wait_sleep,           0, 1);
	CMD4(CCC_Integer,							"r3_backbuffers_count",							&ps_r3_backbuffers_count,	 1,	3);
    CMD4(CCC_Float,                             "r2_ls_depth_scale",                            &ps_r2_ls_depth_scale,       0.5, 1.5);
    CMD4(CCC_Float,                             "r2_ls_depth_bias",                             &ps_r2_ls_depth_bias,       -0.5, +0.5);
    CMD4(CCC_Float,                             "r2_parallax_h",                                &ps_r2_df_parallax_h,       .0f, .5f);
    CMD4(CCC_Float,                             "r2_slight_fade",                               &ps_r2_slight_fade,         .2f, 1.f);
    CMD4(CCC_Float,                             "r2_dof_kernel",                                &ps_r2_dof_kernel_size,     .0f, 10.f);
	CMD4(CCC_Float,								"r__ssa_glod_start",							&ps_r__GLOD_ssa_start,		128,	512);
	CMD4(CCC_Float,								"r__ssa_glod_end",								&ps_r__GLOD_ssa_end,		16,		96);

	CMD4(CCC_Integer,							"r2_vignette",									&ps_r2_vignette,			 0, 1);
	CMD4(CCC_Integer,							"r2_volumetric_fog",							&ps_render_volumetric_fog,   0, 1);

	{
		tw_min.set(0, 0, 0);
		tw_max.set(2, 3, 1);
		CMD4(CCC_Vector3,                       "r2_drops_control",                             &ps_r2_drops_control,       tw_min, tw_max);
	}

    CMD4(CCC_Float,                             "r2_ss_sunshafts_length",                       &ps_r2_ss_sunshafts_length,   .2f, 1.5f);
    CMD4(CCC_Float,                             "r2_ss_sunshafts_radius",                       &ps_r2_ss_sunshafts_radius,   .5f, 2.f);
    CMD4(CCC_DetailsOpt,                        "r2_details_opt",                               &ps_r2_details_opt,           dopt_min, dopt_max);
    CMD4(CCC_Integer,                           "r2_gasmask_effect",                            &ps_r2_gasmask_effect_enable, 0, 1);
	CMD4(CCC_Integer,                           "r3_rain_4p",                                   &ps_r3_rain_4p,               0, 1);
	CMD4(CCC_Integer,                           "r2_rain_drops",                                &ps_r2_rain_drops,            0, 1);
	CMD4(CCC_Integer,                           "r2_rain_drops_diff",                           &ps_r2_rain_drops_effect,     0, 1);
    CMD4(CCC_Integer,                           "r__clear_models_on_unload",                    &ps_clear_models_on_unload,   0, 1); // Alundaio
    CMD4(CCC_Integer,                           "r__use_precompiled_shaders",                   &ps_use_precompiled_shaders,  0, 1); // Alundaio
    CMD4(CCC_Integer,                           "r__enable_grass_shadow",                       &ps_grass_shadow,             0, 1); // Alundaio
    CMD4(CCC_Integer,                           "r__no_scale_on_fade",                          &ps_no_scale_on_fade,         0, 1); // Alundaio
    CMD4(CCC_Integer,                           "r3_dynamic_wet_surfaces_opt",                  &ps_r3_dyn_wet_surf_opt,      0, 1);
    CMD4(CCC_Float,                             "r3_dynamic_wet_surfaces_near",                 &ps_r3_dyn_wet_surf_near,     5, 70);
    CMD4(CCC_Float,                             "r3_dynamic_wet_surfaces_far",                  &ps_r3_dyn_wet_surf_far,      30, 100);
    CMD4(CCC_Integer,                           "r3_dynamic_wet_surfaces_sm_res",               &ps_r3_dyn_wet_surf_sm_res,   64, 2048);
	
	//sweetfx
	CMD4(CCC_Integer,							"r3_technicolor",								&ps_r3_technicolor,			   0, 1);
	CMD4(CCC_Float,								"r3_vibrance",									&ps_r3_vibrance_val,		   .0f,	1.0f);

	CMD4(CCC_Integer,							"r3_hud_rain_drops",							&ps_r3_hud_rain_drops,		   0, 1);
	CMD4(CCC_Integer,							"r3_hud_visor_effect",							&ps_r3_hud_visor_effect,	   0, 1);

	CMD4(CCC_Integer,							"r3_adv_wet_hud",								&ps_r3_adv_wet_hud,			   0, 1);
	CMD4(CCC_Integer,							"r3_hud_visor_shadowing",						&ps_r3_hud_visor_shadowing,	   0, 1);
 
    //Шейдерный туман-с
    CMD4(CCC_Float,                             "fog_height",                                   &debug_f_h,                    0.f, 10.f);
    CMD4(CCC_Float,                             "fog_density",                                  &debug_f_d,                    0.f, 1.f);
    CMD4(CCC_Float,                             "fog_max_dist",                                 &debug_f_maxd,                 0.f, 100.f);
    CMD4(CCC_Float,                             "fog_min_dist",                                 &debug_f_mixd,                 0.f, 90.f);
}

