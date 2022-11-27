// AutoNet.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <fstream>
#include <ctime>
#include <WinSock2.h>
#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>
#include <io.h>

#include <stdio.h>
#include <stdlib.h>


// Need to link with Wlanapi.lib and Ole32.lib
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")
//#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )

using namespace std;

#define TARGET_WIFI_CONNECTED 0
#define ENUM_WLAN_INTERFACE_FAILED -1
#define GET_AVAILABLE_NETWORKLIST_FAILED -2
#define DONT_CONNECT_TARGET_WIFI -3
#define SLEEP_TIME 300000

#define INI_PATH ".\\login.ini"

string request(
    "POST /eportal/InterFace.do?method=login HTTP/1.1\r\n"
    "Host: 172.21.2.10:8080\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:107.0) Gecko/20100101 Firefox/107.0\r\n"
    "Accept: */*\r\n"
    "Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2\r\n"
    "Accept-Encoding: gzip, deflate\r\n"
    "Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n"
    "Content-Length: 197\r\n"
    "Origin: http://172.21.2.10:8080\r\n"
    "Connection: close\r\n"
    "Referer: http://172.21.2.10:8080/eportal/index.jsp?nasip=fbba2526398e9984bd6f73050578e0e2\r\n"
    "Cookie: EPORTAL_USER_GROUP=2020%E7%BA%A7%E5%AD%A6%E7%94%9F; EPORTAL_COOKIE_DOMAIN=false; EPORTAL_COOKIE_OPERATORPWD=; EPORTAL_COOKIE_USERNAME=; EPORTAL_COOKIE_PASSWORD=; EPORTAL_COOKIE_SERVER=; EPORTAL_COOKIE_SERVER_NAME=; EPORTAL_AUTO_LAND=; JSESSIONID=\r\n"
    "\r\n"
);

void initRequest() {

    if (_access(INI_PATH, 0) == -1) {
        WritePrivateProfileStringA(
            "info",
            "userId",
            "",
            INI_PATH
        );
        WritePrivateProfileStringA(
            "info",
            "password",
            "",
            INI_PATH
        );
        WritePrivateProfileStringA(
            "info",
            "service",
            "",
            INI_PATH
        );
        ofstream ini;
        ini.open(INI_PATH, ios::app);
        ini << "\r\n\r\n# service\r\n";
        ini << "# 校园网：%25E6%25A0%25A1%25E5%259B%25AD%25E7%25BD%2591\r\n";
        ini << "# 移动：%25E7%25A7%25BB%25E5%258A%25A8\r\n";
        ini << "# 电信：%25E7%2594%25B5%25E4%25BF%25A1\r\n";
        ini << "# 联通：%25E8%2581%2594%25E9%2580%259A\r\n";
        ini.close();
        MessageBoxA(
            NULL,
            "请在login.ini中输入相关信息后再次运行程序",
            "提示",
            MB_OK
        );
        exit(-1);
    }

    char userId[20];
    char password[20];
    char service[50];
    GetPrivateProfileStringA("info", "userId", "", userId, 20, INI_PATH);
    GetPrivateProfileStringA("info", "password", "", password, 20, INI_PATH);
    GetPrivateProfileStringA("info", "service", "", service, 50, INI_PATH);
    if (!(strlen(userId) || strlen(password) || strlen(service))) {
        MessageBoxA(
            NULL,
            "login.ini中相关信息为空！",
            "提示",
            MB_OK
        );
        exit(-2);
    }
    string body("userId=");
    body.append(userId);
    body.append("&password=");
    body.append(password);
    body.append("&service=");
    body.append(service);
    body.append("&queryString=nasip%253Dfbba2526398e9984bd6f73050578e0e2&operatorPwd=&operatorUserId=&validcode=&passwordEncrypt=false");
    request.append(body);
}

int getwifi() {
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2;  
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;
    DWORD dwRetVal = 0;
    int iRet = 0;

    WCHAR GuidString[39] = { 0 };

    unsigned int i, j, k;

    /* variables used for WlanEnumInterfaces  */

    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    PWLAN_INTERFACE_INFO pIfInfo = NULL;

    PWLAN_AVAILABLE_NETWORK_LIST pBssList = NULL;
    PWLAN_AVAILABLE_NETWORK pBssEntry = NULL;

    int iRSSI = 0;

    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);


    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        //wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
        //return 1;
        dwRetVal = ENUM_WLAN_INTERFACE_FAILED;
    }
    else {
        // wprintf(L"Num Entries: %lu\n", pIfList->dwNumberOfItems);
        // wprintf(L"Current Index: %lu\n", pIfList->dwIndex);
        for (i = 0; i < (int)pIfList->dwNumberOfItems; i++) {
            pIfInfo = (WLAN_INTERFACE_INFO*)&pIfList->InterfaceInfo[i];

            //打印接口索引
            //wprintf(L"  Interface Index[%u]:\t %lu\n", i, i);

            //打印网卡接口GUID
            /*iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)GuidString,
                sizeof(GuidString) / sizeof(*GuidString));
            if (iRet == 0)
                wprintf(L"StringFromGUID2 failed\n");
            else {
                wprintf(L"  InterfaceGUID[%d]: %ws\n", i, GuidString); 
            }*/
            // https://baike.baidu.com/item/GUID/3352285?fr=aladdin

            //打印接口描述信息
            /*wprintf(L"  Interface Description[%d]: %ws", i,
                pIfInfo->strInterfaceDescription);
            wprintf(L"\n");*/

            //打印接口连接状态
            /*wprintf(L"  Interface State[%d]:\t ", i);
            switch (pIfInfo->isState) {
            case wlan_interface_state_not_ready:
                wprintf(L"Not ready\n");
                break;
            case wlan_interface_state_connected:
                wprintf(L"Connected\n");
                break;
            case wlan_interface_state_ad_hoc_network_formed:
                wprintf(L"First node in a ad hoc network\n");
                break;
            case wlan_interface_state_disconnecting:
                wprintf(L"Disconnecting\n");
                break;
            case wlan_interface_state_disconnected:
                wprintf(L"Not connected\n");
                break;
            case wlan_interface_state_associating:
                wprintf(L"Attempting to associate with a network\n");
                break;
            case wlan_interface_state_discovering:
                wprintf(L"Auto configuration is discovering settings for the network\n");
                break;
            case wlan_interface_state_authenticating:
                wprintf(L"In process of authenticating\n");
                break;
            default:
                wprintf(L"Unknown state %ld\n", pIfInfo->isState);
                break;
            }*/
            //wprintf(L"\n");

            dwResult = WlanGetAvailableNetworkList(hClient,
                &pIfInfo->InterfaceGuid,
                0,
                NULL,
                &pBssList);

            if (dwResult != ERROR_SUCCESS) {
                //未开启WIFI进入此分支
                /*wprintf(L"WlanGetAvailableNetworkList failed with error: %u\n",
                    dwResult);*/
                //dwRetVal = 1;
                dwRetVal = GET_AVAILABLE_NETWORKLIST_FAILED;
            }
            else {
                //wprintf(L"WLAN_AVAILABLE_NETWORK_LIST for this interface\n");

                //打印可用网络数
                //wprintf(L"  Num Entries: %lu\n\n", pBssList->dwNumberOfItems);

                dwRetVal = DONT_CONNECT_TARGET_WIFI;

                for (j = 0; j < pBssList->dwNumberOfItems; j++) {

                    pBssEntry = (WLAN_AVAILABLE_NETWORK*)&pBssList->Network[j];

                    //打印该网络相关配置文件名
                    // wprintf(L"  Profile Name[%u]:  %ws\n", j, pBssEntry->strProfileName);

                    //打印网络名称
                    /*wprintf(L"  SSID[%u]:\t\t ", j);
                    if (pBssEntry->dot11Ssid.uSSIDLength == 0)
                        wprintf(L"\n");
                    else {
                        for (k = 0; k < pBssEntry->dot11Ssid.uSSIDLength; k++) {
                            wprintf(L"%c", (int)pBssEntry->dot11Ssid.ucSSID[k]);
                        }
                        wprintf(L"\n");
                    }*/

                    //获取wifi名称
                    const char* targetWifiName = "i-Zijin";
                    char wifiName[33] = {0};
                    for (int k = 0; k < pBssEntry->dot11Ssid.uSSIDLength; k++) {
                        wifiName[k] = pBssEntry->dot11Ssid.ucSSID[k];
                    }

                    if (!strcmp(targetWifiName, wifiName)) {
                        if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED) {
                            dwRetVal = TARGET_WIFI_CONNECTED;
                            break;
                        }
                    }

                    //打印网络类型
                    /*wprintf(L"  BSS Network type[%u]:\t ", j);
                    switch (pBssEntry->dot11BssType) {
                    case dot11_BSS_type_infrastructure:
                        wprintf(L"Infrastructure (%u)\n", pBssEntry->dot11BssType);
                        break;
                    case dot11_BSS_type_independent:
                        wprintf(L"Independent (%u)\n", pBssEntry->dot11BssType);
                        break;
                    default:
                        wprintf(L"Other (%lu)\n", pBssEntry->dot11BssType);
                        break;
                    }*/

                    //打印该网络的MAC地址
                    /*wprintf(L"  BSSID(mac)[%u]:\t ", j);
                    PWLAN_BSS_LIST ppWlanBssList;                   
                    DWORD dwResult2 = WlanGetNetworkBssList(hClient, &pIfInfo->InterfaceGuid,
                        &pBssEntry->dot11Ssid,
                        pBssEntry->dot11BssType,
                        pBssEntry->bSecurityEnabled,
                        NULL,
                        &ppWlanBssList);
                    if (dwResult2 == ERROR_SUCCESS)
                    {
                        WLAN_BSS_ENTRY bssEntry = ppWlanBssList->wlanBssEntries[0];

                        wprintf(L"%02X:%02X:%02X:%02X:%02X:%02X\n",
                            bssEntry.dot11Bssid[0],
                            bssEntry.dot11Bssid[1],
                            bssEntry.dot11Bssid[2],
                            bssEntry.dot11Bssid[3],
                            bssEntry.dot11Bssid[4],
                            bssEntry.dot11Bssid[5]);
                    }*/

                    //指示网络是否可连接。如果设置为TRUE，则网络是可连接的，否则网络将无法连接。
                    /*wprintf(L"  Connectable[%u]:\t ", j);
                    if (pBssEntry->bNetworkConnectable)
                        wprintf(L"Yes\n");
                    else {
                        wprintf(L"No\n");
                        wprintf(L"  Not connectable WLAN_REASON_CODE value[%u]:\t %u\n", j,
                            pBssEntry->wlanNotConnectableReason);
                    }*/

                    // 打印网络质量
                    /*wprintf(L"  Signal Quality[%u]:\t %u (RSSI: %i dBm)\n", j,
                        pBssEntry->wlanSignalQuality, iRSSI);*/

                    //是否启用密码（公共网络或私人？）
                    /*wprintf(l"  security enabled[%u]:\t ", j);
                    if (pbssentry->bsecurityenabled)
                        wprintf(l"yes\n");
                    else
                        wprintf(l"no\n");*/

                    //显示网络的状态（已连接或有配置文件）
                    /*wprintf(L"  Flags[%u]:\t 0x%x", j, pBssEntry->dwFlags);
                    if (pBssEntry->dwFlags) {
                        if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED)
                            wprintf(L" - Currently connected");
                        if (pBssEntry->dwFlags & WLAN_AVAILABLE_NETWORK_HAS_PROFILE)
                            wprintf(L" - Has profile");
                    }*/

                    //wprintf(L"\n");
                    //wprintf(L"\n");
                }
            }
        }

    }
    if (pBssList != NULL) {
        WlanFreeMemory(pBssList);
        pBssList = NULL;
    }

    if (pIfList != NULL) {
        WlanFreeMemory(pIfList);
        pIfList = NULL;
    }

    return dwRetVal;
}

bool pingOnlyOnce(const string IP)
{
    //ofstream outFile;
    //outFile.open("./PingResult.txt", ios::app);

    bool bRet = false;

    // chcp 437 这个命令把操作系统默认语言强制转换成英文显示，便于解析。
    // chcp 437 && ping -n 1 
    const string PING_CMD = "ping -n 1 ";
    string PING_CMD_IP(PING_CMD);
    PING_CMD_IP.append(IP);

    char pingResult[512] = { 0 };

    // 通过管道调用Ping命令
    AllocConsole();    //为调用进程分配一个新的控制台
    ShowWindow(GetConsoleWindow(), SW_HIDE);    //隐藏自己创建的控制台
    FILE* pPipe;
    if (NULL == (pPipe = _popen(PING_CMD_IP.c_str(), "rt")))
    {
        exit(1);
    }

    bRet = false;

    // 解析ping结果
    while (fgets(pingResult, 512, pPipe))
    {
        //outFile << pingResult;

        // Ping 失败通常有两种情况：一种是timeout;
        // 一种是，路由器返回"Destination host unreachable",此时ping命令还没有timeout。所以在ping结果中，两种情况存在任意一种，都是ping失败了。
        if (strstr(pingResult, "请求超时") != NULL
            || strstr(pingResult, "无法访问目标主机") != NULL)
        {
            bRet = false;
            break;
        }
        // 在Ping一次的情况下，Ping成功一定会有"Lost =0"字符串出现。
        else if (strstr(pingResult, "丢失 = 0") != NULL)
        {
            bRet = true;
            //cout << "here2\n";
            break;
        }
    }

    //outFile << "\r\n";
    //outFile.close();

    _pclose(pPipe);
    return bRet;
}

void authentication(ofstream* outFile) {
    WSADATA data;
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &data) != 0)  //如果找不到ws2_32.lib文件，直接返回
    {
        //printf("WSAStartup error !");
        *outFile << "WSAStartup error\r\n";
        return;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (clientSocket == INVALID_SOCKET)
    {
        //printf("Invalid socket !");
        *outFile << "Invalid socket\r\n";
        WSACleanup();
        return;
    }
    
    SOCKADDR_IN serverAddr = { 0 };
    int addrLen = sizeof(sockaddr_in);
    int ret = 0;

    // 设置目标服务器的地址信息
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.S_un.S_addr = inet_addr("172.21.2.10");
    
    // 验证连接是否成功
    ret = connect(clientSocket, (SOCKADDR*)&serverAddr, addrLen);
    if (ret == SOCKET_ERROR) {
        //printf("Connect error !");
        *outFile << "Connection to Server failed\r\n";
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    ret = send(clientSocket, request.c_str(), request.length(), 0);
    if (ret == SOCKET_ERROR) {
        *outFile << "The request failed to send\r\n";
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    *outFile << "Authentication successful\r\n";

    closesocket(clientSocket);

    WSACleanup();
}

int main() {
    // 创建全局句柄，防止脚本后台多开
    /*HANDLE singleEvent = CreateMutex(NULL, FALSE, L"Global\\AutoNet");
    if (!singleEvent || ::GetLastError() == ERROR_ALREADY_EXISTS)  return 1;*/

    initRequest();

    while (true) {
        if (!pingOnlyOnce("39.156.66.10"))
        {
            time_t sysTime = time(0);
            tm* localTime = localtime(&sysTime);
            char* timeStr = ctime(&sysTime);
            *(timeStr + strlen(timeStr) - 1) = '\0';

            int hour = localTime->tm_hour;
            //int weekday = localTime->tm_wday;
            if (hour > 5 && hour < 23) {

                ofstream logFile;
                logFile.open("./result.log", ios::app);
                logFile << '[' << timeStr << ']' << ' ';

                switch (getwifi()) {

                case TARGET_WIFI_CONNECTED: {
                    authentication(&logFile);
                    break;
                }
                case ENUM_WLAN_INTERFACE_FAILED: {
                    logFile << "Enumerating WLAN interfaces failed\r\n";
                    break;
                }
                case GET_AVAILABLE_NETWORKLIST_FAILED: {
                    logFile << "Failed to get list of available networks\r\n";
                    break;
                }
                case DONT_CONNECT_TARGET_WIFI: {
                    logFile << "Not connected to the target network\r\n";
                    break;
                }
                default: {
                    logFile << "An unknown error has occurred\r\n";
                }

                }
                logFile.close();
            }
        }

        Sleep(SLEEP_TIME);
    }
    //if(!pingOnlyOnce("39.156.66.10")) cout << "here3";
    return 0;
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
