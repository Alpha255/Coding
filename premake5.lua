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
		"./Assets/Icon/nvidia.ico",
		"./Out/Configurations.json",
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
			links {
				"Gear"
			}


