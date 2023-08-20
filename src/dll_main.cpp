#include <windows.h>

#include "dll.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      [[maybe_unused]] LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        auto mh_thread_handle =
            CreateThread(nullptr, 0, main_thread, hModule, 0, nullptr);
        if (mh_thread_handle) {
            CloseHandle(mh_thread_handle);
        }
    }

    return TRUE;
}
