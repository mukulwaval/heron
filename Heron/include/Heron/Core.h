#pragma once

#if defined(HRN_PLATFORM_WINDOWS)

#ifdef HRN_BUILD_DLL
#define HERON_API __declspec(dllexport)
#else
#define HERON_API __declspec(dllimport)
#endif

#elif defined(HRN_PLATFORM_LINUX)
#define HERON_API __attribute__((visibility("default")))
#else
#define HERON_API
#endif