// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef PLUGIN2_EXPORTS
#define PLUGIN2_API __declspec(dllexport)
#else
#define PLUGIN2_API __declspec(dllimport)
#endif

// This class is exported from the Plugin2.dll
class PLUGIN2_API CPlugin2 {
public:
	CPlugin2(void);
	// TODO: add your methods here.
};

extern PLUGIN2_API int nPlugin2;

PLUGIN2_API int fnPlugin2(void);
