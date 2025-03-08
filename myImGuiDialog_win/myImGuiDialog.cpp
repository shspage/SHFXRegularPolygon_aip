// Adobe Illustrator Live Effect プラグイン用の設定ダイアログ。
// Dear ImGui のサンプルコード (example_win32_directx9) をベースにしています。
// "#" を付したコメントは私のもので、それ以外の英語のはサンプルコードのものです。

// # Dear ImGui : Copyright (c) 2014-2025 Omar Cornut
// # Licensed under the MIT License
// # https://github.com/ocornut/imgui

#include "imgui.h"
#include "examples/imgui_impl_dx9.h"
#include "examples/imgui_impl_win32.h"
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include "../Source/myImGuiDialog.h"
#include "../Source/myDialogConfig.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace myImGuiDialog
{
    // Data
    static LPDIRECT3D9              g_pD3D = nullptr;
    static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
    static bool                     g_DeviceLost = false;
    static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
    static D3DPRESENT_PARAMETERS    g_d3dpp = {};

    static HWND g_parentHwnd = nullptr;

    // Forward declarations of helper functions
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void ResetDevice();
    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void DisableIllustratorWindows(HWND parentHwnd, HWND dialogHwnd);
    void EnableIllustratorWindows(HWND parentHwnd);

    int runModal(HWND parentHwnd, MyParms* parms, std::function<void(void)> callbackFunc)
    {
        int dialogResult = 0;

        // # initialize
        HWND hwnd = nullptr;
        WNDCLASSEXW wc;
        {
            g_parentHwnd = parentHwnd;

            // Create application window
            WCHAR titlew[64];
            size_t wLen = 0;
            errno_t err = 0;
            err = mbstowcs_s(&wLen, titlew, 64, kMyDialogTitle, _TRUNCATE);

            wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr),
                nullptr, nullptr, nullptr, nullptr, titlew, nullptr };
            ::RegisterClassExW(&wc);

            int windowWidth = (int)kMyDialogWidth;
            int windowHeight = (int)kMyDialogHeight;

            int windowX = 0;
            int windowY = 0;
            RECT rect;
            if (parentHwnd != NULL) {
                GetClientRect(parentHwnd, &rect);
                windowX = max(0, ((rect.left + rect.right) - windowWidth) / 2);
                windowY = max(0, ((rect.top + rect.bottom) - windowHeight) / 2);
            }
            
            hwnd = ::CreateWindowW(wc.lpszClassName, titlew, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                windowX, windowY, windowWidth, windowHeight, parentHwnd, nullptr, wc.hInstance, nullptr);
            if (hwnd == nullptr) return 1;

            // Initialize Direct3D
            if (!CreateDeviceD3D(hwnd))
            {
                CleanupDeviceD3D();
                ::UnregisterClass(wc.lpszClassName, wc.hInstance);
                return 1;
            }

            // Show the window
            ::ShowWindow(hwnd, SW_SHOW);
            ::UpdateWindow(hwnd);

            DisableIllustratorWindows(parentHwnd, hwnd);  // # ダイアログ以外を無効化

            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            //ImGui::StyleColorsClassic();

            ImGuiStyle* style = &ImGui::GetStyle();
            style->Colors[ImGuiCol_FrameBg] = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);

            // Setup Platform/Renderer bindings
            ImGui_ImplWin32_Init(hwnd);
            ImGui_ImplDX9_Init(g_pd3dDevice);

            // Load Fonts
            // Load Fonts
            // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
            // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
            // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
            // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
            // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
            // - Read 'docs/FONTS.md' for more instructions and details.
            // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
            //io.Fonts->AddFontDefault();
            //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
            //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
            //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
            //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
            //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
            //IM_ASSERT(font != nullptr);
        }

        // # 通常のダイアログっぽく見せるためのフラグ。
        // # 基本的に ImGui はウィンドウの中にサブウィンドウ的にダイアログを描画する。
        // # 以下のフラグはこのサブウィンドウに適用される。
        // # ref(Japanese): https://qiita.com/mizuma/items/73218dab2f6b022b0227
        ImGuiWindowFlags flag = 0;
        flag |= ImGuiWindowFlags_NoTitleBar; // タイトルバーを非表示にします。
        flag |= ImGuiWindowFlags_NoResize; // ウィンドウをリサイズ不可にします。
        flag |= ImGuiWindowFlags_NoMove; // ウィンドウを移動不可にします。
        flag |= ImGuiWindowFlags_NoScrollbar; // スクロールバーを無効にします。
        flag |= ImGuiWindowFlags_NoScrollWithMouse; // マウスホイールでのスクロール操作を無効にします。
        flag |= ImGuiWindowFlags_NoCollapse; // タイトルバーをダブルクリックして閉じる挙動を無効にします。
        flag |= ImGuiWindowFlags_NoBackground; // ウィンドウ内の背景を非表示にします。
        flag |= ImGuiWindowFlags_NoBringToFrontOnFocus; // ウィンドウをクリックしてフォーカスした際に他のウィンドウよりも前面に表示する挙動を無効にします。
        flag |= ImGuiWindowFlags_NoNav; // ゲームパッドやキーボードでのUIの操作を無効にします。
        flag |= ImGuiWindowFlags_NoSavedSettings; // imgui.iniでウィンドウの位置などを自動保存/ロードさせないようにします。
        flag |= ImGuiWindowFlags_AlwaysAutoResize; // 自動でウィンドウ内のコンテンツに合わせてリサイズします。
        flag |= ImGuiWindowFlags_NoFocusOnAppearing; // 表示/非表示の際のトランジションアニメーションを無効にします。

        ImVec4 clear_color = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        bool show_preview = true;
        callbackFunc();  // # 初回のプレビューを表示

        // Main loop
        bool done = false;
        while (!done) {
            // Poll and handle messages (inputs, window resize, etc.)
            // See the WndProc() function below for our to dispatch events to the Win32 backend.
            MSG msg;
            while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT) done = true;
            }
            if (done) break;

            // Handle lost D3D9 device
            if (g_DeviceLost)
            {
                HRESULT hr = g_pd3dDevice->TestCooperativeLevel();
                if (hr == D3DERR_DEVICELOST)
                {
                    ::Sleep(10);
                    continue;
                }
                if (hr == D3DERR_DEVICENOTRESET)
                    ResetDevice();
                g_DeviceLost = false;
            }

            // Handle window resize (we don't resize directly in the WM_SIZE handler)
            if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
            {
                g_d3dpp.BackBufferWidth = g_ResizeWidth;
                g_d3dpp.BackBufferHeight = g_ResizeHeight;
                g_ResizeWidth = g_ResizeHeight = 0;
                ResetDevice();
            }

            // Start the Dear ImGui frame
            ImGui_ImplDX9_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            // # ダイアログの部品。流用する際の可変要素。コードはmacと共通。
            {
                ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));
                static bool is_open = true;

                ImGui::Begin("polygon specs", &is_open, flag);

                ImGui::Text("* polygon specs");
                ImGui::Spacing();

                //if (ImGui::SliderFloat("scale(%)", &(parms->scale), 0.0f, 200.0f)) {
                if (ImGui::InputFloat("scale(%)", &(parms->scale), 1.0f, 10.0f, "%.1f")) {
                    parms->scale = fmax(0.1, parms->scale);
                    if (show_preview) callbackFunc();
                }

                if (ImGui::InputFloat("angle", &(parms->angle), 5.0f, 45.0f, "%.1f")) {
                    if (show_preview) callbackFunc();
                }

                if (ImGui::InputInt("vertices", &(parms->vertices))) {
                    parms->vertices = max(3, parms->vertices);
                    parms->vertices = min(kMaxPathSegments, parms->vertices);
                    if (show_preview) callbackFunc();
                }

                ImGui::Spacing();
                if (ImGui::Checkbox("preview", &show_preview)) {
                    if (show_preview) callbackFunc();
                }

                ImGui::Spacing();
                if (ImGui::Button("Cancel")) {
                    dialogResult = 1;
                }
                ImGui::SameLine();
                if (ImGui::Button("  OK  ")) {
                    dialogResult = 2;
                }

                ImGui::End();
            }
            // # Variable element up to here

            // Rendering
            ImGui::EndFrame();
            g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
            g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
            D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
            g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
            if (g_pd3dDevice->BeginScene() >= 0)
            {
                ImGui::Render();
                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
                g_pd3dDevice->EndScene();
            }
            HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
            if (result == D3DERR_DEVICELOST)
                g_DeviceLost = true;

            if (dialogResult != 0) {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
        }

        // Cleanup
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupDeviceD3D();
        ::DestroyWindow(hwnd);
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        hwnd = nullptr;

        return dialogResult;
    }

    void DisableIllustratorWindows(HWND parentHwnd, HWND dialogHwnd) {
        DWORD threadId = GetWindowThreadProcessId(parentHwnd, nullptr);
        EnumThreadWindows(threadId, [](HWND hwnd, LPARAM lParam) -> BOOL {
            HWND dialogHwnd = (HWND)lParam;
            if (hwnd != dialogHwnd) { // ダイアログウィンドウは無効化しない
                EnableWindow(hwnd, FALSE);
            }
            return TRUE;
            }, (LPARAM)dialogHwnd);
    }

    void EnableIllustratorWindows(HWND parentHwnd) {
        DWORD threadId = GetWindowThreadProcessId(parentHwnd, nullptr);
        EnumThreadWindows(threadId, [](HWND hwnd, LPARAM lParam) -> BOOL {
            EnableWindow(hwnd, TRUE);
            return TRUE;
            }, 0);
    }

    // Helper functions

    bool CreateDeviceD3D(HWND hWnd)
    {
        if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
            return false;

        // Create the D3DDevice
        ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
        g_d3dpp.Windowed = TRUE;
        g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
        g_d3dpp.EnableAutoDepthStencil = TRUE;
        g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
        //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
        if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
            return false;

        return true;
    }

    void CleanupDeviceD3D()
    {
        if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
        if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
    }

    void ResetDevice()
    {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
        if (hr == D3DERR_INVALIDCALL)
            IM_ASSERT(0);
        ImGui_ImplDX9_CreateDeviceObjects();
    }


    // Win32 message handler
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
                return 0;
            g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
            g_ResizeHeight = (UINT)HIWORD(lParam);
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_CLOSE:
            EnableIllustratorWindows(g_parentHwnd); // Illustratorのウィンドウを再度有効化
            ::SetForegroundWindow(g_parentHwnd);
            g_parentHwnd = nullptr;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        }
        return ::DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}
