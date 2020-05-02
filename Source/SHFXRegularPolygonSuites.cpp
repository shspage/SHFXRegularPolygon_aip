#include "IllustratorSDK.h"
#include "SHFXRegularPolygonSuites.h"

// Suite externs
extern "C"
{
    SPBlocksSuite*            sSPBlocks = NULL;
    AIUnicodeStringSuite*    sAIUnicodeString = NULL;
    AILiveEffectSuite* sAILiveEffect = NULL;
    AIArtSuite* sAIArt = NULL;
    AIPathSuite* sAIPath = NULL;
    AIPathStyleSuite* sAIPathStyle = NULL;
    AIUndoSuite* sAIUndo = NULL;
    AIDictionarySuite* sAIDictionary = NULL;
    AIAppContextSuite* sAppContext = NULL;
}

// Import suites
ImportSuite gImportSuites[] =
{
    kSPBlocksSuite, kSPBlocksSuiteVersion, &sSPBlocks,
    kAIUnicodeStringSuite, kAIUnicodeStringVersion, &sAIUnicodeString,
    kAILiveEffectSuite, kAILiveEffectSuiteVersion, &sAILiveEffect,
    kAIArtSuite, kAIArtSuiteVersion, &sAIArt,
    kAIPathSuite, kAIPathSuiteVersion, &sAIPath,
    kAIPathStyleSuite, kAIPathStyleSuiteVersion, &sAIPathStyle,
    kAIUndoSuite, kAIUndoSuiteVersion, &sAIUndo,
    kAIDictionarySuite, kAIDictionarySuiteVersion, &sAIDictionary,
    kAIAppContextSuite, kAIAppContextSuiteVersion, &sAppContext,
    nullptr, 0, nullptr
};
