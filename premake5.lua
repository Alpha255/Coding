function appInclude()
	includedirs { 
		"$(SolutionDir)", 
		"$(SolutionDir)ThirdParty",
		"$(SolutionDir)ThirdParty\\VulkanSDK\\Include",
	}
end

function appLinks()
	libdirs {}

	links { 
		"d3d11",
		"d3d12",
		"dxgi",
		"d3dcompiler", 
		"windowscodecs",
		"Usp10",
		"Comctl32",
		"Shcore",

		"Colorful",
		"ImGui",
		"Gear",
		"AssetTool"
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
			files "./gear/**"
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
			--files "./Colorful/Public/**"
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\"
			defines { "DYNAMIC_LIB" }
			implibname "$(SolutionDir)Out\\Libs\\$(ProjectName)"
			files {
				"./Colorful/Public/GfxRenderer.h",
				"./Colorful/Public/GfxRenderer.cpp",
				"./Colorful/Public/GfxRenderState.h",
				"./Colorful/Public/GfxRenderState.cpp",
				"./Colorful/Public/GfxResource.h"
			}
--[[
		project "Gfx-SoftwareRenderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files "./Colorful/Software/**"
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\"
			links { "Gear" }
		project "Gfx-D3D11Renderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files "./Colorful/D3D/D3D11/**"
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\"
			links { "Gear" } 
		project "Gfx-D3D12Renderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files "./Colorful/D3D/D3D12/**"
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\"
			links { "Gear" }
		project "Gfx-VulkanRenderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files "./Colorful/Vulkan/**"
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\"
			links { "Gear" }
--]]

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
		links { "Gear", "Gfx-GfxRenderer" }
		vpaths {
			["Resource"] = { 
				"./Assets/Icon/Resource.rc",  
				"./Applications/Resource.h"
			},
			[""] = { 
				"./Applications/Colorful/RenderTest/**" 
			}
		}

