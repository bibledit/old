// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the BIBLEDITLIBRARYWRAPPER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// BIBLEDITLIBRARYWRAPPER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef BIBLEDITLIBRARYWRAPPER_EXPORTS
#define BIBLEDITLIBRARYWRAPPER_API __declspec(dllexport)
#else
#define BIBLEDITLIBRARYWRAPPER_API __declspec(dllimport)
#endif

// This class is exported from the bibleditlibrarywrapper.dll
class BIBLEDITLIBRARYWRAPPER_API Cbibleditlibrarywrapper {
public:
	Cbibleditlibrarywrapper(void);
	// TODO: add your methods here.
};

extern BIBLEDITLIBRARYWRAPPER_API int nbibleditlibrarywrapper;

BIBLEDITLIBRARYWRAPPER_API int fnbibleditlibrarywrapper(void);
