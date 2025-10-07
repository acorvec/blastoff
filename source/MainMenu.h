#include "Graphics.h"
#include "Player.h"
#include "Utils.h"
#include "Game.h"

namespace BlastOff
{
    struct MainMenu
    {
        MainMenu(
            const ProgramConfiguration* const programConfig,
    		ImageTextureLoader* const imageTextureLoader,
	    	TextTextureLoader* const textTextureLoader,
    		SoundLoader* const soundLoader,
            const Callback& playCallback,
			const Callback& settingsCallback,
	    	const Font* const font,
		    const Vector2i* const windowPosition,
    		const Vector2i* const windowSize
        );

        void Update();
        void Draw() const;

    private:
        bool m_CutsceneShouldReset = false;
        Vector2f m_CameraPosition = Vector2f::Zero();

        unique_ptr<Cutscene> m_Cutscene = nullptr;
        unique_ptr<Button> m_PlayButton = nullptr;
        unique_ptr<Button> m_SettingsButton = nullptr;
        unique_ptr<InputManager> m_InputManager = nullptr;
        unique_ptr<CoordinateTransformer> m_CoordinateTransformer = nullptr;
        unique_ptr<CameraEmpty> m_CameraEmpty = nullptr;

        const ProgramConfiguration* m_ProgramConfig = nullptr;
        const Font* m_Font = nullptr;
        const Vector2i* m_WindowPosition = nullptr;
        const Vector2i* m_WindowSize = nullptr;
        
        ImageTextureLoader* m_ImageTextureLoader = nullptr;
        TextTextureLoader* m_TextTextureLoader = nullptr;
        SoundLoader* m_SoundLoader = nullptr;

        void InitializeCutscene();
    };
}
