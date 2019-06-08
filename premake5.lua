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
IncludeDir["glm"] = "vendor/glm"

group "Dependencies"
	include "vendor/GLFW"
	include "vendor/Glad"
	include "vendor/imgui"
group ""

project "ShaderPixel"
	location "ShaderPixel"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

   
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"{prj.name}/inc/**.h",
		"{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glew}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
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
