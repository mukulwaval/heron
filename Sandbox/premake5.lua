project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/**.cpp"
    }

    includedirs {
        "%{wks.location}/Heron/include"
    }

    libdirs {
        "%{wks.location}/bin/" .. outputdir .. "/Heron"  -- point to Heron lib output
    }

    links {
        "Heron"
    }

    postbuildcommands
	{
		("{COPYDIR} %{wks.location}/Heron/datasets %{cfg.targetdir}/datasets")
	}

    -- Ensure Heron.dll is copied to Sandbox folder after build
    filter "system:windows"
        systemversion "latest"
        debugdir ("%{cfg.targetdir}")

        postbuildcommands {
            ("{COPYFILE} %{wks.location}/bin/" .. outputdir .. "/Heron/Heron.dll %{cfg.buildtarget.directory}")
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
