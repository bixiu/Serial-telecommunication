#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <tchar.h>
using namespace std;

bool openport(TCHAR * portname);

int main()
{


	system("pause");
	return 0;
}

bool openport(TCHAR * portname)
//"char *" ���͵�ʵ���� "LPCWSTR" ���͵��ββ�����	(������ ��TCHAR����char)
{
	HANDLE hComm;
	hComm = CreateFile(portname,
		GENERIC_READ | GENERIC_WRITE,
		0, //�����Զ�ռ��ʽ��
		0, //�ް�ȫ����
		OPEN_EXISTING, //ͨ���豸�Ѵ�
		FILE_FLAG_OVERLAPPED, //�첽I/O
		0); //������ģ���
	if (hComm == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hComm);
		return false;
	}
	else
	{
		return true;
	}

}