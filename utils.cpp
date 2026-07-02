#include "utils.h"

#include <TlHelp32.h>
#include <Windows.h>

#include <cstdint>
#include <cwchar>

std::uintptr_t get_client_dll_base_address(HANDLE snapshot) noexcept {
    MODULEENTRY32W moduleEntry{};
    moduleEntry.dwSize = sizeof(moduleEntry);

    BOOL wasCopied = Module32FirstW(snapshot, &moduleEntry);
    while (wasCopied) {
        if (std::wcscmp(L"client.dll", moduleEntry.szModule) == 0) {
            return reinterpret_cast<std::uintptr_t>(moduleEntry.modBaseAddr);
        }

        wasCopied = Module32NextW(snapshot, &moduleEntry);
    }

    return 0;
}
