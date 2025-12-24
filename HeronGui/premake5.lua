project "HeronGui"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"src/**.rc",

		"vendor/imgui/imgui.cpp",
		"vendor/imgui/imgui_draw.cpp",
		"vendor/imgui/imgui_tables.cpp",
		"vendor/imgui/imgui_widgets.cpp",
		"vendor/imgui/imgui_demo.cpp",

		"vendor/imgui/backends/imgui_impl_win32.cpp",
		"vendor/imgui/backends/imgui_impl_dx12.cpp"
	}

    includedirs
	{
		"%{prj.location}/src",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGui}/backends",
		"%{wks.location}/Heron/include"
	}

	links {
		"Heron"
	}


	postbuildcommands
	{
		("{COPYDIR} %{wks.location}/Heron/datasets %{cfg.targetdir}/datasets"),
		("{COPYDIR} %{wks.location}/HeronGui/assets %{cfg.targetdir}/assets")
	}

	filter "system:windows"
		systemversion "latest"
		debugdir "%{cfg.targetdir}"
		
		postbuildcommands
		{
			("{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/Heron/Heron.dll %{cfg.targetdir}")
		}

		links
		{
			"d3d12",
			"dxgi",
			"dxguid"
		}

	filter { "system:windows", "configurations:Debug" }
        postbuildcommands {
            -- PDB ONLY in Debug
            ("{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/Heron/Heron.pdb %{cfg.buildtarget.directory}")
        }
	
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