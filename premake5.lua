function appInclude()
	includedirs { 
		"$(SolutionDir)", 
		"$(SolutionDir)ThirdParty",
	}
end

function appLinks()
	libdirs {}

	links {
		"windowscodecs",
		"Usp10",
		"Comctl32",
		"Shcore",
		"Gear",
		"Gfx-GfxRenderer",
		"Gfx-VulkanRenderer",
		"Gfx-D3D11Renderer",
		"Gfx-D3D12Renderer",
		"Gfx-ImGui",
		"ImGui"
	}
end

function appResourceFiles()
	files { 
		"./Assets/Icon/Resource.rc",
		"./Applications/Resource.h"
	}
end

workspace "Miscellaneous"
	location "./"
	configurations { "Debug", "Release" }
	objdir "$(SolutionDir)Out\\Intermediate\\"
	targetdir "$(SolutionDir)Out\\"
	characterset "Unicode"
	platforms "x64"
	targetname "$(ProjectName)"
	warnings "Extra"
	dpiawareness "High"
	systemversion "latest"
	symbolspath "$(IntDir)PDB\\$(TargetName).pdb"
	--flags { "MultiProcessorCompile", "NoIncrementalLink" }
	flags { "MultiProcessorCompile", }
	filter { "configurations:Debug" }
		symbols "On"
		optimize "Debug"
		defines { "DEBUG", "_UNICODE", "UNICODE" }
	filter { "configurations:Release" }
		optimize "Speed"
		defines { "NDEBUG", "_UNICODE", "UNICODE" }
	filter { "platforms:Win64" }
		system "Windows"
		architecture "x64"
	filter {}

	group "Libs"
		project "Gear"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files "./Gear/**"
			targetdir "$(SolutionDir)Out\\Libs\\"
			includedirs { "$(SolutionDir)" }

	group "Fort"
		project "Learner"
			kind "ConsoleApp"
			language "C++"
			location "./Projects"
			files "./Fort/Learner/**"
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\"
			links { "Gear" }

	group "Colorful"
		project "Gfx-GfxRenderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files "./Colorful/Public/**"
			includedirs { 
				"$(SolutionDir)",
				"$(SolutionDir)\\Thirdparty\\glslang",
			}
			targetdir "$(SolutionDir)Out\\"
			defines { "DYNAMIC_LIB" }
			implibname "$(SolutionDir)Out\\Libs\\$(ProjectName)"
			links { 
				"Gear", 
				"glslang",
				"tinyxml",
			}
			--disablewarnings { "4201", "4458", "4100" }

		project "Gfx-VulkanRenderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files "./Colorful/Vulkan/**"
			includedirs { 
				"$(SolutionDir)",
				"$(SolutionDir)ThirdParty\\Vulkan-Headers\\include"
			}
			targetdir "$(SolutionDir)Out\\"
			defines { "DYNAMIC_LIB" }
			implibname "$(SolutionDir)Out\\Libs\\$(ProjectName)"
			links { "Gear" }

		project "Gfx-D3D11Renderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files { 
				"./Colorful/D3D/D3D11/**",
				"./Colorful/D3D/DXGI_Interface.h",
				"./Colorful/D3D/DXGI_Interface.cpp",
			}
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\"
			defines { "DYNAMIC_LIB" }
			implibname "$(SolutionDir)Out\\Libs\\$(ProjectName)"
			links { 
				"Gear",
				"d3d11",
				"dxgi",
				"d3dcompiler"
			}

		project "Gfx-D3D12Renderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files { 
				"./Colorful/D3D/D3D12/**",
				"./Colorful/D3D/DXGI_Interface.h",
				"./Colorful/D3D/DXGI_Interface.cpp",
			}
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\"
			defines { "DYNAMIC_LIB" }
			implibname "$(SolutionDir)Out\\Libs\\$(ProjectName)"
			links { 
				"Gear",
				"d3d12",
				"dxgi",
				"d3dcompiler"
			}

		project "Gfx-SoftwareRenderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files "./Colorful/Software/**"
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\"
			defines { "DYNAMIC_LIB" }
			implibname "$(SolutionDir)Out\\Libs\\$(ProjectName)"
			links { "Gear" }

		project "Gfx-ImGui"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files "./Colorful/ImGui/**"
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\"

	group "ThirdParty"
		project "glslang"
		kind "StaticLib"
		language "C++"
		location "./Projects"
		files {
			"./ThirdParty/glslang/glslang/**.h",
			"./ThirdParty/glslang/glslang/**.cpp",
			"./ThirdParty/glslang/SPIRV/**.h",
			"./ThirdParty/glslang/SPIRV/**.cpp",
			"./ThirdParty/glslang/OGLCompilersDLL/**.h",
			"./ThirdParty/glslang/OGLCompilersDLL/**.cpp",
			"./ThirdParty/glslang/StandAlone/**.h",
			"./ThirdParty/glslang/StandAlone/**.cpp",
		}
		targetdir "$(SolutionDir)Out\\Libs\\"
		includedirs { 
			"$(SolutionDir)Thirdparty\\glslang",
			"$(SolutionDir)Thirdparty\\glslang\\build",
		}
		removefiles {
			"./ThirdParty/glslang/glslang/OSDependent/Unix/**",
			"./ThirdParty/glslang/glslang/OSDependent/Web/**",
			"./ThirdParty/glslang/glslang/OSDependent/Windows/main.cpp",
			"./ThirdParty/glslang/StandAlone/StandAlone.cpp",
			"./ThirdParty/glslang/StandAlone/spirv-remap.cpp",
		}
		--disablewarnings { "4456", "4457", "4458", "4244", "4702" }
		filter { "configurations:Debug" }
			defines { 
				"WIN32",
				"_WINDOWS",
				"ENABLE_HLSL",
				"GLSLANG_OSINCLUDE_WIN32",
				"ENABLE_OPT=0"
			}
		filter { "configurations:Release" }
			defines { 
				"WIN32",
				"_WINDOWS",
				"NDEBUG",
				"ENABLE_HLSL",
				"GLSLANG_OSINCLUDE_WIN32",
				"ENABLE_OPT=0"
			}

		project "ImGui"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files { 
				"./ThirdParty/ImGUI/**.h", 
				"./ThirdParty/ImGUI/**.cpp" 
			}
			removefiles { 
				"./ThirdParty/ImGUI/examples/**", 
				"./ThirdParty/ImGUI/misc/fonts/**",
				"./ThirdParty/ImGUI/misc/freetype/**" 
			}
			targetdir "$(SolutionDir)Out\\Libs\\"
			includedirs { 
				"$(SolutionDir)ThirdParty\\ImGUI"
			}

		project "tinyxml"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files {
				"./Thirdparty/tinyxml2/tinyxml2.h",
				"./Thirdparty/tinyxml2/tinyxml2.cpp"
			}
			targetdir "$(SolutionDir)Out\\Libs\\"

		project "spirv-cross"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files {
				"./ThirdParty/SPIRV-Cross/GLSL.std.450.h",
				"./ThirdParty/SPIRV-Cross/spirv.hpp",
				"./ThirdParty/SPIRV-Cross/spirv_cfg.hpp",
				"./ThirdParty/SPIRV-Cross/spirv_common.hpp",
				"./ThirdParty/SPIRV-Cross/spirv_cross.hpp",
				"./ThirdParty/SPIRV-Cross/spirv_cross_containers.hpp",
				"./ThirdParty/SPIRV-Cross/spirv_cross_error_handling.hpp",
				"./ThirdParty/SPIRV-Cross/spirv_cross_parsed_ir.hpp",
				"./ThirdParty/SPIRV-Cross/spirv_cfg.cpp",
				"./ThirdParty/SPIRV-Cross/spirv_cross.cpp",
				"./ThirdParty/SPIRV-Cross/spirv_cross_parsed_ir.cpp",
				"./ThirdParty/SPIRV-Cross/spirv_parser.cpp",

				"./ThirdParty/SPIRV-Cross/spirv_hlsl.hpp",
				"./ThirdParty/SPIRV-Cross/spirv_hlsl.cpp",

				"./ThirdParty/SPIRV-Cross/spirv_glsl.hpp",
				"./ThirdParty/SPIRV-Cross/spirv_glsl.cpp",

				"./ThirdParty/SPIRV-Cross/spirv_reflect.hpp",
				"./ThirdParty/SPIRV-Cross/spirv_reflect.cpp",

				"./ThirdParty/SPIRV-Cross/spirv_cross_util.hpp",
				"./ThirdParty/SPIRV-Cross/spirv_cross_util.cpp"
			}
			targetdir "$(SolutionDir)Out\\Libs\\"
			includedirs { "$(SolutionDir)ThirdParty\\SPIRV-Cross" }
			vpaths {
				["core"] = {
					"./ThirdParty/SPIRV-Cross/GLSL.std.450.h",
					"./ThirdParty/SPIRV-Cross/spirv.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cfg.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_common.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross_containers.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross_error_handling.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross_parsed_ir.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cfg.cpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross.cpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross_parsed_ir.cpp",
					"./ThirdParty/SPIRV-Cross/spirv_parser.cpp",
				},
				["hlsl"] = {
					"./ThirdParty/SPIRV-Cross/spirv_hlsl.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_hlsl.cpp",
				},
				["glsl"] = {
					"./ThirdParty/SPIRV-Cross/spirv_glsl.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_glsl.cpp",
				},
				["reflect"] = {
					"./ThirdParty/SPIRV-Cross/spirv_reflect.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_reflect.cpp",
				},
				["util"] = {
					"./ThirdParty/SPIRV-Cross/spirv_cross_util.hpp",
					"./ThirdParty/SPIRV-Cross/spirv_cross_util.cpp"
				}
			}


	group "Applications"
		project "RenderTest"
		kind "WindowedApp"
		language "C++"
		location "./Projects"
		files {
			"./Applications/Colorful/RenderTest/**",
		}
		appResourceFiles()
		includedirs { "$(SolutionDir)" }
		targetdir "$(SolutionDir)Out\\"
		appLinks()
		vpaths {
			["Resource"] = { 
				"./Assets/Icon/Resource.rc",  
				"./Applications/Resource.h"
			},
			[""] = { 
				"./Applications/Colorful/RenderTest/**" 
			}
		}

