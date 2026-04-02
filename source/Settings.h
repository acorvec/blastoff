#pragma once

#include "Utils.h"

#include <rapidjson/document.h>

namespace BlastOff
{
    using namespace rapidjson;

    struct Settings
    {
        static unique_ptr<Settings> LoadOrDefault(
            const Vector2f aspectRatio,
            const int windowSizeIncrement
        );

        float GetAudioVolume() const;
        bool IsAudioMuted() const;

        Vector2i GetScreenSize() const;
        Vector2i GetWindowPosition() const;
        Vector2i GetWindowSize() const;

        bool* IsControlsPopupDismissed();

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
        Settings(const Document& document, const Vector2f aspectRatio);

    private:
        static const char* const c_DefaultPath;

        Vector2f m_AspectRatio = Vector2f::Zero();

        float m_AudioVolume = 1;
        bool m_AudioIsMuted = false;
        bool m_ControlsPopupIsDisabled = false;
        
        Vector2i m_ScreenSize = Vector2i::Zero();
        Vector2i m_WindowPosition = Vector2i::Zero();
        Vector2i m_WindowSize = Vector2i::Zero();

        static unique_ptr<Settings> LoadFromDefaultPath
            (const Vector2f aspectRatio);

        void UpdateScreenSize();

        void WriteToJSONWriter(Writer<StringBuffer>& writer) const;
    };
}
