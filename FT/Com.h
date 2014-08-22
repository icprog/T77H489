class CCom
{ 
	
public:
	
	CCom();
	
	virtual ~CCom();
	
	void PreOpenSetupQueue(DWORD dwInQueue, DWORD dwOutQueue);
	
	
	BOOL Open(char* pPort, int nBaud);
	
	
	BOOL SetupQueue(DWORD dwInQueue, DWORD dwOutQueue);
	
	
	BOOL ResetParity(char Parity);//parity = 'N', 'O', 'E',  'M',  'S' ��?�ִ�С?
	
	//��?��ʾ   no, odd, even, mark, space 
	
	// ��Open()ǰ?��?Ч��
	
	BOOL SendData(LPCVOID lpBuf, DWORD dwToWrite);
	
	DWORD ReadData(LPVOID lpBuf, DWORD dwToRead, int ntimeout);

	void  SetParentWnd(HWND hWnd);
	
	void Close();
	
	HANDLE m_hCom;
	HWND   m_hParentWnd;
	
protected:

	BOOL m_bOpened;
	
	OVERLAPPED m_osReader;
	
	OVERLAPPED m_osWriter;
	
	DWORD m_dwInBuf;
	
	DWORD m_dwOutBuf;
	
}; //////////////////////////////////////////////////////////