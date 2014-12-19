unit books;

interface

function BookIdToParatext (ID : integer) : string;

implementation

type
  TBook = Record
    bibledit : integer;
    paratext : string;
  end;
var
  BookData : array [1..88] of TBook =
  (
    ( bibledit : 67; paratext :   'FRT'; ),
    ( bibledit : 1; paratext :   'GEN'; ),
    ( bibledit : 2; paratext :   'EXO'; ),
    ( bibledit : 3; paratext :   'LEV'; ),
    ( bibledit : 4; paratext :   'NUM'; ),
    ( bibledit : 5; paratext :   'DEU'; ),
    ( bibledit : 6; paratext :   'JOS'; ),
    ( bibledit : 7; paratext :   'JDG'; ),
    ( bibledit : 8; paratext :   'RUT'; ),
    ( bibledit : 9; paratext :   '1SA'; ),
    ( bibledit : 10; paratext :   '2SA'; ),
    ( bibledit : 11; paratext :   '1KI'; ),
    ( bibledit : 12; paratext :   '2KI'; ),
    ( bibledit : 13; paratext :   '1CH'; ),
    ( bibledit : 14; paratext :   '2CH'; ),
    ( bibledit : 15; paratext :   'EZR'; ),
    ( bibledit : 16; paratext :   'NEH'; ),
    ( bibledit : 17; paratext :   'EST'; ),
    ( bibledit : 18; paratext :   'JOB'; ),
    ( bibledit : 19; paratext :   'PSA'; ),
    ( bibledit : 20; paratext :   'PRO'; ),
    ( bibledit : 21; paratext :   'ECC'; ),
    ( bibledit : 22; paratext :   'SNG'; ),
    ( bibledit : 23; paratext :   'ISA'; ),
    ( bibledit : 24; paratext :   'JER'; ),
    ( bibledit : 25; paratext :   'LAM'; ),
    ( bibledit : 26; paratext :   'EZK'; ),
    ( bibledit : 27; paratext :   'DAN'; ),
    ( bibledit : 28; paratext :   'HOS'; ),
    ( bibledit : 29; paratext :   'JOL'; ),
    ( bibledit : 30; paratext :   'AMO'; ),
    ( bibledit : 31; paratext :   'OBA'; ),
    ( bibledit : 32; paratext :   'JON'; ),
    ( bibledit : 33; paratext :   'MIC'; ),
    ( bibledit : 34; paratext :   'NAM'; ),
    ( bibledit : 35; paratext :   'HAB'; ),
    ( bibledit : 36; paratext :   'ZEP'; ),
    ( bibledit : 37; paratext :   'HAG'; ),
    ( bibledit : 38; paratext :   'ZEC'; ),
    ( bibledit : 39; paratext :   'MAL'; ),
    ( bibledit : 40; paratext :   'MAT'; ),
    ( bibledit : 41; paratext :   'MRK'; ),
    ( bibledit : 42; paratext :   'LUK'; ),
    ( bibledit : 43; paratext :   'JHN'; ),
    ( bibledit : 44; paratext :   'ACT'; ),
    ( bibledit : 45; paratext :   'ROM'; ),
    ( bibledit : 46; paratext :   '1CO'; ),
    ( bibledit : 47; paratext :   '2CO'; ),
    ( bibledit : 48; paratext :   'GAL'; ),
    ( bibledit : 49; paratext :   'EPH'; ),
    ( bibledit : 50; paratext :   'PHP'; ),
    ( bibledit : 51; paratext :   'COL'; ),
    ( bibledit : 52; paratext :   '1TH'; ),
    ( bibledit : 53; paratext :   '2TH'; ),
    ( bibledit : 54; paratext :   '1TI'; ),
    ( bibledit : 55; paratext :   '2TI'; ),
    ( bibledit : 56; paratext :   'TIT'; ),
    ( bibledit : 57; paratext :   'PHM'; ),
    ( bibledit : 58; paratext :   'HEB'; ),
    ( bibledit : 59; paratext :   'JAS'; ),
    ( bibledit : 60; paratext :   '1PE'; ),
    ( bibledit : 61; paratext :   '2PE'; ),
    ( bibledit : 62; paratext :   '1JN'; ),
    ( bibledit : 63; paratext :   '2JN'; ),
    ( bibledit : 64; paratext :   '3JN'; ),
    ( bibledit : 65; paratext :   'JUD'; ),
    ( bibledit : 66; paratext :   'REV'; ),
    ( bibledit : 68; paratext :   'BAK'; ),
    ( bibledit : 69; paratext :   'OTH'; ),
    ( bibledit : 70; paratext :   'TOB'; ),
    ( bibledit : 71; paratext :   'JDT'; ),
    ( bibledit : 72; paratext :   'ESG'; ),
    ( bibledit : 73; paratext :   'WIS'; ),
    ( bibledit : 74; paratext :   'SIR'; ),
    ( bibledit : 75; paratext :   'BAR'; ),
    ( bibledit : 76; paratext :   'LJE'; ),
    ( bibledit : 77; paratext :   'S3Y'; ),
    ( bibledit : 78; paratext :   'SUS'; ),
    ( bibledit : 79; paratext :   'BEL'; ),
    ( bibledit : 80; paratext :   '1MA'; ),
    ( bibledit : 81; paratext :   '2MA'; ),
    ( bibledit : 82; paratext :   '1ES'; ),
    ( bibledit : 83; paratext :   'MAN'; ),
    ( bibledit : 84; paratext :   'PS2'; ),
    ( bibledit : 85; paratext :   '3MA'; ),
    ( bibledit : 86; paratext :   '2ES'; ),
    ( bibledit : 87; paratext :   '4MA'; ),
    ( bibledit : 88; paratext :   'DNT'; )
  );

  function BookIdToParatext (ID : integer) : string;
  var
    i : integer;
  begin
    Result := '';
    for i := 1 to 88 do
    begin
      if ID = BookData[i].bibledit then
      begin
        Result := BookData[i].paratext;
      end;
    end;
  end;


end.
