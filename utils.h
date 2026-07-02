#ifndef UTILS_H
#define UTILS_H

#include <Windows.h>

#include <cstdint>

/// <summary>
/// Gets the client.dll base address from a process module snapshot.
/// </summary>
/// <param name="snapshot">
/// A handle created from CreateToolhelp32Snapshot.
/// </param>
/// <returns>The address if successful or 0 if not found.</returns>
std::uintptr_t get_client_dll_base_address(HANDLE snapshot) noexcept;

#endif
