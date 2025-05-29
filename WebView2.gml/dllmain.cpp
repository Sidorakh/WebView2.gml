// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <iostream>
#include <tchar.h>
#include <wrl.h>
#include <wil/com.h>
// <IncludeHeader>
// include WebView2 header
#include "WebView2.h"
// </IncludeHeader>

using namespace Microsoft::WRL;

#define dllx extern "C" __declspec(dllexport)

// pointers to webview controller and webview
static wil::com_ptr<ICoreWebView2Controller> controller;
static wil::com_ptr<ICoreWebView2> webview;
static wil::com_ptr<ICoreWebView2Environment> environment;

wchar_t* char_array_to_wstr(const char* cstr) {
    static size_t len = 16384;
    static wchar_t* out = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, cstr, -1, out, len);
    return out;
}

char* wstr_to_char_array(const wchar_t* wstr) {
    static size_t len = 16384;
    static char* out = new char[len];
    WideCharToMultiByte(CP_UTF8, 0, wstr, len,out,len,NULL,FALSE);
    return out;
}
char* lpwstr_to_char_array(const LPWSTR* wstr) {
    static size_t len = 16384;
    static char* out = new char[len];
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
    wcstombs(out, *wstr, len);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
    return out;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

HWND window = nullptr;
std::string target = "";

BOOL webview_ready = false;

std::map<int, PCWSTR> async_output;
int async_num = 0;


dllx double webview2_init(HWND hwnd) {
    CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [hwnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                environment = env;
                env->CreateCoreWebView2Controller(hwnd, Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                    [hwnd](HRESULT result, ICoreWebView2Controller* ctr) -> HRESULT {
                    if (ctr != nullptr) {
                        controller = ctr;
                        controller->get_CoreWebView2(&webview);
                    }
                    webview_ready = true;
                    return S_OK;
                }).Get());
                return S_OK;
            }
            
        ).Get()
    );
    return 1.0;
}

dllx double webview2_is_ready() {
    return webview_ready ? 1.0 : 0.0;
}

dllx double webview2_is_visible() {
    if (webview_ready == false) {
        return -1.0;
    }
    BOOL is_visible = false;
    controller->get_IsVisible(&is_visible);
    return is_visible ? 1.0 : 0.0;
}

dllx double webview2_set_visible(double visible) {
    if (webview_ready == false) {
        return -1.0;
    }
    controller->put_IsVisible(visible >= 1.0 ? true : false);
    return 1.0;
}

dllx double webview2_maximize(HWND hwnd) {
    if (webview_ready == false) {
        return -1.0;
    }
    RECT bounds;
    GetClientRect(hwnd, &bounds);
    controller->put_Bounds(bounds);

    return 1.0;
}

dllx double webview2_set_size(double left, double top, double right, double bottom) {
    if (webview_ready == false) {
        return -1.0;
    }
    RECT bounds;
    bounds.left = left;
    bounds.top = top;
    bounds.right = right;
    bounds.bottom = bottom;
    
    controller->put_Bounds(bounds);
    return 1.0;
}

dllx double webview2_async_result_exists(double num) {
    int id = (int)num;
    return async_output.count(id) == 1 ? 1.0 : 0.0;
}

dllx char* webview2_async_result_get(double num) {
    int id = (int)num;
    if (async_output.count(id) == 1) {
        char* out = wstr_to_char_array(async_output.at(num));
        std::cout << "Value found! - " << out;
        return out;
    } else {
        char out[8];
        sprintf_s(out, "default");
        std::cout << "Value not found!";
        return out;
    }
}

dllx double webview2_navigate(char* url) {
    if (webview_ready == false) {
        return -1.0;
    }
    wchar_t* target = char_array_to_wstr(url);
    webview->Navigate(target);
    return 1.0;

}

dllx double webview2_navigate_to_string(char* html) {
    if (webview_ready == false) {
        return -1.0;
    }
    wchar_t* page = char_array_to_wstr(html);

    webview->NavigateToString(page);
    return 1.0;
}

dllx double webview2_close() {
    if (webview_ready == false) {
        return -1.0;
    }
    controller->Close();
    return 1.0;
}



dllx double webview2_execute_code(char* code) {
    if (webview_ready == false) {
        return -1.0;
    }
    wchar_t* script = char_array_to_wstr(code);
    int num = async_num++;
    webview->ExecuteScript(script, Callback<ICoreWebView2ExecuteScriptCompletedHandler>(
        [num](HRESULT error, PCWSTR result) -> HRESULT {
            async_output.insert_or_assign(num,result);
            return S_OK;
        }
    ).Get());
    return (double)num;
}

dllx char* webview2_get_document_title() {
    static LPWSTR* title = new LPWSTR[256];
    webview->get_DocumentTitle(title);
    char* out = lpwstr_to_char_array(title);
    return out;
}
