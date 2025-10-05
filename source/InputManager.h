#include "Graphics.h"
#include "ProgramConfiguration.h"
#include "Utils.h"

namespace BlastOff
{
    // Oct. 5th, 2025:
    //
    // NONE of this code is final. probably contains lots of missing features.
    // this is because the code has only been used in BlastOff at this point.
    // expect this to be finished some time before the next decade :)
    //
    // - Andrew C.

    struct InputManager
    {
        InputManager(const CoordinateTransformer* const coordTransformer);
        virtual ~InputManager()
        {

        }

        virtual bool GetKeyDown(const int keyEnum) const = 0;
        virtual bool GetMouseButtonPressed(const int buttonEnum) const = 0;
        virtual Vector2f CalculateMousePosition() const = 0;

        virtual void Update();

    protected:
        const CoordinateTransformer* const m_CoordTransformer = nullptr;
    };

    struct PlayableInputManager : public InputManager
    {
        PlayableInputManager
            (const CoordinateTransformer* const coordTransformer);

        bool GetKeyDown(const int keyEnum) const override;
        bool GetMouseButtonPressed(const int buttonEnum) const override;
        Vector2f CalculateMousePosition() const override;
    };

    struct CutsceneInputManager : public InputManager
    {
        CutsceneInputManager(
            const CoordinateTransformer* const coordTransformer,
            const Vector2f* const playerPosition,
            const ProgramConfiguration* const programConfig
        );

        bool GetKeyDown(const int keyEnum) const override;
        bool GetMouseButtonPressed(const int buttonEnum) const override;
        Vector2f CalculateMousePosition() const override;

        void Update() override;

    protected:
        static const float c_MaxOscillationTick;

        const Vector2f* m_PlayerPosition;
        const ProgramConfiguration* m_ProgramConfig;

        float m_OscillationTick = 0;
    };
}
