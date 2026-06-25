#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <raylib.h>
#include <fstream>

#include "Settings.h"
#include "Platform.h"
#include "Utils.h"
#include "Logging.h"

namespace BlastOff
{
#if COMPILE_TARGET_EMSCRIPTEN
    EM_JS(char*, getCookies, (), {
        const result = document.cookie;
        return stringToNewUTF8(result);
    });
#endif

    unique_ptr<Settings> Settings::LoadOrDefault(
        const Vector2f aspectRatio,
        const int windowSizeIncrement
    )
    {
#if !COMPILE_TARGET_EMSCRIPTEN
        unique_ptr<Settings> attempt = LoadFromDefaultPath(aspectRatio);
        if (attempt && attempt->IsValid())
            return attempt;
        else
        {
            return std::make_unique<Settings>(
                aspectRatio, 
                windowSizeIncrement
            );
        }
#else
        const auto cookies = getCookies();
        auto attempt = std::make_unique<Settings>(cookies, aspectRatio);
        std::free(cookies);

        if (attempt->IsValid())
            return attempt;
        else
        {
            return std::make_unique<Settings>(
                aspectRatio, 
                windowSizeIncrement
            );
        }
#endif
    }

    bool Settings::IsValid() const
    {
        return m_IsValid;
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

    bool* Settings::IsControlsPopupDismissed()
    {
        return &m_ControlsPopupIsDisabled;
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
#if !COMPILE_TARGET_EMSCRIPTEN
        StringBuffer buffer;
        PrettyWriter<StringBuffer> writer(buffer);
        WriteToJSONWriter(writer);
        const string prettyJson = buffer.GetString();

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
            Logging::LogWarning(message.c_str());
            return;
        }

        output << prettyJson;
        if (!output)
        {
            const string message = getErrorMessage();
            Logging::LogWarning(message.c_str());
            return;
        }

        output.close();
#else // emscripten/web target
        const auto setCookie = [](const string& cookie)
        {
            EM_ASM({
                document.cookie = UTF8ToString($0);
            }, cookie.c_str());
        };

        [&, this]
        {
            string cookie = std::format("audioIsMuted={}", m_AudioIsMuted);
            setCookie(cookie);
        }();

        [&, this]
        {
            string cookie = std::format(
                "controlsPopupIsDisabled={}", 
                m_ControlsPopupIsDisabled
            );
            setCookie(cookie);
        }();

        [&, this]
        {
            string cookie = std::format("audioVolume={}", m_AudioVolume);
            setCookie(cookie);
        }();

        [&, this]
        {
            string cookie = std::format(
                "windowPosition={}", 
                m_WindowPosition.ToJSON()
            );
            setCookie(cookie);
        }();

        [&, this]
        {
            string cookie = std::format("windowSize={}", m_WindowSize.ToJSON());
            setCookie(cookie);
        }();
#endif
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
                const float multiplier = GetWindowHeightMultiplier();
                const float scaledHeight = m_ScreenSize.y * multiplier;
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

        UpdateScreenSize();

        calculateWindowSize();
        calculateWindowPosition();

        ApplyVolume();
    }

    Settings::Settings(const Document& document, const Vector2f aspectRatio)  :
        m_AspectRatio(aspectRatio)
    {
        UpdateScreenSize();

        const Value& audioIsMuted = document["audioIsMuted"];
        const Value& controlsPopupIsDisabled = 
        {
            document["controlsPopupIsDisabled"]
        };
        const Value& audioVolume = document["audioVolume"];
        const Value& windowPosition = document["windowPosition"];
        const Value& windowSize = document["windowSize"];

        if (audioIsMuted.IsBool())
            m_AudioIsMuted = audioIsMuted.GetBool();
        else
            m_IsValid = false;

        if (controlsPopupIsDisabled.IsBool())
            m_ControlsPopupIsDisabled = controlsPopupIsDisabled.GetBool();
        else
            m_IsValid = false;

        if (audioVolume.IsFloat())
            m_AudioVolume = audioVolume.GetFloat();
        else
            m_IsValid = false;

        const auto windowPosition2 = Vector2i::FromJSONValue(windowPosition);
        const auto windowSize2 = Vector2i::FromJSONValue(windowSize);

        if (windowPosition2)
            m_WindowPosition = *windowPosition2;
        else
            m_IsValid = false;

        if (windowSize2)
            m_WindowSize = *windowSize2;
        else
            m_IsValid = false;
    }

    Settings::Settings(const string& cookies, const Vector2f aspectRatio) :
        m_AspectRatio(aspectRatio)
    {
        const auto getCookie = 
            [&](const char* const name)
            {
                const auto cookieArr = SplitString(cookies.c_str(), ';');
                for (auto token : cookieArr)
                {
                    token = TrimWhitespace(token.c_str());
                    const auto pair = SplitString(token.c_str(), '=');
                    if (pair[0] == name)
                        return pair[1];
                }
                return string("");
            };

        UpdateScreenSize();

        const auto audioIsMuted = getCookie("audioIsMuted");
        const string controlsPopupIsDisabled = 
        {
            getCookie("controlsPopupIsDisabled")
        };
        const auto audioVolume = getCookie("audioVolume");
        const auto windowPosition = getCookie("windowPosition");
        const auto windowSize = getCookie("windowSize");

        const auto audioIsMuted2 = StringToBool(audioIsMuted);
        const auto controlsPopupIsDisabled2 = StringToBool(controlsPopupIsDisabled);
        const auto audioVolume2 = StringToFloat(audioVolume);
        const auto windowPosition2 = Vector2i::FromJSONString(windowPosition);
        const auto windowSize2 = Vector2i::FromJSONString(windowSize);

        if (audioIsMuted2)
            m_AudioIsMuted = *audioIsMuted2;
        else
            m_IsValid = false;

        if (controlsPopupIsDisabled2)
            m_ControlsPopupIsDisabled = *controlsPopupIsDisabled2;
        else
            m_IsValid = false;

        if (audioVolume2)
            m_AudioVolume = *audioVolume2;
        else
            m_IsValid = false;

        if (windowPosition2)
            m_WindowPosition = *windowPosition2;
        else
            m_IsValid = false;

        if (windowSize2)
            m_WindowSize = *windowSize2;
        else
            m_IsValid = false;
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
            Logging::LogWarning(message.c_str());
            return nullptr;
        }

        const string text = readBuffer.str();

        Document document;
        document.Parse(text.c_str());
        return std::make_unique<Settings>(document, aspectRatio);
    }

    void Settings::UpdateScreenSize() 
    {
#if COMPILE_TARGET_EMSCRIPTEN
        const int width = Emscripten::GetScreenWidth();
        const int height = Emscripten::GetScreenHeight();
        m_ScreenSize = { width, height };
#else
        m_ScreenSize = 
        {
            GetScreenWidth(), GetScreenHeight()
        };
#endif
    }

    void Settings::WriteToJSONWriter(Writer<StringBuffer>& writer) const
    {
        writer.StartObject();
        writer.Key("audioIsMuted");
        writer.Bool(m_AudioIsMuted);
        writer.Key("controlsPopupIsDisabled");
        writer.Bool(m_ControlsPopupIsDisabled);
        writer.Key("audioVolume");
        writer.Double((double)m_AudioVolume);
        writer.Key("windowPosition");
        m_WindowPosition.WriteToJSONWriter(writer);
        writer.Key("windowSize");
        m_WindowSize.WriteToJSONWriter(writer);
        writer.EndObject();
    }
}
