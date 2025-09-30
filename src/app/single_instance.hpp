#include <Windows.h>


struct mutex {
    HANDLE h = nullptr;
    bool isAvailable = true;
    DWORD lastError = ERROR_SUCCESS;
};



mutex InitiateMutex() {
    mutex out;

    out.h = CreateMutexW(
        nullptr,
        TRUE,
        L"Global\\WindowsControllerKeybinder"
    );

    if (GetLastError() == ERROR_INVALID_HANDLE) {
        out.isAvailable = false;
        out.lastError = GetLastError();
    }

    return out;
}