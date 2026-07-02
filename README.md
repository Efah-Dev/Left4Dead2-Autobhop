<div align="center">

# Left4Dead2-Autobhop

Lightweight external **Auto Bunny Hop** utility for **Left 4 Dead 2**.

Automatically performs bunny hopping while the **Space** key is held, providing smooth, consistent movement with minimal system resource usage.

![Platform](https://img.shields.io/badge/Platform-Windows-blue)
![Language](https://img.shields.io/badge/Language-C%2B%2B-00599C)
![IDE](https://img.shields.io/badge/Visual%20Studio-2022%2B-purple)
![License](https://img.shields.io/github/license/Efah-Dev/Left4Dead2-Autobhop)

</div>

---

## ✨ Features

* Lightweight external application
* Modern C++ implementation
* No DLL injection
* Reads game memory using `ReadProcessMemory`
* Automatically detects `client.dll`
* Automatically waits for the game to launch
* Automatically jumps only while the player is on the ground
* Minimal CPU and memory usage
* Easy to build and use

---

## ⚙️ How It Works

The application performs the following steps:

1. Waits for the **Left 4 Dead 2** process to start.
2. Opens the game process with the required permissions.
3. Locates the base address of `client.dll`.
4. Reads the local player's `m_fFlags`.
5. Monitors the **Space** key.
6. Simulates jump input whenever the player is on the ground.

Since the tool is completely external, no code is injected into the game process.

---

## 🚀 Build

Requirements:

* **Visual Studio 2022 or later**
* **Windows x64**

Open the solution:

```text
l4d2-autobhop.sln
```

Select the **Release x64** configuration and build the project.

---

## ▶️ Usage

1. Launch **Left 4 Dead 2**.
2. Start **Left4Dead2-Autobhop**.
3. Wait until the game has fully loaded.
4. Hold the **Space** key.
5. Bunny hopping will be performed automatically.

> The console window hides itself automatically after initialization.

---

## 🔧 Technical Details

This project utilizes:

* Win32 API
* ToolHelp32 Snapshots
* `ReadProcessMemory`
* Window Messaging (`SendMessage`)
* Modern C++ RAII wrappers for Windows handles

The application operates entirely from outside the game process without DLL injection or memory patching.

---

## ⚠️ Disclaimer

This project is provided for educational and research purposes only.

Using third-party software in online games may violate the game's Terms of Service and could result in warnings, restrictions, or permanent account penalties.

The author assumes no responsibility for any misuse of this software.

---

## 📄 License

Licensed under the **MIT License**.

See the **LICENSE** file for more information.
