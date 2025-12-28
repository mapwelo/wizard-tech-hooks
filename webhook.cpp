// made by mapwelo 

#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

void SendDiscordWebhook(const wchar_t* webhookUrl, const char* messageJson)
{
    URL_COMPONENTS urlComp{};
    urlComp.dwStructSize = sizeof(urlComp);

    wchar_t host[256];
    wchar_t path[1024];
    urlComp.lpszHostName = host;
    urlComp.dwHostNameLength = _countof(host);
    urlComp.lpszUrlPath = path;
    urlComp.dwUrlPathLength = _countof(path);

    if (!WinHttpCrackUrl(webhookUrl, 0, 0, &urlComp))
        return;

    HINTERNET hSession = WinHttpOpen(L"DiscordWebhook/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
    if (!hSession) return;

    HINTERNET hConnect = WinHttpConnect(
        hSession,
        urlComp.lpszHostName,
        urlComp.nPort,
        0
    );
    if (!hConnect) { WinHttpCloseHandle(hSession); return; }

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"POST",
        urlComp.lpszUrlPath,
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        (urlComp.nPort == 443 ? WINHTTP_FLAG_SECURE : 0)
    );
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return;
    }

    WinHttpSendRequest(
        hRequest,
        L"Content-Type: application/json\r\n",
        -1,
        (LPVOID)messageJson,
        strlen(messageJson),
        strlen(messageJson),
        0
    );

    WinHttpReceiveResponse(hRequest, NULL);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
		const wchar_t* webhook = L"     "; // your webhook goes inbetween the quotes

        const char* json =
            "{"content": "🔔
 DLL Injected: Your DLL has been loaded successfully."}";

        SendDiscordWebhook(webhook, json);
    }
    return TRUE;
}