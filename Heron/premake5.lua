project "Heron"
	kind "SharedLib"
	language "C++"
	cppdialect "C++23"
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

	filter "system:linux"
		systemversion "latest"

		libdirs {
        	"%{cfg.targetdir}"
   	 	}	

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"HRN_PLATFORM_WINDOWS",
			"HRN_BUILD_DLL"
		}

	filter "system:linux"
		systemversion "latest"

		defines
		{
			"HRN_PLATFORM_LINUX",
			"HRN_BUILD_SO"
		}

		pic "On"

	filter "configurations:Debug"
		defines "HRN_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "HRN_RELEASE"
		runtime "Release"
		optimize "on"
		symbols "off"

	filter "configurations:Dist"
		defines "HRN_DIST"
		runtime "Release"
		optimize "on"
		symbols "off"

	filter {}
