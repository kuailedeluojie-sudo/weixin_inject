#include "inject.h"

/// @brief 将 DLL 注入到指定的进程中
/// @param dwProcessId 被注入的进程 ID
/// @param lpDllPath DLL 文件路径
/// @return BOOL 类型
BOOL Inject::InjectDLL(DWORD dwProcessId, const wchar_t *lpDllPath) {

	HANDLE hProcess = NULL, hThread = NULL;  // process handle
	LPVOID DllNameAddress = NULL;
	WCHAR optBuf[MAX_PATH] = { 0 };  // output message buffer

	// 获取目标进程句柄
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (hProcess == NULL) {
		wsprintf(optBuf, L"[error]OpenProcess(%d)", GetLastError());
		OutputDebugString(optBuf);
		return FALSE;
	}

	// 计算 DLL 路径所需字节数
	int _ = 1 + wcslen(lpDllPath);  // 宽字符数量
	int charSize = _ * sizeof(wchar_t);

	// 在宿主进程内开辟存放 DLL 路径的内存
	DllNameAddress = VirtualAllocEx(hProcess, NULL, charSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (DllNameAddress == NULL) {
		wsprintf(optBuf, L"[error]VirtualAllocEx(%d)", GetLastError());
		OutputDebugString(optBuf);
		return FALSE;
	}

	// 将 DLL 路径写入开辟的内存
	if (!WriteProcessMemory(hProcess, DllNameAddress, lpDllPath, charSize, NULL)) {
		wsprintf(optBuf, L"[error]WriteProcessMemory(%d)", GetLastError());
		OutputDebugString(optBuf);
		return FALSE;
	}

	// 获取 kernel32.dll 的句柄
	HMODULE hk32 = GetModuleHandle(L"kernel32.dll");
	if (hk32 == NULL) {
		wsprintf(optBuf, L"[error]GetModuleHandle(%d)", GetLastError());
		OutputDebugString(optBuf);
		return FALSE;
	}
	// 获取 LoadLibraryW 在 Kernel32.dll 中的地址
	LPVOID address = (LPVOID)GetProcAddress(hk32, "LoadLibraryW");  // LoadLibraryA 使用的是窄字符，会导致加载失败
	if (address == NULL) {
		wsprintf(optBuf, L"[error]GetProcAddress(%d)", GetLastError());
		OutputDebugString(optBuf);
		return FALSE;
	}

	// 创建远程线程
	hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)address, DllNameAddress, NULL, NULL);
	if (hThread == NULL) {
		wsprintf(optBuf, L"[error]CreateRemoteThread(%d)", GetLastError());
		OutputDebugString(optBuf);
		return FALSE;
	}

	// 等待远程线程结束
	WaitForSingleObject(hThread, INFINITE);

	// 注入 DLL 完毕，关闭进程的句柄
	CloseHandle(hProcess);
	return TRUE;
}