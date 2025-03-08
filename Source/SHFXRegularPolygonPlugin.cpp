#define _USE_MATH_DEFINES
#include <math.h>

#include "IllustratorSDK.h"
#include "SHFXRegularPolygonSuites.h"
#include "SHFXRegularPolygonPlugin.h"
#include "SDKErrors.h"
#include "myImGuiDialog.h"
#include <functional>

#ifndef CHKERR
#define CHKERR aisdk::check_ai_error(error)
#endif

Plugin* AllocatePlugin(SPPluginRef pluginRef)
{
    return new SHFXRegularPolygonPlugin(pluginRef);
}

void FixupReload(Plugin* plugin)
{
    SHFXRegularPolygonPlugin::FixupVTable((SHFXRegularPolygonPlugin*)plugin);
}

SHFXRegularPolygonPlugin::SHFXRegularPolygonPlugin(SPPluginRef pluginRef) :
    Plugin(pluginRef),
    fLiveEffect(NULL),
    m_effectMessage(NULL),
    m_previewed(false),
     m_parms({3, 100.0f, 0.0f})
{
#ifdef WIN_ENV
    strncpy(fPluginName, kMaxStringLength, kSHFXRegularPolygonPluginName, kMaxStringLength);
#else
    strncpy(fPluginName, kSHFXRegularPolygonPluginName, kMaxStringLength);
#endif
}

SHFXRegularPolygonPlugin::~SHFXRegularPolygonPlugin()
{
}

ASErr SHFXRegularPolygonPlugin::StartupPlugin(SPInterfaceMessage *message)
{
    ASErr error = kNoErr;
    try {
        error = Plugin::StartupPlugin(message); CHKERR;
        error = AddLiveEffect(message); CHKERR;
    }
    catch (ai::Error& ex) {
        error = ex;
    }
    catch (...) {
        error = kCantHappenErr;
    }
    return error;
}

ASErr SHFXRegularPolygonPlugin::ShutdownPlugin(SPInterfaceMessage *message)
{
    ASErr error = kNoErr;
    error = Plugin::ShutdownPlugin(message);
    return error;
}

// 効果をメニューに追加する
ASErr SHFXRegularPolygonPlugin::AddLiveEffect(SPInterfaceMessage * message) {
    ASErr error = kNoErr;
    AILiveEffectData efdata;
    efdata.self = message->d.self;
    efdata.name = kSHFXRegularPolygonEffectName;  // 一意の識別名称。ローカライズされていないもの
    char title[128];
    ai::UnicodeString(kSHFXRegularPolygonEffectTitle, kAIUTF8CharacterEncoding).as_Platform(title, 128);
    efdata.title = title;  // ローカライズされた表示用名称。アピアランスパレットに表示される
    efdata.majorVersion = 1;
    efdata.minorVersion = 1;
    efdata.prefersAsInput = kPathInputArt;
    efdata.styleFilterFlags = kPreEffectFilter;

    AddLiveEffectMenuData emdata;
    char category[128];
    ai::UnicodeString(kSHFXRegularPolygonEffectCategory, kAIUTF8CharacterEncoding).as_Platform(category, 128);
    emdata.category = category;  // 「効果」メニューの項目名。サブメニューを作らない場合はNULLにする
    char menuTitle[128];
    ai::UnicodeString(kSHFXRegularPolygonEffectCategoryTitle, kAIUTF8CharacterEncoding).as_Platform(menuTitle, 128);
    emdata.title = menuTitle;  // メニュー項目の、ローカライズ可能な表示名称
    emdata.options = 0;

    error = sAILiveEffect->AddLiveEffect(&efdata, &fLiveEffect);
    error = sAILiveEffect->AddLiveEffectMenuItem(fLiveEffect,
        efdata.name,
        &emdata,
        NULL, NULL);
    return error;
}

// 効果を適用する
ASErr SHFXRegularPolygonPlugin::GoLiveEffect(AILiveEffectGoMessage *message) {
    ASErr error = kNoErr;
    try {
        AIArtHandle art = message->art;
        this->getDictionaryValues(message->parameters);

        short type;
        error = sAIArt->GetArtType(art, &type); CHKERR;

        AIRealRect bounds;
        if (type == kPathArt) {
            error = sAIArt->GetArtTransformBounds(art, nullptr, kNoStrokeBounds, &bounds);  CHKERR;
        }
        else {
            error = sAIArt->GetArtBounds(art, &bounds);  CHKERR;
        }
        AIRealPoint center = { (bounds.left + bounds.right) / 2,
            (bounds.top + bounds.bottom) / 2 };

        AIReal radius = sqrt(pow(bounds.right - center.h, 2) + pow(bounds.top - center.v, 2));

        radius *= m_parms.scale / 100;
        if (radius <= 0) return error;

        if (type != kPathArt) {
            // 処理対象がパスでない場合はパスを新規作成する
            AIArtHandle artNew;
            // (# kPlaceBelow か kPlaceAbove かどちらでもいいのかはわからない)
            error = sAIArt->NewArt(kPathArt, kPlaceBelow, art, &artNew); CHKERR;
            art = artNew;
        }

        ai::int16 vcount = max(min(kMaxPathSegments, m_parms.vertices), 3);

        std::vector<AIPathSegment> segs;
        AIReal t = M_PI / 2 + m_parms.angle * M_PI / 180;
        AIReal tUnit = M_PI * 2 / vcount;

        AIRealPoint p = { center.h + radius * cos(t),
            center.v + radius * sin(t) };
        segs.push_back(AIPathSegment{ p,p,p,true });

        for (ai::int16 i = 1; i < vcount; i++) {
            t += tUnit;
            p.h = center.h + radius * cos(t);
            p.v = center.v + radius * sin(t);
            segs.push_back(AIPathSegment{ p,p,p,true });
        }
        
        error = sAIPath->SetPathSegmentCount(art, segs.size()); CHKERR;
        error = sAIPath->SetPathSegments(art, 0, segs.size(), segs.data()); CHKERR;
        error = sAIPath->SetPathClosed(art, true);

        message->art = art;
     }
    catch (ai::Error& ex) {
        error = ex;
    }
    catch (...) {
        error = kCantHappenErr;
    }
    return error;
}

// 処理対象オブジェクトから設定値を取得する。
// 取得できない場合は既定値になる。
ASErr SHFXRegularPolygonPlugin::getDictionaryValues(const AILiveEffectParameters &params) {
    ASErr error = kNoErr;
    AIReal tmpval;
    
    AIDictKey key = sAIDictionary->Key("SHFXRegularPolygon_vcount");
    if (sAIDictionary->IsKnown(params, key)) {
        error = sAIDictionary->GetIntegerEntry(params, key, &m_parms.vertices); CHKERR;
    }
    else {
        m_parms.vertices = 3;
    }

    key = sAIDictionary->Key("SHFXRegularPolygon_scale");
    if (sAIDictionary->IsKnown(params, key)) {
        error = sAIDictionary->GetRealEntry(params, key, &tmpval); CHKERR;
        m_parms.scale = (float)tmpval;
    }
    else {
        m_parms.scale = 100.0f;
    }

    key = sAIDictionary->Key("SHFXRegularPolygon_angle");
    if (sAIDictionary->IsKnown(params, key)) {
        error = sAIDictionary->GetRealEntry(params, key, &tmpval); CHKERR;
        m_parms.angle = (float)tmpval;
    }
    else {
        m_parms.angle = 0.0f;
    }
    return error;
}

// 設定ダイアログに渡すコールバック関数。
// オブジェクトが保持する辞書に値を設定する。
// UpdateParametersを実行すると、GoLiveEffectが実行される。
void SHFXRegularPolygonPlugin::callbackFunc() {
    ASErr error = kNoErr;
    try {
        AIDictKey key = sAIDictionary->Key("SHFXRegularPolygon_vcount");
        error = sAIDictionary->SetIntegerEntry(m_effectMessage->parameters, key, m_tmpParms.vertices); CHKERR;
        key = sAIDictionary->Key("SHFXRegularPolygon_scale");
        error = sAIDictionary->SetRealEntry(m_effectMessage->parameters, key, m_tmpParms.scale); CHKERR;
        key = sAIDictionary->Key("SHFXRegularPolygon_angle");
        error = sAIDictionary->SetRealEntry(m_effectMessage->parameters, key, m_tmpParms.angle); CHKERR;
        error = sAILiveEffect->UpdateParameters(m_effectMessage->context);
        m_previewed = true;
    }
    catch (...) {
        throw;
    }
}

// 設定編集時に呼ばれるメソッド
ASErr SHFXRegularPolygonPlugin::EditLiveEffectParameters(AILiveEffectEditParamMessage *message) {
    ASErr error = kNoErr;
    
    try {
        // プレビューが表示されたかどうか。
        m_previewed = false;
        // コールバック関数で使用するため、message を保持する
        m_effectMessage = message;
        // 処理対象オブジェクトから効果の設定値を取得する。
        // 初回処理時は既定値になる。
        getDictionaryValues(message->parameters);
        // 現時点の設定値を保持する。キャンセル時に復旧するため。
        MyParms saved_parms = m_parms;
        
        // オブジェクトに当該効果が複数使われている場合、クラス変数の m_parms は
        // プレビューで効果が累積的に適用される際にそれぞれの設定値に置き換えられる。
        // このため、編集中の設定値は一時的な構造体に保持する。
        m_tmpParms = m_parms;

        std::function<void(void)> getCallback = std::bind(&SHFXRegularPolygonPlugin::callbackFunc, this);
#ifdef MAC_ENV
        int dialogResult = myImGuiDialog::runModal(&m_tmpParms, getCallback);
#else
        // Illustrator のメインウィンドウのハンドルを取得する。
        // このハンドルを親としてCreateWindowでダイアログを作成しても、
        // メインウィンドウ以外の分離した書類ウィンドウやパレット類に対しては
        // モーダルにならない。この点の対応はmyImGuiDialog側で行なっている。
        AIWindowRef hwndParent;
        error = sAIAppContext->GetPlatformAppWindow(&hwndParent); CHKERR;
        int dialogResult = myImGuiDialog::runModal((HWND)hwndParent, &m_tmpParms, getCallback);
#endif
        // dialogResult = 0:close, 1:cancel, 2:OK
        if (dialogResult == 2) {
            //m_parms = m_tmpParms;
            this->callbackFunc();
        }
        else if (m_previewed) {
            if (message->isNewInstance) 
            {
                // 初回処理時にキャンセルした場合、効果を削除する。
                // AIDocumentSuite にも Undo という関数があるがうまく機能しない。
                error = sAIUndo->UndoChanges(); CHKERR;
            }
            else 
            {
                // 効果を復旧する
                m_tmpParms = saved_parms;
                this->callbackFunc();
            }
            
        }
    }
    catch (ai::Error& ex) {
        error = ex;
    }
    catch (...) {
        error = kCantHappenErr;
    }
    return error;
}

