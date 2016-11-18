#include <iostream>
#include <Windows.h>
using namespace	std;


OVERLAPPED m_ov;
COMSTAT comstat;
DWORD m_dwCommEvent;
HANDLE hComm;

bool openport(const char * portname);
bool setupdcb(int rate_arg);
bool setuptimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD ReadTotalconstant, DWORD WriteTotalMultiplier, DWORD WriteTotalconstant);
void ReciveChar();
bool WriteChar(const BYTE * m_szWriteBuffer, DWORD m_nTosend);

int main()
{
	cout << "Serial Port: ";
	string com = "com";

	for (int i = 0; i < 11; i++)
	{
		string port = "com";
		char a[5];
		sprintf_s(a, "%d", i);
		port += a;
		bool open = openport(port.c_str());
		if (open)
		{
			cout << port.c_str() << " open scuessful!!!" << endl << endl <<"information: "<<endl;
			if (setupdcb(9600))
			{
				cout << "setupDCB scuessful!!!" << endl;
			}
			if (setuptimeout(0,0,0,0,0))  //�������д��ʱ������Ϊ�㣬����д
			{
				cout << "setupTimeout scuessful!!!" << endl;
			}
			PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
			// �ڶ�д����֮ǰ����Ҫ��PurgeComm()������ջ�����
			//PURGE_TXABORT	  �ж�����д�������������أ���ʹд������û����ɡ�
			//PURGE_RXABORT	  �ж����ж��������������أ���ʹ��������û����ɡ�
			//PURGE_TXCLEAR	  ������������
			//PURGE_RXCLEAR	  ������뻺����
			PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT); // �ڶ�д����֮ǰ����Ҫ��PurgeComm()������ջ�����
			//PURGE_TXABORT	  �ж�����д�������������أ���ʹд������û����ɡ�
			//PURGE_RXABORT	  �ж����ж��������������أ���ʹ��������û����ɡ�
			//PURGE_TXCLEAR	  ������������
			//PURGE_RXCLEAR	  ������뻺����
			WriteChar((BYTE*)"please send data now", 20);
			

			cout << "recive data: ";
			ReciveChar();
			break;
		}
		else
		{
			cout << port.c_str() << " open failed" << endl;
		}
	}
	cout << endl;

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
							  //����ڵ���CreateFile�������ʱָ
							  //����FILE_FLAG_OVERLAPPED��־����ô
							  //����ReadFile��WriteFile�Ըþ����
							  //�еĲ�����Ӧ�����ص��ģ����δָ��
							  //�ص���־�����д����Ӧ����ͬ����
							  //��ͬ��ִ��ʱ������ֱ��������ɺ�ŷ��ء�����ζ��ͬ��ִ��ʱ�̻߳ᱻ��������
							  //������Ч���½������ص�ִ��ʱ����ʹ������δ��ɣ�����������Ҳ���������أ���
							  //ʱ��I/O�����ں�̨����
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
	memset(&dcb, 0, sizeof(dcb));//��һ���ڴ�������ĳ��������ֵ���ǶԽϴ�Ľṹ
								 //�������������������һ����췽��
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
//����readfile��writefile��д���п�ʱ����Ҫ���ǳ�ʱ����, ��д���ڵĳ�ʱ����
//�֣������ʱ���ܳ�ʱ, д����ֻ֧���ܳ�ʱ�������������ֳ�ʱ��֧��, �������
//д��ʱ������Ϊ0����ô�Ͳ�ʹ��д��ʱ��
{
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = ReadInterval; //�������ʱ
	timeouts.ReadTotalTimeoutConstant = ReadTotalconstant; //��ʱ��ϵ��
	timeouts.ReadTotalTimeoutMultiplier = ReadTotalMultiplier; //��ʱ�䳣��
	timeouts.WriteTotalTimeoutConstant = WriteTotalconstant; // дʱ��ϵ��
	timeouts.WriteTotalTimeoutMultiplier = WriteTotalMultiplier; //дʱ�䳣
											//��, �ܳ�ʱ�ļ��㹫ʽ�ǣ��ܳ�ʱ��ʱ��ϵ����Ҫ���/д���ַ�����ʱ�䳣��
	if (!SetCommTimeouts(hComm, &timeouts))
	{
		return false;
	}
	else
		return true;
}

void ReciveChar()
{
	BOOL bRead = TRUE;
	BOOL bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	char RXBuff;

	bool _case = 1;
	char sentenses[100];
	
	int i = 0;

	for (;;)
	{
		bResult = ClearCommError(hComm, &dwError, &comstat);
		//ʹ��Readfile�������ж�����ʱ��Ӧʹ��ClearCommError�����������

		if (comstat.cbInQue == 0)// COMSTAT�ṹ���ش�����Ϣ
								//�˴�ֻ�õ���cbInQue�������ó�Ա�������������뻺�������ֽ���
		{
			continue;
		}
		
		
		
		if (bRead)
		{
			bRead = ReadFile
			(	hComm,		//Hande to COMM ���ڵľ��
				&RXBuff,	//RX Buffer Pointer
							//�������ݵĵ�ַ������������ݽ��洢��
							//�Ը�ָ���ֵΪ�׵�һƬ�ڴ���
				1,			//Read one Byte Ҫ������ֽ���
				&BytesRead, // Stores number of bytes read, ָ��һ��DWORD
							//��ֵ������ֵ���ض�����ʵ�ʶ�����ֽ���
				&m_ov		//pointer to m_ov structer

			);				//�ص�����ʱ���ò���ָ��һ��OVERLAPPED�ṹ��ͬ������ʱ���ò���ΪBULL
			//cout << RXBuff;   //�������
					
			if(RXBuff != '\n' || i>100)
			{
				sentenses[i] = RXBuff;
				i++;
				if (!bResult)	//��ReadFile��WriteFile����FALSEʱ����һ�����ǲ���ʧ��
								//�߳�Ӧ�õ���GetLastError�����������ؽ��
				{
					switch (dwError = GetLastError())
					{
					case ERROR_IO_PENDING:
					{
						bRead = FALSE;
						break;
					}
					default:
						break;
					}
				}
				else
				{
					bRead = TRUE;
				}  //close if(bRead)
				if (!bRead)
				{
					bRead = TRUE;
					bResult = GetOverlappedResult(hComm, &m_ov, &BytesRead, TRUE);
				}
			}
			else
			{
				_case = false;
			}		

			
		}
		if (_case==false)
		{
			break;
		}
	}
	/*int len = strlen(sentenses);      //�������
	int len_ = sizeof(sentenses);*/

	for (int j = 0; j < i; j++)
	{
		cout << sentenses[j];
	}
	string msg = "";
	for (int j = 0; j < i; j++)
	{
		msg += sentenses[j];
	}
}
bool WriteChar(const BYTE * m_szWriteBuffer, DWORD m_nToSend)
{
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;
	DWORD BytesSent = 0;
	HANDLE m_hWriteEven = 0;
	ResetEvent(m_hWriteEven);
	if (bWrite)
	{
		m_ov.Offset = 0;
		m_ov.OffsetHigh = 0;
	}
	// Clear buffer
	bResult = WriteFile
	(	hComm,				// Handle to COMM Port, ���ڵľ��

		m_szWriteBuffer,	// Pointer to message buffer in calling finction
							// ���Ը�ָ���ֵΪ�׵�ַ��nNumberOfBytesToWrite
							// ���ֽڵ����ݽ�Ҫд�봮�ڵķ������ݻ�����

		m_nToSend,	// Length of message to send, Ҫд������ݵ��ֽ���

		&BytesSent,	 // Where to store the number of bytes sent
					 // ָ��ָ��һ��DWORD��ֵ������ֵ����ʵ��д����ֽ���

		&m_ov);	    // Overlapped structure
					// �ص�����ʱ���ò���ָ��һ��OVERLAPPED�ṹ��
					// ͬ������ʱ���ò���ΪNULL

	if (!bResult)	// ��ReadFile��WriteFile����FALSEʱ����һ�����ǲ���ʧ
					//�ܣ��߳�Ӧ�õ���GetLastError�����������صĽ��
	{
		DWORD dwError = GetLastError();
		switch (dwError)
		{
		case ERROR_IO_PENDING:
		{
			BytesSent = 0;
			bWrite = FALSE;
			break;	
		}
		default:
			break;
		}
	}
	if (!bWrite)
	{
		bWrite = TRUE;
		bResult = GetOverlappedResult
		(
			hComm,
			&m_ov,
			&BytesSent,
			TRUE
		);
		//deal with the error code
		if (!bResult)
		{
			cout << "GetOverlappedResults() in WriteFile()";
		}
		if (BytesSent != m_nToSend)
		{
			cout << "WARNING: WriteFile() error.. Bytes Sent: " << BytesSent
				<< "; Message Length: " << strlen((char*)m_szWriteBuffer) << endl;
		}
		return TRUE;
	}

}