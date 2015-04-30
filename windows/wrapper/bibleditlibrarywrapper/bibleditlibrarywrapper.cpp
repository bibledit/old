// bibleditlibrarywrapper.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "bibleditlibrarywrapper.h"


// This is an example of an exported variable
BIBLEDITLIBRARYWRAPPER_API int nbibleditlibrarywrapper=0;

// This is an example of an exported function.
BIBLEDITLIBRARYWRAPPER_API int fnbibleditlibrarywrapper(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see bibleditlibrarywrapper.h for the class definition
Cbibleditlibrarywrapper::Cbibleditlibrarywrapper()
{
	return;
}
