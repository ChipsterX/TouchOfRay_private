#include "stdafx.h"
#include "Environment.h"
#include "xr_efflensflare.h"
#include "thunderbolt.h"
#include "Rain.h"
#include "IGame_Level.h"
#include "Common/object_broker.h"
#include "Common/LevelGameDef.h"

void CEnvModifier::load(IReader* fs, u32 version)
{
    use_flags.one();
    fs->r_fvector3(position);
    radius = fs->r_float();
    power = fs->r_float();
    far_plane = fs->r_float();
    fs->r_fvector3(fog_color);
    fog_density = fs->r_float();
    fs->r_fvector3(ambient);
    fs->r_fvector3(sky_color);
    fs->r_fvector3(hemi_color);

    if (version >= 0x0016)
    {
        use_flags.assign(fs->r_u16());
    }
}

#include "IGame_Persistent.h"

void CEnvDescriptor::EnvSwingValue::lerp(const EnvSwingValue& A, const EnvSwingValue& B, float f)
{
    float fi = 1.f - f;
    amp1 = fi * A.amp1 + f * B.amp1;
    amp2 = fi * A.amp2 + f * B.amp2;
    rot1 = fi * A.rot1 + f * B.rot1;
    rot2 = fi * A.rot2 + f * B.rot2;
    speed = fi * A.speed + f * B.speed;
}


float CEnvModifier::sum(CEnvModifier& M, Fvector3& view)
{
    float _dist_sq = view.distance_to_sqr(M.position);
    if (_dist_sq >= (M.radius * M.radius))
        return 0;

    float _att = 1 - _sqrt(_dist_sq) / M.radius; 
    float _power = M.power * _att;

    if (M.use_flags.test(eViewDist))
    {
        far_plane += M.far_plane * _power;
        use_flags.set(eViewDist, TRUE);
    }
    if (M.use_flags.test(eFogColor))
    {
        fog_color.mad(M.fog_color, _power);
        use_flags.set(eFogColor, TRUE);
    }
    if (M.use_flags.test(eFogDensity))
    {
        fog_density += M.fog_density * _power;
        use_flags.set(eFogDensity, TRUE);
    }

    if (M.use_flags.test(eAmbientColor))
    {
        ambient.mad(M.ambient, _power);
        use_flags.set(eAmbientColor, TRUE);
    }

    if (M.use_flags.test(eSkyColor))
    {
        sky_color.mad(M.sky_color, _power);
        use_flags.set(eSkyColor, TRUE);
    }

    if (M.use_flags.test(eHemiColor))
    {
        hemi_color.mad(M.hemi_color, _power);
        use_flags.set(eHemiColor, TRUE);
    }

    return _power;
}

//-----------------------------------------------------------------------------
// Environment ambient
//-----------------------------------------------------------------------------
void CEnvAmbient::SSndChannel::load(CInifile& config, LPCSTR sect)
{
    m_load_section = sect;

    m_sound_dist.x = config.r_float(m_load_section, "min_distance");
    m_sound_dist.y = config.r_float(m_load_section, "max_distance");
    m_sound_period.x = config.r_s32(m_load_section, "period0");
    m_sound_period.y = config.r_s32(m_load_section, "period1");
    m_sound_period.z = config.r_s32(m_load_section, "period2");
    m_sound_period.w = config.r_s32(m_load_section, "period3");

    R_ASSERT(m_sound_period.x <= m_sound_period.y && m_sound_period.z <= m_sound_period.w);
    R_ASSERT2(m_sound_dist.y > m_sound_dist.x, sect);

    LPCSTR snds = config.r_string(sect, "sounds");
    u32 cnt = _GetItemCount(snds);
    string_path tmp;
    R_ASSERT3(cnt, "sounds empty", sect);

    m_sounds.resize(cnt);

    for (u32 k = 0; k < cnt; ++k)
    {
        _GetItem(snds, k, tmp);
        m_sounds[k].create(tmp, st_Effect, sg_SourceType);
    }
}

CEnvAmbient::SEffect* CEnvAmbient::create_effect(CInifile& config, LPCSTR id)
{
    SEffect* result = new SEffect();
    result->life_time = iFloor(config.r_float(id, "life_time") * 1000.f);
    result->particles = config.r_string(id, "particles");
    VERIFY(result->particles.size());
    result->offset = config.r_fvector3(id, "offset");
    result->wind_gust_factor = config.r_float(id, "wind_gust_factor");

    if (config.line_exist(id, "sound"))
        result->sound.create(config.r_string(id, "sound"), st_Effect, sg_SourceType);

    if (config.line_exist(id, "wind_blast_strength"))
    {
        result->wind_blast_strength = config.r_float(id, "wind_blast_strength");
        result->wind_blast_direction.setHP(deg2rad(config.r_float(id, "wind_blast_longitude")), 0.f);
        result->wind_blast_in_time = config.r_float(id, "wind_blast_in_time");
        result->wind_blast_out_time = config.r_float(id, "wind_blast_out_time");
        return (result);
    }

    result->wind_blast_strength = 0.f;
    result->wind_blast_direction.set(0.f, 0.f, 1.f);
    result->wind_blast_in_time = 0.f;
    result->wind_blast_out_time = 0.f;

    return (result);
}

CEnvAmbient::SSndChannel* CEnvAmbient::create_sound_channel(CInifile& config, LPCSTR id)
{
    SSndChannel* result = new SSndChannel();
    result->load(config, id);
    return (result);
}

CEnvAmbient::~CEnvAmbient() { destroy(); }
void CEnvAmbient::destroy()
{
    delete_data(m_effects);
    delete_data(m_sound_channels);
}

void CEnvAmbient::load(
    CInifile& ambients_config, CInifile& sound_channels_config, CInifile& effects_config, const shared_str& sect)
{
    m_ambients_config_filename = ambients_config.fname();
    m_load_section = sect;
    string_path tmp;

    // sounds
    LPCSTR channels = ambients_config.r_string(sect, "sound_channels");
    u32 cnt = _GetItemCount(channels);
    m_sound_channels.resize(cnt);

    for (u32 i = 0; i < cnt; ++i)
        m_sound_channels[i] = create_sound_channel(sound_channels_config, _GetItem(channels, i, tmp));

    // effects
    m_effect_period.set(iFloor(ambients_config.r_float(sect, "min_effect_period") * 1000.f),
        iFloor(ambients_config.r_float(sect, "max_effect_period") * 1000.f));
    LPCSTR effs = ambients_config.r_string(sect, "effects");
    cnt = _GetItemCount(effs);

    m_effects.resize(cnt);
    for (u32 k = 0; k < cnt; ++k)
        m_effects[k] = create_effect(effects_config, _GetItem(effs, k, tmp));

    R_ASSERT(!m_sound_channels.empty() || !m_effects.empty());
}

//-----------------------------------------------------------------------------
// Environment descriptor
//-----------------------------------------------------------------------------
CEnvDescriptor::CEnvDescriptor(shared_str const& identifier) : m_identifier(identifier)
{
    exec_time = 0.0f;
    exec_time_loaded = 0.0f;

    clouds_color.set(1, 1, 1, 1);
    sky_color.set(1, 1, 1);
    sky_rotation = 0.0f;

    far_plane = 400.0f;
    ;

    fog_color.set(1, 1, 1);
    fog_density = 0.0f;
    fog_distance = 400.0f;

    rain_density = 0.0f;
    rain_color.set(0, 0, 0);

    bolt_period = 0.0f;
    bolt_duration = 0.0f;

    wind_velocity = 0.0f;
    wind_direction = 0.0f;
    clouds_velocity_0 = 0.0f;
    clouds_velocity_1 = 0.0f;
    wind_volume = 0.0f;

    ambient.set(0, 0, 0);
    hemi_color.set(1, 1, 1, 1);
    sun_color.set(1, 1, 1);
    sun_dir.set(0, -1, 0);

    m_fSunShaftsIntensity = 0;
    m_fWaterIntensity = 1;

    m_fTreeAmplitudeIntensity = 0.01f;
	m_fEffecrDrops = 0.01f;
//	m_fEffecrVignette = 0.01f;

    dof_value.set(-1.25f, 1.4f, 600.f);
    dof_kernel = 5.0f;
    dof_sky = 30.0f;

	m_fFogShaders_height = 0;   
	m_fFogShaders_density = 0;
	m_fFogShaders_max_dist = 0;
	m_fFogShaders_min_dist = 0;

    lens_flare_id = "";
    tb_id = "";

    env_ambient = NULL;
}

#define C_CHECK(C)                                                           \
    if (C.x < 0 || C.x > 2 || C.y < 0 || C.y > 2 || C.z < 0 || C.z > 2)      \
    {                                                                        \
        Msg("! Invalid '%s' in env-section '%s'", #C, m_identifier.c_str()); \
    }

void CEnvDescriptor::load(CEnvironment& environment, CInifile& config)
{
    Ivector3 tm = {0, 0, 0};
    sscanf(m_identifier.c_str(), "%d:%d:%d", &tm.x, &tm.y, &tm.z);
    R_ASSERT3((tm.x >= 0) && (tm.x < 24) && (tm.y >= 0) && (tm.y < 60) && (tm.z >= 0) && (tm.z < 60),
        "Incorrect weather time", m_identifier.c_str());
    exec_time = tm.x * 3600.f + tm.y * 60.f + tm.z;
    exec_time_loaded = exec_time;
    string_path st, st_env;
    xr_strcpy(st, config.r_string(m_identifier.c_str(), "sky_texture"));
    strconcat(sizeof(st_env), st_env, st, "#small");
    sky_texture_name = st;
    sky_texture_env_name = st_env;
    clouds_texture_name = config.r_string(m_identifier.c_str(), "clouds_texture");
    clouds_color = config.r_fvector4(m_identifier.c_str(), "clouds_color");

    sky_color = config.r_fvector3(m_identifier.c_str(), "sky_color");

    if (config.line_exist(m_identifier.c_str(), "sky_rotation"))
        sky_rotation = deg2rad(config.r_float(m_identifier.c_str(), "sky_rotation"));
    else
        sky_rotation = 0;
    far_plane = config.r_float(m_identifier.c_str(), "far");
    fog_color = config.r_fvector3(m_identifier.c_str(), "fog_color");
    fog_density = config.r_float(m_identifier.c_str(), "fog_density");
    fog_distance = config.r_float(m_identifier.c_str(), "fog_distance");
    rain_density = config.r_float(m_identifier.c_str(), "rain_density");
    clamp(rain_density, 0.f, 1.f);
    rain_color = config.r_fvector3(m_identifier.c_str(), "rain_color");
    wind_velocity = config.r_float(m_identifier.c_str(), "wind_velocity");
    wind_direction = deg2rad(config.r_float(m_identifier.c_str(), "wind_direction"));

    wind_volume = config.line_exist(m_identifier.c_str(), "wind_sound_volume") ? config.r_float(m_identifier.c_str(), "wind_sound_volume") : 0.0f;
    clouds_velocity_0 = config.line_exist(m_identifier.c_str(), "clouds_velocity_0") ? config.r_float(m_identifier.c_str(), "clouds_velocity_0") : 0.1f;
    clouds_velocity_1 = config.line_exist(m_identifier.c_str(), "clouds_velocity_1") ? config.r_float(m_identifier.c_str(), "clouds_velocity_1") : 0.05f;

    ambient = config.r_fvector3(m_identifier.c_str(), "ambient");
    hemi_color = config.r_fvector4(m_identifier.c_str(), "hemi_color");
    sun_color = config.r_fvector3(m_identifier.c_str(), "source_color");
    sun_dir.setHP(deg2rad(config.r_float(m_identifier.c_str(), "source_altitude")),
        deg2rad(config.r_float(m_identifier.c_str(), "source_longitude")));
    R_ASSERT(_valid(sun_dir));

    lens_flare_id = environment.eff_LensFlare->AppendDef(
        environment, environment.m_suns_config, config.r_string(m_identifier.c_str(), "source"));
    tb_id = environment.eff_Thunderbolt->AppendDef(environment, environment.m_thunderbolt_collections_config,
        environment.m_thunderbolts_config, config.r_string(m_identifier.c_str(), "thunderbolt_collection"));
    bolt_period = (tb_id.size()) ? config.r_float(m_identifier.c_str(), "thunderbolt_period") : 0.f;
    bolt_duration = (tb_id.size()) ? config.r_float(m_identifier.c_str(), "thunderbolt_duration") : 0.f;
    env_ambient = config.line_exist(m_identifier.c_str(), "env_ambient") ?
        environment.AppendEnvAmb(config.r_string(m_identifier.c_str(), "env_ambient")) :
        0;

    if (config.line_exist(m_identifier.c_str(), "sun_shafts_intensity"))
        m_fSunShaftsIntensity = config.r_float(m_identifier.c_str(), "sun_shafts_intensity");

    if (config.line_exist(m_identifier.c_str(), "water_intensity"))
        m_fWaterIntensity = config.r_float(m_identifier.c_str(), "water_intensity");

	if (config.line_exist(m_identifier.c_str(), "FogS_height"))
        m_fFogShaders_height = config.r_float(m_identifier.c_str(), "FogS_height");
	
	if (config.line_exist(m_identifier.c_str(), "FogS_density"))
        m_fFogShaders_density = config.r_float(m_identifier.c_str(), "FogS_density");
	
	if (config.line_exist(m_identifier.c_str(), "FogS_max_dist"))
        m_fFogShaders_max_dist = config.r_float(m_identifier.c_str(), "FogS_max_dist");
	
	if (config.line_exist(m_identifier.c_str(), "FogS_min_dist"))
        m_fFogShaders_min_dist = config.r_float(m_identifier.c_str(), "FogS_min_dist");

    if (config.line_exist(m_identifier.c_str(), "tree_amplitude"))
        m_fTreeAmplitudeIntensity = config.r_float(m_identifier.c_str(), "tree_amplitude");
	
	if (config.line_exist(m_identifier.c_str(), "effect_drops"))
        m_fEffecrDrops = config.r_float(m_identifier.c_str(), "effect_drops");
	
//	if (config.line_exist(m_identifier.c_str(), "effect_vignette"))
//        m_fEffecrVignette = config.r_float(m_identifier.c_str(), "effect_vignette");

    sun_lumscale = config.line_exist(m_identifier.c_str(), "sun_lumscale") ? config.r_float(m_identifier.c_str(), "sun_lumscale") : 1.f;
    dof_value = config.line_exist(m_identifier.c_str(), "dof") ? config.r_fvector3(m_identifier.c_str(), "dof") : Fvector3().set(-1.25f, 1.4f, 600.f);
    dof_kernel = config.line_exist(m_identifier.c_str(), "dof_kernel") ? config.r_float(m_identifier.c_str(), "dof_kernel") : 7.0f;
    dof_sky = config.line_exist(m_identifier.c_str(), "dof_sky") ? config.r_float(m_identifier.c_str(), "dof_sky") : 30.0f;

    // swing desc
    // normal
    m_cSwingDesc[0].amp1 = config.line_exist(m_identifier.c_str(), "swing_normal_amp1") ? config.r_float(m_identifier.c_str(), "swing_normal_amp1") : pSettings->r_float("details", "swing_normal_amp1");
    m_cSwingDesc[0].amp2 = config.line_exist(m_identifier.c_str(), "swing_normal_amp2") ? config.r_float(m_identifier.c_str(), "swing_normal_amp2") : pSettings->r_float("details", "swing_normal_amp2");
    m_cSwingDesc[0].rot1 = config.line_exist(m_identifier.c_str(), "swing_normal_rot1") ? config.r_float(m_identifier.c_str(), "swing_normal_rot1") : pSettings->r_float("details", "swing_normal_rot1");
    m_cSwingDesc[0].rot2 = config.line_exist(m_identifier.c_str(), "swing_normal_rot2") ? config.r_float(m_identifier.c_str(), "swing_normal_rot2") : pSettings->r_float("details", "swing_normal_rot2");
    m_cSwingDesc[0].speed = config.line_exist(m_identifier.c_str(), "swing_normal_speed") ? config.r_float(m_identifier.c_str(), "swing_normal_speed") : pSettings->r_float("details", "swing_normal_speed");
    // fast
    m_cSwingDesc[1].amp1 = config.line_exist(m_identifier.c_str(), "swing_fast_amp1") ? config.r_float(m_identifier.c_str(), "swing_fast_amp1") : pSettings->r_float("details", "swing_fast_amp1");
    m_cSwingDesc[1].amp2 = config.line_exist(m_identifier.c_str(), "swing_fast_amp2") ? config.r_float(m_identifier.c_str(), "swing_fast_amp2") : pSettings->r_float("details", "swing_fast_amp2");
    m_cSwingDesc[1].rot1 = config.line_exist(m_identifier.c_str(), "swing_fast_rot1") ? config.r_float(m_identifier.c_str(), "swing_fast_rot1") : pSettings->r_float("details", "swing_fast_rot1");
    m_cSwingDesc[1].rot2 = config.line_exist(m_identifier.c_str(), "swing_fast_rot2") ? config.r_float(m_identifier.c_str(), "swing_fast_rot2") : pSettings->r_float("details", "swing_fast_rot2");
    m_cSwingDesc[1].speed = config.line_exist(m_identifier.c_str(), "swing_fast_speed") ? config.r_float(m_identifier.c_str(), "swing_fast_speed") : pSettings->r_float("details", "swing_fast_speed");


    C_CHECK(clouds_color);
    C_CHECK(sky_color);
    C_CHECK(fog_color);
    C_CHECK(rain_color);
    C_CHECK(ambient);
    C_CHECK(hemi_color);
    C_CHECK(sun_color);
    on_device_create();
}

void CEnvDescriptor::on_device_create()
{
    m_pDescriptor->OnDeviceCreate(*this);
}

void CEnvDescriptor::on_device_destroy()
{
    m_pDescriptor->OnDeviceDestroy();
}

//-----------------------------------------------------------------------------
// Environment Mixer
//-----------------------------------------------------------------------------
CEnvDescriptorMixer::CEnvDescriptorMixer(shared_str const& identifier) : CEnvDescriptor(identifier) {}
void CEnvDescriptorMixer::destroy()
{
    m_pDescriptorMixer->Destroy();

    // Reuse existing code
    on_device_destroy();
}

void CEnvDescriptorMixer::clear()
{
    m_pDescriptorMixer->Clear();
}

void CEnvDescriptorMixer::lerp(
    CEnvironment*, CEnvDescriptor& A, CEnvDescriptor& B, float f, CEnvModifier& Mdf, float modifier_power)
{
    float modif_power = 1.f / (modifier_power + 1); // the environment itself
    float fi = 1 - f;

    m_pDescriptorMixer->lerp(&*A.m_pDescriptor, &*B.m_pDescriptor);
 
    weight = f;

    clouds_color.lerp(A.clouds_color, B.clouds_color, f);

    sky_rotation = (fi * A.sky_rotation + f * B.sky_rotation);

    if (Mdf.use_flags.test(eViewDist))
        far_plane = (fi * A.far_plane + f * B.far_plane + Mdf.far_plane) * psVisDistance * modif_power;
    else
        far_plane = (fi * A.far_plane + f * B.far_plane) * psVisDistance;

    fog_color.lerp(A.fog_color, B.fog_color, f);
    if (Mdf.use_flags.test(eFogColor))
        fog_color.add(Mdf.fog_color).mul(modif_power);

    fog_density = (fi * A.fog_density + f * B.fog_density);
    if (Mdf.use_flags.test(eFogDensity))
    {
        fog_density += Mdf.fog_density;
        fog_density *= modif_power;
    }

    fog_distance = (fi * A.fog_distance + f * B.fog_distance);
    fog_near = (1.0f - fog_density) * 0.85f * fog_distance;
    fog_far = 0.99f * fog_distance;

    rain_density = fi * A.rain_density + f * B.rain_density;
    rain_color.lerp(A.rain_color, B.rain_color, f);
    bolt_period = fi * A.bolt_period + f * B.bolt_period;
    bolt_duration = fi * A.bolt_duration + f * B.bolt_duration;
    // wind
    wind_velocity = fi * A.wind_velocity + f * B.wind_velocity;
    wind_direction = fi * A.wind_direction + f * B.wind_direction;
    wind_volume = fi * A.wind_volume + f * B.wind_volume;

    clouds_velocity_0 = fi * A.clouds_velocity_0 + f * B.clouds_velocity_0;
    clouds_velocity_1 = fi * A.clouds_velocity_1 + f * B.clouds_velocity_1;

    m_fSunShaftsIntensity = fi * A.m_fSunShaftsIntensity + f * B.m_fSunShaftsIntensity;

    m_fWaterIntensity = fi * A.m_fWaterIntensity + f * B.m_fWaterIntensity;
	
	m_fFogShaders_height = fi * A.m_fFogShaders_height + f * B.m_fFogShaders_height;
	m_fFogShaders_density = fi * A.m_fFogShaders_density + f * B.m_fFogShaders_density;
	m_fFogShaders_max_dist = fi * A.m_fFogShaders_max_dist + f * B.m_fFogShaders_max_dist;
	m_fFogShaders_min_dist = fi * A.m_fFogShaders_min_dist + f * B.m_fFogShaders_min_dist;

    m_fTreeAmplitudeIntensity = fi * A.m_fTreeAmplitudeIntensity + f * B.m_fTreeAmplitudeIntensity;
	
	m_fEffecrDrops = fi * A.m_fEffecrDrops + f * B.m_fEffecrDrops;
	
//	m_fEffecrVignette = fi * A.m_fEffecrVignette + f * B.m_fEffecrVignette;

    // colors
    sky_color.lerp(A.sky_color, B.sky_color, f);
    if (Mdf.use_flags.test(eSkyColor))
        sky_color.add(Mdf.sky_color).mul(modif_power);

    ambient.lerp(A.ambient, B.ambient, f);
    if (Mdf.use_flags.test(eAmbientColor))
        ambient.add(Mdf.ambient).mul(modif_power);

    hemi_color.lerp(A.hemi_color, B.hemi_color, f);

    sun_lumscale = fi * A.sun_lumscale + f * B.sun_lumscale;
    dof_value.lerp(A.dof_value, B.dof_value, f);
    dof_kernel = fi * A.dof_kernel + f * B.dof_kernel;
    dof_sky = fi * A.dof_sky + f * B.dof_sky;


    m_cSwingDesc[0].lerp(A.m_cSwingDesc[0], B.m_cSwingDesc[0], f);
    m_cSwingDesc[1].lerp(A.m_cSwingDesc[1], B.m_cSwingDesc[1], f);


    if (Mdf.use_flags.test(eHemiColor))
    {
        hemi_color.x += Mdf.hemi_color.x;
        hemi_color.y += Mdf.hemi_color.y;
        hemi_color.z += Mdf.hemi_color.z;
        hemi_color.x *= modif_power;
        hemi_color.y *= modif_power;
        hemi_color.z *= modif_power;
    }

    sun_color.lerp(A.sun_color, B.sun_color, f);

    R_ASSERT(_valid(A.sun_dir));
    R_ASSERT(_valid(B.sun_dir));
    sun_dir.lerp(A.sun_dir, B.sun_dir, f).normalize();
    R_ASSERT(_valid(sun_dir));

}

//-----------------------------------------------------------------------------
// Environment IO
//-----------------------------------------------------------------------------
CEnvAmbient* CEnvironment::AppendEnvAmb(const shared_str& sect)
{
    for (auto it = Ambients.begin(); it != Ambients.end(); it++)
        if ((*it)->name().equal(sect))
            return (*it);

    Ambients.push_back(new CEnvAmbient());
    Ambients.back()->load(*m_ambients_config, *m_sound_channels_config, *m_effects_config, sect);
    return (Ambients.back());
}

void CEnvironment::mods_load()
{
    Modifiers.clear();
    string_path path;
    if (FS.exist(path, "$level$", "level.env_mod"))
    {
        IReader* fs = FS.r_open(path);
        u32 id = 0;
        u32 ver = 0x0015;
        u32 sz;

        while (0 != (sz = fs->find_chunk(id)))
        {
            if (id == 0 && sz == sizeof(u32))
            {
                ver = fs->r_u32();
            }
            else
            {
                CEnvModifier E;
                E.load(fs, ver);
                Modifiers.push_back(E);
            }
            id++;
        }
        FS.r_close(fs);
    }
}

void CEnvironment::mods_unload() { Modifiers.clear(); }

CEnvDescriptor* CEnvironment::create_descriptor(shared_str const& identifier, CInifile* config)
{
    CEnvDescriptor* result = new CEnvDescriptor(identifier);
    if (config)
        result->load(*this, *config);
    return (result);
}

void CEnvironment::load_weathers()
{
    if (!WeatherCycles.empty())
        return;

    typedef xr_vector<LPSTR> file_list_type;
    file_list_type* file_list = FS.file_list_open("$game_weathers$", "");
    VERIFY(file_list);
    xr_string id;
    file_list_type::const_iterator i = file_list->begin();
    file_list_type::const_iterator e = file_list->end();
    for (; i != e; ++i)
    {
        u32 length = xr_strlen(*i);
        VERIFY(length >= 4);
        VERIFY((*i)[length - 4] == '.');
        VERIFY((*i)[length - 3] == 'l');
        VERIFY((*i)[length - 2] == 't');
        VERIFY((*i)[length - 1] == 'x');
        id.assign(*i, length - 4);
        EnvVec& env = WeatherCycles[id.c_str()];

        string_path file_name;
        FS.update_path(file_name, "$game_weathers$", id.c_str());
        xr_strcat(file_name, ".ltx");
        CInifile* config = CInifile::Create(file_name);

        typedef CInifile::Root sections_type;
        sections_type& sections = config->sections();

        env.reserve(sections.size());

        sections_type::const_iterator i2 = sections.begin();
        sections_type::const_iterator e2 = sections.end();
        for (; i2 != e2; ++i2)
        {
            CEnvDescriptor* object = create_descriptor((*i2)->Name, config);
            env.push_back(object);
        }

        CInifile::Destroy(config);
    }

    FS.file_list_close(file_list);

    // sorting weather envs
    auto _I = WeatherCycles.begin();
    auto _E = WeatherCycles.end();
    for (; _I != _E; _I++)
    {
        R_ASSERT3(_I->second.size() > 1, "Environment in weather must >=2", *_I->first);
        std::sort(_I->second.begin(), _I->second.end(), sort_env_etl_pred);
    }
    R_ASSERT2(!WeatherCycles.empty(), "Empty weathers.");
    SetWeather((*WeatherCycles.begin()).first.c_str());
}

void CEnvironment::load_weather_effects()
{
    if (!WeatherFXs.empty())
        return;

    typedef xr_vector<LPSTR> file_list_type;
    file_list_type* file_list = FS.file_list_open("$game_weather_effects$", "");
    VERIFY(file_list);
    xr_string id;
    file_list_type::const_iterator i = file_list->begin();
    file_list_type::const_iterator e = file_list->end();
    for (; i != e; ++i)
    {
        u32 length = xr_strlen(*i);
        VERIFY(length >= 4);
        VERIFY((*i)[length - 4] == '.');
        VERIFY((*i)[length - 3] == 'l');
        VERIFY((*i)[length - 2] == 't');
        VERIFY((*i)[length - 1] == 'x');
        id.assign(*i, length - 4);
        EnvVec& env = WeatherFXs[id.c_str()];

        string_path file_name;
        FS.update_path(file_name, "$game_weather_effects$", id.c_str());
        xr_strcat(file_name, ".ltx");
        CInifile* config = CInifile::Create(file_name);

        typedef CInifile::Root sections_type;
        sections_type& sections = config->sections();

        env.reserve(sections.size() + 2);
        env.push_back(create_descriptor("00:00:00", false));

        sections_type::const_iterator i2 = sections.begin();
        sections_type::const_iterator e2 = sections.end();
        for (; i2 != e2; ++i2)
        {
            CEnvDescriptor* object = create_descriptor((*i2)->Name, config);
            env.push_back(object);
        }

        CInifile::Destroy(config);

        env.push_back(create_descriptor("24:00:00", false));
        env.back()->exec_time_loaded = DAY_LENGTH;
    }

    FS.file_list_close(file_list);

    // sorting weather envs
    auto _I = WeatherFXs.begin();
    auto _E = WeatherFXs.end();
    for (; _I != _E; _I++)
    {
        R_ASSERT3(_I->second.size() > 1, "Environment in weather must >=2", *_I->first);
        std::sort(_I->second.begin(), _I->second.end(), sort_env_etl_pred);
    }
}

void CEnvironment::load()
{
    if (!CurrentEnv)
        create_mixer();

    m_pRender->OnLoad();
    if (!eff_Rain)
        eff_Rain = new CEffect_Rain();
    if (!eff_LensFlare)
        eff_LensFlare = new CLensFlare();
    if (!eff_Thunderbolt)
        eff_Thunderbolt = new CEffect_Thunderbolt();

    load_weathers();
    load_weather_effects();
}

void CEnvironment::unload()
{
    // clear weathers
    auto _I = WeatherCycles.begin();
    auto _E = WeatherCycles.end();
    for (; _I != _E; _I++)
    {
        for (auto it = _I->second.begin(); it != _I->second.end(); it++)
            xr_delete(*it);
    }

    WeatherCycles.clear();
    // clear weather effect
    _I = WeatherFXs.begin();
    _E = WeatherFXs.end();
    for (; _I != _E; _I++)
    {
        for (auto it = _I->second.begin(); it != _I->second.end(); it++)
            xr_delete(*it);
    }
    WeatherFXs.clear();
    // clear ambient
    for (auto it = Ambients.begin(); it != Ambients.end(); it++)
        xr_delete(*it);
    Ambients.clear();
    // misc
    xr_delete(eff_Rain);
    xr_delete(eff_LensFlare);
    xr_delete(eff_Thunderbolt);
    CurrentWeather = 0;
    CurrentWeatherName = 0;
    CurrentEnv->clear();
    Invalidate();

    m_pRender->OnUnload();
}
