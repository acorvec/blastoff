#pragma once

#include "Utils.h"

#include <glaze/core/common.hpp>
#include <glaze/glaze.hpp>

namespace BlastOff
{
    struct V2IntReflect
    {
        int x, y;
    };
    static_assert(glz::reflectable<V2IntReflect>);

    struct ReflectableSettings
    {
        float audioVolume = 0;
        bool audioIsMuted = false;
        V2IntReflect windowPosition = {};
        V2IntReflect windowSize = {};
    };
    static_assert(glz::reflectable<ReflectableSettings>);

    struct Settings
    {
        using Reflectable = ReflectableSettings;

        static unique_ptr<Settings> LoadOrDefault(
            const Vector2f aspectRatio,
            const int windowSizeIncrement
        );

        float GetAudioVolume() const;
        bool IsAudioMuted() const;

        Vector2i GetScreenSize() const;
        Vector2i GetWindowPosition() const;
        Vector2i GetWindowSize() const;

        void ChangeWindowHeight(const int windowHeight);

        void UpdateWindowPosition(const Vector2i windowPosition);
        void SaveToDefaultPath() const;

        Settings(
            const Vector2f aspectRatio,
            const int windowSizeIncrement
        );
        Settings(
            const Reflectable& equivalent, 
            const Vector2f aspectRatio
        );

    private:
        static const char* const c_DefaultPath;
        static const Vector2i c_MinWindowSize;

        Vector2f m_AspectRatio = Vector2f::Zero();

        float m_AudioVolume = 1;
        bool m_AudioIsMuted = false;
        
        Vector2i m_ScreenSize = Vector2i::Zero();
        Vector2i m_WindowPosition = Vector2i::Zero();
        Vector2i m_WindowSize = Vector2i::Zero();

        static unique_ptr<Settings> LoadFromDefaultPath
            (const Vector2f aspectRatio);
        Reflectable ToReflectable() const;
    };
}