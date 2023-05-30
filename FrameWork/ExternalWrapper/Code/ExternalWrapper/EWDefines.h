#pragma once

#ifdef FRAMEWORK_EXPORTS
#define FZN_EXPORT __declspec(dllexport)
#else
#define FZN_EXPORT __declspec(dllimport)
#endif
