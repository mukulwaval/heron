project "HeronGui"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.location}/include/**.h",
		"%{prj.location}/HeronGui.cpp",
		"%{prj.location}/source/**.cpp",
		"%{prj.location}/source/**.h",

		-- ImGui core
		"vendor/imgui/imgui.cpp",
		"vendor/imgui/imgui_draw.cpp",
		"vendor/imgui/imgui_tables.cpp",
		"vendor/imgui/imgui_widgets.cpp",
		"vendor/imgui/imgui_demo.cpp",

		-- ImNodeFlow
		"vendor/ImNodeFlow/include/ImNodeFlow.h",
		"vendor/ImNodeFlow/src/**.h",
		"vendor/ImNodeFlow/src/**.cpp",
		"vendor/ImNodeFlow/src/**.inl",

		-- fmt
		"vendor/fmt/src/format.cc",
		"vendor/fmt/src/os.cc",
	}

	includedirs
	{
		"%{prj.location}",
		"%{prj.location}/include",
		"vendor/stb_image",

		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGui}/backends",

		"%{wks.location}/Heron/include",
		"%{wks.location}/HeronGui/vendor/ImNodeFlow/include",
		"%{wks.location}/HeronGui/vendor/fmt/include"
	}

	links { "Heron" }

	postbuildcommands
	{
		("{COPYDIR} %{wks.location}/Heron/datasets %{cfg.targetdir}/datasets"),
		("{COPYDIR} %{wks.location}/HeronGui/assets %{cfg.targetdir}/assets")
	}

	filter "system:windows"
		systemversion "latest"
		debugdir "%{cfg.targetdir}"
		buildoptions { "/utf-8" }

		files
		{
			"%{prj.location}/**.rc",
			"vendor/imgui/backends/imgui_impl_win32.cpp",
			"vendor/imgui/backends/imgui_impl_dx11.cpp",
			"source/platform_win32.cpp",
			"source/renderer_dx11.cpp"
		}

		defines
		{
			"HRN_PLATFORM_WINDOWS",
			"IMGUI_IMPL_WIN32",
			"IMGUI_IMPL_DX11"
		}

		links
		{
			"d3d11",
			"dxgi",
			"dxguid"
		}

		postbuildcommands
		{
			("{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/Heron/Heron.dll %{cfg.targetdir}")
		}

	filter { "system:windows", "configurations:Debug" }
		postbuildcommands
		{
			("{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/Heron/Heron.pdb %{cfg.buildtarget.directory}")
		}

	filter "system:linux"
		systemversion "latest"

		postbuildcommands
		{
			("{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/Heron/libHeron.so %{cfg.targetdir}")
		}

		files
		{
			"vendor/imgui/backends/imgui_impl_glfw.cpp",
			"vendor/imgui/backends/imgui_impl_opengl3.cpp",
			"source/platform_glfw.cpp",
			"source/renderer_opengl.cpp"
		}

		defines
		{
			"HRN_PLATFORM_LINUX",
			"IMGUI_IMPL_GLFW",
			"IMGUI_IMPL_OPENGL3"
		}

		pic "On"

		links
		{
			"GL",
			"X11",
			"pthread",
			"dl"
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

	filter {}
