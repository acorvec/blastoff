#include "Logging.h"

#include <print>
#include <cstdio>
#include <csignal>

namespace BlastOff
{
	void Logging::Initialize(const ProgramConstants* const programConstants)
	{
		m_ProgramConstants = programConstants;
	}

	void Logging::Log(const char* const value) 
	{
		const bool loggingEnabled = 
		{
			m_ProgramConstants->GetCommandLineLoggingEnabled()
		};

		if (loggingEnabled)
		{
			const string banner = CalculateBanner();

			std::print("{}", banner);
			std::println("{}", value);
		}
	}

	void Logging::LogWarning(const char* const value)
	{
		const bool loggingEnabled = 
		{
			m_ProgramConstants->GetCommandLineLoggingEnabled()
		};

		if (loggingEnabled)
		{
			const string banner = CalculateBanner();
			std::fprintf(
				stderr, 
				"WARNING: %s: %s\n", 
				banner.c_str(), 
				value
			);
		}
#if COMPILE_CONFIG_DEBUG
		BreakProgram();
#endif
	}

	string Logging::CalculateBanner() 
	{
		return m_ProgramConstants->CalculateBuildString() + ": ";
	}
}