#include "MainMenu.h"
#include "Game.h"
#include "Utils.h"
#include <memory>

namespace BlastOff
{
    MainMenu::MainMenu(
        const ProgramConfiguration* const programConfig,
        ImageTextureLoader* const imageTextureLoader,
        TextTextureLoader* const textTextureLoader,
        SoundLoader* const soundLoader,
        const Font* const font,
        const Vector2i* const windowPosition,
        const Vector2i* const windowSize
    )
    {
        InitializeCutscene();
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