#include "utils.h"

#include <TlHelp32.h>
#include <Windows.h>

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace {
constexpr wchar_t kWindowTitle[] = L"Left 4 Dead 2 - Direct3D 9";

constexpr std::uintptr_t kPlayerBaseOffset = 0x726BD8;
constexpr std::uintptr_t kMFlagsOffset = 0xF0;

constexpr WPARAM kSpaceKey = ' ';
constexpr LPARAM kJumpLParam = 0x390000;
constexpr DWORD kIdleSleepMs = 5;

constexpr std::uint32_t kOnGroundFlags[] = { 0x80, 0x82, 0x280, 0x282 };

struct HandleCloser {
    void operator()(HANDLE handle) const noexcept {
        if (handle != nullptr && handle != INVALID_HANDLE_VALUE) {
            CloseHandle(handle);
        }
    }
};

using UniqueHandle = std::unique_ptr<std::remove_pointer_t<HANDLE>, HandleCloser>;

UniqueHandle make_unique_handle(HANDLE handle) noexcept {
    return UniqueHandle(handle);
}

bool is_space_pressed() noexcept {
    return (GetAsyncKeyState(static_cast<int>(kSpaceKey)) & 0x8000) != 0;
}

bool is_on_ground(std::uint32_t flags) noexcept {
    for (const std::uint32_t groundFlag : kOnGroundFlags) {
        if (flags == groundFlag) {
            return true;
        }
    }

    return false;
}

template <typename T>
bool read_process_value(HANDLE process, std::uintptr_t address, T& value) noexcept {
    SIZE_T bytesRead = 0;
    return ReadProcessMemory(
        process,
        reinterpret_cast<LPCVOID>(address),
        &value,
        sizeof(T),
        &bytesRead) != FALSE &&
        bytesRead == sizeof(T);
}

HWND wait_for_game_window() {
    HWND gameWindow = nullptr;
    while ((gameWindow = FindWindowW(nullptr, kWindowTitle)) == nullptr) {
        Sleep(1000);
    }

    return gameWindow;
}

DWORD get_window_process_id(HWND window) {
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);

    if (processId == 0) {
        throw std::runtime_error("Could not get process id!");
    }

    return processId;
}

UniqueHandle open_game_process(DWORD processId) {
    UniqueHandle process = make_unique_handle(OpenProcess(PROCESS_VM_READ, FALSE, processId));
    if (!process) {
        throw std::runtime_error("Could not open process!");
    }

    return process;
}

std::uintptr_t wait_for_client_dll_base(DWORD processId) {
    std::uintptr_t clientBaseAddress = 0;

    while (clientBaseAddress == 0) {
        UniqueHandle snapshot = make_unique_handle(CreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
            processId));

        if (snapshot.get() == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Could not create snapshot!");
        }

        clientBaseAddress = get_client_dll_base_address(snapshot.get());
        if (clientBaseAddress == 0) {
            std::cout << "Game loading...\n";
            Sleep(3000);
        }
    }

    return clientBaseAddress;
}

void hide_console_after_delay() noexcept {
    Sleep(5000);

    if (HWND consoleWindow = GetConsoleWindow()) {
        ShowWindow(consoleWindow, SW_HIDE);
    }
}

void run_bhop_tick(HWND gameWindow, HANDLE process, std::uintptr_t clientBaseAddress) noexcept {
    std::uint32_t playerBasePointer = 0;
    if (!read_process_value(process, clientBaseAddress + kPlayerBaseOffset, playerBasePointer)) {
        return;
    }

    if (playerBasePointer == 0) {
        return;
    }

    std::uint32_t flags = 0;
    if (!read_process_value(process, static_cast<std::uintptr_t>(playerBasePointer) + kMFlagsOffset, flags)) {
        return;
    }

    if (is_on_ground(flags)) {
        SendMessageW(gameWindow, WM_KEYUP, kSpaceKey, kJumpLParam);
    }
    else {
        SendMessageW(gameWindow, WM_KEYDOWN, kSpaceKey, kJumpLParam);
    }
}

int run() {
    std::cout << "Preset base address parameters:\r\n"
        << "  PlayerBase offset = " << std::hex << std::uppercase << kPlayerBaseOffset << "\r\n"
        << "  mFlags offset = " << kMFlagsOffset << "\r\n"
        << "  Windows title = Left 4 Dead 2 - Direct3D 9\r\n  "
        << std::dec;

    std::cout << "Launch the game from Steam.\n";

    HWND gameWindow = wait_for_game_window();
    std::cout << "Game found!\n";

    const DWORD processId = get_window_process_id(gameWindow);
    UniqueHandle process = open_game_process(processId);
    const std::uintptr_t clientBaseAddress = wait_for_client_dll_base(processId);

    std::cout << "Autobhop is now functional!\n";
    hide_console_after_delay();

    while (true) {
        if (!IsWindow(gameWindow)) {
            return EXIT_SUCCESS;
        }

        if (GetForegroundWindow() != gameWindow || !is_space_pressed()) {
            Sleep(kIdleSleepMs);
            continue;
        }

        run_bhop_tick(gameWindow, process.get(), clientBaseAddress);

        // Keep Sleep(0) while actively bhopping: non-zero sleeps can break continuous bhop.
        Sleep(0);
    }
}
} // namespace

int main() {
    try {
        return run();
    }
    catch (const std::exception& exception) {
        std::cerr << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
