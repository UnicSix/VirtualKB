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
  static std::function<void(LPWSTR)> callback;
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
      if (callback && result > 0) {
        callback(keyname);
      } else {
        spdlog::info("Fail to get keyname");
      }
    }
    return CallNextHookEx(kb_hook, nCode, wParam, lParam);
  }

 public:
  static bool Initialize(std::function<void(LPWSTR)> key_callback) {
    callback = key_callback;
    kb_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc,
                               GetModuleHandle(NULL), 0);
    return kb_hook != NULL;
  }

  static void Cleanup() {
    if (kb_hook) {
      UnhookWindowsHookEx(kb_hook);
      kb_hook = NULL;
    }
  }
};
