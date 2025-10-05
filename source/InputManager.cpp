#include "InputManager.h"
#include "OperatingSystem.h"

#include "raylib.h"

namespace BlastOff
{
    InputManager::InputManager
        (const CoordinateTransformer* const coordTransformer) :
        m_CoordTransformer(coordTransformer)
    {

    }

    void InputManager::Update()
    {
        // able to be overridden by base classes
    }


    PlayableInputManager::PlayableInputManager
        (const CoordinateTransformer* const coordTransformer) :
        InputManager(coordTransformer)
    {

    }

    bool PlayableInputManager::GetKeyDown(const int keyEnum) const
    {
        return IsKeyDown(keyEnum);
    }

    bool PlayableInputManager::GetMouseButtonPressed(const int buttonEnum) const
    {
        return IsMouseButtonPressed(buttonEnum);
    }

    Vector2f PlayableInputManager::CalculateMousePosition() const
    {
        const optional<CursorPosition> cursor = GetCursorPosition();
        if (!cursor)
            throw std::runtime_error("GetCursorPosition() failed.");

        const Vector2i screenCoords(*cursor);
        const Vector2f engineMouse =
        {
            m_CoordTransformer->ToEngineCoordinates(screenCoords)
        };
        return engineMouse;
    }


    CutsceneInputManager::CutsceneInputManager(
        const CoordinateTransformer* const coordTransformer,
        const Vector2f* const playerPosition,
        const ProgramConfiguration* const programConfig
    ) :
        InputManager(coordTransformer),
        m_PlayerPosition(playerPosition),
        m_ProgramConfig(programConfig)
    {

    }

    bool CutsceneInputManager::GetKeyDown(const int keyEnum) const
    {
        return keyEnum == KEY_SPACE;
    }

    bool CutsceneInputManager::GetMouseButtonPressed(const int buttonEnum) const
    {
        (void)buttonEnum;
        return false;
    }

    Vector2f CutsceneInputManager::CalculateMousePosition() const
    {
        constexpr Vector2f raiseAmount = { 0, 5 };

        const float piRatio = c_MaxOscillationTick / c_Pi;
        const float oscillationX =
        {
            (sinf(m_OscillationTick / piRatio) * 2) - 1
        };
        constexpr float factor = 4 / 5.0f;
        const Vector2f oscillation = { oscillationX * factor, 0 };

        const Vector2f result =
        {
            (*m_PlayerPosition) + oscillation + raiseAmount
        };
        return result;
    }

    void CutsceneInputManager::Update()
    {
        m_OscillationTick += m_ProgramConfig->GetTargetFrametime();

        const float max = c_MaxOscillationTick;
        if (m_OscillationTick > max)
            m_OscillationTick -= max;
    }

    const float CutsceneInputManager::c_MaxOscillationTick = c_Pi * 2;
}
