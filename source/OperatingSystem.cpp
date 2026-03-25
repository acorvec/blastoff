#include "OperatingSystem.h"

#if COMPILE_TARGET_WINDOWS
#include "Debug.h"

#define WIN32_LEAN_AND_MEAN true
#include "windows.h"
#include "Winuser.h"
#endif

#if COMPILE_TARGET_LINUX
#include <X11/Xlib.h>
#endif


#if COMPILE_TARGET_EMSCRIPTEN
#include <emscripten/html5.h>
#endif

#if COMPILE_TARGET_EMSCRIPTEN
void mainloop();
#endif

namespace BlastOff
{
#if COMPILE_TARGET_LINUX
	namespace Linux
	{
		void SetUpPlatform(const int fps)
		{
			(void)fps;
		}

		optional<CursorPosition> GetCursorPosition()
		{
			Display* display = XOpenDisplay(nullptr);
			if (!display)
				return std::nullopt;

			int cursorX = 0, cursorY = 0;
			int windowX = 0, windowY = 0;
			Window rootReturn = { 0 }, childReturn = { 0 };
			Window rootWindow = DefaultRootWindow(display);
			unsigned int maskReturn = 0;

			XQueryPointer(
				display,
				rootWindow,
				&rootReturn,
				&childReturn,
				&cursorX,
				&cursorY,
				&windowX,
				&windowY,
				&maskReturn
			);
			XCloseDisplay(display);
			return CursorPosition{ cursorX, cursorY };
		}

        float GetWindowHeightMultiplier()
        {
            return 9 / 10.0f;
        }
	}
#endif

#if COMPILE_TARGET_WINDOWS
	namespace Windows
	{
		void SetUpPlatform(const int fps)
		{
			(void)fps;
		}

		optional<CursorPosition> GetCursorPosition()
		{
			POINT result;
			const bool success = GetCursorPos(&result);
			if (success)
				return CursorPosition{ (int)result.x, (int)result.y };
			else
				return std::nullopt;
		}

        float GetWindowHeightMultiplier()
        {
            return 9 / 10.0f;
        }
	}
#endif

#if COMPILE_TARGET_EMSCRIPTEN
	namespace Emscripten
	{
        std::function<void(int, int)> setWindowSize;
        float aspectRatio = 0;

        CursorPosition mostRecentCursorPosition = { 0, 0 };
	
		void SetUpPlatform(const int fps)
		{
			char footer[] =
			{
				"this is probably an error with the platform "
				"(emscripten)."
			};

			[&]()
			{
				const auto result = emscripten_set_mousemove_callback(
					EMSCRIPTEN_EVENT_TARGET_WINDOW,
					nullptr,
					EM_TRUE,
					UpdateCursorPosition
				);
				if (result)
				{
					std::fprintf(
						stderr,
						"unable to set mouse movement callback. %s\n",
						footer
					);
					BreakProgram();
				}
			}();

			// disable space bar scrolling
			[&]()
			{
				const auto result = emscripten_set_keydown_callback(
					EMSCRIPTEN_EVENT_TARGET_WINDOW,
					nullptr,
					EM_FALSE,
					ProcessKey
				);
				if (result)
				{
					std::fprintf(
						stderr,
						"unable to set keydown callback. %s\n",
						footer
					);
					BreakProgram();
				}
			}();

			[&]()
			{
				const auto result = emscripten_set_resize_callback(
					EMSCRIPTEN_EVENT_TARGET_WINDOW,
					nullptr,
                    EM_FALSE,
					ProcessResizeEvent
				);
				if (result)
				{
					std::fprintf(
						stderr,
						"unable to set fullscreen change callback. %s\n",
						footer
					);
				}
			}();

			emscripten_set_main_loop(mainloop, fps, EM_TRUE);
		}

		EM_BOOL UpdateCursorPosition(
			const int eventType,
			const EmscriptenMouseEvent* const event,
			void* const userData
		)
		{
			const CursorPosition rawPos =
			{
				event->targetX, event->targetY
			};

            const int cx = GetCanvasLeft();
            const int cy = GetCanvasTop();

            const int wx = GetWindowLeft();
            const int wy = GetWindowTop();

			mostRecentCursorPosition =
			{
				rawPos.x - cx + wx, rawPos.y - cy + wy
			};
			return EM_TRUE;
		}

		EM_BOOL ProcessKey(
			const int eventType,
			const EmscriptenKeyboardEvent* const keyEvent,
			void* const userData
		)
		{
			// disable space bar scrolling
			if (keyEvent->keyCode == 32)
				return EM_TRUE;

			return EM_FALSE;
		}

        EM_BOOL ProcessResizeEvent(
            const int eventType,
            const EmscriptenUiEvent* const event,
            void* const userData
        )
        {
            const int maxHeight = GetScreenHeight();
            const float newHeight = maxHeight * GetWindowHeightMultiplier();
            const int width = (int)(newHeight * aspectRatio);

            setWindowSize(width, newHeight);

            return EM_FALSE;
        }

		optional<CursorPosition> GetCursorPosition()
		{
			return mostRecentCursorPosition;
		}
 
        void SetWindowSizeCallback
            (const std::function<void(int, int)>& callback)
        {
            setWindowSize = callback;
        }

        void SetAspectRatio(const float value)
        {
            aspectRatio = value;
        }

        float GetWindowHeightMultiplier()
        {
            return 8 / 10.0f;
        }

        EM_JS(int, _GetCanvasLeft, (), {
			let canvas = document.querySelector("canvas");
			let rect = canvas.getBoundingClientRect();
			return rect.left;
		});
        EM_JS(int, _GetCanvasTop, (), {
			let canvas = document.querySelector("canvas");
			let rect = canvas.getBoundingClientRect();
			return rect.top;
		});

        EM_JS(int, _GetWindowLeft, (), { return window.screenLeft; });
        EM_JS(int, _GetWindowTop, (), { return window.screenTop; });

        EM_JS(int, _GetScreenWidth, (), { return window.innerWidth; });
        EM_JS(int, _GetScreenHeight, (), { return window.innerHeight; });

        int GetCanvasLeft() { return _GetCanvasLeft(); }
        int GetCanvasTop() { return _GetCanvasTop(); }

        int GetWindowLeft() { return _GetWindowLeft(); }
        int GetWindowTop() { return _GetWindowTop(); }

        int GetScreenWidth() { return _GetScreenWidth(); }
        int GetScreenHeight() { return _GetScreenHeight(); }
    }
#endif
	
    string GetFontDirectory()
    {
        return "resource/ttf/";
    }

	void BreakProgram()
	{
#if COMPILE_TARGET_WINDOWS
		__debugbreak();
#else
		__builtin_trap();
#endif
	}

	string GetFontPath(const char* const name, const char* const extension)
	{
		const string directory = GetFontDirectory();
		return directory + name + "." + extension;
	}

	void SetUpPlatform(const int fps)
	{
#if COMPILE_TARGET_WINDOWS
		Windows::SetUpPlatform(fps);
#elif COMPILE_TARGET_LINUX
		Linux::SetUpPlatform(fps);
#elif COMPILE_TARGET_EMSCRIPTEN
		Emscripten::SetUpPlatform(fps);
#endif
	}

    optional<CursorPosition> GetCursorPosition()
	{
#if COMPILE_TARGET_WINDOWS
		return Windows::GetCursorPosition();
#elif COMPILE_TARGET_LINUX
		return Linux::GetCursorPosition();
#elif COMPILE_TARGET_EMSCRIPTEN
		return Emscripten::GetCursorPosition();
#endif
	}

    float GetWindowHeightMultiplier()
    {
 #if COMPILE_TARGET_WINDOWS
		return Windows::GetWindowHeightMultiplier();
#elif COMPILE_TARGET_LINUX
		return Linux::GetWindowHeightMultiplier();
#elif COMPILE_TARGET_EMSCRIPTEN
		return Emscripten::GetWindowHeightMultiplier();
#endif       
    }
}
