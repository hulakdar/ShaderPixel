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
		defines "IMGUI_API=__declspec(dllexport)"
        buildoptions "/MT"

project "GLFW"
    kind "StaticLib"
    language "C"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
        "GLFW/include/GLFW/glfw3.h",
        "GLFW/include/GLFW/glfw3native.h",
        "GLFW/src/glfw_config.h",
        "GLFW/src/context.c",
        "GLFW/src/init.c",
        "GLFW/src/input.c",
        "GLFW/src/monitor.c",
        "GLFW/src/vulkan.c",
        "GLFW/src/window.c"
    }
    
	filter "system:windows"
        systemversion "latest"
        staticruntime "On"
        
        files
        {
            "GLFW/src/win32_init.c",
            "GLFW/src/win32_joystick.c",
            "GLFW/src/win32_monitor.c",
            "GLFW/src/win32_time.c",
            "GLFW/src/win32_thread.c",
            "GLFW/src/win32_window.c",
            "GLFW/src/wgl_context.c",
            "GLFW/src/egl_context.c",
            "GLFW/src/osmesa_context.c"
        }

		defines 
		{ 
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
		}

    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"

	filter "system:macosx"
        --- staticruntime "On"
        
        files
        {
            "GLFW/src/cocoa_init.m",
            "GLFW/src/cocoa_joystick.m",
            "GLFW/src/cocoa_monitor.m",
            "GLFW/src/cocoa_time.c",
            "GLFW/src/posix_thread.c",
            "GLFW/src/cocoa_window.m",
            "GLFW/src/nsgl_context.m",
            "GLFW/src/egl_context.c",
            "GLFW/src/osmesa_context.c"
        }

		defines 
		{ 
            "_GLFW_COCOA"
		}

