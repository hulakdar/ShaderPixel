project "ImGuiImplGLFW"
    kind "StaticLib"
    language "C++"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
        "imgui/examples/imgui_impl_glfw.h",
        "imgui/examples/imgui_impl_glfw.cpp",
    }

    includedirs {
        "imgui",
        "glfw/include",
    }

	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"
        
project "ImGuiImplGL"
    kind "StaticLib"
    language "C++"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
        "imgui/examples/imgui_impl_opengl3.h",
        "imgui/examples/imgui_impl_opengl3.cpp"
    }

    includedirs {
        "imgui",
        "glad/include"
    }

	defines {
		"IMGUI_IMPL_OPENGL_LOADER_GLAD=1"
	}
    
	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"

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
        staticruntime "On"

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
