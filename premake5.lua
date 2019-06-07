-- premake5.lua
workspace "ShaderPixel"
	architecture "x64"
	startproject "ShaderPixel"

	configurations
	{
		"Debug",
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

project "ShaderSystem"
	location "ShaderPixel"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "RELEASE" }
		optimize "On"

	links 
	{ 
		"Glad",
		"opengl32.lib"
    }

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
		"ShaderSystem",
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

   filter "configurations:Release"
      defines { "RELEASE" }
      optimize "On"
