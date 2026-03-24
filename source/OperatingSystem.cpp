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
				const auto result = emscripten_set_fullscreenchange_callback(
					EMSCRIPTEN_EVENT_TARGET_WINDOW,
					nullptr,
                    EM_FALSE,
					ProcessFullscreenChange
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
			const int cx = EM_ASM_INT({
				let canvas = document.querySelector("canvas");
				let rect = canvas.getBoundingClientRect();
				return rect.left;
			});
			const int cy = EM_ASM_INT({
				let canvas = document.querySelector("canvas");
				let rect = canvas.getBoundingClientRect();
				return rect.top;
			});

			mostRecentCursorPosition =
			{
				rawPos.x - cx, rawPos.y - cy
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

		EM_BOOL ProcessFullscreenChange(
			const int eventType,
			const EmscriptenFullscreenChangeEvent* const event,
			void* const userData
		)
		{
            // this is not working.

			//if (event->isFullscreen)
			//{
			//	const int maxHeight = event->screenHeight;
			//	const int width = (int)(maxHeight * aspectRatio);

			//	setWindowSize(width, maxHeight);
			//}

            return EM_TRUE;
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
}
