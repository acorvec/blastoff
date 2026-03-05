#pragma once

namespace BlastOff
{
	enum class GameOutcome
	{
		None,
		Winner,
		Loser
	};

	enum class ProgramState
	{
		None,
		Loading,
		Game,
		MainMenu,
		SettingsMenu
	};
}
