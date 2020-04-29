#include "stdafx.h"
#include "r2.h"
#include "xrRender/xrRender/ShaderResourceTraits.h"
#include "xrCore/FileCRC32.h"

template <typename T>
static HRESULT create_shader(LPCSTR const pTarget, DWORD const* buffer, u32 const buffer_size, LPCSTR const file_name,
    T*& result, bool const disasm)
{
    HRESULT _hr = ShaderTypeTraits<T>::CreateHWShader(buffer, buffer_size, result->sh);
    if (!SUCCEEDED(_hr))
    {
        Log("! Shader: ", file_name);
        Msg("! CreateHWShader hr == 0x%08x", _hr);
        return E_FAIL;
    }

    LPCVOID data = nullptr;
    _hr = D3DXFindShaderComment(buffer, MAKEFOURCC('C', 'T', 'A', 'B'), &data, nullptr);

    if (SUCCEEDED(_hr) && data)
    {
        // Parse constant table data
        LPD3DXSHADER_CONSTANTTABLE pConstants = LPD3DXSHADER_CONSTANTTABLE(data);
        result->constants.parse(pConstants, ShaderTypeTraits<T>::GetShaderDest());
    }
    else
        Msg("! D3DXFindShaderComment %s hr == 0x%08x", file_name, _hr);

    if (disasm)
    {
        ID3DBlob* disasm = nullptr;
        D3DDisassemble(buffer, buffer_size, FALSE, nullptr, &disasm);
        string_path dname;
        strconcat(sizeof(dname), dname, "disasm\\", file_name, ('v' == pTarget[0]) ? ".vs" : ".ps");
        IWriter* W = FS.w_open("$logs$", dname);
        W->w(disasm->GetBufferPointer(), (u32)disasm->GetBufferSize());
        FS.w_close(W);
        _RELEASE(disasm);
    }

    return _hr;
}

inline HRESULT create_shader(LPCSTR const pTarget, DWORD const* buffer, u32 const buffer_size, LPCSTR const file_name, void*& result, bool const disasm)
{
    if (pTarget[0] == 'p')
        return create_shader(pTarget, buffer, buffer_size, file_name, (SPS*&)result, disasm);
    else if (pTarget[0] == 'v')
        return create_shader(pTarget, buffer, buffer_size, file_name, (SVS*&)result, disasm);

    NODEFAULT;
    return E_FAIL;
}

class includer : public ID3DInclude
{
public:
    virtual HRESULT __stdcall Open(
        D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)

    {
        string_path pname;
        strconcat(sizeof(pname), pname, GEnv.Render->getShaderPath(), pFileName);
#ifdef USE_ENCRYPTED_SHADERS
        //ShaderEncryptedMap
        xr_strlwr(pname);
        if (char* ext = strext(pname))
        {
            if (xr_strcmp(ext, ".hlsl") == 0)
            {
                *ext = '\0';
            }
        }

        if (char* dotInStr = strchr(pname, '.'))
        {
            *dotInStr = '_';
        }

		if (char* dotInStr = strchr(pname, '\\'))
		{
			*dotInStr = '_';
		}


        shared_str targetName(pname);
        xr_map< shared_str, xr_vector<char> >::iterator shaderEntryIter = ShaderEncryptedMap.find(targetName);
        if (shaderEntryIter == ShaderEncryptedMap.end())
        {
            targetName = shared_str(pFileName);
            shaderEntryIter = ShaderEncryptedMap.find(targetName);
            if (shaderEntryIter == ShaderEncryptedMap.end())
            {
                return E_FAIL;
            }
        }
        xr_vector<char>& EncryptedShader = shaderEntryIter->second;

		u32 size = EncryptedShader.size();
		char* data = xr_alloc<char>(size + 1);

        DecryptShader(EncryptedShader, data);
        data[size] = '\0';

		*ppData = data;
		*pBytes = size;
#else
        IReader* R = FS.r_open("$game_shaders$", pname);
        if (nullptr == R)
        {
            // possibly in shared directory or somewhere else - open directly
            R = FS.r_open("$game_shaders$", pFileName);
            if (nullptr == R)
                return E_FAIL;
        }

        // duplicate and zero-terminate
        u32 size = R->length();
        u8* data = xr_alloc<u8>(size + 1);
        CopyMemory(data, R->pointer(), size);
        data[size] = 0;
        FS.r_close(R);

        *ppData = data;
        *pBytes = size;
#endif

        return D3D_OK;
    }
    HRESULT __stdcall Close(LPCVOID pData)
    {
        xr_free(pData);
        return D3D_OK;
    }
};

static inline bool match_shader_id(
    LPCSTR const debug_shader_id, LPCSTR const full_shader_id, FS_FileSet const& file_set, string_path& result);

HRESULT CRender::shader_compile(LPCSTR name, char* shader, size_t shaderSize, LPCSTR pFunctionName, LPCSTR pTarget, DWORD Flags,
    void*& result)
{
    D3D_SHADER_MACRO defines[128];

    int def_it = 0;
    char c_smapsize[32];
    char c_gloss[32];
    char c_sun_shafts[32];
    char c_sun_quality[32];
    char c_ao[32];
#ifndef DISABLE_SSAO_SYSTEM_R2	
	char c_ssao[32];
#endif
    char sh_name[MAX_PATH] = "";
    u32 len = 0;
    // options
    {
        xr_sprintf(c_smapsize, "%04d", u32(o.smapsize));
        defines[def_it].Name = "SMAP_size";
        defines[def_it].Definition = c_smapsize;
        def_it++;
        VERIFY(xr_strlen(c_smapsize) == 4);
        xr_strcat(sh_name, c_smapsize);
        len += 4;
    }

    if (o.fp16_filter)
    {
        defines[def_it].Name = "FP16_FILTER";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(o.fp16_filter);
    ++len;

    if (o.fp16_blend)
    {
        defines[def_it].Name = "FP16_BLEND";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(o.fp16_blend);
    ++len;

    if (o.HW_smap)
    {
        defines[def_it].Name = "USE_HWSMAP";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(o.HW_smap);
    ++len;

    if (o.HW_smap_PCF)
    {
        defines[def_it].Name = "USE_HWSMAP_PCF";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(o.HW_smap_PCF);
    ++len;

    if (o.HW_smap_FETCH4)
    {
        defines[def_it].Name = "USE_FETCH4";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(o.HW_smap_FETCH4);
    ++len;

    if (o.sjitter)
    {
        defines[def_it].Name = "USE_SJITTER";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(o.sjitter);
    ++len;

    if (HW.Caps.raster_major >= 3)
    {
        defines[def_it].Name = "USE_BRANCHING";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(HW.Caps.raster_major >= 3);
    ++len;
	
#ifndef DISABLE_SSAO_SYSTEM_R2	
	if (o.ssao_blur_on)
    {
        defines[def_it].Name = "USE_SSAO_BLUR";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(o.ssao_blur_on);
    ++len;

    if (o.ssao_hbao)
    {
        defines[def_it].Name = "USE_HBAO";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(o.ssao_hbao);
    ++len;

    if (o.ssao_opt_data)
    {
        defines[def_it].Name = "SSAO_OPT_DATA";
        if (o.ssao_half_data)
            defines[def_it].Definition = "2";
        else
            defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(o.ssao_opt_data ? (o.ssao_half_data ? 2 : 1) : 0);
    ++len;
	
	if (RImplementation.o.advancedpp && ps_r_ssao)
    {
        xr_sprintf(c_ssao, "%d", ps_r_ssao);
        defines[def_it].Name = "SSAO_QUALITY";
        defines[def_it].Definition = c_ssao;
        def_it++;
        sh_name[len] = '0' + char(ps_r_ssao);
        ++len;
    }
    else
    {
        sh_name[len] = '0';
        ++len;
    }
#endif	

    if (HW.Caps.geometry.bVTF)
    {
        defines[def_it].Name = "USE_VTF";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(HW.Caps.geometry.bVTF);
    ++len;

    if (o.Tshadows)
    {
        defines[def_it].Name = "USE_TSHADOWS";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(o.Tshadows);
    ++len;

    if (o.mblur)
    {
        defines[def_it].Name = "USE_MBLUR";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(o.mblur);
    ++len;

    if (o.sunfilter)
    {
        defines[def_it].Name = "USE_SUNFILTER";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(o.sunfilter);
    ++len;

    if (o.forcegloss)
    {
        xr_sprintf(c_gloss, "%f", o.forcegloss_v);
        defines[def_it].Name = "FORCE_GLOSS";
        defines[def_it].Definition = c_gloss;
        def_it++;
    }
    sh_name[len] = '0' + char(o.forcegloss);
    ++len;

    if (o.forceskinw)
    {
        defines[def_it].Name = "SKIN_COLOR";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(o.forceskinw);
    ++len;

    // skinning
    if (m_skinning < 0)
    {
        defines[def_it].Name = "SKIN_NONE";
        defines[def_it].Definition = "1";
        def_it++;
        sh_name[len] = '1';
        ++len;
    }
    else
    {
        sh_name[len] = '0';
        ++len;
    }

    if (0 == m_skinning)
    {
        defines[def_it].Name = "SKIN_0";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(0 == m_skinning);
    ++len;

    if (1 == m_skinning)
    {
        defines[def_it].Name = "SKIN_1";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(1 == m_skinning);
    ++len;

    if (2 == m_skinning)
    {
        defines[def_it].Name = "SKIN_2";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(2 == m_skinning);
    ++len;

    if (3 == m_skinning)
    {
        defines[def_it].Name = "SKIN_3";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(3 == m_skinning);
    ++len;

    if (4 == m_skinning)
    {
        defines[def_it].Name = "SKIN_4";
        defines[def_it].Definition = "1";
        def_it++;
    }
    sh_name[len] = '0' + char(4 == m_skinning);
    ++len;

    //	Igor: need restart options
    if (RImplementation.o.advancedpp && ps_r2_ls_flags.test(R2FLAG_SOFT_WATER))
    {
        defines[def_it].Name = "USE_SOFT_WATER";
        defines[def_it].Definition = "1";
        def_it++;
        sh_name[len] = '1';
        ++len;
    }
    else
    {
        sh_name[len] = '0';
        ++len;
    }

    if (RImplementation.o.advancedpp && ps_r2_ls_flags.test(R2FLAG_SOFT_PARTICLES))
    {
        defines[def_it].Name = "USE_SOFT_PARTICLES";
        defines[def_it].Definition = "1";
        def_it++;
        sh_name[len] = '1';
        ++len;
    }
    else
    {
        sh_name[len] = '0';
        ++len;
    }

    if (RImplementation.o.advancedpp && ps_r2_ls_flags.test(R2FLAG_DOF))
    {
        defines[def_it].Name = "USE_DOF";
        defines[def_it].Definition = "1";
        def_it++;
        sh_name[len] = '1';
        ++len;
    }
    else
    {
        sh_name[len] = '0';
        ++len;
    }

    if (RImplementation.o.advancedpp && ps_r_sun_shafts)
    {
        xr_sprintf(c_sun_shafts, "%d", ps_r_sun_shafts);
        defines[def_it].Name = "SUN_SHAFTS_QUALITY";
        defines[def_it].Definition = c_sun_shafts;
        def_it++;
        sh_name[len] = '0' + char(ps_r_sun_shafts);
        ++len;
    }
    else
    {
        sh_name[len] = '0';
        ++len;
    }

    if (RImplementation.o.advancedpp && ps_r_sun_quality)
    {
        xr_sprintf(c_sun_quality, "%d", ps_r_sun_quality);
        defines[def_it].Name = "SUN_QUALITY";
        defines[def_it].Definition = c_sun_quality;
        def_it++;
        sh_name[len] = '0' + char(ps_r_sun_quality);
        ++len;
    }
    else
    {
        sh_name[len] = '0';
        ++len;
    }

    if (RImplementation.o.advancedpp && ps_r2_ls_flags.test(R2FLAG_STEEP_PARALLAX))
    {
        defines[def_it].Name = "ALLOW_STEEPPARALLAX";
        defines[def_it].Definition = "1";
        def_it++;
        sh_name[len] = '1';
        ++len;
    }
    else
    {
        sh_name[len] = '0';
        ++len;
    }

    if (RImplementation.o.advancedpp && ps_r2_ls_flags.test(R2FLAG_AMBIENT_OCCLUSION)) {
		defines[def_it].Name = "USE_AO";
		defines[def_it].Definition = "1";
		def_it++;			
		
        sprintf_s(c_ao, "%d", ps_r_ao_quality);
        defines[def_it].Name = "AO_QUALITY";
        defines[def_it].Definition = c_ao;
        def_it++;
    }

    if (RImplementation.o.advancedpp && ps_r2_ls_flags.test(R2FLAG_SSR))
    {
        defines[def_it].Name = "USE_SSR";
        defines[def_it].Definition = "1";
        def_it++;
        sh_name[len] = '1';
        ++len;
    }
    else
    {
        sh_name[len] = '0';
        ++len;
    }

    sh_name[len] = '\0';

    // finish
    defines[def_it].Name = nullptr;
    defines[def_it].Definition = nullptr;
    def_it++;

    HRESULT _result = E_FAIL;

    char extension[3];
    string_path folder_name, folder;

    strncpy_s(extension, pTarget, 2);
    strconcat(sizeof(folder), folder, "DX9\\objects\\DX9\\", name, ".", extension);

    FS.update_path(folder_name, "$game_shaders$", folder);
    xr_strcat(folder_name, "\\");

    m_file_set.clear();
    FS.file_list(m_file_set, folder_name, FS_ListFiles | FS_RootOnly, "*");

    string_path temp_file_name, file_name;
    if (ps_use_precompiled_shaders == false || !match_shader_id(name, sh_name, m_file_set, temp_file_name))
    {
        string_path file;
        PATH_SHADERS_CACHE_DX9;
        FS.update_path(file_name, "$app_data_root$", file);
    }
    else
    {
        xr_strcpy(file_name, folder_name);
        xr_strcat(file_name, temp_file_name);
    }

    string_path shadersFolder;
    FS.update_path(shadersFolder, "$game_shaders$", GEnv.Render->getShaderPath());

    u32 fileCrc = crc32(shader, shaderSize);

    if (FS.exist(file_name))
    {
        IReader* file = FS.r_open(file_name);
        if (file->length() > 4)
        {
            u32 savedFileCrc = file->r_u32();
            if (savedFileCrc == fileCrc)
            {
                u32 savedBytecodeCrc = file->r_u32();
                u32 bytecodeCrc = crc32(file->pointer(), file->elapsed());
                if (bytecodeCrc == savedBytecodeCrc)
                    _result = create_shader(pTarget, (DWORD*)file->pointer(), file->elapsed(), file_name, result, o.disasm);
            }
        }
        file->close();
    }

    if (FAILED(_result))
    {
        if (0 == xr_strcmp(pFunctionName, "main"))
        {
            if ('v' == pTarget[0])
                pTarget = D3DXGetVertexShaderProfile(HW.pDevice); 
            else
                pTarget = D3DXGetPixelShaderProfile(HW.pDevice); 
        }

        includer Includer;

        LPD3DBLOB pShaderBuf = nullptr;
        LPD3DBLOB pErrorBuf = nullptr;

        LPD3DXCONSTANTTABLE pConstants = nullptr;
        LPD3DXINCLUDE pInclude = (LPD3DXINCLUDE)&Includer;
        DWORD compileFlags = Flags | D3DCOMPILE_OPTIMIZATION_LEVEL3;

        _result = D3DCompile(shader, shaderSize, "", defines, &Includer, pFunctionName, pTarget, compileFlags, 0,
            &pShaderBuf, &pErrorBuf);

        if (SUCCEEDED(_result))
        {
            IWriter* file = FS.w_open(file_name);

            file->w_u32(fileCrc);

            u32 crc = crc32(pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize());
            file->w_u32(crc);

            file->w(pShaderBuf->GetBufferPointer(), (u32)pShaderBuf->GetBufferSize());

            FS.w_close(file);

            _result = create_shader(pTarget, (DWORD*)pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(),
                                    file_name, result, o.disasm);
        }
        else
        {
            Log("! ", file_name);
            if (pErrorBuf)
                Log("! error: ", (LPCSTR)pErrorBuf->GetBufferPointer());
            else
                Msg("Can't compile shader hr=0x%08x", _result);
        }
    }

    return _result;
}

static inline bool match_shader(
    LPCSTR const debug_shader_id, LPCSTR const full_shader_id, LPCSTR const mask, size_t const mask_length)
{
    u32 const full_shader_id_length = xr_strlen(full_shader_id);
    R_ASSERT2(full_shader_id_length == mask_length,
        make_string("bad cache for shader %s, [%s], [%s]", debug_shader_id, mask, full_shader_id));
    char const* i = full_shader_id;
    char const* const e = full_shader_id + full_shader_id_length;
    char const* j = mask;
    for (; i != e; ++i, ++j)
    {
        if (*i == *j)
            continue;

        if (*j == '_')
            continue;

        return false;
    }

    return true;
}

static inline bool match_shader_id(
    LPCSTR const debug_shader_id, LPCSTR const full_shader_id, FS_FileSet const& file_set, string_path& result)
{
	strcpy_s					( result, "" );
	return						false;
}