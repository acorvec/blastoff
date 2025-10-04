#pragma once

#include "Utils.h"
#include "Debug.h"
#include "OperatingSystem.h"

namespace BlastOff
{
	struct ProgramConfiguration
	{
		ProgramConfiguration();

		bool GetSoundEnabled() const;
		bool GetEscapeKeyEnabled() const;
		bool GetCommandLineLoggingEnabled() const;
		bool GetControlQEnabled() const;

		int GetTargetFramerate() const;

		Colour4i GetInvalidColour1() const;
		Colour4i GetInvalidColour2() const;
		Colour4i GetVoidColour() const;
		Colour4i GetInterSceneFadeColour() const;

		float GetTargetFrametime() const;
		float GetSecondsBetweenSceneChange() const;

		const string& GetFontFace() const;
		int GetFontRenderSize() const;

		Vector2i GetDefaultWindowSize() const;

		const string& GetName() const;
		const string& GetVersion() const;

		string CalculateBuildString() const;

		const string& GetBackgroundMusicPath() const;
		float GetBackgroundMusicLoopStart() const;
		float GetBackgroundMusicLoopEnd() const;
		float GetBackgroundMusicVolume() const;

#if COMPILE_CONFIG_DEBUG
		bool GetDebugToolsEnabled() const;

		int GetFastModeKey() const;
		int GetSlowModeKey() const;

		float GetNormalModeMultiplier() const;
		float GetSlowModeMultiplier() const;
		float GetFastModeMultiplier() const;
#endif
	private:
		bool m_SoundEnabled;
		bool m_EscapeKeyEnabled;
		bool m_CommandLineLoggingEnabled;
		bool m_ControlQEnabled;

		int m_TargetFramerate;
		float m_TargetFrametime;

		Colour4i m_InvalidColour1;
		Colour4i m_InvalidColour2;
		Colour4i m_VoidColour;
		Colour4i m_InterSceneFadeColour;

		float m_SecondsBetweenSceneChange;

		string m_FontFace;
		int m_FontRenderSize;

		Vector2i m_DefaultWindowSize;

		string m_Name;
		string m_Version;

		string m_BackgroundMusicPath;
		float m_BackgroundMusicLoopStart;
		float m_BackgroundMusicLoopEnd;
		float m_BackgroundMusicVolume;

#if COMPILE_CONFIG_DEBUG
		bool m_DebugToolsEnabled;

		int m_SlowModeKey = 0;
		int m_FastModeKey = 0;

		float m_NormalModeMultiplier = 0;
		float m_SlowModeMultiplier = 0;
		float m_FastModeMultiplier = 0;
#endif
	};
}