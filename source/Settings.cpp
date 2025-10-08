#include "Settings.h"
#include "Utils.h"

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
        unique_ptr<Settings> attempt = LoadFromDefaultPath();
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
            throw std::runtime_error(message);
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
            const string errorMessage = getErrorMessage();
            throw std::runtime_error(errorMessage);
        }

        output << prettyJson;
        if (!output)
        {
            const string errorMessage = getErrorMessage();
            throw std::runtime_error(errorMessage);
        }

        output.close();
    }

    const char* const Settings::c_DefaultPath = "settings.json";

    Settings::Settings(
        const Vector2f aspectRatio, 
        const int windowSizeIncrement
    )
    {
        const auto calculateWindowSize = 
            [&, this]()
            {
                const float scaledHeight = m_ScreenSize.y * 9 / 10.0f;
                const auto inc = (float)windowSizeIncrement;
                const float roundedY = RoundToFraction(scaledHeight, inc);
                const float scaledWidth = 
                {
                    roundedY * aspectRatio.x / aspectRatio.y
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
    }

    Settings::Settings(const Reflectable& equivalent)
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

    unique_ptr<Settings> Settings::LoadFromDefaultPath()
    {
        std::ifstream reader(c_DefaultPath);
        if (!reader)
            return nullptr;

        std::stringstream readBuffer;
        readBuffer << reader.rdbuf();
        if (reader.fail())
        {
            throw std::runtime_error(
                "File at path \"" + string(c_DefaultPath) + "\" "
                "is present, but READING failed."
            );
        }

        const string json = readBuffer.str();
        const auto parseResult = glz::read_json<Reflectable>(json);
        if (!parseResult)
        {
            throw std::runtime_error(
                "File at path \"" + string(c_DefaultPath) + "\" "
                "is present, but PARSING failed."
            );
        }

        const Reflectable reflectable = parseResult.value();
        return std::make_unique<Settings>(reflectable);
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