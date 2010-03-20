program bibleditonlinebible;

uses
  Forms,
  main in 'main.pas' {Form1},
  pollthread in 'pollthread.pas',
  utilities in 'utilities.pas',
  books in 'books.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
