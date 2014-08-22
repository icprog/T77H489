#include "powermeter.h"
#include <stdio.h>
#include "stdafx.h"
//#include <vcl.h>
#include "ni488.h"
#define BDINDEX               0     // Board Index
#define PRIMARY_ADDR_OF_SCOPE 13    // Primary address of device
#define NO_SECONDARY_ADDR     0     // Secondary address of device
#define TIMEOUT               T10s  // Timeout value = 10 seconds
#define EOTMODE               1     // Enable the END message
#define EOSMODE               0     // Disable the EOS mode

int dev=-1;
bool GpibWrite(char *pszCmd);
bool GpibWrite(char *pszCmd, char *pszReturn);

bool bPreseted=false;

bool GpibWrite(char *pszCmd)
{
	return ibwrt(dev,pszCmd,(int)strlen(pszCmd));
}

bool GpibWrite(char *pszCmd, char *pszReturn)
{
	if(!GpibWrite(pszCmd))
		return false;

	char szReadBuf[256]="";
	int length = ibrd(dev,szReadBuf,(int)sizeof(szReadBuf));

	strcpy(pszReturn,szReadBuf);
	return length;
}

void WaitCmdOver()
{
	while(1)
	{
		char szBuffer[256]="";
		GpibWrite("*OPC?",szBuffer);
		if(strstr(szBuffer,"+1"))
		{
			break;
		}
	}
}

int __stdcall PM_InitialDevice()
{
	dev = ibdev(BDINDEX,PRIMARY_ADDR_OF_SCOPE,NO_SECONDARY_ADDR,TIMEOUT,EOTMODE,EOSMODE);
	return dev;
}

void __stdcall PM_Preset()
{
    if(!bPreseted)
    {
        //dev = ibdev(BDINDEX,PRIMARY_ADDR_OF_SCOPE,NO_SECONDARY_ADDR,TIMEOUT,EOTMODE,EOSMODE);
        GpibWrite("SYSTem:PRESet");
        Sleep(1000);
        WaitCmdOver();
        GpibWrite("*CLS");
        GpibWrite("SENS1:CORR:LOSS2:STAT ON");
        GpibWrite("trig:sour int1");//2
		//3
		GpibWrite("SENS:TRAC:TIME 0.5");
		GpibWrite("DISP:WIND1:TRACE:LOW 0");
        bPreseted=true;
    }
}

void __stdcall PM_SetParam(char *pszFreq, double dOffset)
{
	char szGpibCmd[64]="";
	//1
	sprintf(szGpibCmd,"SENS1:CORR:LOSS2 -%0.2f",dOffset);
	GpibWrite(szGpibCmd);
	sprintf(szGpibCmd,"freq %se6",pszFreq);
	GpibWrite(szGpibCmd);
	GpibWrite("init:cont off");
	GpibWrite("init:cont on");
	Sleep(1200);
}
double __stdcall PM_Measure()
{
	char szBuffer[48]="";
	double res=-9999.99;
	//4
	if(GpibWrite("FETCH?",szBuffer))
	{
		res=atof(szBuffer);
		sprintf(szBuffer,"%0.2f",atof(szBuffer));
		//strcpy(pszValOut,szBuffer);
	}

	return res;
}