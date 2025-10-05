#include "Game.h"
#include "Player.h"

namespace BlastOff
{
    PlayableGame::PlayableGame(
        const bool* const programIsMuted,
        const ProgramConfiguration* const programConfig,
        ImageTextureLoader* const imageTextureLoader,
        TextTextureLoader* const textTextureLoader,
        SoundLoader* const soundLoader,
        const Callback& resetCallback,
        const Callback& muteUnmuteCallback,
        const Font* const font,
        const Vector2i* const windowPosition,
        const Vector2i* const windowSize
    ) :
        Game(
            programConfig,
            imageTextureLoader,
            textTextureLoader,
            soundLoader,
            font,
            windowPosition,
            windowSize
        )
    {
        const auto initializeSound =
            [&, this]()
        {
            const bool isSoundEnabled = m_ProgramConfig->GetSoundEnabled();
            if (!isSoundEnabled)
                return;

            m_WinSound = soundLoader->LazyLoadSound("win.wav");
            m_LoseSound = soundLoader->LazyLoadSound("lose.wav");
            m_EasterEggSound1 = soundLoader->LazyLoadSound("egg1.wav");
            m_EasterEggSound2 = soundLoader->LazyLoadSound("egg2.wav");
        };

        const auto initializeInput =
            [&, this]()
            {
                const auto coordTransformer = m_CoordinateTransformer.get();
                unique_ptr<InputManager> inputManager =
                {
                    std::make_unique<PlayableInputManager>(coordTransformer)
                };
                FinishConstruction(std::move(inputManager));
            };

		const auto initializeGameEndMenus =
			[&, this]()
			{
				m_WinMenu = std::make_unique<WinMenu>(
					resetCallback,
					m_CoordinateTransformer.get(),
					m_InputManager.get(),
					m_ProgramConfig,
					imageTextureLoader,
					textTextureLoader,
					m_CameraEmpty.get(),
					font
				);
				m_LoseMenu = std::make_unique<LoseMenu>(
					resetCallback,
					m_CoordinateTransformer.get(),
					m_InputManager.get(),
					m_ProgramConfig,
					imageTextureLoader,
					textTextureLoader,
					m_CameraEmpty.get(),
					font
				);
			};

		const auto initializeGUIButtons =
			[&, this]()
			{
				m_TopRightResetButton = std::make_unique<TopRightResetButton>(
					m_CoordinateTransformer.get(),
					m_InputManager.get(),
					m_ProgramConfig,
					imageTextureLoader,
					resetCallback,
					m_CameraEmpty.get()
				);
                m_MuteButton = std::make_unique<MuteButton>(
					programIsMuted,
					m_CoordinateTransformer.get(),
					m_InputManager.get(),
                    m_ProgramConfig,
                    imageTextureLoader,
                    muteUnmuteCallback,
                    m_CameraEmpty.get()
                );
			};

        initializeSound();
        initializeInput();
        initializeGameEndMenus();
        initializeGUIButtons();
    }

    void PlayableGame::Update()
    {
		const auto playOutcomeSound =
			[this]()
			{
				const bool isSoundEnabled = m_ProgramConfig->GetSoundEnabled();
				if (!isSoundEnabled)
					return;

				const float easterEggTest = GetRandomFloat();

				if (easterEggTest < powf(10, -3))
					PlaySound(*m_EasterEggSound2);
				else if (easterEggTest < powf(10, -2))
					PlaySound(*m_EasterEggSound1);

				else if (m_Outcome == Outcome::Winner)
					PlaySound(*m_WinSound);
				else if (m_Outcome == Outcome::Loser)
					PlaySound(*m_LoseSound);
				else
				{
					throw std::runtime_error(
						"Game::Update(): "
						"Unable to determine which Sound to play "
						"after getting the GameOutcome. "
					);
				}
			};

		const auto getRelevantEndMenu =
			[this]() -> EndMenu*
			{
				switch (m_Outcome)
				{
					case Outcome::Winner:
						return m_WinMenu.get();

					case Outcome::Loser:
						return m_LoseMenu.get();

					case Outcome::None:
					default:
						return nullptr;
				}
			};

		const auto chooseOutcome =
			[&, this](const Outcome chosenOutcome)
			{
				m_Outcome = chosenOutcome;
				playOutcomeSound();

				EndMenu* const endMenu = getRelevantEndMenu();
				if (endMenu)
					endMenu->Enable();

				m_TopRightResetButton->SlideOut();
			};

		const auto checkForOutcome =
			[&, this]()
			{
				if (m_Outcome != Outcome::None)
					return;

				const float playerTop =
				{
					m_Player->GetEdgePosition(Direction::Up)
				};
				const float playerBottom =
				{
					m_Player->GetEdgePosition(Direction::Down)
				};

				if (playerBottom > GetWorldEdge(Direction::Up))
					chooseOutcome(Outcome::Winner);

				else if (playerTop < GetWorldEdge(Direction::Down))
					chooseOutcome(Outcome::Loser);

				// losing condition:
				// if the player stays stationary for too long without fuel
				const bool isStationary = m_Player->IsStationary();
				const bool fuelIsEmpty = m_Player->IsOutOfFuel();

				if (isStationary && fuelIsEmpty)
					chooseOutcome(Outcome::Loser);
			};

        const auto updateMiscObjects =
            [this]()
            {
                m_WinMenu->Update();
                m_LoseMenu->Update();
                m_TopRightResetButton->Update();
                m_MuteButton->Update();
            };

        Game::Update();

        checkForOutcome();
        updateMiscObjects();
    }

    void PlayableGame::Draw() const
    {
        Game::Draw();

        // Oct. 5th, 2025:
        //
        // TO-DO: if the PlayableGame class ever needs to draw something
        // below ANYTHING in the Game class, this will literally not be
        // possible.
        // a fix may be needed eventually
        //
        // - Andrew Corvec
        m_WinMenu->Draw();
        m_LoseMenu->Draw();
        m_TopRightResetButton->Draw();
        m_MuteButton->Draw();
    }
}
