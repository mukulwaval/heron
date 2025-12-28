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
		"%{prj.location}/blueprints-example.cpp",
		"%{prj.location}/source/**.cpp",
		"%{prj.location}/source/**.h",
		"%{prj.location}/utilities/**.cpp",
		"%{prj.location}/utilities/**.h",
		-- "%{prj.location}/**.rc",

		"vendor/imgui/imgui.cpp",
		"vendor/imgui/imgui_draw.cpp",
		"vendor/imgui/imgui_tables.cpp",
		"vendor/imgui/imgui_widgets.cpp",
		"vendor/imgui/imgui_demo.cpp",

		"vendor/imgui/backends/imgui_impl_win32.cpp",
		"vendor/imgui/backends/imgui_impl_dx11.cpp",

		"vendor/imgui-node-editor/imgui_node_editor.h",
		"vendor/imgui-node-editor/imgui_node_editor.cpp",
		"vendor/imgui-node-editor/imgui_node_editor_api.cpp",
		"vendor/imgui-node-editor/imgui_node_editor_internal.inl",
		"vendor/imgui-node-editor/imgui_node_editor_internal.h",

		"vendor/imgui-node-editor/imgui_canvas.cpp",
   		"vendor/imgui-node-editor/crude_json.cpp",

		"vendor/fmt/src/format.cc",
    	"vendor/fmt/src/os.cc",
	}

    includedirs
	{
		"%{prj.location}",
		"vendor/stb_image",
		"%{prj.location}/include",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGui}/backends",
		"%{wks.location}/HeronGui/vendor/imgui-node-editor",
		"%{wks.location}/Heron/include",
		"%{wks.location}/HeronGui/vendor/fmt/include"
	}

	links {
		"Heron"
	}


	postbuildcommands
	{
		("{COPYDIR} %{wks.location}/Heron/datasets %{cfg.targetdir}/datasets"),
		("{COPYDIR} %{wks.location}/HeronGui/assets %{cfg.targetdir}/assets"),
		("{COPYDIR} %{wks.location}/HeronGui/data %{cfg.targetdir}/data")
	}

	filter "system:windows"
		systemversion "latest"
		debugdir "%{cfg.targetdir}"
		buildoptions { "/utf-8" }
		
		postbuildcommands
		{
			("{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/Heron/Heron.dll %{cfg.targetdir}")
		}

		links
		{
			"d3d11",
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