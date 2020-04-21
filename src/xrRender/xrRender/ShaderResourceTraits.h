#pragma once

#include "ResourceManager.h"

template <typename T>
struct ShaderTypeTraits;

template <>
struct ShaderTypeTraits<SVS>
{
    typedef CResourceManager::map_VS MapType;
    using HWShaderType = ID3DVertexShader*;

    static inline const char* GetShaderExt() { return "vs_"; }
    static inline const char* GetCompilationTarget()
    {
        return "vs_2_0";
    }

    static void GetCompilationTarget(const char*& target, const char*& entry, const char* data)
    {
        target = "vs_2_0";

        if (strstr(data, "main_vs_1_1"))
        {
            target = "vs_1_1";
            entry = "main_vs_1_1";
        }

        if (strstr(data, "main_vs_2_0"))
        {
            target = "vs_2_0";
            entry = "main_vs_2_0";
        }
    }

    static inline HRESULT CreateHWShader(DWORD const* buffer, size_t size, HWShaderType& sh)
    {
        HRESULT _res = 0;
#ifdef USE_DX11
        _res = HW.pDevice->CreateVertexShader(buffer, size, 0, &sh);
#elif defined(USE_DX10)
        _res = HW.pDevice->CreateVertexShader(buffer, size, &sh);
#else
        _res = HW.pDevice->CreateVertexShader(buffer, &sh);
#endif
        return _res;
    }

    static inline u32 GetShaderDest() { return RC_dest_vertex; }
};

template <>
struct ShaderTypeTraits<SPS>
{
    typedef CResourceManager::map_PS MapType;

    using HWShaderType = ID3DPixelShader*;

    static inline const char* GetShaderExt() { return "ps_"; }
    static inline const char* GetCompilationTarget()
    {
#if defined(USE_DX10) || defined(USE_DX11)
        return "ps_4_0";
#else
        return "ps_3_0";
#endif
    }

    static void GetCompilationTarget(const char*& target, const char*& entry, const char* data)
    {
        if (strstr(data, "main_ps_1_1"))
        {
            target = "ps_1_1";
            entry = "main_ps_1_1";
        }
        if (strstr(data, "main_ps_1_2"))
        {
            target = "ps_1_2";
            entry = "main_ps_1_2";
        }
        if (strstr(data, "main_ps_1_3"))
        {
            target = "ps_1_3";
            entry = "main_ps_1_3";
        }
        if (strstr(data, "main_ps_1_4"))
        {
            target = "ps_1_4";
            entry = "main_ps_1_4";
        }

        if (strstr(data, "main_ps_2_0"))
        {
            target = "ps_2_0";
            entry = "main_ps_2_0";
        }
    }

    static inline HRESULT CreateHWShader(DWORD const* buffer, size_t size, HWShaderType& sh)
    {
        HRESULT _res = 0;
#ifdef USE_DX11
        _res = HW.pDevice->CreatePixelShader(buffer, size, 0, &sh);
#elif defined(USE_DX10)
        _res = HW.pDevice->CreatePixelShader(buffer, size, &sh);
#else
        _res = HW.pDevice->CreatePixelShader(buffer, &sh);
#endif
        return _res;
    }

    static inline u32 GetShaderDest() { return RC_dest_pixel; }
};

#if defined(USE_DX10) || defined(USE_DX11)
template <>
struct ShaderTypeTraits<SGS>
{
    typedef CResourceManager::map_GS MapType;

    using HWShaderType = ID3DGeometryShader*;

    static inline const char* GetShaderExt() { return "gs_"; }
    static inline const char* GetCompilationTarget() { return "gs_4_0"; }

    static void GetCompilationTarget(const char*& target, const char*& entry, const char*)
    {
        target = GetCompilationTarget();
        entry = "main";
    }

    static inline HRESULT CreateHWShader(DWORD const* buffer, size_t size, HWShaderType& sh)
    {
        HRESULT _res = 0;
#ifdef USE_DX11
        _res = HW.pDevice->CreateGeometryShader(buffer, size, 0, &sh);
#else
        _res = HW.pDevice->CreateGeometryShader(buffer, size, &sh);
#endif
        return _res;
    }

    static inline u32 GetShaderDest() { return RC_dest_geometry; }
};
#endif

#if defined(USE_DX11)
template <>
struct ShaderTypeTraits<SHS>
{
    typedef CResourceManager::map_HS MapType;
    using HWShaderType = ID3D11HullShader*;

    static inline const char* GetShaderExt() { return "hs_"; }
    static inline const char* GetCompilationTarget() { return "hs_5_0"; }

    static void GetCompilationTarget(const char*& target, const char*& entry, const char*)
    {
        target = GetCompilationTarget();
        entry = "main";
    }

    static inline HRESULT CreateHWShader(DWORD const* buffer, size_t size, HWShaderType& sh)
    {
        HRESULT _res = 0;
        _res = HW.pDevice->CreateHullShader(buffer, size, NULL, &sh);
        return _res;
    }

    static inline u32 GetShaderDest() { return RC_dest_hull; }
};

template <>
struct ShaderTypeTraits<SDS>
{
    typedef CResourceManager::map_DS MapType;
    using HWShaderType = ID3D11DomainShader*;

    static inline const char* GetShaderExt() { return "ds_"; }
    static inline const char* GetCompilationTarget() { return "ds_5_0"; }

    static void GetCompilationTarget(const char*& target, const char*& entry, const char*)
    {
        target = GetCompilationTarget();
        entry = "main";
    }

    static inline HRESULT CreateHWShader(DWORD const* buffer, size_t size, HWShaderType& sh)
    {
        HRESULT _res = 0;
        _res = HW.pDevice->CreateDomainShader(buffer, size, NULL, &sh);

        return _res;
    }

    static inline u32 GetShaderDest() { return RC_dest_domain; }
};

template <>
struct ShaderTypeTraits<SCS>
{
    typedef CResourceManager::map_CS MapType;
    using HWShaderType = ID3D11ComputeShader*;

    static inline const char* GetShaderExt() { return "cs_"; }
    static inline const char* GetCompilationTarget() { return "cs_5_0"; }

    static void GetCompilationTarget(const char*& target, const char*& entry, const char*)
    {
        target = GetCompilationTarget();
        entry = "main";
    }

    static inline HRESULT CreateHWShader(DWORD const* buffer, size_t size, HWShaderType& sh)
    {
        HRESULT _res = 0;
        _res = HW.pDevice->CreateComputeShader(buffer, size, NULL, &sh);

        return _res;
    }

    static inline u32 GetShaderDest() { return RC_dest_compute; }
};
#endif

template <>
inline CResourceManager::map_PS& CResourceManager::GetShaderMap()
{
    return m_ps;
}

template <>
inline CResourceManager::map_VS& CResourceManager::GetShaderMap()
{
    return m_vs;
}

#if defined(USE_DX10) || defined(USE_DX11)
template <>
inline CResourceManager::map_GS& CResourceManager::GetShaderMap()
{
    return m_gs;
}
#endif

#if defined(USE_DX11)
template <>
inline CResourceManager::map_DS& CResourceManager::GetShaderMap()
{
    return m_ds;
}

template <>
inline CResourceManager::map_HS& CResourceManager::GetShaderMap()
{
    return m_hs;
}

template <>
inline CResourceManager::map_CS& CResourceManager::GetShaderMap()
{
    return m_cs;
}
#endif

template <typename T>
inline T* CResourceManager::CreateShader(const char* name, const char* filename, const bool searchForEntryAndTarget)
{
    ShaderTypeTraits<T>::MapType& sh_map = GetShaderMap<ShaderTypeTraits<T>::MapType>();
    LPSTR N = LPSTR(name);
    auto iterator = sh_map.find(N);

    if (iterator != sh_map.end())
        return iterator->second;
    else
    {
        T* sh = new T();

        sh->dwFlags |= xr_resource_flagged::RF_REGISTERED;
        sh_map.insert(std::make_pair(sh->set_name(name), sh));
        if (0 == xr_stricmp(name, "null"))
        {
            sh->sh = NULL;
            return sh;
        }

        // Remove ( and everything after it
        string_path shName;
        {
            if (filename == nullptr)
                filename = name;

            pcstr pchr = strchr(filename, '(');
            ptrdiff_t size = pchr ? pchr - filename : xr_strlen(filename);
            strncpy(shName, filename, size);
            shName[size] = 0;
        }

        // Open file
        string_path cname;
        pcstr shaderExt = ShaderTypeTraits<T>::GetShaderExt();
        strconcat(sizeof(cname), cname, GEnv.Render->getShaderPath(), shaderExt, shName, ".hlsl");
        FS.update_path(cname, "$game_shaders$", cname);

        // Try to open
        IReader* file = FS.r_open(cname);

        R_ASSERT3(file, "Shader file doesnt exist:", cname);

        // Duplicate and zero-terminate
        const auto size = file->length();
        char* const data = (LPSTR)_alloca(size + 1);
        CopyMemory(data, file->pointer(), size);
        data[size] = 0;

        // Select target
        LPCSTR c_target = ShaderTypeTraits<T>::GetCompilationTarget();
        LPCSTR c_entry = "main";
        
        if (searchForEntryAndTarget)
            ShaderTypeTraits<T>::GetCompilationTarget(c_target, c_entry, data);

#if defined(USE_DX10) || defined(USE_DX11)
        DWORD flags = D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
#else
        DWORD flags = D3DXSHADER_DEBUG | D3DXSHADER_PACKMATRIX_ROWMAJOR;
#endif

        if (Core.ParamFlags.test(Core.verboselog))
            Msg("compiling shader %s", name);

        // Compile
        HRESULT const _hr = GEnv.Render->shader_compile(name, file, c_entry, c_target, flags, (void*&)sh);

        FS.r_close(file);

        VERIFY(SUCCEEDED(_hr));

        CHECK_OR_EXIT(!FAILED(_hr), "DirectX: Error in the shaders.\n\nWrite modifications to the developers in the VK group.");

        return sh;
    }
}

template <typename T>
bool CResourceManager::DestroyShader(const T* sh)
{
    if (0 == (sh->dwFlags & xr_resource_flagged::RF_REGISTERED))
        return false;

    ShaderTypeTraits<T>::MapType& sh_map = GetShaderMap<ShaderTypeTraits<T>::MapType>();

    LPSTR N = LPSTR(*sh->cName);
    auto iterator = sh_map.find(N);

    if (iterator != sh_map.end())
    {
        sh_map.erase(iterator);
        return true;
    }
    Msg("! ERROR: Failed to find compiled shader '%s'", sh->cName.c_str());
	return false;
}
