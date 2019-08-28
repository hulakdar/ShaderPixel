-- premake5.lua
workspace "ShaderPixel"
	architecture "x64"
	startproject "ShaderPixel"

	configurations
	{
		"Debug",
		"Development",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["ImGui"] = "vendor/imgui"
IncludeDir["ImGuiImpl"] = "vendor/imgui/examples"
IncludeDir["glm"] = "vendor/glm"
IncludeDir["stb"] = "vendor/stb"
IncludeDir["TinyObjLoader"] = "vendor/tinyobjloader"
IncludeDir["assimp"] = "vendor/assimp"

group "Dependencies"
	include "vendor/GLFW"
	include "vendor/Glad"
	include "vendor/"
group ""


project "ShaderPixel"
	location "ShaderPixel"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	--- staticruntime "on"

   
	targetdir ("bin/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	defines {
		"IMGUI_API=__declspec(dllimport)", 
		"IMGUI_IMPL_API=", 
		"IMGUI_IMPL_OPENGL_LOADER_GLAD"
	} 

	files {
        "vendor/imgui/examples/imgui_impl_opengl3.h",
        "vendor/imgui/examples/imgui_impl_opengl3.cpp",
		"%{prj.name}/include/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/include",
		"Host/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuiImpl}",
		"%{IncludeDir.TinyObjLoader}",
		"%{IncludeDir.glm}"
	}

	links 
	{ 
		"Glad",
		"ImGui",
		--"ImGuiImplGL",
		"TinyObjLoader",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

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

project "Host"
	location "Host"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
   
	targetdir ("bin/ShaderPixel")
	objdir ("bin-int/" .. outputdir .. "/ShaderPixel")

	files {
		"vendor/imgui/examples/imgui_impl_glfw.h",
		"vendor/imgui/examples/imgui_impl_glfw.cpp",
		"%{prj.name}/include/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/include",
		"ShaderPixel/include",
		"%{IncludeDir.glm}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuiImpl}"
	}

	links 
	{ 
		"GLFW",
		"ShaderPixel",
        "ImGui"
        --, "ImGuiImplGL"
	}

	filter "system:windows"
		systemversion "latest"

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
