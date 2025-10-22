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

namespace BlastOff
{
	namespace
	{
#if COMPILE_TARGET_LINUX
		namespace Linux
		{
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
			optional<CursorPosition> GetCursorPosition()
			{
				return CursorPosition{ 0, 0 };
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

	optional<CursorPosition> GetCursorPosition()
	{
#if COMPILE_TARGET_WINDOWS
		return Windows::GetCursorPosition();
#elif COMPILE_TARGET_EMSCRIPTEN
		return Emscripten::GetCursorPosition();
#elif COMPILE_TARGET_LINUX
		return Linux::GetCursorPosition();
#endif
	}
}
