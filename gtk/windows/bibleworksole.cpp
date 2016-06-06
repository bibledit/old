#include <ole2.h> // OLE2 Definitions
#include <stdio.h>

// Source code adapted from https://support.microsoft.com/en-us/kb/216686
// In msys2 on a mingw64 shell:
// Compile with g++ -c bibleworksole.cpp -std=c++11 -mms-bitfields -IC:/msys64/mingw64/include/glibmm-2.4 -IC:/msys64/mingw64/lib/glibmm-2.4/include -IC:/msys64/mingw64/include/glib-2.0 -IC:/msys64/mingw64/lib/glib-2.0/include -IC:/msys64/mingw64/include/sigc++-2.0 -IC:/msys64/mingw64/lib/sigc++-2.0/include
// Link with g++ bibleworksole.o -lole32 -loleaut32 -luuid -mwindows -LC:/msys64/mingw64/lib -lglibmm-2.4 -lgobject-2.0 -lglib-2.0 -lintl -lsigc-2.0
// a.exe will work!

// AutoWrap() - Automation helper function...
HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, const LPOLESTR _ptName, int cArgs...)
{
    // Begin variable-argument list...
    va_list marker;
    va_start(marker, cArgs);

	// The _ptName passed to us is often a string literal, so copy it to a local string
    size_t len = wcslen(_ptName);
	LPOLESTR ptName = (LPOLESTR)malloc((len+1)*sizeof(wchar_t)); // LPOLESTR = wchar_t*
	wcsncpy(ptName, _ptName, len);
	ptName[len] = L'\0';
	
    if(!pDisp) {
        MessageBox(NULL, "NULL IDispatch passed to AutoWrap()", "Error", 0x10010);
        _exit(0);
    }

    // Variables used...
    DISPPARAMS dp = { NULL, NULL, 0, 0 };
    DISPID dispidNamed = DISPID_PROPERTYPUT;
    DISPID dispID;
    HRESULT hr;
    char buf[200];
    char szName[200];

    // Convert down to ANSI
    WideCharToMultiByte(CP_ACP, 0, ptName, -1, szName, 256, NULL, NULL);

    // Get DISPID for name passed...
    hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
	free (ptName);
    if(FAILED(hr)) {
        sprintf(buf, "IDispatch::GetIDsOfNames(\"%s\") failed w/err 0x%08lx", szName, hr);
        MessageBox(NULL, buf, "AutoWrap()", 0x10010);
        _exit(0);
        return hr;
    }
    
    // Allocate memory for arguments...
    VARIANT *pArgs = new VARIANT[cArgs+1];
    // Extract arguments...
    for(int i=0; i<cArgs; i++) {
        pArgs[i] = va_arg(marker, VARIANT);
    }
    
    // Build DISPPARAMS
    dp.cArgs = cArgs;
    dp.rgvarg = pArgs;
    
    // Handle special-case for property-puts!
    if(autoType & DISPATCH_PROPERTYPUT) {
        dp.cNamedArgs = 1;
        dp.rgdispidNamedArgs = &dispidNamed;
    }
    
    // Make the call!
    hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, autoType, &dp, pvResult, NULL, NULL);
    if(FAILED(hr)) {
        sprintf(buf, "IDispatch::Invoke(\"%s\"=%08lx) failed w/err 0x%08lx", szName, dispID, hr);
        MessageBox(NULL, buf, "AutoWrap()", 0x10010);
        _exit(0);
        return hr;
    }
    // End variable-argument section...
    va_end(marker);
    
    delete [] pArgs;
    
    return hr;
}

#include <glibmm/ustring.h>
#include <string>
using namespace Glib;

int main(void)
{
   // Initialize COM for this thread...
   CoInitialize(NULL);

   // Get CLSID for our server...
   CLSID clsid;
   HRESULT hr = CLSIDFromProgID(L"bibleworks.automation", &clsid);

   if(FAILED(hr)) {
      ::MessageBox(NULL, "CLSIDFromProgID() failed", "Error", 0x10010);
      return -1;
   }

   // Start server and get IDispatch...
   IDispatch *pBwApp;
   hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void **)&pBwApp);
   if(FAILED(hr)) {
      ::MessageBox(NULL, "Bibleworks not registered properly", "Error", 0x10010);
      return -2;
   }

   // Tell Bibleworks not to quit. By the way, if you tell it TO quit, it behaves
   // very strangely.
//   {
//      VARIANT x;
//      x.vt = VT_I4;
//      x.lVal = 0; // set AutoQuit to false = 0
//	  AutoWrap(DISPATCH_METHOD, NULL, pBwApp, (wchar_t*)L"AutoQuit", 1, x);
//   }

   // Go to verse
   {
      VARIANT parm;
      parm.vt = VT_BSTR;
	  // Following is the correct way to do this with a string literal, but we won't be
	  // able to use this method in Bibledit
      //parm.bstrVal = ::SysAllocString(L"Luke 14:1");
	  ustring searchstring("Isa 15 1");
	  
	  // This code doesn't result in a memory error or leak.
	  std::wstring wide_string( searchstring.begin(), searchstring.end() );
	  parm.bstrVal = ::SysAllocString(wide_string.c_str()); // this allocates memory, which is freed below

      VARIANT result;
      VariantInit(&result);
      AutoWrap(DISPATCH_METHOD, &result, pBwApp, (wchar_t*)L"GoToVerse", 1, parm);
      VariantClear(&parm); // this will free the memory associated with parm.bstrVal string
   }

   // Wait for user...
   ::MessageBox(NULL, "All done.", "Notice", 0x10000);

   // Release references...
   pBwApp->Release();

   // Uninitialize COM for this thread...
   CoUninitialize();
}