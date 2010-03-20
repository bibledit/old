{
Copyright (©) 2003-2009 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 **
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
}

unit main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, AppEvnts, ExtCtrls, OleCtrls, SHDocVw, Sockets, IdBaseComponent,
  IdComponent, IdTCPConnection, IdTCPClient, IdHTTP, StdCtrls, OleServer;

type
  TForm1 = class(TForm)
    Timer1: TTimer;
    TrayIcon1: TTrayIcon;
    Label4: TLabel;
    Label6: TLabel;
    LabelBody: TLabel;
    LabelID: TLabel;
    LabelCommand: TLabel;
    Label5: TLabel;
    ButtonHide: TButton;
    Label1: TLabel;
    EditURL: TEdit;
    Label2: TLabel;
    ButtonURL: TButton;
    LabelBibleWorks: TLabel;
    procedure Timer1Timer(Sender: TObject);
    procedure TrayIcon1Click(Sender: TObject);
    procedure ButtonHideClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure EditURLChange(Sender: TObject);
    procedure ButtonURLClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

uses
  pollthread, Registry, books, utilities, CSRV_TLB;

var
  thread : TPollThread = nil;
  OnlineBible : IProcessOLBRequest;

procedure TForm1.ButtonHideClick(Sender: TObject);
begin
  Form1.Hide;
end;

procedure TForm1.ButtonURLClick(Sender: TObject);
var
  http : TIdHTTP;
  response : string;
  url : string;
begin
  http := TIdHTTP.Create();
  try
    url := bibledit_web_url + '/ipc/setmessage.php';
    response :=  http.Get(url);
  except
    on E: Exception do
      response := E.Message;
  end;
  if response = '' then response := 'Access okay';
  ShowMessage (response);
end;

procedure TForm1.EditURLChange(Sender: TObject);
var
  registry : TRegistry;
begin
  registry := TRegistry.Create;
  registry.RootKey := HKEY_CURRENT_USER;
  registry.OpenKey('\Software\Bibledit\OnlineBible', True);
  registry.WriteString('url', EditURL.Text);
  registry.CloseKey;
  registry.Destroy;
  bibledit_web_url := EditURL.Text;
end;

procedure TForm1.FormCreate(Sender: TObject);
var
  registry : TRegistry;
begin
  registry := TRegistry.Create;
  registry.RootKey := HKEY_CURRENT_USER;
  registry.OpenKey('\Software\Bibledit\OnlineBible', True);
  bibledit_web_url := registry.ReadString('url');
  registry.CloseKey;
  registry.Destroy;
  if bibledit_web_url = '' then
  begin
    bibledit_web_url := 'http://localhost/bibledit';
  end;
  EditURL.Text := bibledit_web_url;
end;

procedure TForm1.Timer1Timer(Sender: TObject);
var
  reference : TStringList;
  book : string;
  olb_reference : string;
begin
  try
    if thread = nil then
    begin
      thread := TPollThread.Create(false);
    end else
    begin
      if thread.Finished then
      begin
        LabelID.Caption := IntToStr (response_id);
        LabelCommand.Caption := thread.response_command;
        LabelBody.Caption := thread.response_body;
        reference := TStringList.Create;
        Split ('.', thread.response_body, reference);
        if (reference.Count = 3) then
        begin
          book := BookIdToOnlineBible (StrToInt (reference[0]));
          olb_reference := 'ShowPassage AV "' + book + ' ' + reference[1] + ':' + reference[2] + '"';
          OnlineBible.ExecuteRequest (olb_reference);
          LabelBibleWorks.Caption := olb_reference;
        end;
        reference.Free;
        thread := TPollThread.Create(false);
      end;
    end;
  except
  end;
end;

procedure TForm1.TrayIcon1Click(Sender: TObject);
begin
  Form1.Visible := not Form1.Visible;
end;

initialization
  OnlineBible := CoProcessOLBRequest.Create;
  OnlineBible.ExecuteRequest ('OpenServer');

end.
