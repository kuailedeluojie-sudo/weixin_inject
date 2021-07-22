#include "search.h"
// �ο�
// https://docs.microsoft.com/zh-cn/windows/win32/toolhelp/taking-a-snapshot-and-viewing-processes


// forward declaration
BOOL isTarget(DWORD);


DWORD Search::FindProcessID() {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    // ȡ��ϵͳ�����н��̵Ŀ���
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        OutputDebugString(L"CreateToolhelp32Snapshot (of processes)");
        return 0;
    }

    // ��ʹ��֮ǰ���ýṹ��Ĵ�С
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // ȡ�ñ����Եĵ�һ�����̵������Ϣ, �����ȡʧ�ܵĻ����˳�.
    // CreateToolhelp32Snapshot ���������ϵͳ�еĵ�һ������, ������Ҫ��δ���
    //if (!Process32First(hProcessSnap, &pe32))   {
    //    OutputDebugString(L"Process32First");
    //    CloseHandle(hProcessSnap);          // clean the snapshot object
    //    return(FALSE);
    //}

    // ����������̵Ŀ���, ��������ʾÿ�����̵���Ϣ.
    // ����֤�����������һ����伴�ɻ�ȡģ��Ļ�ַ
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

    // ��ȡָ������������ģ��Ŀ���
    // ����� 64 λ��������Ҫʹ�� TH32CS_SNAPMODULE32
    // �� TH32CS_SNAPMODULE32 ��Ч����ʹ�� TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        OutputDebugString(L"CreateToolhelp32Snapshot (of modules)");
        return(FALSE);
    }

    // ��ʹ��֮ǰ�����ýṹ��Ĵ�С
    me32.dwSize = sizeof(MODULEENTRY32);

    // ��ȡ�����Եĵ�һ��ģ�����Ϣ, �����ȡʧ�����˳�
    //if (!Module32First(hModuleSnap, &me32))   {
    //    OutputDebugString(L"Module32First");
    //    // printError(TEXT("Module32First"));  // show cause of failure
    //    CloseHandle(hModuleSnap);           // clean the snapshot object
    //    return(FALSE);
    //}

    // ��ʼ��������е�����ģ��, ����ʾÿ��ģ�����Ϣ
    int find = FALSE;
    do {
        wchar_t *name = me32.szModule;
        // ����Ƿ�Ϊ WeChatWin.dll ģ��
        OutputDebugStringW(name);
        if (wcscmp(name, L"WeChatWin.dll") == 0) {
            find = TRUE;
            // �����������ȡģ���ַ
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