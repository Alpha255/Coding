function includeCommon()
	includedirs { 
		"$(SolutionDir)", 
		"$(SolutionDir)Colorful", 
		"$(SolutionDir)ThirdParty",
		"$(SolutionDir)ThirdParty\\vcglib",
		"$(SolutionDir)ThirdParty\\assimp\\include",
		"$(SolutionDir)ThirdParty\\assimp\\build\\include",
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

		"Colorful",
		"ImGui",
		"DirectXTK",
		"gear",
		"assimp",
		"pugixml",
	}
end

function resourceInclude()
	files { 
		"./Assets/Icon/Resource.rc",
		"./Assets/Icon/directx.ico",
		"./Assets/Icon/vulkan.ico",
	}
end

function configInclude()
	files {
		"./Out/appConfig.json",
		"./Out/assetConfig.json",
		"./Out/shaderConfig.json",
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
	filter { }

	group "Libs"
		project "gear"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files "./gear/**"
			includedirs { 
				"$(SolutionDir)",
				"$(VK_SDK_PATH)\\Include" 
			}
			targetdir "$(SolutionDir)Out\\Libs\\"

		project "colorful"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files "./Colorful/**"
			targetdir "$(SolutionDir)Out\\Libs\\"
			includeCommon()
	
	group "Fort"
		project "Learner"
			kind "ConsoleApp"
			language "C++"
			location "./Projects"
			files "./Fort/Learner/**"
			includedirs { 
				"$(SolutionDir)",
				"$(SolutionDir)ThirdParty",
				"$(SolutionDir)ThirdParty\\googletest",
				"$(SolutionDir)ThirdParty\\googletest\\googletest",
				"$(SolutionDir)ThirdParty\\googletest\\googlemock",
				"$(SolutionDir)ThirdParty\\googletest\\googletest\\include",
				"$(SolutionDir)ThirdParty\\googletest\\googlemock\\include",
			 }
			links {
				"gear",
				"googletest"
			}

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

  		project "assimp"
  			kind "SharedLib"
			location "./Projects"
			targetdir "$(SolutionDir)Out\\"
			buildoptions { "/bigobj" }
			disablewarnings { "4315", "4458", "4456", "4245", "4127", "4244", "4706", "4701", "4702", "4100", "4389", "4267", "4457", "4131", "4996", "4310", "4459", "4189" }
			files {
				"./ThirdParty/assimp/code/**",
				"./ThirdParty/assimp/include/**",
				"./ThirdParty/assimp/contrib/irrXML/**",
				"./ThirdParty/assimp/contrib/unzip/**",
				"./ThirdParty/assimp/contrib/clipper/**",
				"./ThirdParty/assimp/contrib/Open3DGC/**",
				"./ThirdParty/assimp/contrib/openddlparser/**",
				"./ThirdParty/assimp/contrib/poly2tri/**",
				"./ThirdParty/assimp/contrib/zip/src/**",
				"./ThirdParty/assimp/contrib/zlib/**.c",
				"./ThirdParty/assimp/contrib/zlib/**.h",
			}
			removefiles { 
				"./ThirdParty/assimp/contrib/zlib/contrib/inflate86/**",
				"./ThirdParty/assimp/code/Importer/IFC/IFCReaderGen_4.h",
				"./ThirdParty/assimp/code/Importer/IFC/IFCReaderGen_4.cpp",
				"./ThirdParty/assimp/contrib/zlib/contrib/**",
			}
			includedirs { 
				"$(SolutionDir)ThirdParty/assimp",
				"$(SolutionDir)ThirdParty/assimp/include",
				"$(SolutionDir)ThirdParty/assimp/code",
				"$(SolutionDir)ThirdParty/assimp/contrib/",
				"$(SolutionDir)ThirdParty/assimp/contrib/irrXML",
				"$(SolutionDir)ThirdParty/assimp/contrib/zlib",
				"$(SolutionDir)ThirdParty/assimp/contrib/unzip",
				"$(SolutionDir)ThirdParty/assimp/contrib/openddlparser/include/",
				"$(SolutionDir)ThirdParty/assimp/build",
				"$(SolutionDir)ThirdParty/assimp/build/include",
				"$(SolutionDir)ThirdParty/assimp/build/contrib/zlib",
				"$(SolutionDir)ThirdParty/assimp/contrib/rapidjson/include"
			}
			filter { "configurations:Debug" }
				defines { 
					"WIN32",
					"_WINDOWS",
					"_DEBUG",
					"EBUG:FULL",
					"WIN32_LEAN_AND_MEAN",
					"UNICODE",
					"_UNICODE",
					"ASSIMP_BUILD_NO_C4D_IMPORTER",
					"MINIZ_USE_UNALIGNED_LOADS_AND_STORES=0",
					"ASSIMP_IMPORTER_GLTF_USE_OPEN3DGC=1",
					"ASSIMP_BUILD_DLL_EXPORT",
					"_SCL_SECURE_NO_WARNINGS",
					"_CRT_SECURE_NO_WARNINGS",
					"OPENDDLPARSER_BUILD",
					"assimp_EXPORTS",
				}
			filter { "configurations:Release" }
				defines { 
					"WIN32",
					"_WINDOWS",
					"NDEBUG",
					"WIN32_LEAN_AND_MEAN",
					"UNICODE",
					"_UNICODE",
					"ASSIMP_BUILD_NO_C4D_IMPORTER",
					"MINIZ_USE_UNALIGNED_LOADS_AND_STORES=0",
					"ASSIMP_IMPORTER_GLTF_USE_OPEN3DGC=1",
					"ASSIMP_BUILD_DLL_EXPORT",
					"_SCL_SECURE_NO_WARNINGS",
					"_CRT_SECURE_NO_WARNINGS",
					"OPENDDLPARSER_BUILD",
					"assimp_EXPORTS", 
				}
		
				project "googletest"
					kind "StaticLib"
					language "C++"
					location "./Projects"
					includedirs { 
						"$(SolutionDir)ThirdParty\\googletest\\googletest",
						"$(SolutionDir)ThirdParty\\googletest\\googletest\\include",
						"$(SolutionDir)ThirdParty\\googletest\\googlemock",
						"$(SolutionDir)ThirdParty\\googletest\\googlemock\\include",
					 }
					vpaths {
						["gtest/include"] = {
							"./ThirdParty/googletest/googletest/include/gtest/**"
						},
						["gtest/src"] = {
							"./ThirdParty/googletest/googletest/src/**"
						},
						["gmock/include"] = {
							"./ThirdParty/googletest/googlemock/include/gmock/**"
						},
						["gmock/src"] = {
							"./ThirdParty/googletest/googlemock/src/**"
						},
					}
					files {
						"./ThirdParty/googletest/googletest/include/gtest/**",
						"./ThirdParty/googletest/googletest/src/gtest-all.cc",
						"./ThirdParty/googletest/googlemock/include/gmock/**",
						"./ThirdParty/googletest/googlemock/src/gmock-all.cc"
					}
					removefiles {
						"./ThirdParty/googletest/googletest/include/gtest/internal/custom/README.md",
						"./ThirdParty/googletest/googletest/src/gtest_main.cc",
						"./ThirdParty/googletest/googlemock/include/gmock/internal/custom/README.md",
						"./ThirdParty/googletest/googlemock/src/gmock_main.cc",
						"./ThirdParty/googletest/**.pump"
					}
					filter { "configurations:Debug" }
						defines {
							"WIN32",
							"_WINDOWS",
							"_UNICODE",
							"UNICODE",
							"_WIN32",
							"STRICT",
							"WIN32_LEAN_AND_MEAN",
							"GTEST_HAS_PTHREAD=0",
							"_HAS_EXCEPTIONS=1"
						}
					filter { "configurations:Release" }
						defines {
							"WIN32",
							"_WINDOWS",
							"NDEBUG",
							"_UNICODE",
							"UNICODE",
							"_WIN32",
							"STRICT",
							"WIN32_LEAN_AND_MEAN",
							"GTEST_HAS_PTHREAD=0",
							"_HAS_EXCEPTIONS=1"
						}

		project "pugixml"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files { "./ThirdParty/pugixml/src/**" }
			targetdir "$(SolutionDir)Out\\Libs\\"

	group "Colorful"
		project "rRenderTest"
			kind "WindowedApp"
			language "C++"
			location "./Projects"
			vpaths {
				["Resource"] = { 
					"./Assets/Icon/Resource.rc", 
					"./Assets/Icon/directx.ico", 
					"./Assets/Icon/vulkan.ico",
					"./Applications/Resource.h"
					},
				[""] = { "./Applications/Colorful/rRenderTest/**" },
				["Shaders"] = { 
					"./Assets/Shaders/rRenderTest.xml"
				},
				["Configuration"] = { 
					"./Out/appConfig.json",
					"./Out/assetConfig.json",
					"./Out/shaderConfig.json",
				}
			}
			files { 
				"./Applications/Colorful/rRenderTest/**",
				"./Assets/Shaders/rRenderTest.xml",
				"./Applications/Resource.h"
			}
			includeCommon()
			resourceInclude()
			linkLibsCommon()
			configInclude()



