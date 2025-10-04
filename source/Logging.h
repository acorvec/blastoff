#pragma once

#include "Utils.h"
#include "ProgramConfiguration.h"

#include <cstdio>

namespace BlastOff
{
	struct Logging
	{
		static void Initialize(const ProgramConfiguration* const programConfig);

		static void Log(const char* const value);
		static void Log(const string& value);

	private:
		static const inline ProgramConfiguration* m_ProgramConfig = nullptr;

		static string CalculateBanner();
	};
}