unit books;

interface

function BookIdToBibleWorks (ID : integer) : string;

implementation

type
  TBook = Record
    bibledit : integer;
    bibleworks : string;
  end;
var
  BookData : array [1..88] of TBook =
  (
    ( bibledit : 67; bibleworks :   ''; ),
    ( bibledit : 1; bibleworks :   'Gen'; ),
    ( bibledit : 2; bibleworks :   'Exo'; ),
    ( bibledit : 3; bibleworks :   'Lev'; ),
    ( bibledit : 4; bibleworks :   'Num'; ),
    ( bibledit : 5; bibleworks :   'Deu'; ),
    ( bibledit : 6; bibleworks :   'Jos'; ),
    ( bibledit : 7; bibleworks :   'Jdg'; ),
    ( bibledit : 8; bibleworks :   'Rut'; ),
    ( bibledit : 9; bibleworks :   '1Sa'; ),
    ( bibledit : 10; bibleworks :   '2Sa'; ),
    ( bibledit : 11; bibleworks :   '1Ki'; ),
    ( bibledit : 12; bibleworks :   '2Ki'; ),
    ( bibledit : 13; bibleworks :   '1Ch'; ),
    ( bibledit : 14; bibleworks :   '2Ch'; ),
    ( bibledit : 15; bibleworks :   'Ezr'; ),
    ( bibledit : 16; bibleworks :   'Neh'; ),
    ( bibledit : 17; bibleworks :   'Est'; ),
    ( bibledit : 18; bibleworks :   'Job'; ),
    ( bibledit : 19; bibleworks :   'Psa'; ),
    ( bibledit : 20; bibleworks :   'Pro'; ),
    ( bibledit : 21; bibleworks :   'Ecc'; ),
    ( bibledit : 22; bibleworks :   'Sol'; ),
    ( bibledit : 23; bibleworks :   'Isa'; ),
    ( bibledit : 24; bibleworks :   'Jer'; ),
    ( bibledit : 25; bibleworks :   'Lam'; ),
    ( bibledit : 26; bibleworks :   'Eze'; ),
    ( bibledit : 27; bibleworks :   'Dan'; ),
    ( bibledit : 28; bibleworks :   'Hos'; ),
    ( bibledit : 29; bibleworks :   'Joe'; ),
    ( bibledit : 30; bibleworks :   'Amo'; ),
    ( bibledit : 31; bibleworks :   'Oba'; ),
    ( bibledit : 32; bibleworks :   'Jon'; ),
    ( bibledit : 33; bibleworks :   'Mic'; ),
    ( bibledit : 34; bibleworks :   'Nah'; ),
    ( bibledit : 35; bibleworks :   'Hab'; ),
    ( bibledit : 36; bibleworks :   'Zep'; ),
    ( bibledit : 37; bibleworks :   'Hag'; ),
    ( bibledit : 38; bibleworks :   'Zec'; ),
    ( bibledit : 39; bibleworks :   'Mal'; ),
    ( bibledit : 40; bibleworks :   'Mat'; ),
    ( bibledit : 41; bibleworks :   'Mar'; ),
    ( bibledit : 42; bibleworks :   'Luk'; ),
    ( bibledit : 43; bibleworks :   'Joh'; ),
    ( bibledit : 44; bibleworks :   'Act'; ),
    ( bibledit : 45; bibleworks :   'Rom'; ),
    ( bibledit : 46; bibleworks :   '1Co'; ),
    ( bibledit : 47; bibleworks :   '2Co'; ),
    ( bibledit : 48; bibleworks :   'Gal'; ),
    ( bibledit : 49; bibleworks :   'Eph'; ),
    ( bibledit : 50; bibleworks :   'Phi'; ),
    ( bibledit : 51; bibleworks :   'Col'; ),
    ( bibledit : 52; bibleworks :   '1Th'; ),
    ( bibledit : 53; bibleworks :   '2Th'; ),
    ( bibledit : 54; bibleworks :   '1Ti'; ),
    ( bibledit : 55; bibleworks :   '2Ti'; ),
    ( bibledit : 56; bibleworks :   'Tit'; ),
    ( bibledit : 57; bibleworks :   'Phm'; ),
    ( bibledit : 58; bibleworks :   'Heb'; ),
    ( bibledit : 59; bibleworks :   'Jam'; ),
    ( bibledit : 60; bibleworks :   '1Pe'; ),
    ( bibledit : 61; bibleworks :   '2Pe'; ),
    ( bibledit : 62; bibleworks :   '1Jo'; ),
    ( bibledit : 63; bibleworks :   '2Jo'; ),
    ( bibledit : 64; bibleworks :   '3Jo'; ),
    ( bibledit : 65; bibleworks :   'Jud'; ),
    ( bibledit : 66; bibleworks :   'Rev'; ),
    ( bibledit : 68; bibleworks :   ''; ),
    ( bibledit : 69; bibleworks :   ''; ),
    ( bibledit : 70; bibleworks :   'Tob'; ),
    ( bibledit : 71; bibleworks :   'Jdt'; ),
    ( bibledit : 72; bibleworks :   'EsG'; ),
    ( bibledit : 73; bibleworks :   'Wis'; ),
    ( bibledit : 74; bibleworks :   'Sir'; ),
    ( bibledit : 75; bibleworks :   'Bar'; ),
    ( bibledit : 76; bibleworks :   'LJe'; ),
    ( bibledit : 77; bibleworks :   'S3Y'; ),
    ( bibledit : 78; bibleworks :   'Sus'; ),
    ( bibledit : 79; bibleworks :   'Bel'; ),
    ( bibledit : 80; bibleworks :   '1Ma'; ),
    ( bibledit : 81; bibleworks :   '2Ma'; ),
    ( bibledit : 82; bibleworks :   '1Es'; ),
    ( bibledit : 83; bibleworks :   'Man'; ),
    ( bibledit : 84; bibleworks :   'Ps2'; ),
    ( bibledit : 85; bibleworks :   '3Ma'; ),
    ( bibledit : 86; bibleworks :   '2Es'; ),
    ( bibledit : 87; bibleworks :   '4Ma'; ),
    ( bibledit : 88; bibleworks :   'Dnt'; )
  );

  function BookIdToBibleWorks (ID : integer) : string;
  var
    i : integer;
  begin
    Result := '';
    for i := 1 to 88 do
    begin
      if ID = BookData[i].bibledit then
      begin
        Result := BookData[i].bibleworks;
      end;
    end;
  end;


end.
