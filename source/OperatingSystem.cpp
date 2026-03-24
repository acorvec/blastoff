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
	namespace
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
			EM_BOOL UpdateCursorPosition(
				const int eventType,
				const EmscriptenMouseEvent* const event,
				void* const userData
			);

			void SetUpPlatform(const int fps)
			{
				char* const footer =
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

				[&]()
				{
					const auto result = emscripten_request_pointerlock(
						"#canvas",
						EM_TRUE
					);
					if (result)
					{
						std::fprintf(
							stderr,
							"unable to lock pointer. %s\n",
							footer
						);
					}
				}();

				emscripten_set_main_loop(mainloop, fps, EM_TRUE);
			}

			CursorPosition mostRecentCursorPosition = { 0, 0 };

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

				std::printf("%d\n", cy);

				mostRecentCursorPosition =
				{
					rawPos.x - cx, rawPos.y - cy
				};
				return EM_TRUE;
			}

			optional<CursorPosition> GetCursorPosition()
			{
				return mostRecentCursorPosition;
			}
		}
#endif
	
		string GetFontDirectory()
		{
			return "resource/ttf/";
		}
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
