function includeCommon()
	includedirs { 
		"$(SolutionDir)", 
		"$(SolutionDir)Colorful", 
		"$(SolutionDir)ThirdParty",
		"$(SolutionDir)ThirdParty\\vcglib",
		"$(VK_SDK_PATH)", 
		"$(VK_SDK_PATH)\\Include" 
	}
end

function linkLibsCommon()
	libdirs {
		"$(SolutionDir)ThirdParty\\freetype\\win64",
		"$(VK_SDK_PATH)\\Lib",
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

		"Base",
		"Colorful",
		"AssetTool",
		"ImGui",
		"DirectXTK",
		"gear",
	}
end

function resourceInclude()
	files { 
		"./Assets/Icon/Resource.rc",
		"./Assets/Icon/directx.ico",
		"./Assets/Icon/vulkan.ico",
		"./Base/Resource.h",
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
		defines { "DEBUG", "_UNICODE", "UNICODE", "Platform_Win32" }
	filter { "configurations:Release" }
		optimize "Speed"
	filter { "platforms:Win64" }
		system "Windows"
		architecture "x64"
		defines { "NDEBUG", "_UNICODE", "UNICODE", "Platform_Win32" }
	filter { }

	group "Libs"
		project "Base"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files "./Base/**"
			targetdir "$(SolutionDir)Out\\Libs\\"

		project "gear"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files "./gear/**"
			includedirs { "$(SolutionDir)" }
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
			disablewarnings { "4100", "4201", "4458", "4244", "4267", }
	
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
			removefiles { "./ThirdParty/ImGUI/examples/**", "./ThirdParty/ImGUI/misc/fonts/**" }
			targetdir "$(SolutionDir)Out\\Libs\\"
			includedirs { 
				"$(SolutionDir)ThirdParty\\ImGUI",
				"$(SolutionDir)ThirdParty\\freetype\\include",
			}

		project "DirectXTK"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			pchheader "pch.h"
			pchsource "./ThirdParty/DirectXTK/Src/pch.cpp"
			floatingpoint "Fast"
			files { 
				"./ThirdParty/DirectXTK/**.h", 
				"./ThirdParty/DirectXTK/**.cpp", 
				"./ThirdParty/DirectXTK/**.fx", 
				"./ThirdParty/DirectXTK/**.fxh",
				"./ThirdParty/DirectXTK/**.hlsl", 
				"./ThirdParty/DirectXTK/**.hlsli",
				"./ThirdParty/DirectXTK/**.inc",
				"./ThirdParty/DirectXTK/**.inl",
				"./ThirdParty/DirectXTK/**.cmd",
				"./ThirdParty/DirectXTK/**.txt", 
			}
			removefiles { 
				"./ThirdParty/DirectXTK/Audio/**",
				"./ThirdParty/DirectXTK/Inc/Audio.h",
				"./ThirdParty/DirectXTK/Inc/XboxDDSTextureLoader.h",
				"./ThirdParty/DirectXTK/Src/XboxDDSTextureLoader.cpp",
				"./ThirdParty/DirectXTK/XWBTool/XWBTool.cpp", 
			}
			targetdir "$(SolutionDir)Out\\Libs\\"
			includedirs { "$(SolutionDir)ThirdParty\\DirectXTK\\Inc", }

			filter { "files:**.hlsl" }
  				flags { "ExcludeFromBuild" }

  			filter { "configurations:Debug" }
  				defines { 
  					"_WIN32_WINNT=0x0600",
  					"_WIN7_PLATFORM_UPDATE",
  					"WIN32",
  					"_DEBUG",
  					"_LIB",
  					"_CRT_STDIO_ARBITRARY_WIDE_SPECIFIERS",
  				}

  			filter { "configurations:Release" }
  				defines { 
  					"_WIN32_WINNT=0x0600",
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
			vpaths {
				["Resource"] = { 
					"./Assets/Icon/Resource.rc", 
					"./Assets/Icon/directx.ico", 
					"./Assets/Icon/vulkan.ico",
					"./Base/Resource.h"
					},
				[""] = { "./Demo/Box/**" },
			}
			files { "./Demo/Box/**" }
			resourceInclude()
			includeCommon()
			linkLibsCommon()

		project "MultithreadedRendering"
			kind "WindowedApp"
			language "C++"
			location "./Projects"
			vpaths {
				["Resource"] = { 
					"./Assets/Icon/Resource.rc", 
					"./Assets/Icon/directx.ico", 
					"./Assets/Icon/vulkan.ico",
					"./Base/Resource.h"
					},
				[""] = { "./Demo/MultithreadedRendering/**" },
			}
			files { "./Demo/MultithreadedRendering/**" }
			resourceInclude()
			includeCommon()
			linkLibsCommon()

		project "vkTest"
			kind "WindowedApp"
			language "C++"
			location "./Projects"
			vpaths {
				["Resource"] = { 
					"./Assets/Icon/Resource.rc", 
					"./Assets/Icon/directx.ico", 
					"./Assets/Icon/vulkan.ico",
					"./Base/Resource.h"
					},
				[""] = { "./Demo/vkTest/**" },
				["Shaders"] = {
					"./Assets/Shaders/vkTest.shader"
				}
			}
			files { 
				"./Demo/vkTest/**",
				"./Assets/Shaders/vkTest.shader"
			}
			resourceInclude()
			includeCommon()
			linkLibsCommon()

		group "Colorful/HLSLCookBook"
			project "ForwardLighting"
				kind "WindowedApp"
				language "C++"
				location "./Projects"
				vpaths {
					["Resource"] = { 
						"./Assets/Icon/Resource.rc", 
						"./Assets/Icon/directx.ico", 
						"./Assets/Icon/vulkan.ico",
						"./Base/Resource.h"
						},
					[""] = { "./Demo/HLSLCookBook/ForwardLighting/**" },
				}
				files { "./Demo/HLSLCookBook/ForwardLighting/**" }
				resourceInclude()
				includeCommon()
				linkLibsCommon()

			project "DeferredShading"
				kind "WindowedApp"
				language "C++"
				location "./Projects"
				vpaths {
					["Resource"] = { 
						"./Assets/Icon/Resource.rc", 
						"./Assets/Icon/directx.ico", 
						"./Assets/Icon/vulkan.ico",
						"./Base/Resource.h"
						},
					[""] = { "./Demo/HLSLCookBook/DeferredShading/**" },
				}
				files { "./Demo/HLSLCookBook/DeferredShading/**" }
				resourceInclude()
				includeCommon()
				linkLibsCommon()



