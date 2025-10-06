#include "Graphics.h"
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
	    	const Font* const font,
		    const Vector2i* const windowPosition,
    		const Vector2i* const windowSize
        );

        void Update();
        void Draw() const;

    private:
        bool m_CutsceneShouldReset = false;
        unique_ptr<Cutscene> m_Cutscene;

        const ProgramConfiguration* m_ProgramConfig;
        const Font* m_Font;
        const Vector2i* m_WindowPosition;
        const Vector2i* m_WindowSize;
        
        ImageTextureLoader* m_ImageTextureLoader;
        TextTextureLoader* m_TextTextureLoader;
        SoundLoader* m_SoundLoader;

        void InitializeCutscene();
    };
}
