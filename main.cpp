#include <QCoreApplication>
#include "search.h"
#include "inject.h"
#include <string>
#include <iostream>
#include <QCoreApplication>
#include <tchar.h>
int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);
    DWORD pid = Search::FindProcessID();
    if (pid != 0) {
         wchar_t  lpDllPath[200] = {0};
        QString fileName = QCoreApplication::applicationDirPath();
         _stprintf(lpDllPath,L"%s/Wx_SocketHelper_2.dll",(fileName.toStdString()).c_str());

        int result = Inject::InjectDLL(pid, lpDllPath);
        if (result) {
           ;// printf("Sucessfully inject\n");
        }
        else {
            ;//printf("failed to inject\n");
        }
    }
    else {
       ;// printf("pid is 0\n");
    }
   ;// printf("Press any key to exit.");
   ;// system("pause");
    return 0;
    return a.exec();
}
