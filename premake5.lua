include "Dependencies.lua"

workspace "Heron"
	architecture "x86_64"
	startproject "HeronGui"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Core"
	include "Heron"
group ""

group "App"
	include "HeronGui"
	include "Sandbox"
group ""
