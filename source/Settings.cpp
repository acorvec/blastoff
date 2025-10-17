#include "Settings.h"
#include "OperatingSystem.h"
#include "Utils.h"
#include "Logging.h"

#if USE_GLAZE
#include <glaze/json/prettify.hpp>
#else
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#endif

#include <raylib.h>
#include <fstream>

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
#if USE_GLAZE
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
#else
        StringBuffer buffer;
        PrettyWriter<StringBuffer> writer(buffer);
        WriteToJSONWriter(writer);
        const string prettyJson = buffer.GetString();
#endif

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
                const Vector2f unrounded = 
                {
                    (m_ScreenSize - m_WindowSize) / 2.0f
                };
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

#if USE_GLAZE
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
#else
    Settings::Settings(const Document& document, const Vector2f aspectRatio)  :
        m_AspectRatio(aspectRatio)
    {
        m_ScreenSize =
        {
            GetScreenWidth(), GetScreenHeight()
        };

        const Value& audioIsMuted = document["audioIsMuted"];
        m_AudioIsMuted = audioIsMuted.GetBool();

        const Value& audioVolume = document["audioVolume"];
        m_AudioVolume = audioVolume.GetFloat();

        const Value& windowPosition = document["windowPosition"];
        m_WindowPosition = Vector2i::FromJSONValue(windowPosition);

        const Value& windowSize = document["windowSize"];
        m_WindowSize = Vector2i::FromJSONValue(windowSize);
    }
#endif

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

        const string text = readBuffer.str();

#if USE_GLAZE
        const auto parseResult = glz::read_json<Reflectable>(text);
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
#else
        Document document;
        document.Parse(text.c_str());
        return std::make_unique<Settings>(document, aspectRatio);
#endif
    }

#if USE_GLAZE
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
#else
    void Settings::WriteToJSONWriter(Writer<StringBuffer>& writer) const
    {
        writer.StartObject();
        writer.Key("audioIsMuted");
        writer.Bool(m_AudioIsMuted);
        writer.Key("audioVolume");
        writer.Double((double)m_AudioVolume);
        writer.Key("windowPosition");
        m_WindowPosition.WriteToJSONWriter(writer);
        writer.Key("windowSize");
        m_WindowSize.WriteToJSONWriter(writer);
        writer.EndObject();
    }
#endif
}