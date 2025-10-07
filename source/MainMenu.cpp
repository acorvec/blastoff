#include "MainMenu.h"
#include "Graphics.h"
#include "Player.h"
#include "Utils.h"
#include "raylib.h"

#include <memory>

namespace BlastOff
{
    MainMenu::MainMenu(
        const ProgramConfiguration* const programConfig,
        const CoordinateTransformer* const coordTransformer,
        const CameraEmpty* const cameraEmpty,
        ImageTextureLoader* const imageTextureLoader,
        TextTextureLoader* const textTextureLoader,
        SoundLoader* const soundLoader,
        const Callback& playCallback,
        const Callback& settingsCallback,
        const Callback& exitCallback,
        const Font* const font,
        const Vector2i* const windowPosition,
        const Vector2i* const windowSize
    ) :
        m_ProgramConfig(programConfig),
        m_CoordTransformer(coordTransformer),
        m_CameraEmpty(cameraEmpty),
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
                m_InputManager = 
                {
                    std::make_unique<PlayableInputManager>(m_CoordTransformer)
                };
            };

        const auto initializeButtons = 
            [&, this]()
            {
                m_SettingsButton = std::make_unique<SettingsButton>(
                    m_CoordTransformer,
                    m_InputManager.get(),
                    m_ProgramConfig,
                    m_ImageTextureLoader,
                    settingsCallback,
                    m_CameraEmpty
                );
                m_PlayButton = std::make_unique<PlayButton>(
                    m_CoordTransformer,
                    m_InputManager.get(),
                    m_ProgramConfig,
                    m_ImageTextureLoader,
                    playCallback,
                    m_CameraEmpty
                );
                m_ExitButton = std::make_unique<MainMenuExitButton>(
                    m_CoordTransformer,
                    m_InputManager.get(),
                    m_ProgramConfig,
                    m_ImageTextureLoader,
                    exitCallback,
                    m_CameraEmpty
                );
            };

        initializeInput();
        initializeButtons();
    }
    
    void MainMenu::Update()
    {
        m_SettingsButton->Update();
        m_PlayButton->Update();
        m_ExitButton->Update();
    }

    void MainMenu::Draw() const
    {
        m_SettingsButton->Draw();
        m_PlayButton->Draw();
        m_ExitButton->Draw();
    }
}