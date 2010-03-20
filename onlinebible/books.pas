unit books;

interface

function BookIdToOnlineBible (ID : integer) : string;

implementation

type
  TBook = Record
    bibledit : integer;
    onlinebible : string;
  end;
var
  BookData : array [1..66] of TBook =
  (
    ( bibledit : 1; onlinebible : 'Ge'; ),
    ( bibledit : 2; onlinebible : 'Ex'; ),
    ( bibledit : 3; onlinebible : 'Le'; ),
    ( bibledit : 4; onlinebible : 'Nu'; ),
    ( bibledit : 5; onlinebible : 'De'; ),
    ( bibledit : 6; onlinebible : 'Jos'; ),
    ( bibledit : 7; onlinebible : 'Jud'; ),
    ( bibledit : 8; onlinebible : 'Ru'; ),
    ( bibledit : 9; onlinebible : '1Sa'; ),
    ( bibledit : 10; onlinebible : '2Sa'; ),
    ( bibledit : 11; onlinebible : '1Ki'; ),
    ( bibledit : 12; onlinebible : '2Ki'; ),
    ( bibledit : 13; onlinebible : '1Ch'; ),
    ( bibledit : 14; onlinebible : '2Ch'; ),
    ( bibledit : 15; onlinebible : 'Ezr'; ),
    ( bibledit : 16; onlinebible : 'Ne'; ),
    ( bibledit : 17; onlinebible : 'Es'; ),
    ( bibledit : 18; onlinebible : 'Job'; ),
    ( bibledit : 19; onlinebible : 'Ps'; ),
    ( bibledit : 20; onlinebible : 'Pr'; ),
    ( bibledit : 21; onlinebible : 'Ec'; ),
    ( bibledit : 22; onlinebible : 'So'; ),
    ( bibledit : 23; onlinebible : 'Isa'; ),
    ( bibledit : 24; onlinebible : 'Jer'; ),
    ( bibledit : 25; onlinebible : 'La'; ),
    ( bibledit : 26; onlinebible : 'Eze'; ),
    ( bibledit : 27; onlinebible : 'Da'; ),
    ( bibledit : 28; onlinebible : 'Ho'; ),
    ( bibledit : 29; onlinebible : 'Joe'; ),
    ( bibledit : 30; onlinebible : 'Am'; ),
    ( bibledit : 31; onlinebible : 'Ob'; ),
    ( bibledit : 32; onlinebible : 'Jon'; ),
    ( bibledit : 33; onlinebible : 'Mic'; ),
    ( bibledit : 34; onlinebible : 'Na'; ),
    ( bibledit : 35; onlinebible : 'Hab'; ),
    ( bibledit : 36; onlinebible : 'Zep'; ),
    ( bibledit : 37; onlinebible : 'Hag'; ),
    ( bibledit : 38; onlinebible : 'Zec'; ),
    ( bibledit : 39; onlinebible : 'Mal'; ),
    ( bibledit : 40; onlinebible : 'Mt'; ),
    ( bibledit : 41; onlinebible : 'Mr'; ),
    ( bibledit : 42; onlinebible : 'Lu'; ),
    ( bibledit : 43; onlinebible : 'Joh'; ),
    ( bibledit : 44; onlinebible : 'Ac'; ),
    ( bibledit : 45; onlinebible : 'Ro'; ),
    ( bibledit : 46; onlinebible : '1Co'; ),
    ( bibledit : 47; onlinebible : '2Co'; ),
    ( bibledit : 48; onlinebible : 'Ga'; ),
    ( bibledit : 49; onlinebible : 'Eph'; ),
    ( bibledit : 50; onlinebible : 'Php'; ),
    ( bibledit : 51; onlinebible : 'Col'; ),
    ( bibledit : 52; onlinebible : '1Th'; ),
    ( bibledit : 53; onlinebible : '2Th'; ),
    ( bibledit : 54; onlinebible : '1Ti'; ),
    ( bibledit : 55; onlinebible : '2Ti'; ),
    ( bibledit : 56; onlinebible : 'Tit'; ),
    ( bibledit : 57; onlinebible : 'Phm'; ),
    ( bibledit : 58; onlinebible : 'Heb'; ),
    ( bibledit : 59; onlinebible : 'Jas'; ),
    ( bibledit : 60; onlinebible : '1Pe'; ),
    ( bibledit : 61; onlinebible : '2Pe'; ),
    ( bibledit : 62; onlinebible : '1Jo'; ),
    ( bibledit : 63; onlinebible : '2Jo'; ),
    ( bibledit : 64; onlinebible : '3Jo'; ),
    ( bibledit : 65; onlinebible : 'Jude'; ),
    ( bibledit : 66; onlinebible : 'Re'; )
  );

  function BookIdToOnlineBible (ID : integer) : string;
  var
    i : integer;
  begin
    Result := '';
    for i := 1 to 88 do
    begin
      if ID = BookData[i].bibledit then
      begin
        Result := BookData[i].onlinebible;
      end;
    end;
  end;


end.
