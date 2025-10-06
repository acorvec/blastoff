#include "MainMenu.h"
#include "Game.h"
#include "Graphics.h"
#include "Player.h"
#include "Utils.h"

#include <memory>

namespace BlastOff
{
    MainMenu::MainMenu(
        const ProgramConfiguration* const programConfig,
        ImageTextureLoader* const imageTextureLoader,
        TextTextureLoader* const textTextureLoader,
        SoundLoader* const soundLoader,
        const Callback& settingsCallback,
        const Font* const font,
        const Vector2i* const windowPosition,
        const Vector2i* const windowSize
    ) :
        m_ProgramConfig(programConfig),
        m_ImageTextureLoader(imageTextureLoader),
        m_TextTextureLoader(textTextureLoader),
        m_SoundLoader(soundLoader),
        m_Font(font),
        m_WindowPosition(windowPosition),
        m_WindowSize(windowSize)
    {
        const auto initializeInput = 
            [this]()
            {
                const CoordinateTransformer* const coordTransformer = 
                {
                    m_Cutscene->GetCoordinateTransformer()
                };
                m_InputManager = 
                {
                    std::make_unique<PlayableInputManager>(coordTransformer)
                };
            };

        const auto initializeButtons = 
            [&, this]()
            {
                m_SettingsButton = std::make_unique<SettingsButton>(
                    m_Cutscene->GetCoordinateTransformer(),
                    m_InputManager.get(),
                    m_ProgramConfig,
                    m_ImageTextureLoader,
                    settingsCallback,
                    m_Cutscene->GetCameraEmpty(),
                    m_ProgramConfig->GetTopRightButtonMargins()
                );
            };

        InitializeCutscene();
        initializeInput();
        initializeButtons();
    }
    
    void MainMenu::Update()
    {
        m_Cutscene->Update();

        if (m_CutsceneShouldReset)
        {
            InitializeCutscene();
            m_CutsceneShouldReset = false;
        }
    }

    void MainMenu::Draw() const
    {
        m_Cutscene->Draw();
    }

    void MainMenu::InitializeCutscene()
    {
        const auto resetCallback = 
            [this]()
            {
                m_CutsceneShouldReset = true;
            };

        m_Cutscene = std::make_unique<Cutscene>(
            m_ProgramConfig,
            m_ImageTextureLoader,
            m_TextTextureLoader,
            m_SoundLoader,
            resetCallback,
            m_Font,
            m_WindowPosition,
            m_WindowSize
        );       
    }
}