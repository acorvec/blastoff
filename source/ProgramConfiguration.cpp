#include "ProgramConfiguration.h"

namespace BlastOff
{
	ProgramConfiguration::ProgramConfiguration() :
		m_SoundEnabled(true),
		m_EscapeKeyEnabled(false),
		m_CommandLineLoggingEnabled(true),
		m_ControlQEnabled(true),
		m_TargetFramerate(60),
		m_TargetFrametime(1 / (float)m_TargetFramerate),
		m_InvalidColour1(0xFF, 0x00, 0xFF),
		m_InvalidColour2(c_Black),
		m_VoidColour(c_Black),
		m_InterSceneFadeColour(c_Black),
		m_SecondsBetweenSceneChange(1 / 2.0f),
		m_FontFace("Ubuntu-Medium"),
		m_FontRenderSize(192),
		m_DefaultWindowSize(Vector2i{ 480, 854 }),
		m_Name("BlastOff++"),
		m_Version("alpha 1.0.0"),
		m_BackgroundMusicPath("rifftastic.ogg"),
		m_BackgroundMusicLoopStart(77.3f),
		m_BackgroundMusicLoopEnd(141.3f),
		m_BackgroundMusicVolume(1 / 4.0f)
	{
#if COMPILE_CONFIG_DEBUG
			m_DebugToolsEnabled = true;

			m_FastModeKey = KEY_ONE;
			m_SlowModeKey = KEY_TWO;

			m_NormalModeMultiplier = 1;
			m_FastModeMultiplier = 1 / 6.0f;
			m_SlowModeMultiplier = 6;
#endif
	}

	bool ProgramConfiguration::GetSoundEnabled() const
	{
		return m_SoundEnabled;
	}

	bool ProgramConfiguration::GetEscapeKeyEnabled() const
	{
		return m_EscapeKeyEnabled;
	}

	bool ProgramConfiguration::GetCommandLineLoggingEnabled() const
	{
		return m_CommandLineLoggingEnabled;
	}

	bool ProgramConfiguration::GetControlQEnabled() const
	{
		return m_ControlQEnabled;
	}

	int ProgramConfiguration::GetTargetFramerate() const
	{
		return m_TargetFramerate;
	}

	Colour4i ProgramConfiguration::GetInvalidColour1() const
	{
		return m_InvalidColour1;
	}

	Colour4i ProgramConfiguration::GetInvalidColour2() const
	{
		return m_InvalidColour2;
	}

	Colour4i ProgramConfiguration::GetVoidColour() const
	{
		return m_VoidColour;
	}

	Colour4i ProgramConfiguration::GetInterSceneFadeColour() const
	{
		return m_InterSceneFadeColour;
	}

	float ProgramConfiguration::GetTargetFrametime() const
	{
		return m_TargetFrametime;
	}

	float ProgramConfiguration::GetSecondsBetweenSceneChange() const
	{
		return m_SecondsBetweenSceneChange;
	}

	const string& ProgramConfiguration::GetFontFace() const
	{
		return m_FontFace;
	}

	int ProgramConfiguration::GetFontRenderSize() const
	{
		return m_FontRenderSize;
	}

	Vector2i ProgramConfiguration::GetDefaultWindowSize() const
	{
		return m_DefaultWindowSize;
	}

	const string& ProgramConfiguration::GetName() const
	{
		return m_Name;
	}

	const string& ProgramConfiguration::GetVersion() const
	{
		return m_Version;
	}

	string ProgramConfiguration::CalculateBuildString() const
	{
		return m_Name + "(" + m_Version + ")";
	}

	const string& ProgramConfiguration::GetBackgroundMusicPath() const
	{
		return m_BackgroundMusicPath;
	}

	float ProgramConfiguration::GetBackgroundMusicLoopStart() const
	{
		return m_BackgroundMusicLoopStart;
	}

	float ProgramConfiguration::GetBackgroundMusicLoopEnd() const
	{
		return m_BackgroundMusicLoopEnd;
	}

	float ProgramConfiguration::GetBackgroundMusicVolume() const
	{
		return m_BackgroundMusicVolume;
	}

#if COMPILE_CONFIG_DEBUG
	bool ProgramConfiguration::GetDebugToolsEnabled() const
	{
		return m_DebugToolsEnabled;
	}

	int ProgramConfiguration::GetSlowModeKey() const
	{
		return m_SlowModeKey;
	}

	int ProgramConfiguration::GetFastModeKey() const
	{
		return m_FastModeKey;
	}

	float ProgramConfiguration::GetNormalModeMultiplier() const
	{
		return m_NormalModeMultiplier;
	}

	float ProgramConfiguration::GetSlowModeMultiplier() const
	{
		return m_SlowModeMultiplier;
	}

	float ProgramConfiguration::GetFastModeMultiplier() const
	{
		return m_FastModeMultiplier;
	}
#endif
}
