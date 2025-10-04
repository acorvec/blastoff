#pragma once

#include <optional>
#include <string>
#include <memory>

#include "Debug.h"

#ifdef _WIN32
#define COMPILE_TARGET_WINDOWS _WIN32
#else
#define COMPILE_TARGET_WINDOWS 0
#endif

#define COMPILE_TARGET_LINUX __linux__


namespace BlastOff
{
	using std::optional, std::unique_ptr;
	using std::string;

	struct CursorPosition
	{
		int x;
		int y;
	};

#if COMPILE_CONFIG_DEBUG
	void BreakProgram();
#endif

	optional<CursorPosition> GetCursorPosition();

	string GetFontPath(const char* const name, const char* const extension);
}