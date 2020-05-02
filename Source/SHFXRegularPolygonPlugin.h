#ifndef __SHFXRegularPolygonPlugin_h__
#define __SHFXRegularPolygonPlugin_h__

#include "Plugin.hpp"
#include "SHFXRegularPolygonID.h"
#include "MyParms.h"

Plugin* AllocatePlugin(SPPluginRef pluginRef);

void FixupReload(Plugin* plugin);

class SHFXRegularPolygonPlugin : public Plugin
{
public:
    SHFXRegularPolygonPlugin(SPPluginRef pluginRef);
    virtual ~SHFXRegularPolygonPlugin();

    FIXUP_VTABLE_EX(SHFXRegularPolygonPlugin, Plugin);

    ASErr StartupPlugin(SPInterfaceMessage * message);
    ASErr ShutdownPlugin(SPInterfaceMessage * message);
    ASErr GoLiveEffect(AILiveEffectGoMessage *message);
    ASErr EditLiveEffectParameters(AILiveEffectEditParamMessage *message);

    void callbackFunc();
private:
    AILiveEffectHandle fLiveEffect;
    AILiveEffectEditParamMessage* m_effectMessage;
    MyParms m_parms;
    MyParms m_tmpParms;
    bool m_previewed;

    ASErr AddLiveEffect(SPInterfaceMessage * message);
    ASErr getDictionaryValues(const AILiveEffectParameters &params);
};

#endif
