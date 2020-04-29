// Giperion April 2020
// [EUREKA] 3.13.0
// ToR Project

#include "xrCore/stdafx.h"
#include "include/xrRender/ShaderEncryptionKey.h"
#include <sstream>
#include <iomanip>
#include "xrLiteVisualStudioProjectGenerator.h"
#include <direct.h>

std::string ByteArrayToHexStr(const xr_string& in) 
{
	std::stringstream ss;

	ss << std::hex << std::setfill('0');
	for (size_t i = 0; in.length() > i; ++i) {
		ss << "\\x" << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(in[i]));
	}

	return ss.str();
}

xr_string GetPlainShaderName(const xr_string& InShaderName)
{
	xr_string result;

	result.reserve(InShaderName.size());

	for (size_t i = 0; i < InShaderName.size(); i++)
	{
		char ch = InShaderName[i];
		if (ch == '\\' || ch == ' ')
		{
			result.push_back('_');
		}
		else if (ch == '.')
		{
			if ((InShaderName.size() - i) > 4 &&
				InShaderName[i + 1] == 'h' && InShaderName[i + 2] == 'l' && InShaderName[i + 3] == 's' && InShaderName[i + 4] == 'l')
			{
				break;
			}
			else
			{
				result.push_back('_');
			}
		}
		else
		{
			result.push_back(ch);
		}
	}

	return result;
}

int main(int argc, char** argv, char** envp)
{
	xrDebug::Initialize(GEnv.isDedicatedServer);
	Core.Initialize("xrShaderCryptor");
	EnableForwardToConsole(true);
	Msg("xrShaderCryptor version 1.0");

	if (argc < 2)
	{
		Msg("Usage: xrShaderCryptor -out=\"C:\\Output\"");
		Msg("Keep in mind - working directory must be a root game folder");
		Msg("Program also require \"VS2019INSTALLDIR\" environment variable to be presented, as well as Visual Studio 2019");
		return 1;
	}

	// Get output directory
	char* OutDir = argv[1];
	if (OutDir == nullptr)
	{
		Msg("Invalid output directory");
		return 1;
	}

	xr_string OutputDirectory;
	xr_string strOutDir = OutDir;
	if (memcmp(strOutDir.data(), "-out=", 5) == 0)
	{
		std::string_view OutputPath(&strOutDir[5], strOutDir.size() - 5);
		_mkdir(OutputPath.data());
		if (!FS.can_write_to_folder(OutputPath.data()))
		{
			Msg("Invalid output directory");
			return 1;
		}
		OutputDirectory = OutputPath;
	}
	else
	{
		Msg("Invalid output directory");
		return 1;
	}

	xr_vector<ShaderFileEntry> AllOutputSourceFiles;
	xr_vector<pstr>* AllShaderFiles = FS.file_list_open("$game_shaders$", "");
	Msg("Found %zu shaders", AllShaderFiles->size());

	_rmdir("generated");

	for (const char* ShaderFile : *AllShaderFiles)
	{
		Msg("Working on \"%s\"", ShaderFile);

		IReader* hShaderFile = FS.r_open("$game_shaders$", ShaderFile);
		if (hShaderFile == nullptr) continue;

		int InputShaderSize = hShaderFile->length();

		const char* pShaderSource = reinterpret_cast<const char*> (hShaderFile->pointer());

		xr_string ShaderSource(pShaderSource, InputShaderSize);
		
		// step 1: minify
#pragma todo("Giperion: Need implement")

		// step 2: encode

		// Encode with fixed key for now.

		xr_string EncKey(SHADER_ENC_KEY);
		size_t EncKeyIter = 0;
		for (char& ch : ShaderSource)
		{
			ch = ch ^ EncKey[EncKeyIter++];
			EncKeyIter %= EncKey.size();
		}

		// step 3: prepare for writing.
		// escape " symbols
		std::string HexShaderSource = ByteArrayToHexStr(ShaderSource);

		xr_string PlainShaderName = GetPlainShaderName(ShaderFile);
		// For every file: Generate .cpp file with all content. Then generate common .h file with all pointers to encrypted content
		string_path GeneratedFileName;
		strconcat(sizeof(GeneratedFileName), GeneratedFileName, PlainShaderName.c_str(), ".cpp");
		AllOutputSourceFiles.emplace_back(ShaderFileEntry{ GeneratedFileName, ShaderSource.size() });

		string_path GeneratedFilePath;
		strconcat(sizeof(GeneratedFilePath), GeneratedFilePath, "generated\\", GeneratedFileName);

		IWriter* hShaderCodeOutput = FS.w_open(GeneratedFilePath);

		// break lines every 32 characters. Fix compiler bug C2026
		xr_vector<xr_string> SplittedLines;
		for (size_t lineIter = 0; lineIter < ((HexShaderSource.size() / 32) + (HexShaderSource.size() % 32 == 0 ? 0 : 1)); lineIter++)
		{
			size_t begin = lineIter * 32;
			size_t end = (lineIter * 32) + 32;
			end = std::min(end, HexShaderSource.size());

			SplittedLines.emplace_back(xr_string(&HexShaderSource.c_str()[begin], end - begin));
		}

		std::stringstream ss;
		ss << "// generated file, do not modify" << std::endl;
		ss << "// Source: " << ShaderFile << std::endl;
		if (char* dash = (char*)strchr(PlainShaderName.c_str(), '-'))
		{
			*dash = '_';
		}
		ss << "const char* " << "gShaderFile_" << PlainShaderName << " = " << std::endl;
		for (const xr_string& line : SplittedLines)
		{
			ss << '\"' << line << '\"' << std::endl;
		}
		ss << ';' << std::endl;
		ss << std::endl;

		std::string FinalContent = ss.str();
		hShaderCodeOutput->w(FinalContent.data(), FinalContent.size());

		FS.w_close(hShaderCodeOutput);
		FS.r_close(hShaderFile);
	}

	// generate common file that include all source code

	//ShaderSourceMap.h
	{
		IWriter* hShaderCodeMapHeader = FS.w_open("generated\\ShaderSourceMap.h");
		std::stringstream ss;
		ss << "// generated file, do not modify" << std::endl;
		ss << std::endl;
		ss << "struct ShaderSourceEntry" << std::endl;
		ss << "{" << std::endl;
		ss << "\t" << "const char* Source;" << std::endl;
		ss << "\t" << "const unsigned int SourceSize;" << std::endl;
		ss << "\t" << "const char* Code;" << std::endl;
		ss << "\t" << "const unsigned int CodeSize;" << std::endl;
		ss << "};" << std::endl;
		ss << std::endl;
		ss << "extern const ShaderSourceEntry shaders[];" << std::endl;
		ss << std::endl;

		std::string ShaderSourceMapContent = ss.str();
		hShaderCodeMapHeader->w(ShaderSourceMapContent.data(), ShaderSourceMapContent.size());

		FS.w_close(hShaderCodeMapHeader);
		AllOutputSourceFiles.emplace_back(ShaderFileEntry{ "ShaderSourceMap.h", ShaderSourceMapContent.size() });
	}

	//ShaderSourceMap_Private.h
	{
		IWriter* hShaderCodeMapPrivateHeader = FS.w_open("generated\\ShaderSourceMap_Private.h");
		std::stringstream ss;
		ss << "// generated file, do not modify" << std::endl;
		ss << std::endl;
		for (const char* ShaderFile : *AllShaderFiles)
		{
			xr_string sShaderFile(ShaderFile);

			xr_string PlainShaderName = GetPlainShaderName(sShaderFile);
			if (char* dash = (char*)strchr(PlainShaderName.c_str(), '-'))
			{
				*dash = '_';
			}
			ss << "extern const char* gShaderFile_" << PlainShaderName << ";" << std::endl;
		}
		ss << std::endl;

		std::string ShaderSourceMapPrivateContent = ss.str();
		hShaderCodeMapPrivateHeader->w(ShaderSourceMapPrivateContent.data(), ShaderSourceMapPrivateContent.size());

		FS.w_close(hShaderCodeMapPrivateHeader);
		AllOutputSourceFiles.emplace_back(ShaderFileEntry{ "ShaderSourceMap_Private.h", ShaderSourceMapPrivateContent.size() });
	}

	//ShaderSourceMap.cpp
	{
		IWriter* hShaderCodeMap = FS.w_open("generated\\ShaderSourceMap.cpp");

		xr_string EncKey(SHADER_ENC_KEY);
		size_t EncKeyIter = 0;
		std::stringstream ss;
		ss << "// generated file, do not modify" << std::endl;
		ss << "#include \"ShaderSourceMap.h\"" << std::endl;
		ss << "#include \"ShaderSourceMap_Private.h\"" << std::endl;
		ss << std::endl;
		ss << "const ShaderSourceEntry shaders[] = {" << std::endl;
		for (const ShaderFileEntry& ShaderFile : AllOutputSourceFiles)
		{
			xr_string sShaderFile(ShaderFile.Name);
			if (sShaderFile[sShaderFile.size() - 1] == 'h')
			{
				continue;
			}
			sShaderFile.erase(sShaderFile.end() - 4, sShaderFile.end());
			xr_string ClearName = sShaderFile;

			ss << "\t{\"";
			// name is encoded
			for (char& ch : sShaderFile)
			{
				ch = ch ^ EncKey[EncKeyIter++];
				EncKeyIter %= EncKey.size();
			}
			if (char* dash = (char*)strchr(ClearName.c_str(), '-'))
			{
				*dash = '_';
			}
			std::string hexShaderFile = ByteArrayToHexStr(sShaderFile);
			ss << hexShaderFile;
			ss << "\", ";
			ss << sShaderFile.size();
			ss << ", ";
			ss << "gShaderFile_" << ClearName;
			ss << ", ";
			ss << ShaderFile.SourceSize;
			ss << "}," << std::endl;
		}
		// insert null entry to mark the end of the list
		ss << "\t{\"\", 0, \"\", 0}," << std::endl;

		ss << "};" << std::endl;

		std::string ShaderSourceMapContent = ss.str();
		hShaderCodeMap->w(ShaderSourceMapContent.data(), ShaderSourceMapContent.size());

		FS.w_close(hShaderCodeMap);
		AllOutputSourceFiles.emplace_back(ShaderFileEntry{ "ShaderSourceMap.cpp", ShaderSourceMapContent.size() });
	}

	FS.file_list_close(AllShaderFiles);

	// build all cpp files
	{
		IWriter* hShaderCodeProject = FS.w_open("generated\\xrShaderCode.vcxproj");

		xr_string VcxProj;
		GenerateVisualStudioProjectForFiles(AllOutputSourceFiles, VcxProj);
		hShaderCodeProject->w(VcxProj.data(), VcxProj.size());

		FS.w_close(hShaderCodeProject);
	}

	Msg("Source code generation complete, proceed to building");
	char** envpIter = envp;

	bool bFoundMsBuild = false;
	while (envpIter != nullptr)
	{
		char* envVar = *envpIter;

		if (envVar == nullptr)
		{
			break;
		}

		char* strRef = strstr(envVar, "VS2019INSTALLDIR");

		if (strRef != nullptr)
		{
			char* EnvValue =(char*) memchr(envVar, '=', xr_strlen(envVar));
			EnvValue++;

			bFoundMsBuild = true;
			xr_string MsBuildPath = EnvValue;

			MsBuildPath += "\\MSBuild\\Current\\Bin\\amd64\\MSBuild.exe";

			xr_string MsBuildCommandLine = "\"";
			MsBuildCommandLine.append(MsBuildPath);
			MsBuildCommandLine.append("\"");
			MsBuildCommandLine += " xrshadercode.vcxproj -property:Configuration=Release -property:Platform=x64";
			MsBuildCommandLine.append(5, '\0');

			xr_string MsBuildWorkingDirectory = Core.WorkingPath;
			MsBuildWorkingDirectory += "\\generated";

			STARTUPINFOA cif;
			ZeroMemory(&cif, sizeof(STARTUPINFO));
			PROCESS_INFORMATION pi;
			if (!CreateProcessA(MsBuildPath.c_str(), (LPSTR)MsBuildCommandLine.c_str(), NULL, NULL, FALSE, NULL, NULL, MsBuildWorkingDirectory.c_str(), &cif, &pi))
			{
				Msg("! Can't invoke msbuild - Win32 code: %u", GetLastError());
			}

			WaitForSingleObject(pi.hProcess, INFINITE);
			break;
		}

		envpIter++;
	}
	
	if (!bFoundMsBuild)
	{
		Msg("! Shader Compiler can't find the \"VS2019INSTALLDIR\" environment variable in order to use msbuild");
		return 1;
	}

	// copy library and include files
	xr_string AppFolder = Core.WorkingPath;

	FS.file_copy((AppFolder + "\\generated\\ShaderSourceMap.h").c_str(), (OutputDirectory + "\\ShaderSourceMap.h").c_str(), FSType::External);
	FS.file_copy((AppFolder + "\\generated\\x64\\Release\\xrshadercode.lib").c_str(), (OutputDirectory + "\\xrshadercode.lib").c_str(), FSType::External);

	_rmdir("generated");

	Msg("Shader Compiler finished");
	return 0;
}
