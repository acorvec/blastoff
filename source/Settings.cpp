#include "Settings.h"

#include <raylib.h>

namespace BlastOff
{
    float Settings::GetAudioVolume() const
    {
        return m_AudioVolume;
    }

    bool Settings::IsAudioMuted() const
    {
        return m_AudioIsMuted;
    }

    Vector2i Settings::GetWindowSize() const
    {
        return m_WindowSize;
    }

    Vector2i Settings::GetWindowPosition() const
    {
        return m_WindowPosition;
    }

    const char* const Settings::c_DefaultPath = "settings.json";

    Settings::Settings(const Vector2f aspectRatio)
    {
        const Vector2i screenSize = 
        {
            GetScreenWidth(), GetScreenHeight()
        };

        const auto calculateWindowSize = 
            [&, this]()
            {
                const float scaledHeight = screenSize.y * 9 / 10.0f;
                const float scaledWidth = 
                {
                    scaledHeight * aspectRatio.x / aspectRatio.y
                };   
                m_WindowSize = 
                { 
                    (int)roundf(scaledWidth), 
                    (int)roundf(scaledHeight) 
                };
            };

        const auto calculateWindowPosition =
            [&, this]()
            {
                const Vector2f unrounded = (screenSize - m_WindowSize) / 2.0f;
                m_WindowPosition = unrounded.ToVector2i();
            };

        calculateWindowSize();
        calculateWindowPosition();
    }
}