// Giperion April 2020
// [EUREKA] 3.13.0
// ToR Project
#include "xrCore/stdafx.h"
#include "xrLiteVisualStudioProjectGenerator.h"
#include <sstream>

const char* VcxProj_Prologue =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
"<Project DefaultTargets=\"Build\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\r\n"
"  <ItemGroup Label=\"ProjectConfigurations\">\r\n"
"    <ProjectConfiguration Include=\"Release|x64\">\r\n"
"      <Configuration>Release</Configuration>\r\n"
"      <Platform>x64</Platform>\r\n"
"    </ProjectConfiguration>\r\n"
"  </ItemGroup>\r\n";

const char* VcxProj_Epilogue =
"   <PropertyGroup Label=\"Globals\">\r\n"
"    <VCProjectVersion>16.0</VCProjectVersion>\r\n"
"    <ProjectGuid>{3BC6F532-1447-4F89-89DC-95976196F280}</ProjectGuid>\r\n"
"    <Keyword>Win32Proj</Keyword>\r\n"
"    <RootNamespace>xrShaderCode</RootNamespace>\r\n"
"    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>\r\n"
"  </PropertyGroup>\r\n"
"  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\r\n"
"  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\" Label=\"Configuration\">\r\n"
"    <ConfigurationType>StaticLibrary</ConfigurationType>\r\n"
"    <UseDebugLibraries>false</UseDebugLibraries>\r\n"
"    <PlatformToolset>v142</PlatformToolset>\r\n"
"    <WholeProgramOptimization>true</WholeProgramOptimization>\r\n"
"    <CharacterSet>Unicode</CharacterSet>\r\n"
"  </PropertyGroup>\r\n"
"  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />\r\n"
"  <ImportGroup Label=\"ExtensionSettings\">\r\n"
"  </ImportGroup>\r\n"
"  <ImportGroup Label=\"Shared\">\r\n"
"  </ImportGroup>\r\n"
"  <ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">\r\n"
"    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />\r\n"
"  </ImportGroup>\r\n"
"  <PropertyGroup Label=\"UserMacros\" />\r\n"
"  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">\r\n"
"    <LinkIncremental>false</LinkIncremental>\r\n"
"  </PropertyGroup>\r\n"
"  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='Release|x64'\">\r\n"
"    <ClCompile>\r\n"
"      <WarningLevel>Level3</WarningLevel>\r\n"
"      <PreprocessorDefinitions>NDEBUG;_LIB;%(PreprocessorDefinitions)</PreprocessorDefinitions>\r\n"
"      <ConformanceMode>true</ConformanceMode>\r\n"
"    </ClCompile>\r\n"
"    <Link>\r\n"
"      <SubSystem>Windows</SubSystem>\r\n"
"      <EnableCOMDATFolding>true</EnableCOMDATFolding>\r\n"
"      <OptimizeReferences>true</OptimizeReferences>\r\n"
"      <GenerateDebugInformation>true</GenerateDebugInformation>\r\n"
"    </Link>\r\n"
"  </ItemDefinitionGroup>\r\n"
"  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\r\n"
"  <ImportGroup Label=\"ExtensionTargets\">\r\n"
"  </ImportGroup>\r\n"
"</Project>\r\n";

void GenerateVisualStudioProjectForFiles(const xr_vector<ShaderFileEntry>& Files, xr_string& OutVcxproj)
{
	std::stringstream ss;

	ss << VcxProj_Prologue;

	// split include and source files
	xr_vector<const xr_string*> Includes;
	xr_vector<const xr_string*> Sources;

	for (const ShaderFileEntry& file : Files)
	{
		// very rough sort using last character.
		// rude, but effective
		char lastChar = file.Name[file.Name.size() - 1];
		if (lastChar == 'p')
		{
			Sources.push_back(&file.Name);
		}
		else if (lastChar == 'h')
		{
			Includes.push_back(&file.Name);
		}
	}

	ss << "  <ItemGroup>" << std::endl;
	for (const xr_string* source : Sources)
	{
		ss << "    " << "<ClCompile Include=\"" << source->c_str() << "\" />" << std::endl;
	}
	ss << "  </ItemGroup>" << std::endl;

	ss << "  <ItemGroup>" << std::endl;
	for (const xr_string* header : Includes)
	{
		ss << "    " << "<ClInclude Include=\"" << header->c_str() << "\" />" << std::endl;
	}
	ss << "  </ItemGroup>" << std::endl;

	ss << VcxProj_Epilogue;

	std::string result = ss.str();

	OutVcxproj.append(result.c_str(), result.size());
}