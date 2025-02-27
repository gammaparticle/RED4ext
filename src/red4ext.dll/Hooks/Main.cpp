#include "stdafx.hpp"
#include "Hooks/Main.hpp"
#include "App.hpp"
#include "REDhook.hpp"

namespace
{
int64_t _Main();
REDhook<decltype(&_Main)> Main_Func;

int64_t _Main()
{
    auto app = App::Get();

    app->Init();

    auto pluginsDir = app->GetPluginsDirectory();
    auto pluginsManager = app->GetPluginsManager();
    pluginsManager->PreloadAll(pluginsDir);

    auto result = Main_Func();
    app->Shutdown();

    return result;
}
} // namespace

void Main::Attach()
{
    new (&Main_Func)
        REDhook<decltype(&_Main)>({0x40, 0x53, 0x48, 0x81, 0xEC, 0xC0, 0x01, 0x00, 0x00, 0xFF, 0x15, 0xCC, 0xCC,
                                   0xCC, 0xCC, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC, 0xE8, 0xCC, 0xCC, 0xCC, 0xCC},
                                  &_Main, 1);

    auto result = Main_Func.Attach();
    if (result != NO_ERROR)
    {
        auto message = fmt::format(L"Could not attach hook for main function, attach returned {}.\n\nProcess will exit "
                                   L"to prevent further problems.",
                                   result);
        MessageBox(nullptr, message.c_str(), L"RED4ext", MB_ICONERROR | MB_OK);

        ExitProcess(1);
    }
}

void Main::Detach()
{
    Main_Func.Detach();
}
