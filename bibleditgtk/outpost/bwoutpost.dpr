program bwoutpost;

uses
  Forms,
  mainunit in 'mainunit.pas' {Form1},
  santafefocus in 'santafefocus.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm1, Form1);
  Application.ShowMainForm := false;
  Application.Run;
end.
