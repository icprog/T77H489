//#include <vcl.h>
#include "ni488.h"
#include "stdafx.h"
int __stdcall PM_InitialDevice();
void __stdcall PM_Preset();
void __stdcall PM_SetParam(char *pszFreq, double dOffset);
double __stdcall PM_Measure();
