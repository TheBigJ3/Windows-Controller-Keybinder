#include <Windows.h>
#include <iostream>
#include <winrt/base.h>                      // <-- needed
#include <winrt/Windows.Foundation.h>

#include "single_instance.hpp"
#include "../../utils/path_util.hpp"
#include "../loader/config_loader.hpp"
#include "../input/bind_listener.hpp"

#pragma comment(lib, "windowsapp")

void AllocateAndRedirectConsole() {
    // Allocate a new console for the calling process
    if (AllocConsole()) {
        // Redirect standard output to the new console
        FILE* pCout;
        // freopen_s(&pCout, "CONOUT$", "w", stdout);

        // Redirect standard error to the new console
        FILE* pCerr;
        // freopen_s(&pCerr, "CONOUT$", "w", stderr);

        // Redirect standard input from the new console
        FILE* pCin;
        // freopen_s(&pCin, "CONIN$", "r", stdin);

        // Clear the state of the C++ standard streams
        // This is important if they were used before redirection
        std::cout.clear();
        std::clog.clear();
        std::cerr.clear();
        std::cin.clear();
    }
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nShowCmd) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    AllocateAndRedirectConsole();
    mutex mutextInstance = InitiateMutex();

    std::cout << "Mutext isAvailable : " << mutextInstance.isAvailable << std::endl;

    if (!mutextInstance.isAvailable) {
        return 0;
    }

    

    auto cfgPath = ResolveConfigPath();
    AppConfig cfg = LoadConfig(cfgPath);

    BindListener listener(cfg);

    while (true) {
        listener.Update();
        Sleep(1);
    }


    Sleep(5000);

    return 0;
}