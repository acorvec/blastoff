#pragma once

#include "Utils.h"
#include "JSONDefs.h"

// TODO: glaze isn't built for MSVC. 
// how can we bring glaze performance to Windows?
#if USE_GLAZE
#include <glaze/core/common.hpp>
#include <glaze/glaze.hpp>
#else
#include <rapidjson/document.h>
#endif

namespace BlastOff
{
#if USE_GLAZE
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
#else
    using namespace rapidjson;
#endif

    struct Settings
    {
#if USE_GLAZE
        using Reflectable = ReflectableSettings;
#endif

        static unique_ptr<Settings> LoadOrDefault(
            const Vector2f aspectRatio,
            const int windowSizeIncrement
        );

        float GetAudioVolume() const;
        bool IsAudioMuted() const;

        Vector2i GetScreenSize() const;
        Vector2i GetWindowPosition() const;
        Vector2i GetWindowSize() const;

        void MuteOrUnmute();
        void ChangeVolume(const float volume);
        void ChangeWindowHeight(const int windowHeight);

        void ApplyVolume() const;

        void UpdateWindowPosition(const Vector2i windowPosition);
        void SaveToDefaultPath() const;

        Settings(
            const Vector2f aspectRatio,
            const int windowSizeIncrement
        );
#if USE_GLAZE
        Settings(const Reflectable& equivalent, const Vector2f aspectRatio);
#else
        Settings(const Document& document, const Vector2f aspectRatio);
#endif

    private:
        static const char* const c_DefaultPath;

        Vector2f m_AspectRatio = Vector2f::Zero();

        float m_AudioVolume = 1;
        bool m_AudioIsMuted = false;
        
        Vector2i m_ScreenSize = Vector2i::Zero();
        Vector2i m_WindowPosition = Vector2i::Zero();
        Vector2i m_WindowSize = Vector2i::Zero();

        static unique_ptr<Settings> LoadFromDefaultPath
            (const Vector2f aspectRatio);

#if USE_GLAZE
        Reflectable ToReflectable() const;
#else
        void WriteToJSONWriter(Writer<StringBuffer>& writer) const;
#endif
    };
}