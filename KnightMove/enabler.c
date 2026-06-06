#include "module.h"
BOOL enableWebClient(IN SC_HANDLE Shandle) {
	SC_HANDLE ServiceHandle = OpenServiceW(Shandle, L"WebClient", SERVICE_CHANGE_CONFIG | SERVICE_START);
	if (ServiceHandle == NULL) {
		printf("failed to get handle of webclient %d\n", GetLastError());
		return FALSE;
	}

	if (!ChangeServiceConfigW(ServiceHandle, SERVICE_NO_CHANGE, 0x00000002, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL)) {
		printf("failed to change coinfig %d\n", GetLastError());
        CloseServiceHandle(ServiceHandle);
        return FALSE;
	}
	if (StartServiceW(ServiceHandle, 0, NULL) == 0) {
        int error = GetLastError();
        if (error == ERROR_SERVICE_ALREADY_RUNNING) {
            printf("Service is already running\n");
        }
        else {
            printf("WebClient failed to start %d", error);
            CloseServiceHandle(ServiceHandle);
            return FALSE;
        }
        
	}
	return TRUE;
}




LPQUERY_SERVICE_CONFIGW Getconfig(IN SC_HANDLE Chandle) {
    DWORD bytesNeeded = 0; 
    if (!QueryServiceConfigW(Chandle, NULL, 0, &bytesNeeded)) {
        DWORD errorCode = GetLastError();
        if (errorCode != ERROR_INSUFFICIENT_BUFFER) {
            printf("QueryServiceConfigW failed: %d\n", errorCode);
            return NULL;
           // goto cleanup;
        }
    }
    LPQUERY_SERVICE_CONFIGW serviceConfig = (LPQUERY_SERVICE_CONFIGW)LocalAlloc(LPTR, bytesNeeded);
    if (serviceConfig == NULL) {
        printf(L"Memory allocation failed\n");
        return NULL;
        //goto cleanup;
    }
    if (!QueryServiceConfigW(Chandle, serviceConfig, bytesNeeded, &bytesNeeded)) {
        DWORD errorCode = GetLastError();
        printf("QueryServiceConfigW failed: %lu\n", errorCode);
        return NULL;
        //goto cleanup;
    }
    if (serviceConfig->lpServiceStartName != NULL) {
        printf("  Start Name:%ls\n", serviceConfig->lpServiceStartName);
    }
    if (serviceConfig->dwStartType != NULL) {
        printf("  start type:%lx\n", serviceConfig->dwStartType);
    }
    if (serviceConfig->dwServiceType != NULL) {
        printf("  service type:%lx\n", serviceConfig->dwServiceType);
    }
    return serviceConfig;
}


BOOL EnableRemoteReg(IN SC_HANDLE Shandle) {
    if (!(ChangeServiceConfigW(
        Shandle, SERVICE_NO_CHANGE, 0x0000002, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL)
        )) {
        int error = GetLastError();
        printf("changing config for remote reg failed with error=%d\n",error);
        
        return FALSE;
    }
    printf("config for reg sucessfuly changed \n");

    if (!StartService(Shandle, 0, NULL)) {
        int error = GetLastError();
        if (error == ERROR_SERVICE_ALREADY_RUNNING) {
            printf("Service is already running\n");
        }
        else {
            printf("Remote Reg failed to start %d", error);
            return FALSE;
        }
    }
    return TRUE;
}