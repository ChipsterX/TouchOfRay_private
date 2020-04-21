#ifndef _TextureDescrManager_included_
#define _TextureDescrManager_included_

#pragma once
#include "ETextureParams.h"

class cl_dt_scaler;

#ifdef NEW_LOADING_TEXTURES
class cl_gloss_coef_and_offset : public R_constant_setup 
{
public:
    float				coef;
    float				offset;

    cl_gloss_coef_and_offset(float s, float x) : coef(s), offset(x) {};
    virtual void setup(R_constant* C)
    {
        RCache.set_c(C, coef, offset, 0, 0);
    }
};
#endif

class CTextureDescrMngr
{
    struct texture_assoc
    {
        shared_str detail_name;
        u8 usage;
#ifdef NEW_LOADING_TEXTURES
        u8 m_tesselation_method;
#endif
#ifndef NEW_LOADING_TEXTURES
        texture_assoc() : usage(0) {}
#else
        texture_assoc() : usage(0) { m_tesselation_method = 32; }
#endif
        ~texture_assoc() { }
    };
    struct texture_spec
    {
        shared_str m_bump_name;
        float m_material;
        bool m_use_steep_parallax;
#ifdef NEW_LOADING_TEXTURES
        cl_gloss_coef_and_offset* textureglossparams;
        texture_spec() { textureglossparams = NULL; }
        ~texture_spec() { xr_delete(textureglossparams); }
#endif
    };
    struct texture_desc
    {
        texture_assoc* m_assoc;
        texture_spec* m_spec;
        texture_desc() : m_assoc(nullptr), m_spec(nullptr) {}
    };

    using map_TD = xr_map<shared_str, texture_desc>;
    using map_CS = xr_map<shared_str, cl_dt_scaler*>;

    map_TD m_texture_details;
    map_CS m_detail_scalers;

#ifndef NEW_LOADING_TEXTURES
    void LoadTHM(LPCSTR initial);
#else
    void LoadLTX();
    void LoadTHM();
    void LoadMiniLTX();
    void CheckAndCreate_Assoc(texture_desc*& desc);
    void CheckAndCreate_Spec(texture_desc*& desc);

#endif

public:
    ~CTextureDescrMngr();
    void Load();
    void UnLoad();

public:
    shared_str GetBumpName(const shared_str& tex_name) const;
    float GetMaterial(const shared_str& tex_name) const;
    void GetTextureUsage(const shared_str& tex_name, bool& bDiffuse, bool& bBump) const;
    BOOL GetDetailTexture(const shared_str& tex_name, LPCSTR& res, R_constant_setup*& CS) const;
    BOOL UseSteepParallax(const shared_str& tex_name) const;
#ifdef NEW_LOADING_TEXTURES
    u8 TessMethod(const shared_str& tex_name) const;
    void GetGlossParams(const shared_str& tex_name, R_constant_setup*& GlossParams) const;
#endif
};
#endif
