#include "Logging.h"

namespace BlastOff
{
	void Logging::Initialize(const ProgramConfiguration* const programConfig)
	{
		m_ProgramConfig = programConfig;
	}

	void Logging::Log(const string& value) 
	{
		Log(value.c_str());
	}

	void Logging::Log(const char* const value) 
	{
		const bool loggingEnabled = 
		{
			m_ProgramConfig->GetCommandLineLoggingEnabled()
		};

		if (loggingEnabled)
		{
			const string banner = CalculateBanner();

			printf("%s", banner.c_str());
			printf("%s\n", value);
		}
	}

	string Logging::CalculateBanner() 
	{
		return m_ProgramConfig->CalculateBuildString() + ": ";
	}
}