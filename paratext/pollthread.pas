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

unit pollthread;

interface

uses
  Classes {$IFDEF MSWINDOWS} , Windows {$ENDIF};

var
  response_id : integer;
  bibledit_web_url : string;
  bibledit_web_user : string;

type
  TPollThread = class(TThread)
  protected
    procedure Execute; override;
  public
    request_url : string;
    response_command : string;
    response_body : string;
    response_raw : string;
  end;

implementation

{
  Important: Methods and properties of objects in visual components can only be
  used in a method called using Synchronize, for example,

      Synchronize(UpdateCaption);

  and UpdateCaption could look like,

    procedure TPollThread.UpdateCaption;
    begin
      Form1.Caption := 'Updated in a thread';
    end;

    or

    Synchronize(
      procedure
      begin
        Form1.Caption := 'Updated in thread via an anonymous method'
      end
      )
    );

  where an anonymous method is passed.

  Similarly, the developer can call the Queue method with similar parameters as
  above, instead passing another TThread class as the first parameter, putting
  the calling thread in a queue with the other thread.

}

{ TPollThread }

uses
  IdHTTP, SysUtils, utilities;

procedure TPollThread.Execute;
var
  http : TIdHTTP;
  strings : TStringList;
begin
  NameThreadForDebugging('PollThread1');
  { Place thread code here }
  http := TIdHTTP.Create();
  try
    request_url := bibledit_web_url + '/ipc/getmessage.php?user=' + bibledit_web_user + '&channel=paratext&id=' + IntToStr (response_id);
    response_raw :=  http.Get(request_url);
    strings := TStringList.Create;
    Split (#13, response_raw, strings);
    if (strings.Count = 3) then
    begin
      response_id := StrToInt (strings[0]);
      response_command := strings[1];
      response_body := strings[2];
    end;
    strings.Free;
  except
  end;
end;

initialization
  response_id := 0;

end.
