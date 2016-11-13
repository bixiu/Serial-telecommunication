#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <tchar.h>
using namespace std;

HANDLE hComm;



bool openport(const char * portname);

bool setupdcb(int rate_arg);

bool setuptimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD ReadTotalconstant, DWORD WriteTotalMultiplier, DWORD WriteTotalconstant);

int main()
{
	bool open;
	

	for (int i = 0; i < 10; i++)
	{
		string port = "com";
		char a[5];
		sprintf_s(a, "%d", i); //ת��
		port += a;
		open = openport(port.c_str());
		if (open)
			cout << "open scuess";
		/*else
		{
			cout << port.c_str() << " open failed" << endl;
		}*/
		if (setupdcb(9600))
			cout << "setupDCB success" << endl;
		if (setuptimeout(0, 0, 0, 0, 0))
			cout << "setuptimeout success" << endl;
		SetCommMask(hComm, EV_RXCHAR); //�����ַ���inbuf��ʱ��������¼�
									   //������ڵ����в���
		PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	}


	system("pause");
	return 0;
}

bool openport(const char * portname)
//"char *" ���͵�ʵ���� "LPCWSTR" ���͵��ββ�����	(������ ��TCHAR����char)
{
	
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

//����DCB,�Ȼ�ȡDCB���ã������ã�����Ƿ����ú�   
bool setupdcb(int rate_arg)
{
	DCB  dcb;
	int rate = rate_arg;
	memset(&dcb, 0, sizeof(dcb));//��һ���ڴ�������ĳ��������ֵ���ǶԽϴ�Ľṹ//�������������������һ����췽��
	if (!GetCommState(hComm, &dcb))//��ȡ��ǰDCB����
		return FALSE;
	// set DCB to configure the serial port
	dcb.DCBlength = sizeof(dcb);//DCB�ṹ���С
	/* ---------- Serial Port Config ------- */
	dcb.BaudRate = rate;  //�����ʴ�С
	dcb.Parity = NOPARITY;//��żЧ��
	dcb.fParity = 0;    
	dcb.StopBits = ONESTOPBIT;   //ֹͣ����
	dcb.ByteSize = 8;			 //���ݿ�ȣ�һ��Ϊ8����ʱ��Ϊ7 
	dcb.fOutxCtsFlow = 0;		 //CTS���ϵ�Ӳ������
	dcb.fOutxDsrFlow = 0;		 //Dsr���ϵ�Ӳ������
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = 0;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fOutX = 0;				//�Ƿ�ʹ��XON/XOFFЭ��
	dcb.fInX = 0;				 //�Ƿ�ʹ��XON/XOFFЭ��
	/* ----------------- misc parameters ----- */
	dcb.fErrorChar = 0;
	dcb.fBinary = 1;
	dcb.fNull = 0;
	dcb.fAbortOnError = 0;
	dcb.wReserved = 0;
	dcb.XonLim = 2;
	dcb.XoffLim = 4;
	dcb.XonChar = 0x13;	  //���ñ�ʾXON���ַ���һ�����Ox11
	dcb.XoffChar = 0x19;  //���ñ�ʾXOFF�ַ����ַ��� һ�����Ox13
	dcb.EvtChar = 0;    //reserved ;do not use
	// set DCB
	if (!SetCommState(hComm, &dcb))
		return false;
	else
		return true;
}


bool setuptimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD ReadTotalconstant, DWORD WriteTotalMultiplier, DWORD WriteTotalconstant)
{
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = ReadInterval;
	timeouts.ReadTotalTimeoutConstant = ReadTotalconstant;
	timeouts.ReadTotalTimeoutMultiplier = ReadTotalMultiplier;
	timeouts.WriteTotalTimeoutConstant = WriteTotalconstant;
	timeouts.WriteTotalTimeoutMultiplier = WriteTotalMultiplier;
	if (!SetCommTimeouts(hComm, &timeouts))
		return false;
	else
		return true;
}


