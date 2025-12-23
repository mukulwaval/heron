project "Heron"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"include/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{prj.location}/include"
	}

	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		postbuildcommands
		{
			("{COPYFILE} %{cfg.buildtarget.relpath} %{wks.location}/bin/" .. outputdir .. "/HeronGui/Heron.dll")
		}

		defines
		{
			"HRN_PLATFORM_WINDOWS",
			"HRN_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "HRN_DEBUG"
		runtime "Debug"
		symbols "on"


	filter "configurations:Release"
		defines "HRN_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "HRN_DIST"
		runtime "Release"
		optimize "on"