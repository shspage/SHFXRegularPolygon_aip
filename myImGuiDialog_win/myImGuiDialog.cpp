// Adobe Illustrator Live Effect �v���O�C���p�̐ݒ�_�C�A���O�B
// Dear ImGui �̃T���v���R�[�h (example_win32_directx9) ���x�[�X�ɂ��Ă��܂��B
// "#" ��t�����R�����g�͎��̂��̂ŁA����ȊO�̉p��̂̓T���v���R�[�h�̂��̂ł��B

// # Dear ImGui : Copyright (c) 2014-2020 Omar Cornut
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
	static LPDIRECT3D9              g_pD3D = NULL;
	static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
	static D3DPRESENT_PARAMETERS    g_d3dpp = {};

    static HWND g_hwndParent = NULL;

	// Forward declarations of helper functions
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void ResetDevice();
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void EnableParentAndChildren(BOOL enable);

    int runModal(HWND hwnd, MyParms* parms, std::function<void(void)> callbackFunc)
    {
        int dialogResult = 0;

        // # initialize
        HWND hwndDialog;
        WNDCLASSEX wc;
        {
            g_hwndParent = hwnd;
            // Create application window
            WCHAR titlew[64];
            size_t wLen = 0;
            errno_t err = 0;
            err = mbstowcs_s(&wLen, titlew, 64, kMyDialogTitle, _TRUNCATE);

            wc = { sizeof(WNDCLASSEX), CS_OWNDC, WndProc, 0L, 0L, GetModuleHandle(NULL),
                NULL, NULL, NULL, NULL, titlew, NULL };
            ::RegisterClassEx(&wc);

            int windowWidth = kMyDialogWidth;
            int windowHeight = kMyDialogHeight;

            int windowX = 0;
            int windowY = 0;
            RECT rect;
            if (g_hwndParent != NULL) {
                GetClientRect(g_hwndParent, &rect);
                windowX = max(0, ((rect.left + rect.right) - windowWidth) / 2);
                windowY = max(0, ((rect.top + rect.bottom) - windowHeight) / 2);
            }
            
            hwndDialog = ::CreateWindow(wc.lpszClassName, titlew,
                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                windowX, windowY, windowWidth, windowHeight, g_hwndParent, NULL, wc.hInstance, NULL);
            if (hwndDialog == NULL) return 0;

            // Initialize Direct3D
            if (!CreateDeviceD3D(hwndDialog))
            {
                CleanupDeviceD3D();
                ::UnregisterClass(wc.lpszClassName, wc.hInstance);
                return 0;
            }

            // Show the window
            ::ShowWindow(hwndDialog, SW_SHOW);
            ::UpdateWindow(hwndDialog);

            if (g_hwndParent != NULL)EnableParentAndChildren(FALSE);

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
            ImGui_ImplWin32_Init(hwndDialog);
			ImGui_ImplDX9_Init(g_pd3dDevice);

            // Load Fonts
            // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
            // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
            // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
            // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
            // - Read 'docs/FONTS.txt' for more instructions and details.
            // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
            //io.Fonts->AddFontDefault();
            //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
            //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
            //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
            //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
            //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
            //IM_ASSERT(font != NULL);
        }

		// # �ʏ�̃_�C�A���O���ۂ������邽�߂̃t���O�B
		// # ��{�I�� ImGui �̓E�B���h�E�̒��ɃT�u�E�B���h�E�I�Ƀ_�C�A���O��`�悷��B
		// # �ȉ��̃t���O�͂��̃T�u�E�B���h�E�ɓK�p�����B
		// # ref(Japanese): https://qiita.com/mizuma/items/73218dab2f6b022b0227
		ImGuiWindowFlags flag = 0;
		flag |= ImGuiWindowFlags_NoTitleBar; // �^�C�g���o�[���\���ɂ��܂��B
		flag |= ImGuiWindowFlags_NoResize; // �E�B���h�E�����T�C�Y�s�ɂ��܂��B
		flag |= ImGuiWindowFlags_NoMove; // �E�B���h�E���ړ��s�ɂ��܂��B
		flag |= ImGuiWindowFlags_NoScrollbar; // �X�N���[���o�[�𖳌��ɂ��܂��B
		flag |= ImGuiWindowFlags_NoScrollWithMouse; // �}�E�X�z�C�[���ł̃X�N���[������𖳌��ɂ��܂��B
		flag |= ImGuiWindowFlags_NoCollapse; // �^�C�g���o�[���_�u���N���b�N���ĕ��鋓���𖳌��ɂ��܂��B
		flag |= ImGuiWindowFlags_NoBackground; // �E�B���h�E���̔w�i���\���ɂ��܂��B
		flag |= ImGuiWindowFlags_NoBringToFrontOnFocus; // �E�B���h�E���N���b�N���ăt�H�[�J�X�����ۂɑ��̃E�B���h�E�����O�ʂɕ\�����鋓���𖳌��ɂ��܂��B
		flag |= ImGuiWindowFlags_NoNav; // �Q�[���p�b�h��L�[�{�[�h�ł�UI�̑���𖳌��ɂ��܂��B
		flag |= ImGuiWindowFlags_NoSavedSettings; // imgui.ini�ŃE�B���h�E�̈ʒu�Ȃǂ������ۑ�/���[�h�����Ȃ��悤�ɂ��܂��B
		flag |= ImGuiWindowFlags_AlwaysAutoResize; // �����ŃE�B���h�E���̃R���e���c�ɍ��킹�ă��T�C�Y���܂��B
		flag |= ImGuiWindowFlags_NoFocusOnAppearing; // �\��/��\���̍ۂ̃g�����W�V�����A�j���[�V�����𖳌��ɂ��܂��B

        ImVec4 clear_color = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        bool show_preview = true;
		callbackFunc();  // # ����̃v���r���[��\��

        // Main loop
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        while (msg.message != WM_QUIT)
        {
            // Poll and handle messages (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            BOOL bEat;
            if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
				// # �_�C�A���O�����[�_���ȋ����ɂ��邽�߂̏����B
				// # �ȉ��ł̓A�v���P�[�V�����̃E�B���h�E�̈���ŁA�_�C�A���O�̊O���ł̃}�E�X����𖳎����Ă���B
				// # �܂��p���b�g�̑O��֌W�ύX�͉\�ŁA�|�C���^�ʒu�ɉ����ăJ�[�\���`�󂪕ς����������邪�A
				// # ����ȏ�̑���͂ł��Ȃ��B���ރE�B���h�E�̈ړ��Ȃǂ𐧌����邽�߁A�ʓr EnableParentAndChildren
				// # �𕹗p���Ă���B
                // ref: https://stackoverflow.com/questions/734674/creating-a-win32-modal-window-with-createwindow
                bEat = FALSE;
                if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
                {
                    RECT rect;
                    GetWindowRect(hwndDialog, &rect);
                    if (!::PtInRect(&rect, msg.pt)) {
                        if (msg.message == WM_LBUTTONDOWN || msg.message == WM_RBUTTONDOWN
                            || msg.message == WM_MBUTTONDOWN) {
                            MessageBeep(MB_ICONASTERISK);
                        }
                        bEat = TRUE;
                    }
                }
                if (!bEat)
                {
                    ::TranslateMessage(&msg);
                    ::DispatchMessage(&msg);
                }
				continue;
            }

            // Start the Dear ImGui frame
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

			// # �_�C�A���O�̕��i�B���p����ۂ̉ϗv�f�B�R�[�h��mac�Ƌ��ʁB
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
			D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*255.0f), (int)(clear_color.y*255.0f), (int)(clear_color.z*255.0f), (int)(clear_color.w*255.0f));
			g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
			if (g_pd3dDevice->BeginScene() >= 0)
			{
				ImGui::Render();
				ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
				g_pd3dDevice->EndScene();
			}
			HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

			// Handle loss of D3D9 device
			if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
				ResetDevice();

            if (dialogResult != 0) {
                PostMessage(hwndDialog, WM_CLOSE, 0, 0);
            }
        }

        // Cleanup
        {
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();

            CleanupDeviceD3D();
            ::DestroyWindow(hwndDialog);
            ::UnregisterClass(wc.lpszClassName, wc.hInstance);
            hwndDialog = NULL;
        }

        return dialogResult;
    }

    // Helper functions

	bool CreateDeviceD3D(HWND hWnd)
	{
		if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
			return false;

		// Create the D3DDevice
		ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
		g_d3dpp.Windowed = TRUE;
		g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
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
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
		if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
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
    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_SIZE:
			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				g_d3dpp.BackBufferWidth = LOWORD(lParam);
				g_d3dpp.BackBufferHeight = HIWORD(lParam);
				ResetDevice();
			}
			return 0;
		case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_CLOSE:
            // # �e�E�B���h�E��L�������邽�߂̒ǉ������B
			// # �_�C�A���O������O�ɍs���K�v������B
            EnableParentAndChildren(TRUE);
            g_hwndParent = NULL;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        }
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }

    BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lp)
    {
        if (IsWindowVisible(hwnd)) {
            EnableWindow(hwnd, (BOOL)lp);
        }
        return TRUE;
    }

    void EnableParentAndChildren(BOOL enable)
    {
        if (g_hwndParent != NULL) {
            HWND tmpHwnd = GetWindow(g_hwndParent, GW_HWNDFIRST);
            while (tmpHwnd != 0) {
                if (IsWindowVisible(tmpHwnd)) {
                    {
                        HWND parentHwnd = GetParent(tmpHwnd);
                        if (parentHwnd == g_hwndParent) {
                            EnableWindow(tmpHwnd, enable);
                        }
                    }
                }
                tmpHwnd = GetWindow(tmpHwnd, GW_HWNDNEXT);
            }
			// # �q�E�B���h�E�ɓK�p���鏈���B���ʂ��m�F�ł��Ȃ��̂ŃR�����g�A�E�g���Ă���B
            //EnumChildWindows(g_hwndParent, EnumChildProc, (LPARAM)enable);

            EnableWindow(g_hwndParent, enable);
        }
    }
}
