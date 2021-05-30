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
		"GfxRenderer",
		"VulkanRenderer",
		"D3D11Renderer",
		"D3D12Renderer",
		"ImGui",
		"ImGuiRenderer"
	}
end

function appResourceFiles()
	files { 
		"./Assets/Icon/Resource.rc",
		"./Applications/Resource.h"
	}
end

workspace "Rockcat"
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
		defines { "_DEBUG", "_UNICODE", "UNICODE" }
	filter { "configurations:Release" }
		optimize "Speed"
		defines { "NDEBUG", "_UNICODE", "UNICODE" }
	filter { "platforms:Win64" }
		system "Windows"
		architecture "x64"
	filter {}

	group "Fort"
		project "Gear"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files "./Gear/**"
			targetdir "$(SolutionDir)Out\\Libs\\$(Configuration)\\"
			includedirs { "$(SolutionDir)" }

		project "Learner"
			kind "ConsoleApp"
			language "C++"
			location "./Projects"
			files "./Fort/Learner/**"
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\"
			links { "Gear" }

	group "Colorful"
		project "GfxRenderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files "./Colorful/Gfx/**"
			removefiles "./Colorful/Gfx/ImGui/**"
			includedirs { 
				"$(SolutionDir)",
				"$(SolutionDir)Thirdparty\\glslang",
				"$(SolutionDir)ThirdParty\\KTX-Software\\include",
				"$(SolutionDir)ThirdParty\\KTX-Software\\other_include"
			}
			defines { "DYNAMIC_LIB", "STB_IMAGE_IMPLEMENTATION", "KHRONOS_STATIC", "LIBKTX" }
			targetdir "$(SolutionDir)Out\\"
			implibname "$(SolutionDir)Out\\Libs\\$(Configuration)\\$(ProjectName)"
			links { 
				"Gear",
				"glslang",
				"tinyxml",
				"spirv-cross",
				"libktx",
				"dxcompiler",
				"d3dcompiler",
				"assimp",
				"ImGui"
			}
			libdirs {
				"$(SolutionDir)Thirdparty\\dxc\\lib"
			}
			postbuildcommands {
				"{COPY} $(SolutionDir)Thirdparty\\dxc\\bin\\*.dll $(SolutionDir)Out"
			}

		project "VulkanRenderer"
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
			implibname "$(SolutionDir)Out\\Libs\\$(Configuration)\\$(ProjectName)"
			links { 
				"Gear", 
				"GfxRenderer"
			}

		project "D3D11Renderer"
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
			implibname "$(SolutionDir)Out\\Libs\\$(Configuration)\\$(ProjectName)"
			links { 
				"Gear",
				"d3d11",
				"dxgi",
				"GfxRenderer"
			}

		project "D3D12Renderer"
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
			implibname "$(SolutionDir)Out\\Libs\\$(Configuration)\\$(ProjectName)"
			links { 
				"Gear",
				"d3d12",
				"dxgi",
				"GfxRenderer"
			}

		project "SoftwareRenderer"
			kind "SharedLib"
			language "C++"
			location "./Projects"
			files "./Colorful/Software/**"
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\"
			defines { "DYNAMIC_LIB" }
			implibname "$(SolutionDir)Out\\Libs\\$(Configuration)\\$(ProjectName)"
			links { 
				"Gear",
				"GfxRenderer"
			}
		
		project "ImGuiRenderer"
			kind "StaticLib"
			language "C++"
			location "./Projects/"
			files "./Colorful/Gfx/ImGui/**"
			includedirs { "$(SolutionDir)" }
			targetdir "$(SolutionDir)Out\\Libs\\$(Configuration)\\"

	group "ThirdParty"
		project "glslang"
		kind "StaticLib"
		language "C++"
		location "./Projects"
		files {
			"./ThirdParty/glslang/**.h",
			"./ThirdParty/glslang/**.cpp",
		}
		targetdir "$(SolutionDir)Out\\Libs\\$(Configuration)\\"
		includedirs { 
			"$(SolutionDir)Thirdparty\\glslang",
			"$(SolutionDir)Thirdparty\\glslang\\build",
		}
		removefiles {
			"./Thirdparty/glslang/gtests/**",
			"./Thirdparty/ndk_test/**",
			"./Thirdparty/glslang/Test/**",
			"./ThirdParty/glslang/glslang/OSDependent/Unix/**",
			"./ThirdParty/glslang/glslang/OSDependent/Web/**",
			"./ThirdParty/glslang/glslang/OSDependent/Windows/main.cpp",
			"./ThirdParty/glslang/StandAlone/StandAlone.cpp",
			"./ThirdParty/glslang/StandAlone/spirv-remap.cpp",
		}
		disablewarnings { "4702", "4458", "4456", "4127", "4457", "4244", "4100", "4189", "4389", "4701" }
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

		project "libktx"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files {
				"./Thirdparty/KTX-Software/other_include/KHR/**",
				"./Thirdparty/KTX-Software/include/**",
				"./Thirdparty/KTX-Software/lib/**.h",
				"./Thirdparty/KTX-Software/lib/**.c",
				"./Thirdparty/KTX-Software/lib/**.inl",
				"./Thirdparty/KTX-Software/lib/basisu/zstd/**",
			}
			removefiles {
				"./Thirdparty/KTX-Software/lib/basisu/zstd/zstddeclib.c",
				"./ThirdParty/KTX-Software/lib/dfdutils/vulkan/vulkan_core.h",
				"./Thirdparty/KTX-Software/lib/dfdutils/endswap.c",
				"./Thirdparty/KTX-Software/lib/dfdutils/testbidirectionalmapping.c",
				"./Thirdparty/KTX-Software/lib/dfdutils/createdfdtest.c",
				"./Thirdparty/KTX-Software/lib/gl_funclist.inl",
				"./Thirdparty/KTX-Software/lib/gl_funcs.c",
				"./Thirdparty/KTX-Software/lib/gl_funcs.h",
				"./Thirdparty/KTX-Software/lib/vk_funclist.inl",
				"./Thirdparty/KTX-Software/lib/vk_funcs.h",
				"./Thirdparty/KTX-Software/lib/vk_funcs.c",
				"./Thirdparty/KTX-Software/lib/vk_format_check.c",
				"./Thirdparty/KTX-Software/lib/vk_format_enum.h",
				"./Thirdparty/KTX-Software/lib/vk_format_str.c",
				"./Thirdparty/KTX-Software/lib/vkloader.c",
				"./Thirdparty/KTX-Software/lib/writer1.c",
				"./Thirdparty/KTX-Software/lib/writer2.c",
			}
			targetdir "$(SolutionDir)Out\\Libs\\$(Configuration)\\"
			includedirs {
				"$(SolutionDir)",
				"$(SolutionDir)ThirdParty\\KTX-Software\\include",
				"$(SolutionDir)ThirdParty\\KTX-Software\\other_include",
			}
			filter {
				defines {
					"KHRONOS_STATIC",
					"LIBKTX"
				}
			}
			disablewarnings { "4389", "4706", "4100", "4127", "4189", "4701", "4702", "4101", "4456" }

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
				"./ThirdParty/ImGUI/misc/freetype/**",
				"./Thirdparty/ImGUI/backends/**"
			}
			targetdir "$(SolutionDir)Out\\Libs\\$(Configuration)\\"
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
			targetdir "$(SolutionDir)Out\\Libs\\$(Configuration)\\"

		project "spirv-cross"
			kind "StaticLib"
			language "C++"
			location "./Projects"
			files {
				"./ThirdParty/SPIRV-Cross/**.h",
				"./ThirdParty/SPIRV-Cross/**.hpp",
				"./ThirdParty/SPIRV-Cross/**.cpp",
			}
			removefiles {
				"./Thirdparty/SPIRV-Cross/samples/**",
				"./Thirdparty/SPIRV-Cross/main.cpp",
				"./Thirdparty/SPIRV-Cross/tests-other/**"
			}
			targetdir "$(SolutionDir)Out\\Libs\\$(Configuration)\\"
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
			disablewarnings { "4065", "4702", "4706"}

		project "assimp"
  			kind "SharedLib"
			location "./Projects"
			targetdir "$(SolutionDir)Out\\"
			buildoptions { "/bigobj" }
			disablewarnings { "4819", "4189", "4131", "4996", "4127", "4244" }
			implibname "$(SolutionDir)Out\\Libs\\$(Configuration)\\$(ProjectName)"
			files {
				"./ThirdParty/assimp/**.h",
				"./ThirdParty/assimp/**.cpp",
				"./ThirdParty/assimp/**.hpp",
				"./ThirdParty/assimp/**.c",
				"./ThirdParty/assimp/**.cc",
			}
			removefiles { 
				"./ThirdParty/assimp/contrib/zlib/contrib/inflate86/**",
				"./ThirdParty/assimp/code/AssetLib/IFC/IFCReaderGen_4.h",
				"./ThirdParty/assimp/code/AssetLib/IFC/IFCReaderGen_4.cpp",
				"./ThirdParty/assimp/contrib/zlib/contrib/**",
				"./ThirdParty/assimp/test/**",
				"./ThirdParty/assimp/tools/**",
				"./ThirdParty/assimp/contrib/gtest/**",
				"./Thirdparty/assimp/build/CMakeFiles/**",
				"./Thirdparty/assimp/include/port/AndroidJNI/**",
				"./Thirdparty/assimp/port/**",
				"./Thirdparty/assimp/code/AMF/**",
				"./Thirdparty/assimp/samples/**",
				"./Thirdparty/assimp/contrib/zip/test/**",
				"./Thirdparty/assimp/contrib/draco/**"
			}
			includedirs { 
				"$(SolutionDir)ThirdParty/assimp/build/include",
				"$(SolutionDir)ThirdParty/assimp/build",
				"$(SolutionDir)ThirdParty/assimp/include",
				"$(SolutionDir)ThirdParty/assimp/code",
				"$(SolutionDir)ThirdParty/assimp",
				"$(SolutionDir)ThirdParty/assimp/contrib/zlib",
				"$(SolutionDir)ThirdParty/assimp/build/contrib/zlib",
				"$(SolutionDir)ThirdParty/assimp/contrib/rapidjson/include",
				"$(SolutionDir)ThirdParty/assimp/contrib/",
				"$(SolutionDir)ThirdParty/assimp/contrib/pugixml/src",
				"$(SolutionDir)ThirdParty/assimp/contrib/unzip",
				"$(SolutionDir)ThirdParty/assimp/contrib/irrXML",
				"$(SolutionDir)ThirdParty/assimp/contrib/openddlparser/include"
			}
			filter { "configurations:Debug" }
				defines { 
					"WIN32",
					"_WINDOWS",
					"_DEBUG",
					"WIN32_LEAN_AND_MEAN",
					"UNICODE",
					"_UNICODE",
					"ASSIMP_BUILD_NO_C4D_IMPORTER",
					"MINIZ_USE_UNALIGNED_LOADS_AND_STORES=0",
					"ASSIMP_IMPORTER_GLTF_USE_OPEN3DGC=1",
					"RAPIDJSON_HAS_STDSTRING=1",
					"RAPIDJSON_NOMEMBERITERATORCLASS",
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
					"RAPIDJSON_HAS_STDSTRING=1",
					"RAPIDJSON_NOMEMBERITERATORCLASS",
					"ASSIMP_BUILD_DLL_EXPORT",
					"_SCL_SECURE_NO_WARNINGS",
					"_CRT_SECURE_NO_WARNINGS",
					"OPENDDLPARSER_BUILD",
					"assimp_EXPORTS", 
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

