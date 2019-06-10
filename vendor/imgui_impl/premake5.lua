project "ImGuiImpl"
    kind "StaticLib"
    language "C++"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
        "imgui_impl_glfw.h",
        "imgui_impl_glfw.cpp",
        "imgui_impl_opengl3.h",
        "imgui_impl_opengl3.cpp"
    }

    includedirs {
        "../imgui",
        "../glfw/include",
        "../glad/include"
    }

	defines {
		"IMGUI_IMPL_OPENGL_LOADER_GLAD=1"
	}
    
	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"
        
    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"
