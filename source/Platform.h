#pragma once

#include <optional>
#include <string>
#include <memory>
#include <cstdio>
#include <functional>

#ifdef _WIN32
#define COMPILE_TARGET_WINDOWS _WIN32
#else
#define COMPILE_TARGET_WINDOWS 0
#endif

#define COMPILE_TARGET_LINUX __linux__

#if COMPILE_TARGET_WINDOWS
#define COMPILE_TARGET_DESKTOP 1
#elif COMPILE_TARGET_LINUX
#define COMPILE_TARGET_DESKTOP 1
#else
#define COMPILE_TARGET_DESKTOP 0 
#endif

#ifdef __EMSCRIPTEN__
#define COMPILE_TARGET_EMSCRIPTEN 1
#define COMPILE_TARGET_WEB 1
#else
#define COMPILE_TARGET_EMSCRIPTEN 0
#define COMPILE_TARGET_WEB 0
#endif

#define COMPILE_FEATURE_EXCEPTIONS (!COMPILE_TARGET_EMSCRIPTEN)

#if COMPILE_TARGET_EMSCRIPTEN
#include <emscripten/html5.h>
#endif

namespace BlastOff
{
	using std::optional, std::unique_ptr;
	using std::string;

	struct CursorPosition
	{
		int x;
		int y;
	};

#if COMPILE_TARGET_EMSCRIPTEN
    namespace Emscripten
    {
        EM_BOOL UpdateCursorPosition(
		    const int eventType,
			const EmscriptenMouseEvent* const event,
			void* const userData
		);
        EM_BOOL ProcessKey(
            const int eventType, 
            const EmscriptenKeyboardEvent* const keyEvent, 
            void* const userData
        );
        EM_BOOL ProcessResizeEvent(
            const int eventType,
            const EmscriptenUiEvent* const event,
            void* const userData
        );

        void SetWindowSizeCallback
            (const std::function<void(int, int)>& callback);
        void SetAspectRatio(const float value);

        int GetCanvasLeft();
        int GetCanvasTop();

        int GetWindowLeft();
        int GetWindowTop();

        int GetScreenWidth();
        int GetScreenHeight();

        float GetWindowHeightMultiplier(); 
    }
#endif

	void BreakProgram();

	void SetUpPlatform(const int fps);
	optional<CursorPosition> GetCursorPosition();
    float GetWindowHeightMultiplier();

	string GetFontPath(const char* const name, const char* const extension);
}
