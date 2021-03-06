#include "stdafx.h"
#include "Local_com.h"
#include "HCSG_ManagerDlg.h"
#include "md5.h"
#include  <stdio.h>   
#include  <stdlib.h>   
#include  <httpext.h>   
#include  <windef.h>   
#include  <Nb30.h>
#pragma comment(lib, "Netapi32.lib")
#pragma warning(disable:4996)

Common::Common(void)
{
}

Common::~Common(void)
{
}

HANDLE Common::hMutex = NULL;

BOOL Common::SanGuoServerIsRuning = FALSE;

HWND Common::DlgCwarOrg;
HWND Common::DlgCwarPlayer;
HWND Common::DlgAttr;
HWND Common::DlgCB;
HWND Common::DlgServer;
HWND Common::DlgXubao;
HWND Common::DlgLog;
HWND Common::DlgVip;
HWND Common::DlgSendAwards;
HWND Common::DlgAttrList;
HWND Common::DlgItems;
HWND Common::DlgStore;

int Common::LogLevel = 1;

BOOL Common::IsEnConn = TRUE;
BOOL Common::IsDbConct = FALSE;

CString Common::ServerPath;

CString Common::SQLServer;
CString Common::SQLAccount;
CString Common::SQLPassWord;

CString Common::TableAccountName;
CString Common::TableXbName;


CString Common::mEditAcc;
CString Common::mEditDB;
CString Common::mEditLogin;
CString Common::mEditVT;
CString Common::mEditMap1;
CString Common::mEditMap2;
CString Common::mEditMap3;
CString Common::mEditMap4;
CString Common::mEditBackup1;
CString Common::mEditBackup2;
CString Common::mEditGate;

CString Common::m_CbMinKills;
CString Common::m_CbChkMinKills;
CString Common::m_CbMinHonors;
CString Common::m_CbChkMinHonors;
CString Common::m_CbMapPath;

CString Common::m_CWarMinKills;
CString Common::m_CWarChkMinKills;
CString Common::m_CWarMinHonors;
CString Common::m_CWarChkMinHonors;

CString Common::m_CWarAwardsChkTime;

CStringArray Common::VipAwardsPoints;

_ConnectionPtr Common::m_pConnection;
_RecordsetPtr Common::m_pRecordset;
_ConnectionPtr Common::m_pConnXb;
_RecordsetPtr Common::m_pRecordsetXb;

CList <GameAcc, GameAcc&> Common::LGameAcc;
CList <AccAttr, AccAttr&> Common::LAccAttr;
CList <ItemDef, ItemDef&> Common::LItemDef;
CList <StageDef, StageDef&> Common::LStageDef;
CList <OrganizeAttr, OrganizeAttr&> Common::LOrganizeAttr;
CList <SaveItems, SaveItems&> Common::LSaveItems;
CList <SaveStores, SaveStores&> Common::LSaveStores;

ServerConfigTime Common::TimeForCB;
ServerConfigTime Common::TimeForCwar;

int Common::g_iError = 0;
TimeData Common::td;

CString  Common::GB2Big(char*  sGb)
{  
	char*  pszGbt  =  NULL;    
	char*  pszGbs  =  NULL;    
	wchar_t*  wszUnicode  =  NULL;    
	char*  pszBig5  =  NULL;    
	CString  sBig5;    
	int  iLen=0;    

	pszGbs  =  sGb;
	iLen  =  MultiByteToWideChar(936,  0,  pszGbs,  -1,  NULL,0);
	pszGbt  =  new  char[iLen*2+1];
	LCMapString(0x0804,LCMAP_TRADITIONAL_CHINESE,  pszGbs,  -1,  pszGbt,  iLen*2);
	wszUnicode  =  new  wchar_t[iLen+1];
	MultiByteToWideChar  (936,  0,  pszGbt,  -1,  wszUnicode,iLen);
	iLen  =  WideCharToMultiByte  (950,  0,  (PWSTR)wszUnicode,  -1,  NULL,  0,  NULL,  NULL);
	pszBig5  =  new  char[iLen+1];
	WideCharToMultiByte  (950,  0,  (PWSTR)wszUnicode,  -1,  pszBig5,  iLen,  NULL,  NULL);
	sBig5  =  pszBig5;
	delete  []  wszUnicode;  
	wszUnicode  =  NULL;  
	delete  []  pszGbt;  
	pszGbt  =  NULL;  
	delete  []  pszBig5;  
	pszBig5  =  NULL;  

	return  sBig5;  
}

CString Common::Big2GB(char*  sBig5) 
{
	// TODO: Add your control notification handler code here
	char buf[2048];
	char GbBuf[2048];
	WCHAR wbuf[2048];

	CString  sRetBig5;   

	ZeroMemory(buf, sizeof(buf));
	ZeroMemory(GbBuf, sizeof(GbBuf));
	ZeroMemory(wbuf, sizeof(wbuf));
	MultiByteToWideChar(950, 0, (LPCTSTR)sBig5, -1, wbuf, 2046);

	//转换Unicode码到Gb码繁体，使用API函数WideCharToMultiByte
	WideCharToMultiByte (936, 0, wbuf, -1, buf, sizeof(buf), NULL, NULL);

	//转换Gb码繁体到Gb码简体，使用API函数LCMapString
	LCMapString(0x0804, LCMAP_SIMPLIFIED_CHINESE, buf, -1, GbBuf, 2048);

	sRetBig5  =  GbBuf;
	return  sRetBig5;  
}

CString Common::convert(int n)
{ 
	static CString str = ""; 
	if(n<10) 
	{ 
		str=(char)(n%10+'0'); 
		return str; 
	} 
	else 
	{ 
		str=convert(n/10)+(char)(n%10+'0'); 
		return str; 
	} 
}

void Common::CStringGap(CString strSrc, CStringArray *strDest, CString Key, CString strGap, int Skip)
{
	int nPos = strSrc.Find(strGap);
	CString strLeft = _T("");
	while(0 <= nPos)
	{
		strLeft = strSrc.Left(nPos);
		if (Skip-- <= 0)
			strDest->Add(strLeft);

		int  len = strSrc.GetLength();
		strSrc = strSrc.Right(strSrc.GetLength() - nPos - 1);
		nPos = strSrc.Find(strGap);
	}

	if (!strSrc.IsEmpty() && Skip-- <= 0) {
		strDest->Add(strSrc);
	}
}

void Common::ReadConfig()
{
	CString StrValue;
	CString des="";
	::GetCurrentDirectory(1024,des.GetBuffer(1024));
	des.ReleaseBuffer();
	des += CONFIG_FILE_PATH;
	des += CONFIG_FILE;

	WaitForSingleObject(hMutex,INFINITE);  // 互斥量

	GetPrivateProfileString("SQLSERVER","SERVERIP","",StrValue.GetBuffer(128),128,des);
	SQLServer = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SQLSERVER","ACCOUNT","",StrValue.GetBuffer(128),128,des);
	SQLAccount = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SQLSERVER","PASSWORD","",StrValue.GetBuffer(128),128,des);
	SQLPassWord = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SETTING","SERVER_PATH","",StrValue.GetBuffer(128),128,des);
	ServerPath = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SETTING", "TABLE_ACCOUNT_NAME", "", StrValue.GetBuffer(128), 128, des);
	TableAccountName = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SETTING", "TABLE_XB_NAME", "", StrValue.GetBuffer(128), 128, des);
	TableXbName = StrValue;
	StrValue.ReleaseBuffer();


	GetPrivateProfileString("SYSTEM","Account","",StrValue.GetBuffer(128),128,des);
	mEditAcc = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SYSTEM","DB","",StrValue.GetBuffer(128),128,des);
	mEditDB = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SYSTEM","Login","",StrValue.GetBuffer(128),128,des);
	mEditLogin = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SYSTEM","VT","",StrValue.GetBuffer(128),128,des);
	mEditVT = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SYSTEM","MAP-1","",StrValue.GetBuffer(128),128,des);
	mEditMap1 = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SYSTEM","MAP-2","",StrValue.GetBuffer(128),128,des);
	mEditMap2 = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SYSTEM","MAP-3","",StrValue.GetBuffer(128),128,des);
	mEditMap3 = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SYSTEM","MAP-4","",StrValue.GetBuffer(128),128,des);
	mEditMap4 = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SYSTEM","Backup-1","",StrValue.GetBuffer(128),128,des);
	mEditBackup1 = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SYSTEM","Backup-2","",StrValue.GetBuffer(128),128,des);
	mEditBackup2 = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("SYSTEM","Gate","",StrValue.GetBuffer(128),128,des);
	mEditGate = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("HISTORY_CB","CbMinKills","",StrValue.GetBuffer(128),128,des);
	m_CbMinKills = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("HISTORY_CB","CbChkMinKills","",StrValue.GetBuffer(128),128,des);
	m_CbChkMinKills = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("HISTORY_CB","CbMinHonors","",StrValue.GetBuffer(128),128,des);
	m_CbMinHonors = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("HISTORY_CB","CbChkMinHonors","",StrValue.GetBuffer(128),128,des);
	m_CbChkMinHonors = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("HISTORY_CB", "CbMapPath", "Map", StrValue.GetBuffer(128), 128, des);
	m_CbMapPath = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("HISTORY_CWAR","CWarMinKills","",StrValue.GetBuffer(128),128,des);
	m_CWarMinKills = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("HISTORY_CWAR","CWarChkMinKills","",StrValue.GetBuffer(128),128,des);
	m_CWarChkMinKills = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("HISTORY_CWAR","CWarMinHonors","",StrValue.GetBuffer(128),128,des);
	m_CWarMinHonors = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("HISTORY_CWAR","CWarChkMinHonors","",StrValue.GetBuffer(128),128,des);
	m_CWarChkMinHonors = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("HISTORY_CWAR","AWARDECHKTIME","5",StrValue.GetBuffer(128),128,des);
	m_CWarAwardsChkTime = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("LOGSERVER","LOGLEVEL","3",StrValue.GetBuffer(128),128,des);
	LogLevel = _ttoi(StrValue);
	StrValue.ReleaseBuffer();
	
// 	for(int i=1; i<=VIP_LEVEL; i++)
// 	{
// 		CString strKey = "";
// 		strKey.Format("VIP%d", i);
// 		GetPrivateProfileString("VIPAWARDE",strKey, convert(i * 100), StrValue.GetBuffer(512),512,des);
// 		VipAwardsPoints.Add(StrValue);
// 		StrValue.ReleaseBuffer();
// 	}

	ReleaseMutex(hMutex);
}

void Common::SaveConfig()
{
	CString StrValue;
	CString des="";
	::GetCurrentDirectory(1024,des.GetBuffer(1024));
	des.ReleaseBuffer();
	des += CONFIG_FILE_PATH;
	des += CONFIG_FILE;

	WaitForSingleObject(hMutex,INFINITE);  // 互斥量

	WritePrivateProfileString("SQLSERVER","SERVERIP",SQLServer,des);
	WritePrivateProfileString("SQLSERVER","ACCOUNT",SQLAccount,des);
	WritePrivateProfileString("SQLSERVER","PASSWORD",SQLPassWord,des);
	WritePrivateProfileString("SETTING","SERVER_PATH",ServerPath,des);
	WritePrivateProfileString("SETTING", "TABLE_ACCOUNT_NAME", TableAccountName, des);
	WritePrivateProfileString("SETTING", "TABLE_XB_NAME", TableXbName, des);

	WritePrivateProfileString("SYSTEM","Account",mEditAcc,des);
	WritePrivateProfileString("SYSTEM","DB",mEditDB,des);	
	WritePrivateProfileString("SYSTEM","Login",mEditLogin,des);
	WritePrivateProfileString("SYSTEM","VT",mEditVT,des);
	WritePrivateProfileString("SYSTEM","MAP-1",mEditMap1,des);
	WritePrivateProfileString("SYSTEM","MAP-2",mEditMap2,des);
	WritePrivateProfileString("SYSTEM","MAP-3",mEditMap3,des);
	WritePrivateProfileString("SYSTEM","MAP-4",mEditMap4,des);
	WritePrivateProfileString("SYSTEM","Backup-1",mEditBackup1,des);
	WritePrivateProfileString("SYSTEM","Backup-2",mEditBackup2,des);	
	WritePrivateProfileString("SYSTEM","Gate",mEditGate,des);
	WritePrivateProfileString("HISTORY_CB","CbMinKills",m_CbMinKills,des);
	WritePrivateProfileString("HISTORY_CB","CbChkMinKills",m_CbChkMinKills,des);	
	WritePrivateProfileString("HISTORY_CB","CbMinHonors",m_CbMinHonors,des);
	WritePrivateProfileString("HISTORY_CB","CbChkMinHonors",m_CbChkMinHonors,des);
	WritePrivateProfileString("HISTORY_CB", "CbMapPath", m_CbMapPath, des);
	WritePrivateProfileString("HISTORY_CWAR","CWarMinKills",m_CWarMinKills,des);
	WritePrivateProfileString("HISTORY_CWAR","CWarChkMinKills",m_CWarChkMinKills,des);	
	WritePrivateProfileString("HISTORY_CWAR","CWarMinHonors",m_CWarMinHonors,des);
	WritePrivateProfileString("HISTORY_CWAR","CWarChkMinHonors",m_CWarChkMinHonors,des);
	WritePrivateProfileString("HISTORY_CWAR","AWARDECHKTIME",m_CWarAwardsChkTime,des);
	WritePrivateProfileString("LOGSERVER","LOGLEVEL",convert(LogLevel),des);

	ReleaseMutex(hMutex);
}

BOOL Common::SaveAwardsConfig()
{
	CString StrValue;
	CString strFilePath="";
	::GetCurrentDirectory(1024,strFilePath.GetBuffer(1024));
	strFilePath.ReleaseBuffer();
	strFilePath += CONFIG_FILE_PATH;
	strFilePath += AWARDS_CONFIG;

	DWORD dwRe = GetFileAttributes(strFilePath);
	if ( dwRe != (DWORD)-1 )
	{
		DeleteFile(strFilePath);
	}

	//调用各个Tab的配置保存方法, 调整成使用sqlite3，用三张表解决问题
	::SendMessage(Common::DlgCwarOrg, WM_SAVECWARORGAWARDS, 0, (LPARAM)(&strFilePath));
	::SendMessage(Common::DlgCwarPlayer, WM_SAVECWARPLAYERAWARDS, 0, (LPARAM)(&strFilePath));
	::SendMessage(Common::DlgCB, WM_SAVECBAWARDS, 0, (LPARAM)(&strFilePath));
	//::SendMessage(Common::DlgXubao, WM_SAVEXUBAOCONFIG, 0, (LPARAM)(&strFilePath_1));

	return TRUE;
}

// 硬盘序列号，注意：有的硬盘没有序列号
BOOL Common::GetHdSn()
{
// 	OSVERSIONINFO ovi = { 0 };
// 	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
// 	GetVersionEx(&ovi);
// 
// 	if (ovi.dwPlatformId != VER_PLATFORM_WIN32_NT)
// 	{
// 		// Only Windows 2000, Windows XP, Windows Server 2003...
// 		return FALSE;
// 	}
// 	else
// 	{
// 		if (!WinNTHDSerialNumAsPhysicalRead(szSystemInfo, &uSystemInfoLen, 1024))
// 		{
// 			return WinNTHDSerialNumAsScsiRead(szSystemInfo, &uSystemInfoLen, 1024);
// 		}
// 		else
			return TRUE;
//	}
}

// CPU ID
BOOL Common::GetCpuId()
{
// 	BOOL bException = FALSE;
// 	BYTE szCpu[16] = { 0 };
// 	UINT uCpuID = 0U;
// 
// 	__try
// 	{
// 		_asm
// 		{
// 			mov eax, 0
// 				cpuid
// 				mov dword ptr szCpu[0], ebx
// 				mov dword ptr szCpu[4], edx
// 				mov dword ptr szCpu[8], ecx
// 				mov eax, 1
// 				cpuid
// 				mov uCpuID, edx
// 		}
// 	}
// 	__except (EXCEPTION_EXECUTE_HANDLER)
// 	{
// 		bException = TRUE;
// 	}
// 
// 	if (!bException)
// 	{
// 		CopyMemory(szSystemInfo + uSystemInfoLen, &uCpuID, sizeof(UINT));
// 		uSystemInfoLen += sizeof(UINT);
// 
// 		uCpuID = strlen((char*)szCpu);
// 		CopyMemory(szSystemInfo + uSystemInfoLen, szCpu, uCpuID);
// 		uSystemInfoLen += uCpuID;

		return TRUE;
// 	}
// 	else
// 		return FALSE;
}

CString Common::GetMachineCode()
{

	unsigned long s1, s2;
	unsigned char vendor_id[] = "------------";//CPU提供商ID
	CString str1, str2, str3;
	//以下为获得CPU ID的汇编语言指令
	_asm    // 得到CPU提供商信息 
	{  
		xor eax,eax   // 将eax清0
			cpuid         // 获取CPUID的指令
			mov dword ptr vendor_id,ebx
			mov dword ptr vendor_id[+4],edx
			mov dword ptr vendor_id[+8],ecx  
	}
	str1.Format("%s-",vendor_id);

	_asm    // 得到CPU ID的高32位 
	{
		mov eax, 01h
			xor edx, edx
			cpuid
			mov s2, eax
	}
	str2.Format("%08X-", s2);

	_asm    // 得到CPU ID的低64位
	{
		mov eax, 03h
			xor ecx, ecx
			xor edx, edx
			cpuid
			mov s1, edx
			mov s2, ecx
	}

	str3.Format("%08X-%08X\n", s1, s2);

	char   mac[200];
	NCB ncb;
	typedef struct _ASTAT_
	{
		ADAPTER_STATUS   adapt;
		NAME_BUFFER   NameBuff[30];
	}ASTAT, *PASTAT;
	ASTAT Adapter;

	typedef struct _LANA_ENUM
	{
		UCHAR   length;
		UCHAR   lana[MAX_LANA];
	}LANA_ENUM;
	LANA_ENUM lana_enum;

	UCHAR uRetCode;
	memset(&ncb, 0, sizeof(ncb));
	memset(&lana_enum, 0, sizeof(lana_enum));

	ncb.ncb_command = NCBENUM;
	ncb.ncb_buffer = (unsigned char *)&lana_enum;
	ncb.ncb_length = sizeof(LANA_ENUM);
	uRetCode = Netbios(&ncb);


	for (int lana = 0; lana < lana_enum.length; lana++)
	{
		ncb.ncb_command = NCBRESET;
		ncb.ncb_lana_num = lana_enum.lana[lana];
		uRetCode = Netbios(&ncb);
		if (uRetCode == NRC_GOODRET)
			break;
	}


	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBASTAT;
	ncb.ncb_lana_num = lana_enum.lana[0];
	strcpy((char*)ncb.ncb_callname, "*");
	ncb.ncb_buffer = (unsigned char *)&Adapter;
	ncb.ncb_length = sizeof(Adapter);
	uRetCode = Netbios(&ncb);

	sprintf(mac, "%02X%02X%02X%02X%02X%02X",
			Adapter.adapt.adapter_address[0],
			Adapter.adapt.adapter_address[1],
			Adapter.adapt.adapter_address[2],
			Adapter.adapt.adapter_address[3],
			Adapter.adapt.adapter_address[4],
			Adapter.adapt.adapter_address[5]
	);
	CString m_str1;
	CString str;
	m_str1.Format("-%s", mac);

	str2 = str1 + str2 + str3;
	str = str2 + m_str1;

	return str;
}

BOOL Common::LicenseDecodeAndCheck()
{
	// 老是有问题，索性不要了
	//	return TRUE;
	
	CString strPathName;
	struct StageDef TmpStageDef;
	int ret = 0;
	CString csMachineCode = "", csStartDate = "", csEndDate = "", csName = "";

	strPathName.Format("%sLicense.exe", CONFIG_FILE_PATH);

	//检查文件是否存在
	DWORD dwRe = GetFileAttributes(strPathName);
	if ( dwRe != (DWORD)-1 )
	{
		//ShellExecute(NULL, NULL, strFilePath, NULL, NULL, SW_RESTORE); 
	}
	else 
	{
		CString errormessage;
		errormessage.Format("授权机不存在！");
		Log(Error, errormessage);
		return FALSE;
	}

	struct sLicense	TmpLicense;

	CFile iFile(strPathName, CFile::modeRead | CFile::modeNoTruncate | CFile::shareDenyNone);  
	CArchive iar(&iFile, CArchive::load); 

	iar.Read(&TmpLicense,sizeof(TmpLicense));

	iar.Close();
	iFile.Close();

	int nIndex = TmpLicense.A1;

	for(int i=0; i < TmpLicense.nMachineCodeLen; i++)
	{
		csMachineCode += TmpLicense.Data[nIndex];
		nIndex += 2;
	}

	// 先只限制时间，以后再晚上这个机器码的东西
	//CString sss = GetMachineCode();
	//if (csMachineCode != GetMachineCode())
	//	return FALSE;

	nIndex += TmpLicense.A2;

	for(int i=0; i < TmpLicense.nStartDateLen; i++)
	{
		csStartDate += TmpLicense.Data[nIndex];
		nIndex += 2;
	}
	nIndex += TmpLicense.A3;

	for(int i=0; i < TmpLicense.nEndDateLen; i++)
	{
		csEndDate += TmpLicense.Data[nIndex];
		nIndex += 2;
	}
	nIndex += TmpLicense.A4;

	for(int i=0; i < TmpLicense.nNameLen; i++)
	{
		csName += TmpLicense.Data[nIndex];
		nIndex += 2;
	}

	CString csDate = "";
	ConnectAndGetData(&td);
	csDate.Format("20%02d%02d%02d", td.year, td.month, td.day);

	if(csStartDate <= csDate && csDate <= csEndDate)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// Connects to one of the servers, gets and parses the string with the date and time
// returns 0 if success
// returns -1 if socket problem
// returns -2 if cannot connect to the server
// returns -3 if connection is closed
// returns -4 if recv failed
// returns -5 if parsing error
int Common::ConnectAndGetData(TimeData *td)
{
	char *g_szServer[] = {"64.113.32.5", "129.6.15.28", "129.6.15.29", "128.138.140.44","207.200.81.113", "64.147.116.229", "64.90.182.55",
		"96.47.67.105", "165.193.126.229", "206.246.122.250", "64.250.177.145"};
	unsigned int g_port = 13;	// Port number
	int g_numOfServers = 11;

	char recvbuf[BUF_SIZE];
	int recvbuflen = BUF_SIZE;
	//----------------------
	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR){		
		//printf("Error at WSAStartup()\n");
		g_iError = -1;
		return g_iError;
	}

	//----------------------
	// Create a SOCKET for connecting to server
	SOCKET ConnectSocket;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ConnectSocket == INVALID_SOCKET) {
		//printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		g_iError = -1;
		return g_iError;
	}
	//printf("Socket %i created. \n", ConnectSocket);	

	//----------------------
	// Try connecting to one of the servers
	int i = 0;
	for(i=0; i < g_numOfServers; i++){
		// The sockaddr_in structure specifies the address family,
		// IP address, and port of the server to be connected to.
		sockaddr_in clientService; 
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr( g_szServer[i] );
		clientService.sin_port = htons( g_port );

		//----------------------
		// Connect to server.	
		if ( connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService) ) 
			== SOCKET_ERROR) {
				if(i < g_numOfServers - 1)
					continue;
				else{
					//printf( "Failed to connect.\n" );
					closesocket(ConnectSocket);
					WSACleanup();
					g_iError = -2;
					return g_iError;				}
		}// end if connect

		//printf("Connected to server %s.\n", g_szServer[i]);

		// Clean the buffer
		memset(recvbuf, 0, BUF_SIZE*sizeof(char));
		// Receive the data.
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if ( iResult > 0 ){
			//printf("\rBytes received: %d\n", iResult);
			//printf("\r%s\n", recvbuf);

			// Parse the string
			iResult = ParseTimeString(td, recvbuf);
			if(iResult < 0)	// if parsing error
				g_iError = -5;
			else break;
		}
		else if ( iResult == 0 )
			//printf("Connection closed.\n");
			g_iError = -3;
		else
			//printf("recv failed: %d\n", WSAGetLastError());
			g_iError = -4;

		// If connected, then break the loop.
		if(g_iError >= 0)
			break;
	}// end for i

	closesocket(ConnectSocket);

	// Cleanup
	WSACleanup();

	return g_iError;
}

// Parse time string
// Its format is 
// 56319 13-01-27 21:16:34 00 0 0 268.5 UTC(NIST) * 
// returns -1 in case of error
int Common::ParseTimeString(TimeData *td, char* buf)
{
	char szTmp[5];
	int iError = 0;
	memset(szTmp, 0, 5);

	int i = 0, j = 0, k = 0;

	// Skip the first number
	while(buf[i] != ' '){
		++i;
		// Prevent infinite loops
		if(BUF_SIZE - 1 == i){
			iError = -1;
			return iError;
		}
	}
	// Skip the space itself
	++i;

	// Get year, month, day
	j = 0;
	k = 0; // number of dividors, e. g. -, or :	
	while(buf[i] != ' '){
		szTmp[j] = buf[i];
		++j; ++i;
		if(buf[i] == '-'){
			++i;//skip '-'
			szTmp[j] = '\0';
			j = 0;
			if(k == 0)			
				td->year = atoi(szTmp);				
			else if(k == 1){				
				CheckFirstEl(szTmp);// check whether we have 0 at the beginning
				td->month = atoi(szTmp);				
			}
			++k;
		}// end if
		// Prevent from infinite loop
		if(BUF_SIZE - 1 == k){
			iError = -1;
			return iError;
		}
	}// end while !=' '
	++i;// skip ' '
	++j;// get space for the terminating 0
	szTmp[j] = '\0';
	CheckFirstEl(szTmp);// check whether we have 0 at the beginning
	td->day = atoi(szTmp);

	// Get hour, minutes, seconds
	j = 0;
	j = 0;
	k = 0; // number of dividors, e. g. -, or :	
	while(buf[i] != ' '){
		szTmp[j] = buf[i];
		++j; ++i;
		if(buf[i] == ':'){
			++i;//skip ':'
			szTmp[j] = '\0';
			j = 0;
			if(k == 0){
				CheckFirstEl(szTmp);// check whether we have 0 at the beginning
				td->hour = atoi(szTmp);
			}
			else if(k == 1){				
				CheckFirstEl(szTmp);// check whether we have 0 at the beginning
				td->minute = atoi(szTmp);				
			}
			++k;
		}// end if
		// Prevent from infinite loop
		if(BUF_SIZE - 1 == k){
			iError = -1;
			return iError;
		}
	}// end while !=' '
	++i;// skip ' '
	++j;// get space for the terminating 0
	szTmp[j] = '\0';
	CheckFirstEl(szTmp);// check whether we have 0 at the beginning
	td->second = atoi(szTmp);

	return iError;
}

int Common::CheckFirstEl(char *szTmp){
	if(szTmp[0] == '0'){// skip 0 at the beginning of numbers
		szTmp[0] = szTmp[1];
		szTmp[1] = '\0';
	}
	return 0;
}

void Common::GetNetTime(CString csDate)
{  
	TIME_ZONE_INFORMATION tzinfo;  
	DWORD dwStandardDaylight;  
	int nRet;  

	/* Connect to time server, and get time */  
	SOCKET sockfd;  

	char cTimeBuf[40] = { 0 };  
	unsigned long ulTime = 0;  
	int nTry = 0;  

	do   
	{  
		if (5 == nTry++)  
		{  
			return;  
		}  

		sockfd = socket(AF_INET, SOCK_STREAM, 0);  
		if (INVALID_SOCKET == sockfd)  
		{  
			continue;  
		}  

		int TimeOut = 3000;//设置接收超时6秒  
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&TimeOut, sizeof(TimeOut));  

		sockaddr_in    addr;  

		memset(&addr, 0, sizeof(addr));  
		addr.sin_family         = AF_INET;  
		addr.sin_port           = htons(37);  
		addr.sin_addr.s_addr    = inet_addr("210.72.145.44");

		nRet = connect(sockfd, (sockaddr *)&addr, sizeof(addr));  
		if (SOCKET_ERROR == nRet)  
		{  
			continue;  
		}  

		nRet = recv(sockfd, (char *)&ulTime, sizeof(ulTime), 0);  
		if ((SOCKET_ERROR != nRet) && (0 != nRet))  
		{  
			break;  
		}  

		closesocket(sockfd);  
	} while (1);  

	closesocket(sockfd);  

	unsigned long ulTimehl = ntohl(ulTime);  
	csDate = ConvertTime(ulTimehl);  
}

CString Common::ConvertTime(unsigned long ulTime)  
{  
    // Windows文件时间是一个64位的值，它是从1601年1月1日中午12:00到现在的时间间隔，  
    // 单位是1/10,000,000秒，即1000万分之1秒(100-nanosecond)  
    FILETIME ft;  
    SYSTEMTIME st;  
  
    // 首先将基准时间（1900年1月1日0点0分0秒0毫秒）转化为Windows文件时间  
    st.wYear = 1900;  
    st.wMonth = 1;  
    st.wDay = 1;  
    st.wHour = 0;  
    st.wMinute = 0;  
    st.wSecond = 0;  
    st.wMilliseconds = 0;  
  
    SystemTimeToFileTime(&st, &ft);  
  
    // 然后将Time Protocol使用的基准时间加上逝去的时间（ulTime）  
    LONGLONG *pLLong = (LONGLONG *)&ft;  
  
    /* 注意： 
       文件时间单位是1/1000 0000秒(即100ns)， 
       需要将从时间服务器上获取的以秒为单位的ulTime做一下转换 
    */  
    *pLLong += (LONGLONG) 10000000 * ulTime;  
  
    // 再将时间转化回来，更新系统时间  
    FileTimeToSystemTime(&ft, &st);  
  
	CString csDate;
    csDate.Format("%04d%02d%02d %02d:%02d:%02d\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );  
  
    return csDate;  
}  

void Common::SystemInit()
{
	// 创建互斥量
	hMutex = CreateMutex(NULL, FALSE, "Hcsg_Gm_Mutex");
	// 检查错误代码
	if (GetLastError() == ERROR_ALREADY_EXISTS) 
	{
		// 如果已有互斥量存在则释放句柄并复位互斥量
		CloseHandle(hMutex);
		hMutex = NULL;
	}

	// 创建路径结构
	ConnToSQLServer();
	GetItemDef();
	GetStageDef();
	GetUpdateCLists();
	GetServerConfig();
}

void Common::GetUpdateCLists()
{
	GetGameAccFormDB();
	GetAccAttr();
	GetOrganizeAttr();
}

void Common::ConnToSQLServer()
{
	if(!IsEnConn)
	{
		Common::Log(Info, "管理员已经断开数据库连接！");
		return;
	}
	if(!IsDbConct)
	{
		HRESULT hr;
		try
		{
			if(SQLServer.IsEmpty() || SQLAccount.IsEmpty() || SQLPassWord.IsEmpty())
				return;
			hr = m_pConnection.CreateInstance("ADODB.Connection");
			if (SUCCEEDED(hr))
			{ 
				//connect database
				CString StrConn;
				CString temp;
				StrConn = "Provider=SQLOLEDB.1;Server=" + SQLServer + ";DATABASE=" + TableAccountName + ";UID=" 
					+ SQLAccount + ";PWD=" + SQLPassWord + ";";
				m_pConnection->Open((_bstr_t)StrConn,"","",adModeUnknown);
				Log(Info, "Account 数据库连接成功");
				IsDbConct = TRUE;
			}
			else
			{
				Common::Log(Error, "CreateInstance 执行失败，但是没有抛出异常！");
				return;
			}

			hr = m_pConnXb.CreateInstance("ADODB.Connection");
			if (SUCCEEDED(hr))
			{ 
				//connect database
				CString StrConn;
				StrConn = "Provider=SQLOLEDB.1;Server=" + SQLServer + ";DATABASE=" + TableXbName + ";UID="
					+ SQLAccount + ";PWD=" + SQLPassWord + ";";
				m_pConnXb->Open((_bstr_t)StrConn,"","",adModeUnknown);
				Log(Info, "sanvt数据库连接成功");
			}
			else
			{
				Common::Log(Error, "CreateInstance 执行失败，但是没有抛出异常！");
				return;
			}
		}
		catch (_com_error e)
		{
			CString errormessage;
			errormessage.Format("连接数据库失败！\r\n错误信息:%s",e.ErrorMessage());
			Log(Error, errormessage);
			Common::m_pConnection->Close();
			Common::m_pConnection = NULL;
			return ;
		}
	}
	else
	{
		try
		{
			if(Common::m_pConnection != NULL)
			{
				Common::m_pConnection->Close();
				Common::m_pConnection = NULL;
			}
			if(Common::m_pConnXb != NULL)
			{
				Common::m_pConnXb->Close();
				Common::m_pConnXb = NULL;
			}
			IsDbConct = FALSE;
		}
		catch (_com_error e)
		{
			CString errormessage;
			errormessage.Format("断开数据库失败！\r\n错误信息:%s",e.ErrorMessage());
			Log(Error, errormessage);
			return ;
		}
	}
}

BOOL Common::UpdatePoints(int nTotalPoints, CString strAccount)
{
	CString strInsert;
	CString SQLInSertCmd;

	if(Common::m_pConnection == NULL)
	{
		return FALSE;
	}

	strInsert.Format("UPDATE game_acc set point='%d' from game_acc where account='%s'", nTotalPoints, strAccount);

	SQLInSertCmd = strInsert;

	//将记录插入数据库中
	TRACE(SQLInSertCmd);
	_variant_t sql;
	sql = SQLInSertCmd;
	try
	{
		HRESULT hr = Common::m_pConnection->Execute((_bstr_t)sql, NULL, adCmdText);
		if(!SUCCEEDED(hr))
		{
			return FALSE;
		}
	}
	catch(_com_error &e) 
	{ 
		Common::Log(Info, (LPCSTR)(e.Description()));
		return FALSE;
	}
	Common::Log(Info, sql);

	return TRUE;
}

CString Common::AddPoints(int nPoints, CString strAccount, CString strTransID)
{
	CString strInsert;
	CString SQLInSertCmd;
	MD5 md5;
	CString strEncryptGamePWD;
	// Create Command Object  
	_CommandPtr m_CommandPtr;  	
	CString outparam;

	if(Common::m_pConnection == NULL)
	{
		return "未连接数据库";
	}

	try
	{
		m_CommandPtr.CreateInstance( __uuidof(Command));  
		m_CommandPtr->ActiveConnection = m_pConnection;  
		m_CommandPtr->CommandType = adCmdStoredProc;  
		m_CommandPtr->CommandText  = _bstr_t("AC_sp_AddVCoin"); 

		//创建参数对象，并给参数赋值  
		_ParameterPtr m_paramGameAccount,m_paramiVCoins,m_paramstrTransID,m_paramdtSubmitDate,m_paramErrInfo;  
		m_paramGameAccount.CreateInstance(_uuidof(Parameter));  
		m_paramiVCoins.CreateInstance(_uuidof(Parameter));
		m_paramstrTransID.CreateInstance(_uuidof(Parameter));
		m_paramdtSubmitDate.CreateInstance(_uuidof(Parameter));
		m_paramErrInfo.CreateInstance(_uuidof(Parameter));  

		m_paramGameAccount = m_CommandPtr->CreateParameter("strGameAccount",adVarChar,adParamInput,strAccount.GetLength()+1,(_variant_t)(strAccount));  
		m_CommandPtr->Parameters->Append(m_paramGameAccount);
		m_paramiVCoins=m_CommandPtr->CreateParameter("iVCoins",adInteger,adParamInput,sizeof(int),nPoints);  
		m_CommandPtr->Parameters->Append(m_paramiVCoins);		
		m_paramstrTransID=m_CommandPtr->CreateParameter("strTransID",adChar,adParamInput,strTransID.GetLength()+1,(_variant_t)(strTransID));		
		m_CommandPtr->Parameters->Append(m_paramstrTransID);
		SYSTEMTIME st;
		CString strDateTime;
		GetLocalTime(&st);
		strDateTime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);
		m_paramdtSubmitDate=m_CommandPtr->CreateParameter("dtSubmitDate",adChar,adParamInput,strDateTime.GetLength()+1,(_variant_t)(strDateTime));  
		m_CommandPtr->Parameters->Append(m_paramdtSubmitDate);
		m_paramErrInfo=m_CommandPtr->CreateParameter("strErrInfo",adVarChar,adParamOutput,512,(_variant_t)(CString)"");  
		m_CommandPtr->Parameters->Append(m_paramErrInfo); 

		/*用命令对象的方法来创建一个参数对象，其中的长度参数（第四个）如果是固定长度的类型，就填-1，如果是字符串等可变长度的就填其实际长度。 
		Parameters是命令对象的一个容器，它的Append方法就是把创建的参数对象追加到该容器里。Append进去的参数按先后顺序与SQL语句中的问号从左至右一一对应。*/  

		_RecordsetPtr m_RecordsetPtr;  
		m_RecordsetPtr.CreateInstance(__uuidof(Recordset));  
		m_RecordsetPtr = m_CommandPtr->Execute(NULL,NULL,adCmdStoredProc);  

		//这里有三种办法来取这些output参数值  
		outparam = m_CommandPtr->Parameters->GetItem("strErrInfo")->GetValue();  
		m_CommandPtr.Detach();  
	}
	catch (_com_error e)
	{
		CString ErrMsg;
		ErrMsg.Format("%s", e.Description());
		Log(Error, ErrMsg);
		return ErrMsg;
	}

	return outparam;
}

BOOL Common::SendXubao(CString strAccount, CStringArray *ItemList)
{
	CString strInsert;
	CString SQLInSertCmd;
	CStringArray TmpItemList;

	if(Common::m_pConnection == NULL)
	{
		return FALSE;
	}

	int count = ItemList->GetCount();
	int i;
	for(i=0;i<count;i++)
		TmpItemList.Add(ItemList->GetAt(i));

	if(ItemList->GetCount() > 10)
	{
		for(;i<20;i++)
			TmpItemList.Add("0");
	}
	else
	{
		for(;i<10;i++)
			TmpItemList.Add("0");
	}
	

// 	COleDateTime oleDate = COleDateTime::GetCurrentTime();
// 	CString strDateTime = oleDate.Format(_T("%Y-%m-%d %H:%M:%S.%ms"));

	SYSTEMTIME st;
	CString strDateTime;
	GetLocalTime(&st);
	strDateTime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);

	strInsert.Format("INSERT INTO vitem(Disable,Login_time,Get_time,SName,CharName,Type,"
		"Account,Card,DataID1,Number1,"
		"DataID2,Number2,DataID3,Number3,DataID4,Number4,DataID5,Number5)"
		" values('0','2015-04-21 18:16:29.000','2015-04-21 18:16:29.000','0','0','0',"
		"'%s','%s',"
		"'%s','%s','%s','%s','%s','%s',"
		"'%s','%s','%s','%s')",
		strAccount, strDateTime, 
		TmpItemList[0], TmpItemList[1], TmpItemList[2], TmpItemList[3], TmpItemList[4], TmpItemList[5],
		TmpItemList[6], TmpItemList[7], TmpItemList[8], TmpItemList[9]);

	SQLInSertCmd = strInsert;

	//将记录插入数据库中
	TRACE(SQLInSertCmd);
	_variant_t sql;
	sql = SQLInSertCmd;
	try
	{
		HRESULT hr = m_pConnXb->Execute((_bstr_t)sql, NULL, adCmdText);
		if(!SUCCEEDED(hr))
		{
			return FALSE;
		}
	}
	catch(_com_error &e) 
	{ 
		Common::Log(Info, (LPCSTR)(e.Description())); 
		return FALSE;
	}
	Common::Log(Info, sql);
	Common::SaveRecored(strAccount, ItemList);

	if(ItemList->GetCount() > 10)
	{
		GetLocalTime(&st);
		strDateTime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond, st.wMilliseconds);

		strInsert.Format("INSERT INTO vitem(Disable,Login_time,Get_time,SName,CharName,Type,"
			"Account,Card,DataID1,Number1,"
			"DataID2,Number2,DataID3,Number3,DataID4,Number4,DataID5,Number5)"
			" values('0','2015-04-21 18:16:29.000','2015-04-21 18:16:29.000','0','0','0',"
			"'%s','%s',"
			"'%s','%s','%s','%s','%s','%s',"
			"'%s','%s','%s','%s')",
			strAccount, strDateTime, 
			TmpItemList[10], TmpItemList[11], TmpItemList[12], TmpItemList[13], TmpItemList[14], TmpItemList[15],
			TmpItemList[16], TmpItemList[17], TmpItemList[18], TmpItemList[19]);

		SQLInSertCmd = strInsert;

		//将记录插入数据库中
		TRACE(SQLInSertCmd);
		_variant_t sql;
		sql = SQLInSertCmd;
		try
		{
			HRESULT hr = m_pConnXb->Execute((_bstr_t)sql, NULL, adCmdText);
			if(!SUCCEEDED(hr))
			{
				return FALSE;
			}
		}
		catch(_com_error &e) 
		{ 
			Common::Log(Info, (LPCSTR)(e.Description())); 
			return FALSE;
		}
		Common::Log(Info, sql);
		Common::SaveRecored(strAccount, ItemList);
	}
	
	return TRUE;
}

void Common::SaveRecored(CString strAccount, CStringArray *ItemList)
{
	CDatabase database;//数据库库需要包含头文件 #include <afxdb.h>
	CString sDriver = "MICROSOFT EXCEL DRIVER (*.XLS)"; // Excel驱动
	CString sSql,strInsert;
	int i,j;
	CStringArray TmpItemList;
	CString strItemId, strItemName, strItemNum;
	struct ItemDef TmpItemDef;
	CList <ItemDef, ItemDef&> *LCurItemDef = &Common::LItemDef;
	CString strFilePath="";
	::GetCurrentDirectory(1024,strFilePath.GetBuffer(1024));
	strFilePath.ReleaseBuffer();
	strFilePath += "\\Recored.xls";
	int count = 0;

	TRY
	{
		// 创建进行存取的字符串
		sSql.Format("DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s",sDriver, strFilePath, strFilePath);

		// 创建数据库 (既Excel表格文件)
		if( database.OpenEx(sSql,CDatabase::noOdbcDialog) )
		{
			count = ItemList->GetCount();
			for(i=0;i<count;)
			{
				strItemId = ItemList->GetAt(i);
				strItemNum = ItemList->GetAt(i+1);
				strItemName = "";
				if(!strItemId.IsEmpty())
				{
					POSITION pos = LCurItemDef->GetHeadPosition();
					for (j=0;j < LCurItemDef->GetCount();j++)
					{
						TmpItemDef = LCurItemDef->GetNext(pos);

						if(strItemId == TmpItemDef.ID)
						{
							CString tempName = TmpItemDef.Name;
							tempName.Replace("item_", "");

							strItemName = Common::Big2GB((LPSTR)(LPCTSTR)tempName);
							break;
						}
					}
				}

				TmpItemList.Add(strItemId);
				TmpItemList.Add(strItemName);
				TmpItemList.Add(strItemNum);
				i += 2;
			}

			for(i=(count/2)*3;i<15;i++)
			{
				TmpItemList.Add("");
			}

			strInsert.Format(" INSERT INTO 发放记录 ( 注册帐号 , 角色名称 ,"
				" 物品ID1 , 物品名1 , 数量1 , 物品ID2 , 物品名2 , 数量2 ,"
				"物品ID3 , 物品名3 , 数量3 , 物品ID4 , 物品名4 , 数量4 ,"
				" 物品ID5 , 物品名5 , 数量5 ) "
				" VALUES (  '%s' , '' , "
				" '%s' , '%s' , '%s' , '%s' , '%s' , '%s' ,"
				" '%s' , '%s' , '%s' , '%s' , '%s' , '%s' ,"
				" '%s' , '%s' , '%s')", 
				strAccount, 
				TmpItemList[0], TmpItemList[1], TmpItemList[2], TmpItemList[3], TmpItemList[4], TmpItemList[5],
				TmpItemList[6], TmpItemList[7], TmpItemList[8], TmpItemList[9], TmpItemList[10], TmpItemList[11],
				TmpItemList[12], TmpItemList[13], TmpItemList[14]);

			sSql = strInsert;

			//将记录插入到表格中
			database.ExecuteSQL(sSql);

		}     
		// 关闭Excel表格文件
		database.Close();

	}
	CATCH_ALL(e)
	{
		//错误类型很多，根据需要进行报错。
		Common::Log(Error, "记录发放记录失败。");
	}
	END_CATCH_ALL;
}

CString Common::CreateAccount(CString strAccount, CString strPassword)
{
	CString strInsert;
	CString SQLInSertCmd;
	MD5 md5;
	// Create Command Object  
	_CommandPtr m_CommandPtr;  	
	CString outparam;

	try
	{
		m_CommandPtr.CreateInstance( __uuidof(Command));  
		m_CommandPtr->ActiveConnection = m_pConnection;  
		m_CommandPtr->CommandType = adCmdStoredProc;  
		m_CommandPtr->CommandText  = _bstr_t("AC_sp_CreateAccount"); 

		//创建参数对象，并给参数赋值  
		_ParameterPtr m_paramGameAccount,m_paramGamePWD,m_paramEncryptGamePWD,m_paramErrInfo;  
		m_paramGameAccount.CreateInstance(_uuidof(Parameter));  
		m_paramGamePWD.CreateInstance(_uuidof(Parameter));  
		m_paramEncryptGamePWD.CreateInstance(_uuidof(Parameter));
		m_paramErrInfo.CreateInstance(_uuidof(Parameter));  

		m_paramGameAccount = m_CommandPtr->CreateParameter("strGameAccount",adVarChar,adParamInput,strAccount.GetLength()+1,(_variant_t)(strAccount));  
		m_CommandPtr->Parameters->Append(m_paramGameAccount);  
		m_paramGamePWD=m_CommandPtr->CreateParameter("strGamePWD",adVarChar,adParamInput,strPassword.GetLength()+1,(_variant_t)(strPassword));  
		m_CommandPtr->Parameters->Append(m_paramGamePWD);
		md5.update(strPassword.GetBuffer());          //因为update函数只接收string类型，所以使用getbuffer()函数转换CString为string
		CString strEncryptGamePWD = md5.toString().c_str();     //toString()函数获得加密字符串，c_str();函数重新转换成CString类型
		strPassword.ReleaseBuffer();
		m_paramEncryptGamePWD=m_CommandPtr->CreateParameter("strEncryptGamePWD",adVarChar,adParamInput,strEncryptGamePWD.GetLength()+1,(_variant_t)(strEncryptGamePWD));  
		m_CommandPtr->Parameters->Append(m_paramEncryptGamePWD);
		m_paramErrInfo=m_CommandPtr->CreateParameter("strErrInfo",adVarChar,adParamOutput,512,(_variant_t)(CString)"");  
		m_CommandPtr->Parameters->Append(m_paramErrInfo); 

		/*用命令对象的方法来创建一个参数对象，其中的长度参数（第四个）如果是固定长度的类型，就填-1，如果是字符串等可变长度的就填其实际长度。 
		Parameters是命令对象的一个容器，它的Append方法就是把创建的参数对象追加到该容器里。Append进去的参数按先后顺序与SQL语句中的问号从左至右一一对应。*/  

		_RecordsetPtr m_RecordsetPtr;  
		m_RecordsetPtr.CreateInstance(__uuidof(Recordset));  
		m_RecordsetPtr = m_CommandPtr->Execute(NULL,NULL,adCmdStoredProc);  

		_variant_t var;  
		CString strValue;//定义字符串存储_variant_t变量中的字符串  

// 		//这里有两种办法来取结果集中各个字段的值  
// 		var=m_RecordsetPtr->GetCollect(_T("adm_power"));  
// 		//var=m_RecordsetPtr->GetCollect((_variant_t)(long)0); //取得结果集中第一个字段的值  
// 		if(var.vt!=VT_NULL)  
// 		{  
// 			strValue=(LPCSTR)_bstr_t(var);  
// 			m_power=strValue;  
// 		}  
// 		MessageBox(m_power);  
// 		m_RecordsetPtr->Close();//关键  

		//这里有三种办法来取这些output参数值  
		outparam = m_CommandPtr->Parameters->GetItem("strErrInfo")->GetValue();  
		//int outparam = m_CommandPtr->GetParameters()->GetItem(short(2));  
		//int outparam = m_CommandPtr->Parameters->GetItem(short(2))->Value;  
// 		MENU cmain;  
// 		switch (outparam)  
// 		{  
// 		case 0:  
// 			MessageBox(_T("账号或密码不正确！"));  
// 			UpdateData(FALSE);  
// 			break;  
// 		case 1:  
// 			OnOK();  
// 			MessageBox(_T("登录成功！"));  
// 			cmain.DoModal();  
// 			break;  
// 		case 2:  
// 			MessageBox(_T("该用户已经在线！"));  
// 			UpdateData(FALSE);  
// 			break;  
// 		}  
		m_CommandPtr.Detach();  
	}
	catch (_com_error e)
	{
		CString ErrMsg;
		ErrMsg.Format("%s", e.Description());
		Log(Error, ErrMsg);
		return ErrMsg;
	}

	return outparam;
}

BOOL Common::DeleteAccount()
{

	AfxMessageBox("不允许删除帐号，除非删档！！！");
	return TRUE;
}

CString Common::ModifyPassword(CString strAccount, CString strOldPassword, CString strNewPassword)
{
	CString strInsert;
	CString SQLInSertCmd;
	MD5 md5;
	CString strEncryptGamePWD;
	// Create Command Object  
	_CommandPtr m_CommandPtr;  	
	CString outparam;

	try
	{
		m_CommandPtr.CreateInstance( __uuidof(Command));  
		m_CommandPtr->ActiveConnection = m_pConnection;  
		m_CommandPtr->CommandType = adCmdStoredProc;  
		m_CommandPtr->CommandText  = _bstr_t("AC_sp_ModifyPassword"); 

		//创建参数对象，并给参数赋值  
		_ParameterPtr m_paramGameAccount,m_paramGameOldPWD,m_paramGameNewPWD,m_paramEncryptGamePWD,m_paramErrInfo;  
		m_paramGameAccount.CreateInstance(_uuidof(Parameter));  
		m_paramGameOldPWD.CreateInstance(_uuidof(Parameter));
		m_paramGameNewPWD.CreateInstance(_uuidof(Parameter));
		m_paramEncryptGamePWD.CreateInstance(_uuidof(Parameter));
		m_paramErrInfo.CreateInstance(_uuidof(Parameter));  

		m_paramGameAccount = m_CommandPtr->CreateParameter("strGameAccount",adVarChar,adParamInput,strAccount.GetLength()+1,(_variant_t)(strAccount));  
		m_CommandPtr->Parameters->Append(m_paramGameAccount);
		md5.update(strOldPassword.GetBuffer());          //因为update函数只接收string类型，所以使用getbuffer()函数转换CString为string
		strEncryptGamePWD = md5.toString().c_str();     //toString()函数获得加密字符串，c_str();函数重新转换成CString类型
		strOldPassword.ReleaseBuffer();
		m_paramGameOldPWD=m_CommandPtr->CreateParameter("strOldEncryptGamePWD",adVarChar,adParamInput,strEncryptGamePWD.GetLength()+1,(_variant_t)(strEncryptGamePWD));  
		m_CommandPtr->Parameters->Append(m_paramGameOldPWD);
		m_paramGameNewPWD=m_CommandPtr->CreateParameter("strNewGamePWD",adVarChar,adParamInput,strNewPassword.GetLength()+1,(_variant_t)(strNewPassword));  
		m_CommandPtr->Parameters->Append(m_paramGameNewPWD);
		md5.reset();
		md5.update(strNewPassword.GetBuffer());          //因为update函数只接收string类型，所以使用getbuffer()函数转换CString为string
		strEncryptGamePWD = md5.toString().c_str();     //toString()函数获得加密字符串，c_str();函数重新转换成CString类型
		strNewPassword.ReleaseBuffer();
		m_paramEncryptGamePWD=m_CommandPtr->CreateParameter("strNewEncryptGamePWD",adVarChar,adParamInput,strEncryptGamePWD.GetLength()+1,(_variant_t)(strEncryptGamePWD));  
		m_CommandPtr->Parameters->Append(m_paramEncryptGamePWD);
		m_paramErrInfo=m_CommandPtr->CreateParameter("strErrInfo",adVarChar,adParamOutput,512,(_variant_t)(CString)"");  
		m_CommandPtr->Parameters->Append(m_paramErrInfo); 

		/*用命令对象的方法来创建一个参数对象，其中的长度参数（第四个）如果是固定长度的类型，就填-1，如果是字符串等可变长度的就填其实际长度。 
		Parameters是命令对象的一个容器，它的Append方法就是把创建的参数对象追加到该容器里。Append进去的参数按先后顺序与SQL语句中的问号从左至右一一对应。*/  

		_RecordsetPtr m_RecordsetPtr;  
		m_RecordsetPtr.CreateInstance(__uuidof(Recordset));  
		m_RecordsetPtr = m_CommandPtr->Execute(NULL,NULL,adCmdStoredProc);  

		//这里有三种办法来取这些output参数值  
		outparam = m_CommandPtr->Parameters->GetItem("strErrInfo")->GetValue();  
		m_CommandPtr.Detach();  
	}
	catch (_com_error e)
	{
		CString ErrMsg;
		ErrMsg.Format("%s", e.Description());
		Log(Error, ErrMsg);
		return ErrMsg;
	}

	return outparam;
}

CString Common::FreezeAccount(CString strAccount, CString strFreezeReason)
{
	CString strInsert;
	CString SQLInSertCmd;
	// Create Command Object  
	_CommandPtr m_CommandPtr;  	
	CString outparam;

	try
	{
		m_CommandPtr.CreateInstance( __uuidof(Command));  
		m_CommandPtr->ActiveConnection = m_pConnection;  
		m_CommandPtr->CommandType = adCmdStoredProc;  
		m_CommandPtr->CommandText  = _bstr_t("AC_sp_FreezeAccount"); 

		//创建参数对象，并给参数赋值  
		_ParameterPtr m_paramGameAccount,m_paramFreezeType, m_paramFreezeReason,m_paramOperator, m_paramErrInfo;  
		m_paramGameAccount.CreateInstance(_uuidof(Parameter));
		m_paramFreezeType.CreateInstance(_uuidof(Parameter));		
		m_paramFreezeReason.CreateInstance(_uuidof(Parameter));
		m_paramOperator.CreateInstance(_uuidof(Parameter));
		m_paramErrInfo.CreateInstance(_uuidof(Parameter));  

		m_paramGameAccount = m_CommandPtr->CreateParameter("strGameAccount",adVarChar,adParamInput,strAccount.GetLength()+1,(_variant_t)(strAccount));  
		m_CommandPtr->Parameters->Append(m_paramGameAccount);
		m_paramFreezeType = m_CommandPtr->CreateParameter("iFreezeType",adInteger,adParamInput,sizeof(int),(_variant_t)(int)1);  
		m_CommandPtr->Parameters->Append(m_paramFreezeType);
		m_paramFreezeReason = m_CommandPtr->CreateParameter("FreezeReason",adVarChar,adParamInput,strFreezeReason.GetLength()+1,(_variant_t)(strFreezeReason));  
		m_CommandPtr->Parameters->Append(m_paramFreezeReason);
		m_paramOperator = m_CommandPtr->CreateParameter("Operator",adVarChar,adParamInput,strlen("GM"),(_variant_t)("GM"));  
		m_CommandPtr->Parameters->Append(m_paramOperator);
		m_paramErrInfo=m_CommandPtr->CreateParameter("strErrInfo",adVarChar,adParamOutput,512,(_variant_t)(CString)"");  
		m_CommandPtr->Parameters->Append(m_paramErrInfo); 

		/*用命令对象的方法来创建一个参数对象，其中的长度参数（第四个）如果是固定长度的类型，就填-1，如果是字符串等可变长度的就填其实际长度。 
		Parameters是命令对象的一个容器，它的Append方法就是把创建的参数对象追加到该容器里。Append进去的参数按先后顺序与SQL语句中的问号从左至右一一对应。*/  

		_RecordsetPtr m_RecordsetPtr;  
		m_RecordsetPtr.CreateInstance(__uuidof(Recordset));  
		m_RecordsetPtr = m_CommandPtr->Execute(NULL,NULL,adCmdStoredProc);  

		//这里有三种办法来取这些output参数值  
		outparam = m_CommandPtr->Parameters->GetItem("strErrInfo")->GetValue();  
		m_CommandPtr.Detach();  
	}
	catch (_com_error e)
	{
		CString ErrMsg;
		ErrMsg.Format("%s", e.Description());
		Log(Error, ErrMsg);
		return ErrMsg;
	}

	return outparam;
}

CString Common::UnFreezeAccount(CString strAccount, CString strFreezeReason)
{
	CString strInsert;
	CString SQLInSertCmd;
	// Create Command Object  
	_CommandPtr m_CommandPtr;  	
	CString outparam;

	try
	{
		m_CommandPtr.CreateInstance( __uuidof(Command));  
		m_CommandPtr->ActiveConnection = m_pConnection;  
		m_CommandPtr->CommandType = adCmdStoredProc;  
		m_CommandPtr->CommandText  = _bstr_t("AC_sp_UnFreezeAccount"); 

		//创建参数对象，并给参数赋值  
		_ParameterPtr m_paramGameAccount,m_paramFreezeType, m_paramFreezeReason,m_paramOperator, m_paramErrInfo;  
		m_paramGameAccount.CreateInstance(_uuidof(Parameter));
		m_paramFreezeType.CreateInstance(_uuidof(Parameter));		
		m_paramFreezeReason.CreateInstance(_uuidof(Parameter));
		m_paramOperator.CreateInstance(_uuidof(Parameter));
		m_paramErrInfo.CreateInstance(_uuidof(Parameter));  

		m_paramGameAccount = m_CommandPtr->CreateParameter("strGameAccount",adVarChar,adParamInput,strAccount.GetLength()+1,(_variant_t)(strAccount));  
		m_CommandPtr->Parameters->Append(m_paramGameAccount);
		m_paramFreezeType = m_CommandPtr->CreateParameter("iFreezeType",adInteger,adParamInput,sizeof(int),(_variant_t)(int)1);  
		m_CommandPtr->Parameters->Append(m_paramFreezeType);
		m_paramFreezeReason = m_CommandPtr->CreateParameter("FreezeReason",adVarChar,adParamInput,strFreezeReason.GetLength()+1,(_variant_t)(strFreezeReason));  
		m_CommandPtr->Parameters->Append(m_paramFreezeReason);
		m_paramOperator = m_CommandPtr->CreateParameter("Operator",adVarChar,adParamInput,strlen("GM"),(_variant_t)("GM"));  
		m_CommandPtr->Parameters->Append(m_paramOperator);
		m_paramErrInfo=m_CommandPtr->CreateParameter("strErrInfo",adVarChar,adParamOutput,512,(_variant_t)(CString)"");  
		m_CommandPtr->Parameters->Append(m_paramErrInfo); 

		/*用命令对象的方法来创建一个参数对象，其中的长度参数（第四个）如果是固定长度的类型，就填-1，如果是字符串等可变长度的就填其实际长度。 
		Parameters是命令对象的一个容器，它的Append方法就是把创建的参数对象追加到该容器里。Append进去的参数按先后顺序与SQL语句中的问号从左至右一一对应。*/  

		_RecordsetPtr m_RecordsetPtr;  
		m_RecordsetPtr.CreateInstance(__uuidof(Recordset));  
		m_RecordsetPtr = m_CommandPtr->Execute(NULL,NULL,adCmdStoredProc);  

		//这里有三种办法来取这些output参数值  
		outparam = m_CommandPtr->Parameters->GetItem("strErrInfo")->GetValue();  
		m_CommandPtr.Detach();  
	}
	catch (_com_error e)
	{
		CString ErrMsg;
		ErrMsg.Format("%s", e.Description());
		Log(Error, ErrMsg);
		return ErrMsg;
	}

	return outparam;
}

BOOL Common::GetGameAccFormDB()
{
	CString strPathName;
	struct GameAcc TmpGameAcc;
	int ret = 0;
	_variant_t strValue;

	if(NULL ==  m_pConnection)
	{
		ConnToSQLServer();
	}

	/* connect failed */
	if (NULL == m_pConnection) return FALSE;

	try
	{
		LGameAcc.RemoveAll();
		_variant_t RecordsAffected; 

		m_pRecordset = m_pConnection->Execute("SELECT account,password,password2,duedate,enable,lock_duedate,"
			"logout_time,ip,create_time,privilege,status,sec_pwd,first_ip,point,trade_psw,IsAdult,OnlineTime,"
			"OfflineTime,LastLoginTime,LastLogoutTime from game_acc",
			&RecordsAffected,adCmdText); 

		while(!m_pRecordset->ADOEOF)
		{
			strValue = Common::m_pRecordset->GetCollect("account");
			TmpGameAcc.account = (strValue.vt != VT_NULL) ? strValue : "";
			TmpGameAcc.account.MakeLower();
			strValue = Common::m_pRecordset->GetCollect("password");
			TmpGameAcc.password = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("password2");
			TmpGameAcc.password2 = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("duedate");
			TmpGameAcc.duedate = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("enable");
			TmpGameAcc.enable = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("lock_duedate");
			TmpGameAcc.lock_duedate = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("logout_time");
			TmpGameAcc.logout_time = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("ip");
			TmpGameAcc.ip = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("create_time");
			TmpGameAcc.create_time = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("privilege");
			TmpGameAcc.privilege = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("status");
			TmpGameAcc.status = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("sec_pwd");
			TmpGameAcc.sec_pwd = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("first_ip");
			TmpGameAcc.first_ip = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("point");
			TmpGameAcc.point = (strValue.vt != VT_NULL) ? strValue : "0";
			strValue = Common::m_pRecordset->GetCollect("trade_psw");
			TmpGameAcc.trade_psw = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("IsAdult");
			TmpGameAcc.IsAdult = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("OnlineTime");
			TmpGameAcc.OnlineTime = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("OfflineTime");
			TmpGameAcc.OfflineTime = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("LastLoginTime");
			TmpGameAcc.LastLoginTime = (strValue.vt != VT_NULL) ? strValue : "";
			strValue = Common::m_pRecordset->GetCollect("LastLogoutTime");
			TmpGameAcc.LastLogoutTime = (strValue.vt != VT_NULL) ? strValue : "";

			LGameAcc.AddTail(TmpGameAcc);
			m_pRecordset->MoveNext();
		}
		m_pRecordset->Close();
	}
	catch (_com_error e)
	{
		CString errormessage;
		errormessage.Format("查询失败！\r\n错误信息:%s",e.ErrorMessage());
		Log(Error ,errormessage);
		return  FALSE;

	}

	return TRUE;
}

void Common::GetAccAttr()
{
	CString strPathName;
	struct AccAttr TmpAccAtrr;
	char Head[32];
	int ret = 0;

	LAccAttr.RemoveAll();

	// 以下代码用于保存，暂时不实现
	// 	CString strTemp;
	// 	CFile mFile;
	// 	mFile.Open("d:\dd\try.TRY",CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite);
	// 	CArchive oar(&mFile,CArchive::store);
	// 	oar << oar.Close();
	// 	mFile.Close();

	strPathName = Common::ServerPath + "DataBase\\saves\\players.dat";
	//检查文件是否存在
	DWORD dwRe = GetFileAttributes(strPathName);
	if ( dwRe != (DWORD)-1 )
	{
		//ShellExecute(NULL, NULL, strFilePath, NULL, NULL, SW_RESTORE); 
	}
	else 
	{
		CString errormessage;
		errormessage.Format(strPathName + " 文件不存在！");
		Log(Error, errormessage);
		return;
	}

	CopyFile(strPathName, ".\\Temp\\players.dat", FALSE);

	CFile iFile(".\\Temp\\players.dat", CFile::modeRead | CFile::modeNoTruncate | CFile::shareDenyNone);  
	CArchive iar(&iFile, CArchive::load); 

	ret = sizeof(TmpAccAtrr);
	ret = iar.Read(Head,sizeof(Head));
	for(;ret > 0;)
	{
		ret = iar.Read(&TmpAccAtrr,sizeof(TmpAccAtrr));
		if(ret <= 0) break;

		LAccAttr.AddTail(TmpAccAtrr);
	}

	iar.Close();
	iFile.Close();
}

void Common::GetOrganizeAttr()
{
	CString strPathName;
	struct OrganizeAttr TmpOrganizeAttr;
	char Head[32];
	int ret = 0;

	LOrganizeAttr.RemoveAll();

	strPathName = Common::ServerPath + "DataBase\\saves\\organize.dat";
	//检查文件是否存在
	DWORD dwRe = GetFileAttributes(strPathName);
	if ( dwRe != (DWORD)-1 )
	{
		//ShellExecute(NULL, NULL, strFilePath, NULL, NULL, SW_RESTORE); 
	}
	else 
	{
		CString errormessage;
		errormessage.Format(strPathName + " 文件不存在！");
		Log(Error, errormessage);
		return;
	}

	CopyFile(strPathName, ".\\Temp\\organize.dat", FALSE);

	CFile iFile(strPathName, CFile::modeRead | CFile::modeNoTruncate | CFile::shareDenyNone);  
	CArchive iar(&iFile, CArchive::load); 

	ret = sizeof(TmpOrganizeAttr);
	ret = iar.Read(Head,sizeof(Head));
	for(;ret > 0;)
	{
		ret = iar.Read(&TmpOrganizeAttr,sizeof(TmpOrganizeAttr));
		if(ret <= 0) break;
		LOrganizeAttr.AddHead(TmpOrganizeAttr);
	}

	iar.Close();
	iFile.Close();
}

void Common::GetItemDef()
{
	CString strPathName;
	struct ItemDef TmpItemDef;
	int ret = 0;

	strPathName.Format("%s%s", CONFIG_FILE_PATH, ITEM_H);
	CString szLine = "";
	CString strGap = _T(" ");
	CStringArray strResult;

	//检查文件是否存在
	DWORD dwRe = GetFileAttributes(strPathName);
	if ( dwRe != (DWORD)-1 )
	{
		//ShellExecute(NULL, NULL, strFilePath, NULL, NULL, SW_RESTORE); 
	}
	else 
	{
		CString errormessage;
		errormessage.Format(strPathName + " 文件不存在！");
		Log(Error, errormessage);
		return;
	}

	LItemDef.RemoveAll();

	//打开文件
	CStdioFile StdFile;
	StdFile.Open(strPathName,CFile::modeRead | CFile::shareDenyNone);

	//逐行读取字符串
	while( StdFile.ReadString( szLine ) )
	{
		strResult.RemoveAll();
		szLine.Replace('\t', ' ');
		int nPos = szLine.Find(strGap);
		CString strLeft = _T("");
		while(0 <= nPos)
		{
			strLeft = szLine.Left(nPos);
			if (!strLeft.IsEmpty())
				strResult.Add(strLeft);

			szLine = szLine.Right(szLine.GetLength() - nPos - 1);
			nPos = szLine.Find(strGap);
		}

		if (!szLine.IsEmpty()) {
			strResult.Add(szLine);
		}

		if(!strResult.IsEmpty() && strResult[0].Compare("#define") ==  0)
		{
			TmpItemDef.Name = strResult[1];
			TmpItemDef.ID = strResult[2];
			LItemDef.AddTail(TmpItemDef);
		}
	}

	//关闭文件
	StdFile.Close();
}

void Common::GetStageDef()
{
	CString strPathName;
	struct StageDef TmpStageDef;
	int ret = 0;

	strPathName.Format("%s%s", CONFIG_FILE_PATH, STAGE_H);
	CString szLine = "";
	CString strGap = _T(" ");
	CStringArray strResult;

	//检查文件是否存在
	DWORD dwRe = GetFileAttributes(strPathName);
	if ( dwRe != (DWORD)-1 )
	{
		//ShellExecute(NULL, NULL, strFilePath, NULL, NULL, SW_RESTORE); 
	}
	else 
	{
		CString errormessage;
		errormessage.Format(strPathName + " 文件不存在！");
		Log(Error, errormessage);
		return;
	}

	LStageDef.RemoveAll();

	//打开文件
	CStdioFile StdFile;
	StdFile.Open(strPathName,CFile::modeRead | CFile::shareDenyNone);

	//逐行读取字符串
	while( StdFile.ReadString( szLine ) )
	{
		strResult.RemoveAll();
		szLine.Replace('\t', ' ');
		int nPos = szLine.Find(strGap);
		CString strLeft = _T("");
		while(0 <= nPos)
		{
			strLeft = szLine.Left(nPos);
			if (!strLeft.IsEmpty())
				strResult.Add(strLeft);

			szLine = szLine.Right(szLine.GetLength() - nPos - 1);
			nPos = szLine.Find(strGap);
		}

		if (!szLine.IsEmpty()) {
			strResult.Add(szLine);
		}

		if(!strResult.IsEmpty() && strResult[0].Compare("#define") ==  0)
		{
			TmpStageDef.Name = strResult[1];
			TmpStageDef.ID = strResult[2];
			LStageDef.AddTail(TmpStageDef);
		}
	}

	//关闭文件
	StdFile.Close();
}

void Common::GetSaveItems()
{
	CString strPathName;
	struct SaveItems TmpSaveItems;
	char Head[32];
	int ret = 0;

	LSaveItems.RemoveAll();

	strPathName = Common::ServerPath + "DataBase\\saves\\item.dat";
	//检查文件是否存在
	DWORD dwRe = GetFileAttributes(strPathName);
	if ( dwRe != (DWORD)-1 )
	{
		//ShellExecute(NULL, NULL, strFilePath, NULL, NULL, SW_RESTORE); 
	}
	else 
	{
		CString errormessage;
		errormessage.Format(strPathName + " 文件不存在！");
		Log(Error, errormessage);
		return;
	}

	CopyFile(strPathName, ".\\Temp\\item.dat", FALSE);

	CFile iFile(".\\Temp\\item.dat", CFile::modeRead | CFile::modeNoTruncate | CFile::shareDenyNone);  
	CArchive iar(&iFile, CArchive::load); 

	ret = sizeof(TmpSaveItems);
	ret = iar.Read(Head,sizeof(Head));
	for(;ret > 0;)
	{
		ret = iar.Read(&TmpSaveItems,sizeof(TmpSaveItems));
		if(ret <= 0) break;
		LSaveItems.AddTail(TmpSaveItems);
	}

	iar.Close();
	iFile.Close();
}

void Common::GetSaveStores()
{
	CString strPathName;
	struct SaveStores TmpSaveStores;
	char Head[32];
	int ret = 0;

	LSaveStores.RemoveAll();

	strPathName = Common::ServerPath + "DataBase\\saves\\store.dat";
	//检查文件是否存在
	DWORD dwRe = GetFileAttributes(strPathName);
	if ( dwRe != (DWORD)-1 )
	{
		//ShellExecute(NULL, NULL, strFilePath, NULL, NULL, SW_RESTORE); 
	}
	else 
	{
		CString errormessage;
		errormessage.Format(strPathName + " 文件不存在！");
		Log(Error, errormessage);
		return;
	}

	CopyFile(strPathName, ".\\Temp\\store.dat", FALSE);

	CFile iFile(".\\Temp\\store.dat", CFile::modeRead | CFile::modeNoTruncate | CFile::shareDenyNone);  
	CArchive iar(&iFile, CArchive::load); 

	ret = sizeof(TmpSaveStores);
	ret = iar.Read(Head,sizeof(Head));
	for(;ret > 0;)
	{
		ret = iar.Read(&TmpSaveStores,sizeof(TmpSaveStores));
		if(ret <= 0) break;
		LSaveStores.AddTail(TmpSaveStores);
	}

	iar.Close();
	iFile.Close();
}

void Common::GetServerConfig()
{
	CString strPathName;
	ServerConfigTime *CurTimeForCB = &Common::TimeForCB;
	int ret = 0;
	int GetValueStep = 0;
	int index;

	//读取赤壁战场配置
	strPathName = Common::ServerPath + "login\\LoginHistory.ini";
	CString szLine = "";
	CString strResult;

	//检查文件是否存在
	DWORD dwRe = GetFileAttributes(strPathName);
	if ( dwRe != (DWORD)-1 )
	{
		//ShellExecute(NULL, NULL, strFilePath, NULL, NULL, SW_RESTORE); 
	}
	else 
	{
		CString errormessage;
		errormessage.Format("%s 配置文件不存在！", strPathName);
		Log(Error, errormessage);
		return;
	}

	//打开文件
	CStdioFile StdFile;
	StdFile.Open(strPathName,CFile::modeRead | CFile::shareDenyNone);

	//逐行读取字符串
	while( StdFile.ReadString( szLine ) )
	{
		szLine.Replace('\t', ' ');
		szLine.Replace(_T(" "), _T(""));
		index = szLine.Find(';');
		if(index >= 0)
			szLine.Delete(szLine.Find(';'), szLine.GetLength() - szLine.Find(';'));
		if(0 != GetValueStep && szLine.Compare("[history]") == 0) {
			GetValueStep = 0;
			continue;
		}
		if(0 == GetValueStep && szLine.Compare("[history]") == 0) {
			GetValueStep++;
			continue;
		}
		if(1 == GetValueStep && szLine.Compare("type=1") == 0)
		{
			GetValueStep++;
			continue;
		}

		if(2 == GetValueStep)
		{
			index = szLine.Find(',');
			for(int i = 0;index > 0; i++)
			{
				if(0 != i%2) szLine.SetAt(index, ':');
				index = szLine.Find(',', index+1);
			}
		}

		if(2 == GetValueStep && szLine.Find("period") == 0)
		{
			szLine.Replace("period=", "");
			CurTimeForCB->Period = szLine;
			continue;
		}
		if(2 == GetValueStep && szLine.Find("time=1") == 0)
		{
			CStringGap(szLine, &(CurTimeForCB->ModStartTimes), "time=1", ",", 1);
			continue;
		}
		if(2 == GetValueStep && szLine.Find("time=2") == 0)
		{
			CStringGap(szLine, &(CurTimeForCB->TuseStartTimes), "time=2", ",", 1);
			continue;
		}
		if(2 == GetValueStep && szLine.Find("time=3") == 0)
		{
			CStringGap(szLine, &(CurTimeForCB->WedStartTimes), "time=3", ",", 1);
			continue;
		}
		if(2 == GetValueStep && szLine.Find("time=4") == 0)
		{
			CStringGap(szLine, &(CurTimeForCB->ThursStartTimes), "time=4", ",", 1);
			continue;
		}
		if(2 == GetValueStep && szLine.Find("time=5") == 0)
		{
			CStringGap(szLine, &(CurTimeForCB->FriStartTimes), "time=5", ",", 1);
			continue;
		}
		if(2 == GetValueStep && szLine.Find("time=6") == 0)
		{
			CStringGap(szLine, &(CurTimeForCB->SatStartTimes), "time=6", ",", 1);
			continue;
		}
		if(2 == GetValueStep && szLine.Find("time=7") == 0)
		{
			CStringGap(szLine, &(CurTimeForCB->SunStartTimes), "time=7", ",", 1);
			continue;
		}
	}
	//关闭文件
	StdFile.Close();
	/************************************************************************************************/

	//读取国战配置
	CString StrValue;
	CString strStartTime;
	CStringArray strStartDate;
	ServerConfigTime *CurTimeForCwar = &Common::TimeForCwar;
	strPathName = Common::ServerPath + "login\\LoginServer.ini";

	//检查文件是否存在
	dwRe = GetFileAttributes(strPathName);
	if ( dwRe != (DWORD)-1 )
	{		
	}
	else 
	{
		CString errormessage;
		errormessage.Format("%s 配置文件不存在！", strPathName);
		Log(Error, errormessage);
		return;
	}

	GetPrivateProfileString("System","country_war_period","",StrValue.GetBuffer(128),128,strPathName);
	CurTimeForCwar->Period = StrValue;
	StrValue.ReleaseBuffer();

	GetPrivateProfileString("System","country_war_time","",StrValue.GetBuffer(128),128,strPathName);
	strStartTime = StrValue;
	StrValue.ReleaseBuffer();
	strStartTime.Replace(',', ':');

	GetPrivateProfileString("System","country_war_date","",StrValue.GetBuffer(128),128,strPathName);
	CString strTemp = StrValue; // 使用StrValue有问题，需要先ReleaseBuffer
	StrValue.ReleaseBuffer();
	CStringGap(strTemp, &strStartDate, "Cwar", ",", 0);
	for(int i = 0; i < strStartDate.GetCount(); i++)
	{
		switch (_ttoi(strStartDate[i]))
		{
		case 1:
			{
				CurTimeForCwar->ModStartTimes.Add(strStartTime);
				break;
			}
		case 2:
			{
				CurTimeForCwar->TuseStartTimes.Add(strStartTime);
				break;
			}
		case 3:
			{
				CurTimeForCwar->WedStartTimes.Add(strStartTime);
				break;
			}
		case 4:
			{
				CurTimeForCwar->ThursStartTimes.Add(strStartTime);
				break;
			}
		case 5:
			{
				CurTimeForCwar->FriStartTimes.Add(strStartTime);
				break;
			}
		case 6:
			{
				CurTimeForCwar->SatStartTimes.Add(strStartTime);
				break;
			}
		case 7:
			{
				CurTimeForCwar->SunStartTimes.Add(strStartTime);
				break;
			}

		default:
			break;
		}
	}
}

void Common::Log(int Level, CString Msg)
{
	if(Level <= LogLevel)
		::SendMessage(Common::DlgLog, WM_LOGSERVER, 0, (LPARAM)(&Msg));
}