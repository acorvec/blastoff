#include "Logging.h"

namespace BlastOff
{
	void Logging::Initialize(const ProgramConstants* const programConstants)
	{
		m_ProgramConstants = programConstants;
	}

	void Logging::Log(const string& value) 
	{
		Log(value.c_str());
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

	string Logging::CalculateBanner() 
	{
		return m_ProgramConstants->CalculateBuildString() + ": ";
	}
}