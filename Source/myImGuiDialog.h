#ifndef MyImGuiDialog_h
#define MyImGuiDialog_h

#ifndef __APPLE__
#include "Windows.h"
#endif
#include <functional>
#include "MyParms.h"

namespace myImGuiDialog {
#ifdef __APPLE__
    // parms : ダイアログで変更するLiveEffectのパラメーターを保持する構造体のポインタ
    // callbackFunc : プラグイン側でプレビューを表示するためのメソッド
    int runModal(MyParms* parms, std::function<void(void)> callbackFunc);
#else
    // hwnd : ダイアログの親ウィンドウのハンドル。
    // イラレSDKでメインウィンドウのHWNDを取得する処理は以下。
    // AIWindowRef hwnd;
    // error = sAIAppContext->GetPlatformAppWindow(&hwnd);
    int runModal(HWND hwnd, MyParms* parms, std::function<void(void)> callbackFunc);
#endif
};

#endif /* MyImGuiDialog_h */
