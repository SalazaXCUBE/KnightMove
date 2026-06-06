#pragma once
#include <Windows.h>
#include <stdio.h>

BOOL enableWebClient(IN SC_HANDLE Shandle);
LPQUERY_SERVICE_CONFIGW Getconfig(IN SC_HANDLE Chandle);
BOOL EnableRemoteReg(IN SC_HANDLE Shandle);