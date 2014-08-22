/* Version Histroy */
/* 
version: 1.1
date: 2011-04-13
1. Add function IP_KillProcName
*/
#ifdef _daixin_support
#define WINAPIN_DLL extern "C" __declspec(dllexport)
#else
#define WINAPIN_DLL extern "C" __declspec(dllimport)
#endif

#ifndef _dx_get_adapter_info_
#define _dx_get_adapter_info_
/* �P�_PC�W�O�_��IP�]�w�b pIpAddr ���q�����d�s�b, ���h��^true, �Ϥ���^false */
/* �ݶǤJ���㪺�ؼ�IP�a�}, ��Ʒ|�R��ARP�w�s�}�ѪRIP���e��,�H�T�{�O�_�����d�P���~�b�P�@���q */
/* ����ƥ�i�ΨӧP�_���wIP���q�����d�O�_�����u���J�}link */
WINAPIN_DLL bool IP_Exits(char *pIpAddr/*in*/);
/* �T�{ IP �a�} pIpAddr �O�_�i�H ping �q, uiPingTimes �����Ƴ]�w,uiPacketSize */
/* ���ʥ]�j�p�A�u�����@�� ping OK, �Y��^ true, �Ϥ���^ false */
WINAPIN_DLL bool IP_Ping(char *pIpAddr/*in*/,unsigned int uiPingTimes/*in*/, unsigned int uiPacketSize/*in, max=65535*/);
/* �T�{ IP �a�} pIpAddr �O�_�i�H ping �q, uiPingTimes �����Ƴ]�w,uiPacketSize */
/* ���ʥ]�j�p�A�u�����@�� ping fail, �Y��^ false, �Ϥ���^ true */
WINAPIN_DLL bool IP_PingContinous(char *pIpAddr/*in*/,unsigned int uiPingTimes/*in*/, unsigned int uiPacketSize/*in, max=65535*/);
/* ���� telnetenable, �Y���榨�\�h��^ true, �Ϥ���^ false */
WINAPIN_DLL bool IP_TelnetEnable(char *pIpAddr/*in*/,char* pMac/*in*/);
/* ������� IP �]�Ʃ��ݪ� MAC �a�}, pMac �e�q���i�֤_13��, �Y����IP�}����� MAC, ��^ true ,�Ϥ���^ false */
WINAPIN_DLL bool IP_ArpMac(char *pIpAddr/*in*/,char* pMac/*out*/);
/* �פ�@�ӫ��w���i�{ */
WINAPIN_DLL bool IP_KillProcName(char *pProcName/*in*/);
#endif