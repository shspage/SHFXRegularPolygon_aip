// Adobe Illustrator Live Effect プラグイン用の設定ダイアログ。
// Dear ImGui のサンプルコード (OSX + OpenGL2) をベースにしています。
// "#" を付したコメントは私のもので、それ以外の英語のはサンプルコードのものです。

// # Dear ImGui : Copyright (c) 2014-2020 Omar Cornut
// # Licensed under the MIT License
// # https://github.com/ocornut/imgui

#include "../imgui/imgui.h"
#include "../imgui/examples/imgui_impl_osx.h"
#include "../imgui/examples/imgui_impl_opengl2.h"
#include <stdio.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import "MyWindowController.h"
#include "../Source/myDialogConfig.h"

@interface MyImGuiView : NSOpenGLView
{
    MyParms* m_parms;
    int m_result;
    ImGuiWindowFlags m_flag;
    std::function<void(void)> m_callbackFunc;
}
-(void)setParms:(MyParms*)parms;
-(void)setCallback: (std::function<void(void)>)callbackFunc;
-(int)getResult;
-(void)updateAndDrawView;
@end

@implementation MyImGuiView
-(void)setParms:(MyParms*)parms
{
    m_parms = parms;
    m_result = 0;
    
    // # 通常のダイアログっぽく見せるためのフラグ。
    // # 基本的に ImGui はウィンドウの中にサブウィンドウ的にダイアログを描画する。
    // # 以下のフラグはこのサブウィンドウに適用される。
    // # ref(Japanese): https://qiita.com/mizuma/items/73218dab2f6b022b0227
    m_flag = 0;
    m_flag |= ImGuiWindowFlags_NoTitleBar; // タイトルバーを非表示にします。
    m_flag |= ImGuiWindowFlags_NoResize; // ウィンドウをリサイズ不可にします。
    m_flag |= ImGuiWindowFlags_NoMove; // ウィンドウを移動不可にします。
    m_flag |= ImGuiWindowFlags_NoScrollbar; // スクロールバーを無効にします。
    m_flag |= ImGuiWindowFlags_NoScrollWithMouse; // マウスホイールでのスクロール操作を無効にします。
    m_flag |= ImGuiWindowFlags_NoCollapse; // タイトルバーをダブルクリックして閉じる挙動を無効にします。
    m_flag |= ImGuiWindowFlags_NoBackground; // ウィンドウ内の背景を非表示にします。
    m_flag |= ImGuiWindowFlags_NoBringToFrontOnFocus; // ウィンドウをクリックしてフォーカスした際に他のウィンドウよりも前面に表示する挙動を無効にします。
    m_flag |= ImGuiWindowFlags_NoNav; // ゲームパッドやキーボードでのUIの操作を無効にします。
    m_flag |= ImGuiWindowFlags_NoSavedSettings; // imgui.iniでウィンドウの位置などを自動保存/ロードさせないようにします。
    m_flag |= ImGuiWindowFlags_AlwaysAutoResize; // 自動でウィンドウ内のコンテンツに合わせてリサイズします。
    m_flag |= ImGuiWindowFlags_NoFocusOnAppearing; // 表示/非表示の際のトランジションアニメーションを無効にします。
}

-(void)setCallback: (std::function<void(void)>)callbackFunc
{
    m_callbackFunc = callbackFunc;
}

-(int)getResult
{
    return m_result;
}

-(void)prepareOpenGL
{
    [super prepareOpenGL];
    
#ifndef DEBUG
    GLint swapInterval = 1;
    [[self openGLContext] setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
    if (swapInterval == 0)
        NSLog(@"Error: Cannot set swap interval.");
#endif
}

-(void)updateAndDrawView
{
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplOSX_NewFrame(self);
    ImGui::NewFrame();
    
    static bool show_preview = true;
    static ImVec4 clear_color = ImVec4(0.25f,0.25f,0.25f,1.0f);
    
    // # ダイアログの部品。流用する際の可変要素。コードはwindowsと共通。
    {
        ImGui::SetNextWindowPos(ImVec2(0,0), 0, ImVec2(0,0));
        
        static bool is_open = true;
        ImGui::Begin("polygon specs", &is_open, m_flag);
        
        ImGui::Text("* polygon specs");
        ImGui::Spacing();
        
        //if(ImGui::SliderFloat("scale(%)", &(m_parms->scale), 0.0f, 200.0f)){
        if(ImGui::InputFloat("scale(%)", &(m_parms->scale), 1.0f, 10.0f, "%.1f")){
            m_parms->scale = fmax(0.1, m_parms->scale);
            if(show_preview) m_callbackFunc();
        }
        
        if(ImGui::InputFloat("angle", &(m_parms->angle), 5.0f, 45.0f, "%.1f")){
            if(show_preview) m_callbackFunc();
        }
        
        if(ImGui::InputInt("vertices", &(m_parms->vertices))){
            m_parms->vertices = fmax(3, m_parms->vertices);
            m_parms->vertices = fmin(kMaxPathSegments, m_parms->vertices);
            if(show_preview) m_callbackFunc();
        }
        
        ImGui::Spacing();
        if(ImGui::Checkbox("preview", &show_preview)){
            if(show_preview) m_callbackFunc();
        }
        
        ImGui::Spacing();
        if (ImGui::Button("Cancel"))
        {
            m_result = 1;
        }
        ImGui::SameLine();
        if (ImGui::Button("  OK  "))
        {
            m_result = 2;
        }

        ImGui::End();
    }
    // # 可変要素ここまで
    
    // Rendering
    ImGui::Render();
    [[self openGLContext] makeCurrentContext];
    
    ImDrawData* draw_data = ImGui::GetDrawData();
    GLsizei width  = (GLsizei)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    GLsizei height = (GLsizei)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    glViewport(0, 0, width, height);
    
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL2_RenderDrawData(draw_data);
    
    // Present
    [[self openGLContext] flushBuffer];
}

-(void)reshape
{
    [[self openGLContext] update];
    [self updateAndDrawView];
}

-(void)drawRect:(NSRect)bounds
{
    [self updateAndDrawView];
}

-(BOOL)acceptsFirstResponder { return (YES); }
-(BOOL)becomeFirstResponder  { return (YES); }
-(BOOL)resignFirstResponder  { return (YES); }

// Forward Mouse/Keyboard events to dear imgui OSX back-end. It returns true when imgui is expecting to use the event.
-(void)keyUp:(NSEvent *)event           { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)keyDown:(NSEvent *)event         { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)flagsChanged:(NSEvent *)event    { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)mouseDown:(NSEvent *)event       { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)mouseUp:(NSEvent *)event         { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)mouseMoved:(NSEvent *)event      { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)mouseDragged:(NSEvent *)event    { ImGui_ImplOSX_HandleEvent(event, self); }
-(void)scrollWheel:(NSEvent *)event     { ImGui_ImplOSX_HandleEvent(event, self); }

@end

//-----------------------------------------------------------------------------------
 @implementation MyWindowController

- (instancetype)initWithWindow:(NSWindow*)window;
{
#ifdef IS_STANDALONE
    // # プラグインのダイアログの場合は以下は不要
    // Make the application a foreground application (else it won't receive keyboard events)
    ProcessSerialNumber psn = {0, kCurrentProcess};
    TransformProcessType(&psn, kProcessTransformToForegroundApplication);
#endif

    [window setTitle:[NSString stringWithUTF8String:kMyDialogTitle]];
    [window setAcceptsMouseMovedEvents:YES];
    [window setOpaque:YES];

    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 32,
        0
    };
    
    NSOpenGLPixelFormat* format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    MyImGuiView* imguiview = [[MyImGuiView alloc] initWithFrame:window.frame pixelFormat:format];
    format = nil;
    [imguiview setParms:nil];
#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1070
    if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6)
        [imguiview setWantsBestResolutionOpenGLSurface:YES];
#endif // MAC_OS_X_VERSION_MAX_ALLOWED >= 1070
    [window setContentView:imguiview];
    
    if ([imguiview openGLContext] == nil)
        NSLog(@"No OpenGL Context!");
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.Fonts->AddFontDefault();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGuiStyle* style = &ImGui::GetStyle();
    // background color of input fields
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.5f, 0.5f, 0.5f, 0.5f);

    // Setup Platform/Renderer bindings
    ImGui_ImplOSX_Init();
    ImGui_ImplOpenGL2_Init();
    
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

    return [super initWithWindow:window];
}

- (int) runModal:(MyParms*)parms completion:(std::function<void(void)>)callbackFunc
{
    int result = 0;
    [self.window.contentView setParms:parms];
    [self.window.contentView setCallback:callbackFunc];
    
    NSModalSession session
    = [[NSApplication sharedApplication] beginModalSessionForWindow:self.window];
    callbackFunc();  // # 初回のプレビューを表示
    while([self.window isVisible]){
        if([NSApp runModalSession: session] != NSModalResponseContinue) break;
        [self.window.contentView updateAndDrawView];
        result = [self.window.contentView getResult];
        if(result != 0) break;
    }
    [NSApp endModalSession:session];
    if(result != 0) [self.window performClose:nil];
    return result;
}

- (void) releaseDialog
{
    ImGui_ImplOSX_Shutdown();  // # 何もしていない
    [self.window.contentView clearGLContext];
    ImGui_ImplOpenGL2_Shutdown();
    ImGui::DestroyContext();
    [self.window close];
}
@end

