project "TinyObjLoader"
    kind "StaticLib"
    language "C++"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
        "tinyobjloader/tiny_obj_loader.h",
        "tinyobjloader/tiny_obj_loader.cc",
    }

    includedirs {
        "tinyobjloader",
    }

	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        --- staticruntime "On"

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Development"
		defines { "DEVELOPMENT" }
		symbols "On"
		optimize "On"

	filter "configurations:Release"
		defines { "RELEASE" }
		optimize "On"
		symbols "On"

project "ImGui"
	location "ImGui"
    kind "SharedLib"
    language "C++"
    
	targetdir ("../bin/" .. outputdir .. "/ShaderPixel")
    objdir ("../bin-int/" .. outputdir .. "/ShaderPixel")

    defines "IMGUI_API=__declspec(dllexport)"

	files
	{
        "%{prj.name}/imconfig.h",
        "%{prj.name}/imgui.h",
        "%{prj.name}/imgui.cpp",
        "%{prj.name}/imgui_draw.cpp",
        "%{prj.name}/imgui_internal.h",
        "%{prj.name}/imgui_widgets.cpp",
        "%{prj.name}/imstb_rectpack.h",
        "%{prj.name}/imstb_textedit.h",
        "%{prj.name}/imstb_truetype.h",
        "%{prj.name}/imgui_demo.cpp"
    }
    
	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
       --- taticruntime "On"
        buildoptions "/MT"
