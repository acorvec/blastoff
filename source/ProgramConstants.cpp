#include "ProgramConstants.h"

namespace BlastOff
{
	ProgramConstants::ProgramConstants() :
		m_SoundEnabled(true),
		m_EscapeKeyEnabled(false),
		m_CommandLineLoggingEnabled(true),
		m_ControlQEnabled(true),
		m_TargetFramerate(60),
		m_WindowSizeIncrement(60),
		m_TargetFrametime(1 / (float)m_TargetFramerate),
		m_InvalidColour1(0xFF, 0x00, 0xFF),
		m_InvalidColour2(c_Black),
		m_VoidColour(c_Black),
		m_InterSceneFadeColour(c_Black),
		m_SecondsBetweenSceneChange(1 / 2.0f),
		m_FontFace("Ubuntu-Medium"),
		m_FontRenderSize(192),
		m_DefaultAspectRatio(9, 16),
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
			m_SlowModeMultiplier = 1 / 6.0f;
			m_FastModeMultiplier = 100;
#endif
	}

	bool ProgramConstants::GetSoundEnabled() const
	{
		return m_SoundEnabled;
	}

	bool ProgramConstants::GetEscapeKeyEnabled() const
	{
		return m_EscapeKeyEnabled;
	}

	bool ProgramConstants::GetCommandLineLoggingEnabled() const
	{
		return m_CommandLineLoggingEnabled;
	}

	bool ProgramConstants::GetControlQEnabled() const
	{
		return m_ControlQEnabled;
	}

	int ProgramConstants::GetTargetFramerate() const
	{
		return m_TargetFramerate;
	}

	int ProgramConstants::GetWindowSizeIncrement() const
	{
		return m_WindowSizeIncrement;
	}

	float ProgramConstants::GetTargetFrametime() const
	{
		return m_TargetFrametime;
	}

	float ProgramConstants::GetSecondsBetweenSceneChange() const
	{
		return m_SecondsBetweenSceneChange;
	}

	Colour4i ProgramConstants::GetInvalidColour1() const
	{
		return m_InvalidColour1;
	}

	Colour4i ProgramConstants::GetInvalidColour2() const
	{
		return m_InvalidColour2;
	}

	Colour4i ProgramConstants::GetVoidColour() const
	{
		return m_VoidColour;
	}

	Colour4i ProgramConstants::GetInterSceneFadeColour() const
	{
		return m_InterSceneFadeColour;
	}

	const string& ProgramConstants::GetFontFace() const
	{
		return m_FontFace;
	}

	int ProgramConstants::GetFontRenderSize() const
	{
		return m_FontRenderSize;
	}

	Vector2f ProgramConstants::GetDefaultAspectRatio() const
	{
		return m_DefaultAspectRatio;
	}

	const string& ProgramConstants::GetName() const
	{
		return m_Name;
	}

	const string& ProgramConstants::GetVersion() const
	{
		return m_Version;
	}

	string ProgramConstants::CalculateBuildString() const
	{
		return m_Name + "(" + m_Version + ")";
	}

	const string& ProgramConstants::GetBackgroundMusicPath() const
	{
		return m_BackgroundMusicPath;
	}

	float ProgramConstants::GetBackgroundMusicLoopStart() const
	{
		return m_BackgroundMusicLoopStart;
	}

	float ProgramConstants::GetBackgroundMusicLoopEnd() const
	{
		return m_BackgroundMusicLoopEnd;
	}

	float ProgramConstants::GetBackgroundMusicVolume() const
	{
		return m_BackgroundMusicVolume;
	}

#if COMPILE_CONFIG_DEBUG
	bool ProgramConstants::GetDebugToolsEnabled() const
	{
		return m_DebugToolsEnabled;
	}

	int ProgramConstants::GetSlowModeKey() const
	{
		return m_SlowModeKey;
	}

	int ProgramConstants::GetFastModeKey() const
	{
		return m_FastModeKey;
	}

	float ProgramConstants::GetNormalModeMultiplier() const
	{
		return m_NormalModeMultiplier;
	}

	float ProgramConstants::GetSlowModeMultiplier() const
	{
		return m_SlowModeMultiplier;
	}

	float ProgramConstants::GetFastModeMultiplier() const
	{
		return m_FastModeMultiplier;
	}
#endif
}
