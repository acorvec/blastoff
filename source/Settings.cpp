#include "Settings.h"
#include "OperatingSystem.h"
#include "Utils.h"
#include "Logging.h"

#include <glaze/json/prettify.hpp>
#include <raylib.h>
#include <fstream>
#include <stdexcept>

namespace BlastOff
{
    unique_ptr<Settings> Settings::LoadOrDefault(
        const Vector2f aspectRatio,
        const int windowSizeIncrement
    )
    {
        unique_ptr<Settings> attempt = LoadFromDefaultPath(aspectRatio);
        if (attempt)
            return attempt;
        else
        {
            return std::make_unique<Settings>(
                aspectRatio, 
                windowSizeIncrement
            );
        }
    }

    float Settings::GetAudioVolume() const
    {
        return m_AudioVolume;
    }

    bool Settings::IsAudioMuted() const
    {
        return m_AudioIsMuted;
    }

    Vector2i Settings::GetScreenSize() const
    {
        return m_ScreenSize;
    }

    Vector2i Settings::GetWindowPosition() const
    {
        return m_WindowPosition;
    }

    Vector2i Settings::GetWindowSize() const
    {
        return m_WindowSize;
    }

    void Settings::MuteOrUnmute() 
    {
        m_AudioIsMuted = !m_AudioIsMuted;
		ApplyVolume();
    }

    void Settings::ChangeVolume(const float volume)
    {
        if (volume != m_AudioVolume)
        {
            m_AudioVolume = volume;
            ApplyVolume();
        }
    }

    void Settings::ApplyVolume() const
    {
        if (m_AudioIsMuted)
			SetMasterVolume(0);
        else
			SetMasterVolume(m_AudioVolume);
    }

    void Settings::ChangeWindowHeight(const int windowHeight)
    {
        if (windowHeight != m_WindowSize.y)
        {
            const float newWidth = 
            {
                windowHeight * m_AspectRatio.x / m_AspectRatio.y
            };
            m_WindowSize = { (int)roundf(newWidth), windowHeight };

            SetWindowSize(m_WindowSize.x, m_WindowSize.y);
        }
    }

    void Settings::UpdateWindowPosition(const Vector2i windowPosition)
    {
        m_WindowPosition = windowPosition;
    }

    void Settings::SaveToDefaultPath() const
    {
        const Reflectable reflectable = ToReflectable();
        const auto jsonExpected = glz::write_json(reflectable);
        if (!jsonExpected.has_value())
        {
            const char* const message =
            {
                "Unable to generate json from ReflectableSettings object."
            };
            Logging::Log(message);
            BreakProgram();
        }

        const string json = jsonExpected.value();
        const string prettyJson = glz::prettify_json(json);

        const auto getErrorMessage = 
            []() -> string
            {
                return 
                    "Unable to write json to path "
                    "\"" + string(c_DefaultPath) + "\".";
            };

        std::ofstream output(c_DefaultPath);
        if (!output)
        {
            const string message = getErrorMessage();
            Logging::Log(message.c_str());
            BreakProgram();
        }

        output << prettyJson;
        if (!output)
        {
            const string message = getErrorMessage();
            Logging::Log(message.c_str());
            BreakProgram();
        }

        output.close();
    }

    const char* const Settings::c_DefaultPath = "settings.json";

    Settings::Settings(
        const Vector2f aspectRatio, 
        const int windowSizeIncrement
    ) :
        m_AspectRatio(aspectRatio)
    {
        const auto calculateWindowSize = 
            [&, this]()
            {
                const float scaledHeight = m_ScreenSize.y * 9 / 10.0f;
                const auto inc = (float)windowSizeIncrement;
                const float roundedY = RoundToFraction(scaledHeight, inc);
                const float scaledWidth = 
                {
                    roundedY * m_AspectRatio.x / m_AspectRatio.y
                };   
                const Vector2f result = { scaledWidth, roundedY };
                m_WindowSize = result.ToVector2i();
            };

        const auto calculateWindowPosition =
            [&, this]()
            {
                const Vector2f unrounded = (m_ScreenSize - m_WindowSize) / 2.0f;
                m_WindowPosition = unrounded.ToVector2i();
            };

        m_ScreenSize = 
        {
            GetScreenWidth(), GetScreenHeight()
        };

        calculateWindowSize();
        calculateWindowPosition();

        ApplyVolume();
    }

    Settings::Settings(
        const Reflectable& equivalent, 
        const Vector2f aspectRatio
    ) :
        m_AspectRatio(aspectRatio)
    {
        m_AudioVolume = equivalent.audioVolume;
        m_AudioIsMuted = equivalent.audioIsMuted;

        m_ScreenSize = 
        {
            GetScreenWidth(), GetScreenHeight()
        };
        m_WindowPosition = 
        {
            equivalent.windowPosition.x,
            equivalent.windowPosition.y 
        };
        m_WindowSize = 
        {
            equivalent.windowSize.x,
            equivalent.windowSize.y
        };
    }

    unique_ptr<Settings> Settings::LoadFromDefaultPath
        (const Vector2f aspectRatio)
    {
        std::ifstream reader(c_DefaultPath);
        if (!reader)
            return nullptr;

        std::stringstream readBuffer;
        readBuffer << reader.rdbuf();
        if (reader.fail())
        {
            const string message = 
            {
                "File at path \"" + string(c_DefaultPath) + "\" "
                "is present, but READING failed."
            };
            Logging::Log(message.c_str());
            BreakProgram();
        }

        const string json = readBuffer.str();
        const auto parseResult = glz::read_json<Reflectable>(json);
        if (!parseResult)
        {
            const string message = 
            {
                "File at path \"" + string(c_DefaultPath) + "\" "
                "is present, but PARSING failed."
            };
            Logging::Log(message.c_str());
            BreakProgram();
        }

        const Reflectable reflectable = parseResult.value();
        return std::make_unique<Settings>(reflectable, aspectRatio);
    }

    ReflectableSettings Settings::ToReflectable() const
    {
        return 
        {
            .audioVolume = m_AudioVolume,
            .audioIsMuted = m_AudioIsMuted,
            .windowPosition = { m_WindowPosition.x, m_WindowPosition.y },
            .windowSize = { m_WindowSize.x, m_WindowSize.y }
        };
    }
}