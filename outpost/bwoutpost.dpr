program bwoutpost;

uses
  Forms,
  mainunit in 'mainunit.pas' {Form1},
  santafefocus in 'santafefocus.pas',
  sendkey32 in 'sendkey32.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
