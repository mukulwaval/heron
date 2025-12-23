#pragma once

#ifdef HRN_PLATFORM_WINDOWS
#ifdef HRN_BUILD_DLL
#define HERON_API __declspec(dllexport)
#else
#define HERON_API __declspec(dllimport)
#endif
#else
#define HERON_API
#endif