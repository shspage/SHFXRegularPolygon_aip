#ifndef MyDialogConfig_h
#define MyDialogConfig_h

static const float kMyDialogWidth = 300.0f;

#ifdef __APPLE__
static const float kMyDialogHeight = 170.0f;
#else
static const float kMyDialogHeight = 200.0f;
#endif

static const char* kMyDialogTitle = "Regular Polygon -SHFX";

#ifndef kMaxPathSegments
#define kMaxPathSegments   32000
#endif

#endif /* MyDialogConfig_h */
