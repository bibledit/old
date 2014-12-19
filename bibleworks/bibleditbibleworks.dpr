program bibleditbibleworks;

uses
  Forms,
  main in 'main.pas' {Form1},
  pollthread in 'pollthread.pas',
  utilities in 'utilities.pas',
  BW800_TLB in 'BW800_TLB.pas',
  books in 'books.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
