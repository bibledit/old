unit CSRV_TLB;

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
// File generated on 3/20/2010 6:51:06 PM from Type Library described below.

// ************************************************************************  //
// Type Lib: C:\Program Files\Bible\Olb.Exe (1)
// LIBID: {3F3281A0-BD34-11D3-AD8C-0000B49B45CC}
// LCID: 0
// Helpfile: 
// HelpString: Online Bible Com Interface
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
  CSRVMajorVersion = 1;
  CSRVMinorVersion = 0;

  LIBID_CSRV: TGUID = '{3F3281A0-BD34-11D3-AD8C-0000B49B45CC}';

  IID_IProcessOLBRequest: TGUID = '{3F3281A1-BD34-11D3-AD8C-0000B49B45CC}';
  CLASS_ProcessOLBRequest: TGUID = '{3F3281A3-BD34-11D3-AD8C-0000B49B45CC}';
type

// *********************************************************************//
// Forward declaration of types defined in TypeLibrary                    
// *********************************************************************//
  IProcessOLBRequest = interface;
  IProcessOLBRequestDisp = dispinterface;

// *********************************************************************//
// Declaration of CoClasses defined in Type Library                       
// (NOTE: Here we map each CoClass to its Default Interface)              
// *********************************************************************//
  ProcessOLBRequest = IProcessOLBRequest;


// *********************************************************************//
// Interface: IProcessOLBRequest
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {3F3281A1-BD34-11D3-AD8C-0000B49B45CC}
// *********************************************************************//
  IProcessOLBRequest = interface(IDispatch)
    ['{3F3281A1-BD34-11D3-AD8C-0000B49B45CC}']
    function ExecuteRequest(const Command: WideString): WideString; safecall;
  end;

// *********************************************************************//
// DispIntf:  IProcessOLBRequestDisp
// Flags:     (4416) Dual OleAutomation Dispatchable
// GUID:      {3F3281A1-BD34-11D3-AD8C-0000B49B45CC}
// *********************************************************************//
  IProcessOLBRequestDisp = dispinterface
    ['{3F3281A1-BD34-11D3-AD8C-0000B49B45CC}']
    function ExecuteRequest(const Command: WideString): WideString; dispid 1;
  end;

// *********************************************************************//
// The Class CoProcessOLBRequest provides a Create and CreateRemote method to          
// create instances of the default interface IProcessOLBRequest exposed by              
// the CoClass ProcessOLBRequest. The functions are intended to be used by             
// clients wishing to automate the CoClass objects exposed by the         
// server of this typelibrary.                                            
// *********************************************************************//
  CoProcessOLBRequest = class
    class function Create: IProcessOLBRequest;
    class function CreateRemote(const MachineName: string): IProcessOLBRequest;
  end;

implementation

uses ComObj;

class function CoProcessOLBRequest.Create: IProcessOLBRequest;
begin
  Result := CreateComObject(CLASS_ProcessOLBRequest) as IProcessOLBRequest;
end;

class function CoProcessOLBRequest.CreateRemote(const MachineName: string): IProcessOLBRequest;
begin
  Result := CreateRemoteComObject(MachineName, CLASS_ProcessOLBRequest) as IProcessOLBRequest;
end;

end.
