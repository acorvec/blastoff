#pragma once

#include "Utils.h"
#include "ProgramConstants.h"

namespace BlastOff
{
	struct Logging
	{
		static void Initialize(const ProgramConstants* const programConstants);

		static void Log(const char* const value);
		static void LogWarning(const char* const value);

	private:
		static const inline ProgramConstants* m_ProgramConstants = nullptr;

		static string CalculateBanner();
	};
}