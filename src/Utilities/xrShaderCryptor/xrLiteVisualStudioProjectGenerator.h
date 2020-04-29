// Giperion April 2020
// [EUREKA] 3.13.0
// ToR Project
#pragma once
struct ShaderFileEntry
{
	xr_string Name;
	size_t SourceSize = 0;
};

void GenerateVisualStudioProjectForFiles(const xr_vector<ShaderFileEntry>& Files, xr_string& OutVcxproj);