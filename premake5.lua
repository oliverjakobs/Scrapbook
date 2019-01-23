workspace "Scrapbook"

	configurations
	{
		"Debug",
		"Release"
	}

outputDir = "%{cfg.buildcfg}"   

IncludeDir = {}
IncludeDir["INCLUDE"] = "Scrapbook/external/include" 
IncludeDir["GLFW"] = "Scrapbook/external/glfw/include" 
IncludeDir["GLAD"] = "Scrapbook/external/glad/include"

include "Scrapbook/external/glfw"
include "Scrapbook/external/glad"

project "Scrapbook"
	location "Scrapbook"
	kind "ConsoleApp"
	language "C++"
	
	targetdir ("bin/" .. outputDir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputDir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/res/shader/*.vert",
		"%{prj.name}/res/shader/*.frag"
	}

	links
	{
		"GLFW",
		"GLAD",
		"opengl32"
	}

	includedirs
	{
		"%{prj.name}/src",
		"Scrapbook/external/include",
		"Scrapbook/external/glfw/include",
		"Scrapbook/external/glad/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"
