#include "inject.h"

/// @brief �� DLL ע�뵽ָ���Ľ�����
/// @param dwProcessId ��ע��Ľ��� ID
/// @param lpDllPath DLL �ļ�·��
/// @return BOOL ����
BOOL Inject::InjectDLL(DWORD dwProcessId, const wchar_t *lpDllPath) {

	HANDLE hProcess = NULL, hThread = NULL;  // process handle
	LPVOID DllNameAddress = NULL;
	WCHAR optBuf[MAX_PATH] = { 0 };  // output message buffer

	// ��ȡĿ����̾��
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (hProcess == NULL) {
		wsprintf(optBuf, L"[error]OpenProcess(%d)", GetLastError());
		OutputDebugString(optBuf);
		return FALSE;
	}

	// ���� DLL ·�������ֽ���
	int _ = 1 + wcslen(lpDllPath);  // ���ַ�����
	int charSize = _ * sizeof(wchar_t);

	// �����������ڿ��ٴ�� DLL ·�����ڴ�
	DllNameAddress = VirtualAllocEx(hProcess, NULL, charSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (DllNameAddress == NULL) {
		wsprintf(optBuf, L"[error]VirtualAllocEx(%d)", GetLastError());
		OutputDebugString(optBuf);
		return FALSE;
	}

	// �� DLL ·��д�뿪�ٵ��ڴ�
	if (!WriteProcessMemory(hProcess, DllNameAddress, lpDllPath, charSize, NULL)) {
		wsprintf(optBuf, L"[error]WriteProcessMemory(%d)", GetLastError());
		OutputDebugString(optBuf);
		return FALSE;
	}

	// ��ȡ kernel32.dll �ľ��
	HMODULE hk32 = GetModuleHandle(L"kernel32.dll");
	if (hk32 == NULL) {
		wsprintf(optBuf, L"[error]GetModuleHandle(%d)", GetLastError());
		OutputDebugString(optBuf);
		return FALSE;
	}
	// ��ȡ LoadLibraryW �� Kernel32.dll �еĵ�ַ
	LPVOID address = (LPVOID)GetProcAddress(hk32, "LoadLibraryW");  // LoadLibraryA ʹ�õ���խ�ַ����ᵼ�¼���ʧ��
	if (address == NULL) {
		wsprintf(optBuf, L"[error]GetProcAddress(%d)", GetLastError());
		OutputDebugString(optBuf);
		return FALSE;
	}

	// ����Զ���߳�
	hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)address, DllNameAddress, NULL, NULL);
	if (hThread == NULL) {
		wsprintf(optBuf, L"[error]CreateRemoteThread(%d)", GetLastError());
		OutputDebugString(optBuf);
		return FALSE;
	}

	// �ȴ�Զ���߳̽���
	WaitForSingleObject(hThread, INFINITE);

	// ע�� DLL ��ϣ��رս��̵ľ��
	CloseHandle(hProcess);
	return TRUE;
}