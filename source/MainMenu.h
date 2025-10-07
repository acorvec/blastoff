#include "Graphics.h"
#include "Player.h"
#include "Utils.h"
#include "GUI.h"
#include "Sound.h"

namespace BlastOff
{
    struct MainMenu
    {
        MainMenu(
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
        );

        void Update();
        void Draw() const;

    private:
        unique_ptr<InputManager> m_InputManager = nullptr;
        unique_ptr<Button> m_PlayButton = nullptr;
        unique_ptr<Button> m_SettingsButton = nullptr;
        unique_ptr<ExitButton> m_ExitButton = nullptr;

        const ProgramConfiguration* m_ProgramConfig = nullptr;
        const Font* m_Font = nullptr;
        const CoordinateTransformer* m_CoordTransformer = nullptr;
        const CameraEmpty* m_CameraEmpty = nullptr;
        const Vector2i* m_WindowPosition = nullptr;
        const Vector2i* m_WindowSize = nullptr;
        
        ImageTextureLoader* m_ImageTextureLoader = nullptr;
        TextTextureLoader* m_TextTextureLoader = nullptr;
        SoundLoader* m_SoundLoader = nullptr;
    };
}
