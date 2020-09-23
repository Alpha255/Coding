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
		"Gfx-D3D12Renderer" 
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
			files "./Colorful/Public/**"
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\"
			defines { "DYNAMIC_LIB" }
			implibname "$(SolutionDir)Out\\Libs\\$(ProjectName)"

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
			
--[[
		project "Gfx-SoftwareRenderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files "./Colorful/Software/**"
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

