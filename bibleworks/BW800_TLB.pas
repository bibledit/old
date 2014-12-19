unit BW800_TLB;

// ************************************************************************ //
// WARNING                                                                    
// -------                                                                    
// The types declared in this file were generated from data read from a       
// Type Library. If this type library is explicitly or indirectly (via        
// another type library referring to this type library) re-imported, or the   
// 'Refresh' command of the Type Library Editor activated while editing the   
// Type Library, the contents of this file will be regenerated and all        
// manual modifications will be lost.                                         
// ************************************************************************ //

// $Rev: 17244 $
// File generated on 3/19/2010 8:07:53 PM from Type Library described below.

// ************************************************************************  //
// Type Lib: C:\Program Files\BibleWorks 8\bw800.tlb (1)
// LIBID: {801DD2D6-226D-4E04-9EBA-5CD163B6406E}
// LCID: 0
// Helpfile: 
// HelpString: BW800 6.0 Type Library
// DepndLst: 
//   (1) v2.0 stdole, (C:\Windows\system32\stdole2.tlb)
// ************************************************************************ //
{$TYPEDADDRESS OFF} // Unit must be compiled without type-checked pointers. 
{$WARN SYMBOL_PLATFORM OFF}
{$WRITEABLECONST ON}
{$VARPROPSETTER ON}
{$ALIGN 4}
interface

uses Windows, ActiveX, Classes, Graphics, OleServer, StdVCL, Variants;
  

// *********************************************************************//
// GUIDS declared in the TypeLibrary. Following prefixes are used:        
//   Type Libraries     : LIBID_xxxx                                      
//   CoClasses          : CLASS_xxxx                                      
//   DISPInterfaces     : DIID_xxxx                                       
//   Non-DISP interfaces: IID_xxxx                                        
// *********************************************************************//
const
  // TypeLibrary Major and minor versions
  BW800MajorVersion = 1;
  BW800MinorVersion = 0;

  LIBID_BW800: TGUID = '{801DD2D6-226D-4E04-9EBA-5CD163B6406E}';

  IID_IAutomation: TGUID = '{11A47A67-1133-4D73-BA69-AEDF37DB72F5}';
  CLASS_Automation: TGUID = '{7DE7BFDE-F334-4F7E-BA65-874899A39AAE}';
type

// *********************************************************************//
// Forward declaration of types defined in TypeLibrary                    
// *********************************************************************//
  IAutomation = interface;
  IAutomationDisp = dispinterface;

// *********************************************************************//
// Declaration of CoClasses defined in Type Library                       
// (NOTE: Here we map each CoClass to its Default Interface)              
// *********************************************************************//
  Automation = IAutomation;


// *********************************************************************//
// Declaration of structures, unions and aliases.                         
// *********************************************************************//
  POleVariant1 = ^OleVariant; {*}


// *********************************************************************//
// Interface: IAutomation
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {11A47A67-1133-4D73-BA69-AEDF37DB72F5}
// *********************************************************************//
  IAutomation = interface(IDispatch)
    ['{11A47A67-1133-4D73-BA69-AEDF37DB72F5}']
    function GetCurrentVerse: WideString; safecall;
    procedure SetVersions(const versions: WideString); safecall;
    function GetVersions: WideString; safecall;
    procedure GoToVerse(const verseRef: WideString); safecall;
    procedure GoToBookChapterVerse(const book: WideString; chapter: Integer; verse: Integer); safecall;
    procedure Lookup(const word: WideString); safecall;
    procedure IgnoreErrors(ignore: WordBool); safecall;
    procedure ClipGoToVerse(useClipboard: WordBool); safecall;
    procedure AutoQuit(iautoquit: WordBool); safecall;
    procedure Cmd(const cmdstring: WideString); safecall;
    procedure SetSearchVersion(const cmdstring: WideString); safecall;
    procedure Show(ignore: WordBool); safecall;
    procedure BwHebb2Unicode(var chardata: OleVariant); safecall;
    procedure BwGrkl2Unicode(var chardata: OleVariant); safecall;
    procedure BwLex2Unicode(var chardata: OleVariant); safecall;
    procedure BwSym2Unicode(var chardata: OleVariant); safecall;
    procedure BwLexWordNum(const dtrword: WideString); safecall;
    procedure BwPlayHtmlSwf(const strfile: WideString; const strsizefile: WideString); safecall;
    function BwGetParam(const strname: WideString): Integer; safecall;
    procedure BwSetParam(const strname: WideString; strvalue: Integer); safecall;
    procedure BwSpeak(const strphrase: WideString); safecall;
  end;

// *********************************************************************//
// DispIntf:  IAutomationDisp
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {11A47A67-1133-4D73-BA69-AEDF37DB72F5}
// *********************************************************************//
  IAutomationDisp = dispinterface
    ['{11A47A67-1133-4D73-BA69-AEDF37DB72F5}']
    function GetCurrentVerse: WideString; dispid 1;
    procedure SetVersions(const versions: WideString); dispid 2;
    function GetVersions: WideString; dispid 3;
    procedure GoToVerse(const verseRef: WideString); dispid 4;
    procedure GoToBookChapterVerse(const book: WideString; chapter: Integer; verse: Integer); dispid 5;
    procedure Lookup(const word: WideString); dispid 6;
    procedure IgnoreErrors(ignore: WordBool); dispid 7;
    procedure ClipGoToVerse(useClipboard: WordBool); dispid 8;
    procedure AutoQuit(iautoquit: WordBool); dispid 9;
    procedure Cmd(const cmdstring: WideString); dispid 10;
    procedure SetSearchVersion(const cmdstring: WideString); dispid 11;
    procedure Show(ignore: WordBool); dispid 12;
    procedure BwHebb2Unicode(var chardata: OleVariant); dispid 13;
    procedure BwGrkl2Unicode(var chardata: OleVariant); dispid 14;
    procedure BwLex2Unicode(var chardata: OleVariant); dispid 15;
    procedure BwSym2Unicode(var chardata: OleVariant); dispid 16;
    procedure BwLexWordNum(const dtrword: WideString); dispid 17;
    procedure BwPlayHtmlSwf(const strfile: WideString; const strsizefile: WideString); dispid 18;
    function BwGetParam(const strname: WideString): Integer; dispid 19;
    procedure BwSetParam(const strname: WideString; strvalue: Integer); dispid 20;
    procedure BwSpeak(const strphrase: WideString); dispid 21;
  end;

// *********************************************************************//
// The Class CoAutomation provides a Create and CreateRemote method to          
// create instances of the default interface IAutomation exposed by              
// the CoClass Automation. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoAutomation = class
    class function Create: IAutomation;
    class function CreateRemote(const MachineName: string): IAutomation;
  end;

implementation

uses ComObj;

class function CoAutomation.Create: IAutomation;
begin
  Result := CreateComObject(CLASS_Automation) as IAutomation;
end;

class function CoAutomation.CreateRemote(const MachineName: string): IAutomation;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_Automation) as IAutomation;
end;

end.
