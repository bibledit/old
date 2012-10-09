<?php


require_once 'PHPUnit/Framework.php';

 
class bibleworksTest extends PHPUnit_Framework_TestCase
{


  public function testImport1() // Todo
  {
$bwdata = <<<EOD
Hab 1:1  La sentència <04853b> que <0834> veié <02372> (08804) el profeta <05030> Habacuc <02265>. {notes CATB: Isaïes 13:1; 22:1; Nahum 1:1;  Is 13,1; 15,1; 17,1; Zacaries 9,1}
Hab 1:2 ¿Fins <05704> quan <0575>, Jahveh <03068>, cridaré <07768> auxili <07768> (08765), i no [m']escoltaràs <08085> (08799); clamaré <02199> (08799) a tu [a causa de la] violència <02555>, i no [em] salvaràs <03467>(08686)? 
Hab 1:3 ¿Per què <04100> em mostres <07200> (08686) la iniquitat <0205>, i [em] fas contemplar <05027> (08686) la malícia <05999>? L'espoli <07701> i la violència <02555> [són] davant meu <05048>, i s'aixeca <05375> (08799) la discòrdia <07379> <01961> i la controvèrsia <04066>.{Psalm 55:9-11; Jeremies 20:8; 15:10; 20:18}
EOD;
$usfmdata = <<<EOD
\id HAB
\c 1
\p
\v 1
EOD;
    $usfmdata = explode ("\n", $usfmdata);
    $data = Filter_Bibleworks::import ($bwdata);
    $data = implode ("\n", $data);
    //echo "\n$data\n";






$data = <<<EOD
Hab 1:1  La sentència <04853b> que <0834> veié <02372> (08804) el profeta <05030> Habacuc <02265>. {notes CATB: Isaïes 13:1; 22:1; Nahum 1:1;  Is 13,1; 15,1; 17,1; Zacaries 9,1}
Hab 1:2 ¿Fins <05704> quan <0575>, Jahveh <03068>, cridaré <07768> auxili <07768> (08765), i no [m']escoltaràs <08085> (08799); clamaré <02199> (08799) a tu [a causa de la] violència <02555>, i no [em] salvaràs <03467>(08686)? 
Hab 1:3 ¿Per què <04100> em mostres <07200> (08686) la iniquitat <0205>, i [em] fas contemplar <05027> (08686) la malícia <05999>? L'espoli <07701> i la violència <02555> [són] davant meu <05048>, i s'aixeca <05375> (08799) la discòrdia <07379> <01961> i la controvèrsia <04066>.{Psalm 55:9-11; Jeremies 20:8; 15:10; 20:18}
Hab 1:4 Per <05921> aquest motiu <03651> s'afebleix <06313> (08799) la llei <08451>, i no <03808> <05331> preval <03318> (08799) la justícia <04941>, perquè <03588 -ki> el malvat <07563> envolta <03803> (08688) el just <06662>: per <05921> això  <03651> es capgira <03318> (08799) la justícia <04941 -el judici-jutjament>.
Hab 1:5 Mireu <07200> (08798) les nacions <01471>, i contempleu <05027> (08685), i resteu esbalaïts <08539> (08690); perquè <03588> faig <06466> (08802) una obra <06467> en els vostres dies <03117> que no creureu <0539> (08686) encara que <03588> algú [us] l’expliqués <05608> (08792). {notes CATB: Fets 13:41}
Hab 1:6 Perquè <03588>, heus aquí <02009 -heus ací!>, jo aixeco <06965> (08688) els caldeus <03778>, un poble <01471> amargat <04751> i impetuós <04116> (08737), que va <01980> (08802) per les amplàries <04800> de la terra <0776> per apoderar-se <03423> (08800) dels llocs <04908> habitats <04908>que no [són] seus. {1Reis 24:2}
Hab 1:7 [És] impressionant <0366> i terrible <03372a>, de si mateix <04480+sufix> surt <03318> (08799) seu judici <04941> i la seva dignitat <07613>. {Isa 18:2; Isa 18:7; Jer 39:5-9}
Hab 1:8 Els seus cavalls <05483> són més ràpids <07043> (08804) que <04480> els lleopards <05246>, i més experts <02300> que <04480> els llops <02061> del vespre <06153>; i els seus genets <06571b> avancen orgullosos <06335> (08804; i els seus genets <06571b> vénen <0935> (08799) de lluny <07350>, volen <05774> (08799) com l'àguila <05404> que s'afanya <02363> (08804) a devorar <0398> (08800).
Hab 1:9 Tots <03605> ells vindran <0935> (08799) per fer violència <02555>; els seus rostres <06440> engoliran <04041> el vent de l'est <06921>, i aplegaran <0622> (08799) captius <07628b> com la sorra <02344>.
Hab 1:10 I es burlarà <07046> (08691) dels reis <04428>, i farà escarni <04890> dels prínceps <07336>, es riurà <07832> (08799) de tota fortalesa <04013>, i amuntegarà <06651> (08799) terra <06083>, i la capturarà <03920> (08799).
Hab 1:11 Llavors <0227> canviarà <02498> (08804) de pensament <07307>, i se n'anirà <05674> (08799) ; i serà culpable <0816> (08804) [perquè atribuirà] la seva <02098> força <03581b> al seu déu <0433>.
Hab 1:12 ¿No <03808> [ets] tu des del començament <06924a>, Jahveh <03068>, el meu Déu <0430>, el meu Sant <06918>? No morirem <04191> (08799). Oh Jahveh <03068>, l'has posat <07760> (08804) per a judici <04941>: i tu, oh Roca <06697>, l'has designat <03245> (08804) per castigar <03198> (08687).
Hab 1:13 Els teus ulls <05869> [són] massa <04480> purs <02889> per mirar <07200> (08800) el mal <07451b>, i tu no pots <03201> (08799) contemplar <05027> (08687) la malícia <05999>. ¿Per què <04100> contemples <05027> (08686) els traïdors <0898> (08802), [i] guardes silenci <02790> (08686) quan el dolent <07563> engoleix <01104> (08763) el qui [és] més <04480> just <06662> que <04480> ell?
Hab 1:14 ¿I tractes <06213a> (08799) l’home <0120> com els peixos <01709> del mar <03220>, com els rèptils <07431>, que ningú <03808> no governa <04910> (08802)?
Hab 1:15 A tots <03605> els traurà <05927> (08689) amb un ham <02443>, els agafarà <01641> (08799) amb la seva xarxa <02764b> i els aplegarà <0622> (08799) amb la seva teranyina <04365a>; per  <05921> això <03651> s’alegrarà <08055> (08799) i exultarà <01523> (08799).
Hab 1:16 Per <05921> això <03651> oferirà <02076> (08762) sacrificis <02076> a la seva xarxa <02764b>, i cremarà perfum <06999> (08762) a la seva teranyina <04365a>, perquè <03588> amb elles <01992a> ha engrandit <08082a> la seva porció <02506>, i ha fet suculent <01277> el seu menjar <03978>.
Hab 1:17 ¿Buidarà <07385a> <07324> (08686), doncs <05921> <03651, la seva xarxa <02764b>, i matarà <02026> (08800) contínuament <08548> les nacions <01471> sense tenir-ne pietat <02550> (08799)?
Hab 2:1 M’estaré dret <05975> (08799) sobre el meu [lloc] de guàrdia <04931>, i em situaré <03320> (08691) sobre el baluard <04692, i vigilaré <06822> (08762) per veure <07200> (08800) què <04100> em dirà <01696> (08762), i què <04100> li respondré <07725> (08686) referent <05921> a la meva queixa <08433b>. {Isaïes 21:8; Psalms 05:03; 85:8 }
Hab 2:2 I Jahveh <03068> em va respondre <06030> (08799), i va dir <0559> (08799): Escriu <03789> (08798) la visió <02377>, i grava-la <0874> (08761) sobre unes tauletes <03871> , a fi que <04616/H4616> pugui córrer <07323> (08799) el qui la llegeix <07121> (08802). {Deu 27:8; Romans 15:4; Apocalipsi 1:19}
Hab 2:3 Perquè <03588> la visió <02377> [és] encara <05750> per un temps fixat <04150>, i s’afanya <06315> (08686) per a la fi <07093>, i no <03808> enganya <03576> (08762): encara <0518> que trigui <04102> (08698), espera-la <02442> (08761), perquè certament <0935> (08800) vindrà <0935> (08799), no <03808> es retardarà <0309> (08762). {Is 26v20; Dan 10v14; Psalm 27v14; Ezequiel 12v25; Heb 10v37}
Hab 2:4 Heus aquí <02009>, el qui és orgullós <06075a> (08795) no <03808> és recte <03474> (08804) en la seva ànima<05315>: però el just <06662> viurà <02421> (08799) per la seva fe <0530>. {Psalm 49v18; Isaïes 13v11; Romans 1v17; Gàlates 3v11; Hebreus 10v37-38}
Hab 2:5 I així<0637> , com el vi <03196> és traïdor<0898> (08802), [aquest] home <01397> [és] presumptuós <03093>, i no es mantindrà <05115b> (08799): eixampla <07337> (08689) com l’infern <07585> la seva ànima <05315>, i [és] com la mort <04194>, que mai <S12-vav+03808> no <03808>  s’atipa <07646> (08799); i aplega <0622> (08799) per a si mateix <0213+sufix> totes <03605> les nacions <01471>, i arreplega <06908> (08799) per a si mateix <0213+sufix> tots <03605> els pobles <05971a>. {sepulcre: en hebreu Xeol; Pro 20:1; 21:24; 2Reis 14:10; Pro 27:20; Pro 30:16; Isa 5:11-15}
Hab 2:6 ¿No aixecaran <05375> (08799) tots <03605> aquests <0428> un proverbi <04912> contra <05921> ell, i enigmes <04426> [i] al·legories <02420> contra ell? I diran <0559> (08799): Ai <01945> del qui multiplica <07235> (08688) el que no [és] seu! Fins quan <05704> <04970 es continuarà carregant <03513> (08688) de deutes <05671>?
Hab 2:7 ¿No s’alçaran <06965> (08799) de sobte <06621> els teus creditors <05391a>, i es desvetllaran <03364> (08799) els qui et sacsegen <02111> (08772), i esdevindràs <01961> la seva presa <04933>?
Hab 2:8 Perquè <03588> has espoliat <07997> (08804) moltes <07227> nacions<01471>, tota <03605> la resta <03499> dels pobles <05971a> t’espoliarà <07997b> (08799); a causa de <04480> la sang <01818> humana <0120> i de la violència <02555> [comesa] a la terra <0776>, a la ciutat <07151> i a tots els que hi habiten <03427> (08802) .
Hab 2:9 Ai <01945> del qui cobeja <01214> (08802) guany <01215> injust <07451a> per a la seva casa <01004>, per tal de posar <07760> (08800) ben alt <04791> el seu niu <07064>, per alliberar-se <05337> (08736) del poder <03709> del mal <07451>! {Jer 22:13; Eze 22:27; Jer 49:16}
Hab 2:10 Has maquinat <03289> (08804) la vergonya <01322> per a la teva casa <01004>, has tallat <07096> (08800) molts <07227> pobles<05971>, i has pecat <02398> (08802) [contra] la teva ànima <05315>. {2Reis 9:26; Nahum 1:14; Habacuc 2:16; Jeremies 26:19}
Hab 2:11 Perquè <03588> la pedra <068> de la paret <07023> clamarà <02199> (08799), i li respondrà <06030> (08799) la biga <03714> de fusta <06030a>.
Hab 2:12  Ai <01945> del qui edifica <01129> (08802) una ciutat <05892b> amb sang<01818>, i estableix <03559> (08790) una vila <07151> amb iniquitat<05767b>! { Miquees 3:10; Nahum 3:1 }
Hab 2:13 Heus aquí <02009 -ací> , no [ve] de Jahveh <03068> dels exèrcits <06635>? Els pobles <05971a>, doncs, treballen <03021> (08799) per al <01767> foc <0784>, i les nacions <03816> es fatiguen <03286> (08799) en va <07385b -debades>. { Isaïes 50:11; Jer 51:58 }
Hab 2:14 Perquè <03588 -H3588> la terra <0776> s’omplirà <04390> (08735) del coneixement <03045> (08800)de la glòria <03519b>de Jahveh<03068>, com les aigües <04325>cobreixen <03680> (08762) el mar <03220>. { Psalm 22:27; Isaïes 11:9; Zacaries 14:9}.
Hab 2:15 Ai <01945> del qui dóna beguda <08248> (08688) al seu company <07453>; [tu que els] ofereixes <05596> (08764) la teva bóta <02534> <02573>, i també <0637> [els] fas embriagar <07937> (08763) a fi de <04616> mirar <05027> (08687) llurs nueses <04589>.
Hab 2:16 T’has omplert <07646> (08804) de vergonya <07036>, en lloc <04480> d’honor <03519b>; beu tu també <01571>, i sigues com un incircumcís <06188> (08734): la copa <03563> de la mà dreta <03225> de Jahveh <03068> es torna <05437> (08735) contra tu <05921+sufix>, i la ignomínia <07022> serà sobre el teu honor <03519>.
Hab 2:17 Per <03588> la violència <02555> [comesa] contra el Líban <03844> et cobrirà <03680> (08762), i l’espoli <07701>de les bèsties <0929>t’espantarà <02865> (08686); a causa de la sang <01818> humana  <0120> i de la violència <02555> [comesa] a la terra <0776>, a la ciutat <07151> i a tots  <03605> els qui hi habiten <03427> (08802).
Hab 2:18 Quin <04100> profit hi ha <03276> (08689) en l’estàtua <06459> que ha esculpit <06458> (08804) l’escultor <03335> (08802)? És una escultura de fosa <04541a> que ensenya <03384> (08688) falsedat <08267>: ¿per què l’escultor <03335> (08802), que fa <06213> (08800) ídols <0457> muts <0483>, confia <0982> (08804) en la seva pròpia obra <03336>? { Isa 42:17; Isa 44:9; Jer 2:27, Jer 2:28; Jer 10:8, Jer 10:14; Zec 10:2;  Psa 115:4, Psa 115:8 }
Hab 2:19  Ai  <01945> del qui diu <0559> (08802) a la fusta <06086>: Desvetlla’t <07019a> <06974> (08685), i a la pedra <068> muda <01748>: Anima’t! <05782> (08798) Podrà això ensenyar <03384> (08686)? Heus aquí <02009 - heus ací!>, està recobert <08610> (08803) d’or <02091> i de plata <03701>, però no hi ha <0369> <03605> esperit <07307> dintre seu <07130>. { Jer 2:27, Jer 2:28; Jer 10:3;  1Reis 18:26-29; Psalm 135:15-18; Jer 10:4, Jer 10:9, 14; Psa 135:17 }
Hab 2:20  Però Jahveh <03068> [és] en el seu sant <06944+sufix> temple <01964>: que calli <02013-xt!> (08761) davant <04480> <06440+sufix> d’ell tota <01964> la terra <0776>. { Miquees 1:2;  Sofonies 1:7; Zacaries 2:13 }
Hab 3:1 La pregària  <08605> del profeta <05030> Habacuc<02265>, sobre Xiguionot <07692>.
Hab 3:2 Jahveh <03068>, he escoltat <08085> (08804) el teu anunci<08088>, [i] he tingut por <03372> (08804). Jahveh <03068>, fes reviure <02421a> (08761) la teva obra <06467> enmig <07130> dels anys <08141>, fes-la conèixer <03045> (08686) enmig <07130> dels anys <08141>. En la ira <07267>, recorda’t <02142> (08799) de la misericòrdia <07355> (08763). 
Hab 3:3 Déu <0433> ve <0935> (08799) de Teman <08487>, i el Sant <06918>, de la muntanya <02022> de Paran <06290>. Selah <05542>. La seva majestat  <01935> cobreix <03680> (08765) els cels <08064>, i la seva lloança <08416> omple <04390> (08804) la terra <0776>. 
Hab 3:4 I la resplendor <05051> era com <prefix comparació-S10 K.>><>> la llum <0216>: raigs <07161> [sortien] de la seva mà <03027>, i allí <08033> s’amagava  <02253> la seva força <05797>. { Psalm 18:12; Job 26:14}
Hab 3:5 Davant <06440> seu va <03212> (08799) la pesta <01698>, i la guspira <07565> encesa <07565 -cf plaga/pestilència> surt <03318> (08799) dels seus peus <07272>. {Èxode 12:29, 12:30; Nombres16:46-49 Num 11:1-3; Psalm 18:12-13}
Hab 3:6 S’atura <05975> (08804), i mesura <04128> (08787) la terra <0776>; mira <07200> (08804), i fa tremolar <05425a> (08686) les nacions <01471>; i les velles <05703> muntanyes <02042, 02022> <02022> s’esberlen <06327> <06483a> (08691), els turons <01389> eterns <05769> s’inclinen <07817> (08804). Els seus camins <01979> [són] eterns  <05769>.
Hab 3:7  He vist <07200> (08804) les tendes <0168> de Cuixan  <03572> en aflicció <0205>, tremolen <07264> (08799) les cortines <03407> de la terra <0776> de Madian <04080>. { Nonbres 31:7-8; Jutges 7:24-25; Jdg 8:12 }
Hab 3:8 S’ha aïrat <02734> (08804) Jahveh <03068> contra els rius <05104>? ¿[És] contra el rius <05104> el teu enuig <0639>, contra el mar  <03220> la teva ira <05678>, quan cavalques <07392> (08799) sobre els teus cavalls <05483>, dalt dels teus carros <04818> de salvació <03444>? { Ex 7:19-20; Josuè 3:16; Isa 50:2  Psalm 114:3-5;  Deuteronomi 33:26; Psa 18:10; Hab 3:15}
Hab 3:9 El teu arc <07198> apareix <05783> (08735) nu <05783>; els juraments <07621> [fets] a les tribus <04294>, paraula segura <0562>. Selah <05542> . Solques <01234> (08762) la terra <0776> amb els rius <05104>. { Psalm 7:12-13; Hab 3:11; Psa 78:16; Psalm 105:41}
Hab 3:10 En veure’t <07200> (08804), les muntanyes <02022> es van estremir <02342> (08799): una inundació <02230>  d’aigües <04325> va passar per sobre <05674a> (08804); l’abisme <08415> féu escoltar <05414> (08804) la seva veu <06963>, alçà <05375> (08804) amunt <07315> les seves mans <03027>.  {Psalms 93:3;  98:7-8}
Hab 3:11 El sol <08121> [i] la lluna <03394> es van aturar <05975> (08804) al seu estatge <02073>: se’n van anar <01980> (08762) a la llum <0216> de les teves sagetes <02671>, a la resplendor <05051> de la teva llança <02595>  fulgurant <01300>. { Josuè 10:12-14;  Psalm 18:14 }
Hab 3:12 Amb indignació <02195> vas trepitjar 06805> (08799) la terra <0776>, amb ira <0639> vas trillar <01758> (08799) les nacions <01471>. { Psalm 68:7; Isaïes 41:15; Jeremies 51:33; Miquees 4:13 }
Hab 3:13 Vas sortir <03318> (08804) per salvar <03468> el teu poble <05971>, per salvar[-lo] <03468> amb el teu ungit <04899>; vas esmicolar <04272> (08804) el cap <07218> de la casa <01004> del malvat <07563>, despullant <06168> (08763) el fonament <03247>  fins al coll <06677>. Selah <05542>. { Psalm 110:6 }
Hab 3:14 Vas foradar <05344> (08804)  amb els seus bastons <04294> el cap  <07218> de les seves viles <06518>, que irrompen <05590> (08799) tempestuosos  <05590> per dispersar-me <06327> (08687): exultant <05951> com qui devora <0398> (08800) el pobre <06041> en un amagatall <04565>. {Jutges 7:22;  Daniel 11:40; Zacaries 9:14; Psalms 10:8; 64:2-5}
Hab 3:15 Avançares  <01869> (08804) pel mar <03220> amb els teus cavalls <05483b>, pel llot <02563b> de les moltes <07227a> aigües <04325>. {Psalm 77:19; Habacuc 3:8}
Hab 3:16 Quan ho vaig escoltar <08085> (08804), es van estremir <07264> (08799) les meves entranyes <0990>, els meus llavis <08193> van tremolar <06750> (08804) [en escoltar] la seva veu <06963>; el corc <07538> va entrar <0935> (08799) dins dels meus ossos<06106>, i vaig tremolar <07264> (08799) en el meu lloc; però el dia <03117> de l’aflicció <06869> estaré tranquil <05117> (08799), quan pujarà <05927> (08800) contra el poble <05971> aquell que l’envairà <01464> (08799). {Dan 10:8; Hab 3:2;  Job 30:30; Jer 23:9; Lluc 21:19; Jer 5:15}
Hab 3:17 Encara que <03588> la figuera <08384> no floreixi <06524> (08799), i no [hi hagi] raïm <02981> en els ceps; [encara que] falli <03584> (08765) el producte <04639> de l’olivera <02132>, i els camps <07709> no donin <06213> (08804) aliment <0400>; [encara que] el bestiar menut <06629> sigui exterminat <01504> (08804) de la pleta <04356>, i el gros <01241>, dels estables <07517>: {Joel 1:10-12; Amòs 4:9; 2Co 4:8, 2Co 4:9  Miquees 6:15; Joel 1:18;  Jeremies 5:17}
Hab 3:18 tanmateix jo m’alegraré <05937> (08799) en Jahveh <03068>, exultaré de goig <01523> (08799) en el Déu <0430> de la meva salvació <03468>. {Job 13:15; Isaïes 61:10; Romans 5:2-3}
Hab 3:19 Jahveh <03069>, el Senyor <0136>, [és] la meva força <02428>, i farà <07760> (08799) els meus peus <07272>  [àgils] com les daines <0355>, i em farà caminar <01869> (08686) sobre els meus llocs alts <01116>. Per al director <05329> (08764). Amb Neguinot <05058>.  {Psalms 18:32-33; 27:1;46:1; Isa 45:24; 2Samuel 22:34; Deuteronomi 33:29 }
EOD;

  }


  public function testItalics1()
  {
    $data = Filter_Bibleworks::italics ("Normal text and [italics].");
    $this->assertEquals($data, 'Normal text and \add italics\add*.');
  }
  
  
  public function testItalics2()
  {
    $data = Filter_Bibleworks::italics ("Normal text, [italics], and [italics again].");
    $this->assertEquals($data, 'Normal text, \add italics\add*, and \add italics again\add*.');
  }
  
  
  public function testItalics3()
  {
    $data = Filter_Bibleworks::italics ("[Italics] and [malformed italics.");
    $this->assertEquals($data, '\add Italics\add* and [malformed italics.');
  }
  

  public function testItalics4()
  {
    $data = Filter_Bibleworks::italics ("[Italics] and malformed italics].");
    $this->assertEquals($data, '\add Italics\add* and malformed italics].');
  }
  
  
  public function testItalics5()
  {
    $data = Filter_Bibleworks::italics ("Mal]formed [italics].");
    $this->assertEquals($data, 'Mal]formed \add italics\add*.');
  }
  
  
}
?>


