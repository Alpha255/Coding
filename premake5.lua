function includeCommon()
	includedirs { 
		"$(SolutionDir)", 
		"$(SolutionDir)Colorful", 
		"$(SolutionDir)ThirdParty",
		"$(SolutionDir)ThirdParty\\ImGUI",
		"$(SolutionDir)ThirdParty\\freetype\\include",
		"$(SolutionDir)ThirdParty\\vcglib",
		"$(SolutionDir)ThirdParty\\glm",
		"$(SolutionDir)ThirdParty\\gli",
		"$(SolutionDir)ThirdParty\\DirectXTK\\Inc",
		"$(SolutionDir)ThirdParty\\DirectXTK\\Src",
		"$(SolutionDir)ThirdParty\\DirectXTK\\Audio",
		"$(VK_SDK_PATH)", 
		"$(VK_SDK_PATH)\\Include" 
	}
end

function linkLibsCommon()
	libdirs {
		"$(SolutionDir)ThirdParty\\freetype\\win64",
		"$(VK_SDK_PATH)\\Lib",
		"$(VK_SDK_PATH)\\glslang\\Lib\\$(Configuration)",
	}

	links { 
		"d3d11",
		"dxgi", 
		"d3dcompiler", 
		"windowscodecs",
		"Usp10",
		"Comctl32",
		"Shcore",
		"vulkan-1",
		"SPIRV",
		"glslang",
		"SPVRemapper",
		"glslang-default-resource-limits",
		"OGLCompiler",
		"OSDependent",
		"HLSL",
		"SPIRV-Tools-opt",
		"SPIRV-Tools",

		"Base",
		"Colorful",
		"AssetTool",
		"ImGui",
	}
end

workspace "Test"
	location "./"
	configurations { "Debug", "Release" }
	objdir "$(SolutionDir)Out\\Intermediate\\"
	targetdir "$(SolutionDir)Out\\"
	characterset "Unicode"
	platforms "x64"
	targetname "$(ProjectName)"
	warnings "Extra"
	systemversion "latest"
	symbolspath "$(IntDir)PDB\\$(TargetName).pdb"
	flags { "MultiProcessorCompile" }
	filter { "configurations:Debug" }
		symbols "On"
		optimize "Debug"
	filter { "configurations:Release" }
		optimize "Speed"
	filter { "platforms:Win64" }
		system "Windows"
		architecture "x64"
	filter { }

	group "Libs"
		project "Base"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files "./Base/**"
			targetdir "$(SolutionDir)Out\\Libs\\"

		project "Colorful"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files "./Colorful/**"
			targetdir "$(SolutionDir)Out\\Libs\\"
			includeCommon()

		project "AssetTool"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files "./Tool/AssetTool/**"
			targetdir "$(SolutionDir)Out\\Libs\\"
			includeCommon()
	
	group "Fort"
		project "Learner"
			kind "ConsoleApp"
			language "C++"
			location "./Projects"
			files "./Fort/Learner/**"
			includedirs { "$(SolutionDir)" }

	group "ThirdParty"
		project "ImGui"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files { "./ThirdParty/ImGUI/**.h", "./ThirdParty/ImGUI/**.cpp" }
			removefiles { "./ThirdParty/ImGUI/examples/**" }
			targetdir "$(SolutionDir)Out\\Libs\\"
			includeCommon()

		project "DirectXTK"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			pchheader "pch.h"
			pchsource "pch.cpp"
			files { 
				"./ThirdParty/DirectXTK/**.h", 
				"./ThirdParty/DirectXTK/**.cpp", 
				"./ThirdParty/DirectXTK/**.fx", 
				"./ThirdParty/DirectXTK/**.fxh",
				"./ThirdParty/DirectXTK/**.hlsl", 
				"./ThirdParty/DirectXTK/**.hlsli", 
			}
			targetdir "$(SolutionDir)Out\\Libs\\"
			includeCommon()
			excludes  { "**.hlsl", "**.hlsli" }
  				--flags { "ExcludeFromBuild" }
  			filter { "configurations:Debug" }
  				defines { 
  					"_WIN32_WINNT=0x0601",
  					"_WIN7_PLATFORM_UPDATE",
  					"WIN32",
  					"_DEBUG",
  					"_LIB",
  					"_CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS",
  				}

  			filter { "configurations:Release" }
  				defines { 
  					"_WIN32_WINNT=0x0601",
  					"_WIN7_PLATFORM_UPDATE",
  					"WIN32",
  					"NDEBUG",
  					"_LIB",
  					"_CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS",
  				}

	group "Colorful"
		project "Box"
			kind "WindowedApp"
			language "C++"
			location "./Projects"
			files { "./Demo/Box/**" }
			includeCommon()
			linkLibsCommon()


