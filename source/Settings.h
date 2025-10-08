#pragma once

#include "Utils.h"
#include "Graphics.h"

#include <glaze/glaze.hpp>

namespace BlastOff
{
    struct ReflectableSettings
    {
        float audioVolume = 0;
        bool audioIsMuted = 0;
        array<int, 2> windowSize = {};
        array<int, 2> windowPosition = {};
    };

    static_assert(glz::reflectable<ReflectableSettings>);

    struct Settings
    {
        using Reflectable = ReflectableSettings;

        static unique_ptr<Settings> LoadFromDefaultPath();

        void SaveToDefaultPath() const;

        float GetAudioVolume() const;
        bool IsAudioMuted() const;

        Vector2i GetWindowSize() const;
        Vector2i GetWindowPosition() const;

        Settings(const Vector2f aspectRatio);
        Settings(const Reflectable& equivalent);
        Settings(const char* const loadPath);

    private:
        static const char* const c_DefaultPath;
        static const Vector2i c_MinWindowSize;

        float m_AudioVolume = 1;
        bool m_AudioIsMuted = false;
        
        Vector2i m_WindowSize = Vector2i::Zero();
        Vector2i m_WindowPosition = Vector2i::Zero();

        Reflectable ToReflectable();
    };
}