#include "search.h"
#include "inject.h"
#include <string>
#include <iostream>

#if 0
int main() {
	DWORD pid = Search::FindProcessID();
	if (pid != 0) {
		std::wstring path = L"D:\\CppProgram\\Hooks\\Hooks\\Debug\\Hooks.dll";
		int result = Inject::InjectDLL(pid, path.c_str());
		if (result) {
			printf("Sucessfully inject\n");
		}
		else {
			printf("failed to inject\n");
		}
	}
	else {
		printf("pid is 0\n");
	}
	printf("Press any key to exit.");
	system("pause");
	return 0;
	//DWORD pid = Search::FindProcessID();
	//if (pid != 0) {
	//	printf("微信进程 ID：%x, %d\n", pid, pid);
	//}
	//else {
	//	printf("无法找到微信进程PID\n");
	//}
	//printf("Press any key to exit.\n");
	//system("pause");
	return 0;
}
#endif
