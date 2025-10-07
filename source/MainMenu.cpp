#include "MainMenu.h"
#include "Game.h"
#include "Graphics.h"
#include "Player.h"
#include "Utils.h"
#include "raylib.h"

#include <memory>

namespace BlastOff
{
    MainMenu::MainMenu(
        const ProgramConfiguration* const programConfig,
        ImageTextureLoader* const imageTextureLoader,
        TextTextureLoader* const textTextureLoader,
        SoundLoader* const soundLoader,
        const Callback& playCallback,
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
        const auto initializeGraphics = 
            [this]()
            {
                m_CoordinateTransformer = std::make_unique<CoordinateTransformer>(
                    m_WindowSize,
                    m_WindowPosition,
                    &m_CameraPosition                    
                );
                m_CoordinateTransformer->Update();
                const CoordinateTransformer* const coordTransformer = 
                {
                    m_CoordinateTransformer.get()
                };
                m_CameraEmpty = std::make_unique<CameraEmpty>(
                    m_CoordinateTransformer.get(),
                    m_ProgramConfig,
                    &m_CameraPosition
                );
            };

        const auto initializeInput = 
            [this]()
            {
                const CoordinateTransformer* const coordTransformer = 
                {
                    m_CoordinateTransformer.get()
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
                    m_CoordinateTransformer.get(),
                    m_InputManager.get(),
                    m_ProgramConfig,
                    m_ImageTextureLoader,
                    settingsCallback,
                    m_CameraEmpty.get(),
                    m_ProgramConfig->GetTopRightButtonMargins()
                );
                m_PlayButton = std::make_unique<PlayButton>(
                    m_CoordinateTransformer.get(),
                    m_InputManager.get(),
                    m_ProgramConfig,
                    m_ImageTextureLoader,
                    playCallback,
                    m_CameraEmpty.get(),
                    m_ProgramConfig->GetTopRightButtonMargins()
                );
            };

        initializeGraphics();
        initializeInput();
        InitializeCutscene();
        initializeButtons();
    }
    
    void MainMenu::Update()
    {
        m_Cutscene->Update();
        m_SettingsButton->Update();
        m_PlayButton->Update();

        if (m_CutsceneShouldReset)
        {
            InitializeCutscene();
            m_CutsceneShouldReset = false;
        }
    }

    void MainMenu::Draw() const
    {
        m_Cutscene->Draw();
        m_SettingsButton->Draw();
        m_PlayButton->Draw();
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
            m_CoordinateTransformer.get(),
            m_CameraEmpty.get(),
            m_ImageTextureLoader,
            m_TextTextureLoader,
            m_SoundLoader,
            &m_CameraPosition,
            resetCallback,
            m_Font,
            m_WindowPosition,
            m_WindowSize
        );       
    }
}