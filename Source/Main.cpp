#include <Windows.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <fstream>

#include "Libs/nlohmann/json.hpp"
using json = nlohmann::json;

std::string configFileName = "LauncherConfig.json";

int main(int argc, char** argv, char** envp)
{
    std::ifstream configFile(configFileName);
    json config = json::parse(configFile);

    std::string targetName = config["Target"];
    std::vector<std::string> dlls = config["DLLs"];

    std::cout << "Target: " << targetName << std::endl;
    std::cout << "DLLs: " << std::endl;
    for (const std::string& dll : dlls)
    {
        std::cout << "  " << dll << std::endl;
    }

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcessA(targetName.c_str(), NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
    {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        return 1;
    }

    for (const std::string& dll : dlls)
    {
        LPVOID remoteString = VirtualAllocEx(pi.hProcess, NULL, dll.size() + 1, MEM_COMMIT, PAGE_READWRITE);
        WriteProcessMemory(pi.hProcess, remoteString, dll.c_str(), dll.size() + 1, NULL);
        HANDLE hThread = CreateRemoteThread(pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32"), "LoadLibraryA"), remoteString, 0, NULL);
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        VirtualFreeEx(pi.hProcess, remoteString, 0, MEM_RELEASE);
    }

    ResumeThread(pi.hThread);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
