#include "search.h"
// 参考
// https://docs.microsoft.com/zh-cn/windows/win32/toolhelp/taking-a-snapshot-and-viewing-processes


// forward declaration
BOOL isTarget(DWORD);


DWORD Search::FindProcessID() {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    // 取得系统中所有进程的快照
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        OutputDebugString(L"CreateToolhelp32Snapshot (of processes)");
        return 0;
    }

    // 在使用之前设置结构体的大小
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // 取得被忽略的第一个进程的相关信息, 如果获取失败的话则退出.
    // CreateToolhelp32Snapshot 函数会忽略系统中的第一个进程, 所以需要这段代码
    //if (!Process32First(hProcessSnap, &pe32))   {
    //    OutputDebugString(L"Process32First");
    //    CloseHandle(hProcessSnap);          // clean the snapshot object
    //    return(FALSE);
    //}

    // 现在浏览进程的快照, 并依次显示每个进程的信息.
    // 在验证进程里面添加一条语句即可获取模块的基址
    //DWORD baseAddress = 0;
    DWORD pid = 0;
    do {
        wchar_t *name = pe32.szExeFile;  // typedef wchar_t WCHAR;
        if (wcscmp(name, L"WeChat.exe") == 0) {
            if (isTarget(pe32.th32ProcessID)) {
                pid = pe32.th32ProcessID;
                break;
            }
        }

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return pid;
}


BOOL isTarget(DWORD dwPID) {
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    // 获取指定进程中所有模块的快照
    // 如果是 64 位程序，则需要使用 TH32CS_SNAPMODULE32
    // 若 TH32CS_SNAPMODULE32 无效，则使用 TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        OutputDebugString(L"CreateToolhelp32Snapshot (of modules)");
        return(FALSE);
    }

    // 在使用之前先设置结构体的大小
    me32.dwSize = sizeof(MODULEENTRY32);

    // 获取被忽略的第一个模块的信息, 如果获取失败则退出
    //if (!Module32First(hModuleSnap, &me32))   {
    //    OutputDebugString(L"Module32First");
    //    // printError(TEXT("Module32First"));  // show cause of failure
    //    CloseHandle(hModuleSnap);           // clean the snapshot object
    //    return(FALSE);
    //}

    // 开始浏览进程中的所有模块, 并显示每个模块的信息
    int find = FALSE;
    do {
        wchar_t *name = me32.szModule;
        // 检查是否为 WeChatWin.dll 模块
        OutputDebugStringW(name);
        if (wcscmp(name, L"WeChatWin.dll") == 0) {
            find = TRUE;
            // 可以在这里获取模块基址
            // baseAddress = (DWORD)me32.modBaseAddr;
            break;
        }

    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
    if (find) {
        return (TRUE);
    }
    else {
        return (FALSE);
    }
}