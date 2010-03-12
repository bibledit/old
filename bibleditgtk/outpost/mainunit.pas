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

unit mainunit;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ScktComp, StdCtrls, OleServer, BW800_TLB, CSRV_TLB;

type
  TForm1 = class(TForm)
    Label1: TLabel;
    ServerSocket1: TServerSocket;
    BibleWorksAutomation: TAutomation;
    OnlineBibleAutomation: TProcessOLBRequest;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure ServerSocket1Accept(Sender: TObject;
      Socket: TCustomWinSocket);
    procedure ServerSocket1ClientDisconnect(Sender: TObject;
      Socket: TCustomWinSocket);
    procedure ServerSocket1ClientRead(Sender: TObject;
      Socket: TCustomWinSocket);
    procedure ServerSocket1ClientError(Sender: TObject;
      Socket: TCustomWinSocket; ErrorEvent: TErrorEvent;
      var ErrorCode: Integer);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

const
  Version = '1.7';
  CarriageReturn = #13;
  NewLine = #10;

implementation

uses
  santafefocus, ShellApi;

var
  SocketReceivedTextBuffer : string;
  SocketReceivedTextPure : string;

{$WARN UNIT_PLATFORM OFF}
{$WARN SYMBOL_PLATFORM OFF}
{$R *.dfm}


procedure TForm1.FormCreate(Sender: TObject);
begin
  Label1.Caption := '';
  Caption := 'Bibledit Windows Outpost ' + Version;
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
  try
    BibleWorksAutomation.Disconnect;
  except
  end;
  try
    OnlineBibleAutomation.Disconnect;
  except
  end;
  ServerSocket1.Close;
end;

procedure TForm1.ServerSocket1Accept(Sender: TObject;
  Socket: TCustomWinSocket);
begin
  Label1.Caption := 'Connected to ' + Socket.RemoteAddress;
  try
    Socket.SendText('OK Bibledit Windows Outpost ' + Version + ' Ready' + CarriageReturn + NewLine)
  except
  end;
end;

procedure TForm1.ServerSocket1ClientDisconnect(Sender: TObject;
  Socket: TCustomWinSocket);
begin
  Label1.Caption := 'Disconnected, listening...';
end;

procedure TForm1.ServerSocket1ClientRead(Sender: TObject;
  Socket: TCustomWinSocket);
const
  Error = 'ERROR ';
  Ok = 'OK ';
var
  i : integer;
  EndLine : boolean;
  Reference : string;
  OnlineBible : string;
begin
  SocketReceivedTextBuffer := SocketReceivedTextBuffer + Socket.ReceiveText;
  EndLine := false;
  for i := 1 to Length(SocketReceivedTextBuffer) do
  begin
    if SocketReceivedTextBuffer[i] = CarriageReturn then EndLine := true;
    if SocketReceivedTextBuffer[i] = NewLine then EndLine := true;
  end;
  if EndLine then
  begin
    SocketReceivedTextPure := SocketReceivedTextBuffer;
    SocketReceivedTextBuffer := UpperCase(Trim(SocketReceivedTextBuffer));
    if SocketReceivedTextBuffer = 'QUIT' then
    begin
      try
        Socket.SendText(Ok + 'Goodbye' + CarriageReturn + NewLine);
        Socket.Close;
      except
      end;
    end else if SocketReceivedTextBuffer = 'EXIT' then
    begin
      try
        Socket.SendText(Ok + 'Exiting' + CarriageReturn + NewLine);
        Socket.Close;
        close;
      except
      end;
    end else if SocketReceivedTextBuffer = 'HIDE' then
    begin
      try
        Socket.SendText(Ok + 'Hiding' + CarriageReturn + NewLine);
        Hide;
      except
      end;
    end else if SocketReceivedTextBuffer = 'SHOW' then
    begin
      try
        Socket.SendText(Ok + 'Showing' + CarriageReturn + NewLine);
        Show;
        BringToFront;
      except
      end;
    end else if SocketReceivedTextBuffer = 'VERSION' then
    begin
      try
        Socket.SendText(Ok + Version + CarriageReturn + NewLine);
      except
      end;
    end else if SocketReceivedTextBuffer = 'BIBLEWORKSREFERENCEGET' then
    begin
      begin
        try
          Socket.SendText (Ok + Copy (BibleWorksAutomation.GetCurrentVerse, 5, 100) + CarriageReturn + NewLine);
        except
          Socket.SendText (Error + 'Could not get the reference' + CarriageReturn + NewLine);
        end;
      end;
    end else if Copy (SocketReceivedTextBuffer, 1, 22) = 'BIBLEWORKSREFERENCESET' then
    begin
      Reference := Copy (SocketReceivedTextBuffer, 24, 100);
      try
        BibleWorksAutomation.ClipGoToVerse(false);
        BibleWorksAutomation.IgnoreErrors(true);
        BibleWorksAutomation.GoToVerse(Reference);
        BibleWorksAutomation.IgnoreErrors(false);
        Socket.SendText (Ok + 'Sent ' + Reference + ' to BibleWorks ' + CarriageReturn + NewLine);
      except
        Socket.SendText (Error + 'Could not send the reference' + CarriageReturn + NewLine);
      end;
    end else if SocketReceivedTextBuffer = 'SANTAFEFOCUSREFERENCEGET' then
    begin
      try
        Reference := ReadSantaFeFocusScriptureReference;
        Socket.SendText (Ok + Reference + CarriageReturn + NewLine);
      except
      end;
    end else if Copy (SocketReceivedTextBuffer, 1, 24) = 'SANTAFEFOCUSREFERENCESET' then
    begin
      Reference := Copy (SocketReceivedTextBuffer, 26, 100);
      WriteSantaFeFocusScriptureReference(Reference);
      try
        Socket.SendText (Ok + 'Sent ' + Reference + ' to SantaFeFocus enabled programs' + CarriageReturn + NewLine);
      except
      end;
    end else if Copy (SocketReceivedTextBuffer, 1, 19) = 'SANTAFEFOCUSWORDSET' then
    begin
      Reference := Copy (SocketReceivedTextBuffer, 21, 200);
      WriteSantaFeFocusWord(Reference);
      try
        Socket.SendText (Ok + 'Sent ' + Reference + ' to SantaFeFocus enabled programs' + CarriageReturn + NewLine);
      except
      end;
    end else if Copy (SocketReceivedTextBuffer, 1, 4) = 'OPEN' then
    begin
      Reference := Copy (SocketReceivedTextBuffer, 6, 1000);
      ShellExecute(Handle, 'open', PChar (Reference), nil, nil, SW_SHOWNORMAL);
      try
        Socket.SendText (Ok + 'Opened ' + Reference + CarriageReturn + NewLine);
      except
      end;
    end else if SocketReceivedTextBuffer = 'OLB CONNECT' then
    begin
      try
        OnlineBibleAutomation.Connect();
        OnlineBible := OnlineBibleAutomation.ExecuteRequest ('OpenServer');
        Socket.SendText(Ok + 'Online Bible Connected ' + OnlineBible + CarriageReturn + NewLine);
      except
        Socket.SendText(Error + 'Online Bible not running' + CarriageReturn + NewLine);
      end;
    end else if SocketReceivedTextBuffer = 'OLB DISCONNECT' then
    begin
      try
        OnlineBibleAutomation.Disconnect();
        Socket.SendText(Ok + 'Online Bible Disconnected' + CarriageReturn + NewLine);
      except
      end;
    end else if Copy (SocketReceivedTextBuffer, 1, 3) = 'OLB' then
    begin
      OnlineBible := Copy (SocketReceivedTextPure, 5, 1000);
      try
        OnlineBible := OnlineBibleAutomation.ExecuteRequest (OnlineBible);
        Socket.SendText (Ok + OnlineBible + CarriageReturn + NewLine);
      except
        Socket.SendText (Error + OnlineBible + CarriageReturn + NewLine);
      end;
    end else
    begin
      try
        Socket.SendText(Error + 'Unknown Command: "' + SocketReceivedTextBuffer + '"' + CarriageReturn + NewLine);
      except
      end;
    end;
    SocketReceivedTextBuffer := '';
  end;
end;

procedure TForm1.ServerSocket1ClientError(Sender: TObject;
  Socket: TCustomWinSocket; ErrorEvent: TErrorEvent;
  var ErrorCode: Integer);
begin
  // Discard the error.
  ErrorCode := 0;
end;

initialization

finalization

end.
