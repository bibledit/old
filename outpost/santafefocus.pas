// Copyright (©) 2003-2009 Teus Benschop.
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//  

unit santafefocus;

interface

uses
  Classes;

function ReadSantaFeFocusScriptureReference : string;
procedure SantaFeFocusSendReferences (References : TStringList);
procedure WriteSantaFeFocusScriptureReference (Reference : string);
procedure WriteSantaFeFocusWord (Word : string);

implementation

uses
  Registry, Windows, Messages, SysUtils;

Const
  SantaFeFocusKey = '\Software\SantaFe\Focus\';
  ScriptureReferenceKey = 'ScriptureReference';
  ScriptureReferenceListKey = 'ScriptureReferenceList';
  WordKey = 'Word';


function ReadSantaFeFocusScriptureReference : string;
{ Reads the current Scripture reference focus of Paratext from the registry }
var
  Reg: TRegistry;
begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_CURRENT_USER;
  Reg.OpenKey(SantaFeFocusKey + ScriptureReferenceKey, True);
  Result := Reg.ReadString('');
  Reg.CloseKey;
  Reg.Free;
end;

procedure SantaFeFocusSendReferences (References : TStringList);
{
This procedure sends References to the list tool of Paratext.
It uses information given by Nathan Miles about the SantaFeFocus system:

The key points are

1) using RegisterWindowMessage("SantaFeFocus")
to get the correct message number to send to signal that there is
a new focus and

2) Sending 1 as the first message parameter for indivual references or 2
for multiple references.

3) Placing the focus string in the correct registry
location ("HKCU\Software\SantaFe\Focus\ScriptureReference" or
"HKCU\Software\SantaFe\Focus\ScriptureReferenceList")
}
const
  crlf = #13 + #10;
  TabChar = #9;
var
  Reg: TRegistry;
  S : string;
  SantaFeFocusMessage : UInt;
  i : integer;
  Reference : string;
  Ref : string;
begin
  // Set the right values in the registry.
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_CURRENT_USER;
  Reg.OpenKey(SantaFeFocusKey + ScriptureReferenceListKey, True);
  S := '';
  for i := 0 to References.Count - 1 do
  begin
    // References sent via the message systen and the registry must have
    // a certain format, else Paratext does not accept them.
    // It needs a reference, then a tab character, then a number indicator
    // in the format [n], e.g. [1], and then any text.
    // Check the format.
    Reference := Trim (References.Strings [i]);
    Ref := Reference;
    if Ref <> '' then
    begin
      Delete (Reference, 1, Length (Ref));
      Reference := Trim (Reference);
      if Copy (Reference, 1, 1) <> '[' then Reference := '[1]' + Reference;
      Reference := Ref + TabChar + Reference;
      S := S + Reference + crlf;
    end;
  end;
  Reg.WriteString('', S);
  Reg.CloseKey;
  Reg.Free;

  // Send the right message.
  SantaFeFocusMessage := RegisterWindowMessage ('SantaFeFocus');
  PostMessage(HWND_BROADCAST, SantaFeFocusMessage, 2, 0);
end;


procedure WriteSantaFeFocusScriptureReference (Reference : string);
{ Writes reference to Paratext through the SantaFe focus system }
var
  Reg: TRegistry;
  SantaFeFocusMessage : UInt;
begin
  // Place reference in registry.
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_CURRENT_USER;
  Reg.OpenKey(SantaFeFocusKey + ScriptureReferenceKey, True);
  Reg.WriteString('', Reference);
  Reg.CloseKey;
  Reg.Free;
  // Send the right message.
  SantaFeFocusMessage := RegisterWindowMessage ('SantaFeFocus');
  PostMessage(HWND_BROADCAST, SantaFeFocusMessage, 1, 0);
end;

procedure WriteSantaFeFocusWord (Word : string);
{ Writes a word for Toolbox through the SantaFe focus system }
var
  Reg: TRegistry;
  SantaFeFocusMessage : UInt;
begin
  // Place word in registry.
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_CURRENT_USER;
  Reg.OpenKey(SantaFeFocusKey + WordKey, True);
  Reg.WriteString('', Word);
  Reg.CloseKey;
  Reg.Free;
  // Send the right message.
  SantaFeFocusMessage := RegisterWindowMessage ('SantaFeFocus');
  PostMessage(HWND_BROADCAST, SantaFeFocusMessage, 4, 0);
end;

end.
