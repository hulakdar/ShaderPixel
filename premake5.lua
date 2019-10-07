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
	include "vendor/Glad"
	include "vendor/"
group ""

project "ShaderPixel"
	location "."
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++14"
   
	targetdir ("bin/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	defines {
		"IMGUI_IMPL_OPENGL_LOADER_GLAD" --- this should be re-done eventually. this is hack
	} 

	files {
        "vendor/imgui/examples/imgui_impl_opengl3.h", --- this should be re-done eventually. this is hack
        "vendor/imgui/examples/imgui_impl_opengl3.cpp", --- this should be re-done eventually. this is hack
        "vendor/imgui/examples/imgui_impl_glfw.h", --- this should be re-done eventually. this is hack
        "vendor/imgui/examples/imgui_impl_glfw.cpp", --- this should be re-done eventually. this is hack
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
		"GLFW",
		"TinyObjLoader",
		"Glad",
		"ImGui"
	}

	filter "system:windows"
		systemversion "latest"
		defines {
			"IMGUI_API=", 
			"IMGUI_IMPL_API="
		}
		links {
            "opengl32.lib"
        }

	filter "system:macosx"
		links { 
			"Cocoa.framework",
			"OpenGL.framework",
			"CoreVideo.framework",
			"IOKit.framework"
		}

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

--[[
project "Host"
	location "./"
	language "C++"
	cppdialect "C++14"
   
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
		"%{IncludeDir.TinyObjLoader}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuiImpl}"
	}

--]]