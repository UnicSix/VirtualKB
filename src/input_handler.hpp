#include <coroutine>
#include <cstdio>
#include <functional>

#include "minwindef.h"
#include "spdlog/spdlog.h"
#include "stdlib.h"
#include "windef.h"
#include "windows.h"
#include "winnt.h"
class InputHandler {
 private:
  static HHOOK kb_hook;
  static HHOOK mouse_hook;
  static std::function<void(LPWSTR)> keyboard_callback;
  static std::function<void(int)> mouse_callback;
  static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam,
                                       LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
      KBDLLHOOKSTRUCT* kbstruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
      wchar_t keyname[15];
      BYTE kbstate[256] = {0};
      GetKeyboardState(kbstate);
      int result =
          ToUnicodeEx(kbstruct->vkCode, kbstruct->scanCode, kbstate, keyname,
                      _countof(keyname), kbstruct->flags, NULL);
      if (keyboard_callback && result > 0) {
        keyboard_callback(keyname);
      } else {
        spdlog::info("Fail to get keyname");
      }
    }
    return CallNextHookEx(kb_hook, nCode, wParam, lParam);
  }

  static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
      if (wParam == WM_MOUSEMOVE) {
        spdlog::info("Mouse Moving");
        mouse_callback(0);
      } else if (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP) {
        spdlog::info("LMB");
        mouse_callback(1);
      }
    }
    return CallNextHookEx(mouse_hook, nCode, wParam, lParam);
  }

 public:
  static bool Initialize(std::function<void(LPWSTR)> kb_callback,
                         std::function<void(int)> ms_callback) {
    keyboard_callback = kb_callback;
    mouse_callback = ms_callback;
    kb_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc,
                               GetModuleHandle(NULL), 0);
    mouse_hook =
        SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(NULL), 0);
    return kb_hook != NULL && mouse_hook != NULL;
  }

  static void Cleanup() {
    if (kb_hook) {
      UnhookWindowsHookEx(kb_hook);
      kb_hook = NULL;
    }
  }
};
