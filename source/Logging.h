#pragma once

#include "Utils.h"
#include "ProgramConstants.h"

#include <cstdio>

namespace BlastOff
{
	struct Logging
	{
		static void Initialize(const ProgramConstants* const programConfig);

		static void Log(const char* const value);
		static void Log(const string& value);

	private:
		static const inline ProgramConstants* m_ProgramConfig = nullptr;

		static string CalculateBanner();
	};
}