<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


require_once ("../bootstrap/bootstrap.php");


// The script runs through the cli Server API only.
Filter_Cli::assert ();


$database_config_general = Database_Config_General::getInstance ();
$database_config_bible = Database_Config_Bible::getInstance ();
$database_config_user = Database_Config_User::getInstance ();
$database_logs = Database_Logs::getInstance ();
$database_styles = Database_Styles::getInstance ();
$database_bibles = Database_Bibles::getInstance ();
$database_users = Database_Users::getInstance ();


$session_logic = Session_Logic::getInstance ();


$database_logs->log ("Checking and updating the open demo site");


// Set user to 'admin' as this is the user who is always logged in an open demo installation.
$session_logic->setUsername ("admin");


// Delete the "Standard" stylesheet and re-create it.
// Delete empty sheet that may have been there.
$standard_sheet = "Standard";
$database_styles->deleteSheet ($standard_sheet);
$database_styles->revokeWriteAccess ("", $standard_sheet);
$database_styles->deleteSheet ("");
$database_styles->createSheet ($standard_sheet);
$database_styles->grantWriteAccess ("admin", $standard_sheet);
Styles_Sheets::create_all ();


// Set the export stylesheet to "Standard" for all Bibles and the admin.
$bibles = $database_bibles->getBibles ();
foreach ($bibles as $bible) {
  $database_config_bible->setExportStylesheet ($bible, $standard_sheet);
}
$database_config_user->setStylesheet ($standard_sheet);


// Set the site language to "Default"
$database_config_general->setSiteLanguage (Locale_Logic::filterDefault ("default"));


// Ensure the default users are there.
$users = array (
  array ("guest", Filter_Roles::GUEST_LEVEL),
  array ("member", Filter_Roles::MEMBER_LEVEL),
  array ("consultant", Filter_Roles::CONSULTANT_LEVEL),
  array ("translator", Filter_Roles::TRANSLATOR_LEVEL),
  array ("manager", Filter_Roles::MANAGER_LEVEL),
  array ("admin", Filter_Roles::ADMIN_LEVEL)
);
foreach ($users as $user) {
  if (!$database_users->usernameExists ($user [0])) {
    $database_users->addNewUser($user [0], $user [0], $user [1], "");
  }
  $database_users->updateUserLevel ($user [0], $user [1]);
}


// Ensure the KJV Bible exists.
$database_bibles->createBible ("KJV");
$database_users->grantAccess2Bible ("admin", $bible);


// Store some text into the KJV Bible.
$data = array ();

$usfm = <<<'EOD'
\id JUD 65-JUD-kjv.sfm The King James Version of the Holy Bible Wednesday, October 14, 2009
\ide UTF-8
\h Jude
\toc1 The General Epistle of Jude
\toc2 Jude
\toc3 Jud
\mt The General Epistle of Jude
\c 1
\p
\v 1 Jude, the servant of Jesus Christ, and brother of James, to them that are sanctified by God the Father, and preserved inJesus Christ, \add and\add* called:
\v 2 Mercy unto you, and peace, and love, be multiplied.
\p
\v 3 Beloved, when I gave all diligence to write unto you of the common salvation, it was needful for me to write unto you, andexhort \add you\add* that ye should earnestly contend for the faith which was once delivered unto the saints.
\v 4 For there are certain men crept in unawares, who were before of old ordained to this condemnation, ungodly men, turning the grace of our God into lasciviousness, and denying the only Lord God, and our Lord Jesus Christ.
\v 5 I will therefore put you in remembrance, though ye once knew this, how that the Lord, having saved the people out of the land of Egypt, afterward destroyed them that believed not.
\v 6 And the angels which kept not their first estate, but left their own habitation, he hath reserved in everlasting chains under darkness unto the judgment of the great day.
\v 7 Even as Sodom and Gomorrha, and the cities about them in like manner, giving themselves over to fornication, and going after strange flesh, are set forth for an example, suffering the vengeance of eternal fire.
\v 8 Likewise also these \add filthy\add* dreamers defile the flesh, despise dominion, and speak evil of dignities.
\v 9 Yet Michael the archangel, when contending with the devil he disputed about the body of Moses, durst not bring against him a railing accusation, but said, The Lord rebuke thee.
\v 10 But these speak evil of those things which they know not: but what they know naturally, as brute beasts, in those things they corrupt themselves.
\v 11 Woe unto them! for they have gone in the way of Cain, and ran greedily after the error of Balaam for reward, and perished in the gainsaying of Core.
\v 12 These are spots in your feasts of charity, when they feast with you, feeding themselves without fear: clouds\add they are\add* without water, carried about of winds; trees whose fruit withereth, without fruit, twice dead, plucked up by the roots;
\v 13 Raging waves of the sea, foaming out their own shame; wandering stars, to whom is reserved the blackness of darkness for ever.
\v 14 And Enoch also, the seventh from Adam, prophesied of these, saying, Behold, the Lord cometh with ten thousands of his saints,
\v 15 To execute judgment upon all, and to convince all that are ungodly among them of all their ungodly deeds which they haveungodly committed, and of all their hard \add speeches\add* which ungodly sinners have spoken against him.
\v 16 These are murmurers, complainers, walking after their own lusts; and their mouth speaketh great swelling\add words,\add* having men’s persons in admiration because of advantage.
\p
\v 17 But, beloved, remember ye the words which were spoken before of the apostles of our Lord Jesus Christ;
\v 18 How that they told you there should be mockers in the last time, who should walk after their own ungodly lusts.
\v 19 These be they who separate themselves, sensual, having not the Spirit.
\v 20 But ye, beloved, building up yourselves on your most holy faith, praying in the Holy Ghost,
\v 21 Keep yourselves in the love of God, looking for the mercy of our Lord Jesus Christ unto eternal life.
\v 22 And of some have compassion, making a difference:
\v 23 And others save with fear, pulling \add them\add* out of the fire; hating even the garment spotted by the flesh.
\p
\v 24 Now unto him that is able to keep you from falling, and to present\add you\add* faultless before the presence of his glory with exceeding joy,
\v 25 To the only wise God our Saviour, \add be\add* glory and majesty, dominion and power, both now and ever. Amen.
EOD;
$data [] = $usfm;

$usfm = <<<'EOD'
\id 1JN
\ide UTF-8
\h 1 John
\toc1 The First Epistle General of John
\toc2 1 John
\toc3 1Jn
\mt The First Epistle General of John
\c 1
\p
\v 1 That which was from the beginning, which we have heard, which we have seen with our eyes, which we have looked upon, and our hands have handled, of the Word of life;
\v 2 (For the life was manifested, and we have seen \add it,\add* and bear witness, and shew unto you that eternal life, which was with the Father, and was manifested unto us;)
\v 3 That which we have seen and heard declare we unto you, that ye also may have fellowship with us: and truly our fellowship\add is\add* with the Father, and with his Son Jesus Christ.
\v 4 And these things write we unto you, that your joy may be full.
\p
\v 5 This then is the message which we have heard of him, and declare unto you, that God is light, and in him is no darkness at all.
\v 6 If we say that we have fellowship with him, and walk in darkness, we lie, and do not the truth:
\v 7 But if we walk in the light, as he is in the light, we have fellowship one with another, and the blood of Jesus Christ his Son cleanseth us from all sin.
\v 8 If we say that we have no sin, we deceive ourselves, and the truth is not in us.
\v 9 If we confess our sins, he is faithful and just to forgive us\add our\add* sins, and to cleanse us from all unrighteousness.
\v 10 If we say that we have not sinned, we make him a liar, and his word is not in us.
\c 2
\p
\v 1 My little children, these things write I unto you, that ye sin not. And if any man sin, we have an advocate with the Father, Jesus Christ the righteous:
\v 2 And he is the propitiation for our sins: and not for ours only, but also for\add the sins of\add* the whole world.
\v 3 And hereby we do know that we know him, if we keep his commandments.
\v 4 He that saith, I know him, and keepeth not his commandments, is a liar, and the truth is not in him.
\v 5 But whoso keepeth his word, in him verily is the love of God perfected: hereby know we that we are in him.
\v 6 He that saith he abideth in him ought himself also so to walk, even as he walked.
\p
\v 7 Brethren, I write no new commandment unto you, but an old commandment which ye had from the beginning. The old commandment is the word which ye have heard from the beginning.
\v 8 Again, a new commandment I write unto you, which thing is true in him and in you: because the darkness is past, and the true light now shineth.
\v 9 He that saith he is in the light, and hateth his brother, is in darkness even until now.
\v 10 He that loveth his brother abideth in the light, and there is none occasion of stumbling in him.
\v 11 But he that hateth his brother is in darkness, and walketh in darkness, and knoweth not whither he goeth, because that darkness hath blinded his eyes.
\v 12 I write unto you, little children, because your sins are forgiven you for his name’s sake.
\v 13 I write unto you, fathers, because ye have known him \add thatis\add* from the beginning. I write unto you, young men, because ye have overcome the wicked one. I write unto you, little children, because ye have known the Father.
\v 14 I have written unto you, fathers, because ye have known him\add that is\add* from the beginning. I have written unto you, young men, because ye are strong, and the word of God abideth in you, and ye have overcome the wicked one.
\v 15 Love not the world, neither the things \add that are\add* in the world. If any man love the world, the love of the Father is not in him.
\v 16 For all that \add is\add* in the world, the lust of the flesh, and the lust of the eyes, and the pride of life, is not of the Father, but is of the world.
\v 17 And the world passeth away, and the lust thereof: but he that doeth the will of God abideth for ever.
\p
\v 18 Little children, it is the last time: and as ye have heard that antichrist shall come, even now are there many antichrists; whereby we know that it is the last time.
\v 19 They went out from us, but they were not of us; for if they had been of us, they would\add no doubt\add* have continued with us: but \add they went out,\add* that they might be made manifest that they were not all of us.
\v 20 But ye have an unction from the Holy One, and ye know all things.
\v 21 I have not written unto you because ye know not the truth, but because ye know it, and that no lie is of the truth.
\v 22 Who is a liar but he that denieth that Jesus is the Christ? He is antichrist, that denieth the Father and the Son.
\v 23 Whosoever denieth the Son, the same hath not the Father: \add (but) he that acknowledgeth the Son hath the Father also.\add*
\v 24 Let that therefore abide in you, which ye have heard from the beginning. If that which ye have heard from the beginning shall remain in you, ye also shall continue in the Son, and in the Father.
\v 25 And this is the promise that he hath promised us, \add even\add* eternal life.
\v 26 These \add things\add* have I written unto you concerning them that seduce you.
\v 27 But the anointing which ye have received of him abideth in you, and ye need not that any man teach you: but as the same anointing teacheth you of all things, and is truth, and is no lie, and even as it hath taught you, ye shall abide in him.
\v 28 And now, little children, abide in him; that, when he shall appear, we may have confidence, and not be ashamed before him at his coming.
\v 29 If ye know that he is righteous, ye know that every one that doeth righteousness is born of him.
\c 3
\p
\v 1 Behold, what manner of love the Father hath bestowed upon us, that we should be called the sons of God: therefore the world knoweth us not, because it knew him not.
\v 2 Beloved, now are we the sons of God, and it doth not yet appear what we shall be: but we know that, when he shall appear, we shall be like him; for we shall see him as he is.
\v 3 And every man that hath this hope in him purifieth himself, even as he is pure.
\v 4 Whosoever committeth sin transgresseth also the law: for sin is the transgression of the law.
\v 5 And ye know that he was manifested to take away our sins; and in him is no sin.
\v 6 Whosoever abideth in him sinneth not: whosoever sinneth hath not seen him, neither known him.
\v 7 Little children, let no man deceive you: he that doeth righteousness is righteous, even as he is righteous.
\v 8 He that committeth sin is of the devil; for the devil sinneth from the beginning. For this purpose the Son of God was manifested, that he might destroy the works of the devil.
\v 9 Whosoever is born of God doth not commit sin; for his seed remaineth in him: and he cannot sin, because he is born of God.
\v 10 In this the children of God are manifest, and the children of the devil: whosoever doeth not righteousness is not of God, neither he that loveth not his brother.
\v 11 For this is the message that ye heard from the beginning, that we should love one another.
\v 12 Not as Cain, \add who\add* was of that wicked one, and slew his brother. And wherefore slew he him? Because his own works were evil, and his brother’s righteous.
\p
\v 13 Marvel not, my brethren, if the world hate you.
\v 14 We know that we have passed from death unto life, because we love the brethren. He that loveth not\add his\add* brother abideth in death.
\v 15 Whosoever hateth his brother is a murderer: and ye know that no murderer hath eternal life abiding in him.
\v 16 Hereby perceive we the love \add of God,\add* because he laid down his life for us: and we ought to lay down\add our\add* lives for the brethren.
\v 17 But whoso hath this world’s good, and seeth his brother have need, and shutteth up his bowels\add of compassion\add* from him, how dwelleth the love of God in him?
\v 18 My little children, let us not love in word, neither in tongue; but in deed and in truth.
\v 19 And hereby we know that we are of the truth, and shall assure our hearts before him.
\v 20 For if our heart condemn us, God is greater than our heart, and knoweth all things.
\v 21 Beloved, if our heart condemn us not, \add then\add* have we confidence toward God.
\v 22 And whatsoever we ask, we receive of him, because we keep his commandments, and do those things that are pleasing in his sight.
\v 23 And this is his commandment, That we should believe on the name of his Son Jesus Christ, and love one another, as he gave us commandment.
\v 24 And he that keepeth his commandments dwelleth in him, and he in him. And hereby we know that he abideth in us, by the Spirit which he hath given us.
\c 4
\p
\v 1 Beloved, believe not every spirit, but try the spirits whether they are of God: because many false prophets are gone out into the world.
\v 2 Hereby know ye the Spirit of God: Every spirit that confesseth that Jesus Christ is come in the flesh is of God:
\v 3 And every spirit that confesseth not that Jesus Christ is come in the flesh is not of God: and this is that\add spirit\add* of antichrist, whereof ye have heard that it should come; and even now already is it in the world.
\v 4 Ye are of God, little children, and have overcome them: because greater is he that is in you, than he that is in the world.
\v 5 They are of the world: therefore speak they of the world, and the world heareth them.
\v 6 We are of God: he that knoweth God heareth us; he that is not of God heareth not us. Hereby know we the spirit of truth, and the spirit of error.
\p
\v 7 Beloved, let us love one another: for love is of God; and every one that loveth is born of God, and knoweth God.
\v 8 He that loveth not knoweth not God; for God is love.
\v 9 In this was manifested the love of God toward us, because that God sent his only begotten Son into the world, that we might live through him.
\v 10 Herein is love, not that we loved God, but that he loved us, and sent his Son\add to be\add* the propitiation for our sins.
\v 11 Beloved, if God so loved us, we ought also to love one another.
\v 12 No man hath seen God at any time. If we love one another, God dwelleth in us, and his love is perfected in us.
\v 13 Hereby know we that we dwell in him, and he in us, because he hath given us of his Spirit.
\v 14 And we have seen and do testify that the Father sent the Son\add to be\add* the Saviour of the world.
\v 15 Whosoever shall confess that Jesus is the Son of God, God dwelleth in him, and he in God.
\v 16 And we have known and believed the love that God hath to us. God is love; and he that dwelleth in love dwelleth in God, and God in him.
\v 17 Herein is our love made perfect, that we may have boldness in the day of judgment: because as he is, so are we in this world.
\v 18 There is no fear in love; but perfect love casteth out fear: because fear hath torment. He that feareth is not made perfect in love.
\v 19 We love him, because he first loved us.
\v 20 If a man say, I love God, and hateth his brother, he is a liar: for he that loveth not his brother whom he hath seen, how can he love God whom he hath not seen?
\v 21 And this commandment have we from him, That he who loveth God love his brother also.
\c 5
\p
\v 1 Whosoever believeth that Jesus is the Christ is born of God: and every one that loveth him that begat loveth him also that is begotten of him.
\v 2 By this we know that we love the children of God, when we love God, and keep his commandments.
\v 3 For this is the love of God, that we keep his commandments: and his commandments are not grievous.
\v 4 For whatsoever is born of God overcometh the world: and this is the victory that overcometh the world,\add even\add* our faith.
\v 5 Who is he that overcometh the world, but he that believeth that Jesus is the Son of God?
\v 6 This is he that came by water and blood, \add even\add* Jesus Christ; not by water only, but by water and blood. And it is the Spirit that beareth witness, because the Spirit is truth.
\v 7 For there are three that bear record in heaven, the Father, the Word, and the Holy Ghost: and these three are one.
\v 8 And there are three that bear witness in earth, the spirit, and the water, and the blood: and these three agree in one.
\v 9 If we receive the witness of men, the witness of God is greater: for this is the witness of God which he hath testified of his Son.
\v 10 He that believeth on the Son of God hath the witness in himself: he that believeth not God hath made him a liar; because he believeth not the record that God gave of his Son.
\v 11 And this is the record, that God hath given to us eternal life, and this life is in his Son.
\v 12 He that hath the Son hath life; \add and\add* he that hath not the Son of God hath not life.
\v 13 These things have I written unto you that believe on the name of the Son of God; that ye may know that ye have eternal life, and that ye may believe on the name of the Son of God.
\v 14 And this is the confidence that we have in him, that, if we ask any thing according to his will, he heareth us:
\v 15 And if we know that he hear us, whatsoever we ask, we know that we have the petitions that we desired of him.
\v 16 If any man see his brother sin a sin \add which is\add* not unto death, he shall ask, and he shall give him life for them that sin not unto death. There is a sin unto death: I do not say that he shall pray for it.
\v 17 All unrighteousness is sin: and there is a sin not unto death.
\v 18 We know that whosoever is born of God sinneth not; but he that is begotten of God keepeth himself, and that wicked one toucheth him not.
\v 19 And we know that we are of God, and the whole world lieth in wickedness.
\v 20  \add And\add* we know that the Son of God is come, and hath given us an understanding, that we may know him that istrue, and we are in him that is true, \add even\add* in his Son Jesus Christ. This is the true God, and eternal life.
\v 21 Little children, keep yourselves from idols. Amen.
EOD;
$data [] = $usfm;

$usfm = <<<'EOD'
\id HEB
\ide UTF-8
\h Hebrews
\toc1 The Epistle to the Hebrews
\toc2 Hebrews
\toc3 Heb
\mt The Epistle to the Hebrews
\c 1
\p
\v 1 God, who at sundry times and in divers manners spake in time past unto the fathers by the prophets,
\v 2 Hath in these last days spoken unto us by \add his\add* Son, whom he hath appointed heir of all things, by whom also he made the worlds;
\v 3 Who being the brightness of \add his\add* glory, and the express image of his person, and upholding all things by the word of his power, when he had by himself purged our sins, sat down on the right hand of the Majesty on high;
\v 4 Being made so much better than the angels, as he hath by inheritance obtained a more excellent name than they.
\v 5 For unto which of the angels said he at any time, Thou art my Son, this day have I begotten thee? And again, I will be to him a Father, and he shall be to me a Son?
\v 6 And again, when he bringeth in the firstbegotten into the world, he saith, And let all the angels of God worship him.
\v 7 And of the angels he saith, Who maketh his angels spirits, and his ministers a flame of fire.
\v 8 But unto the Son \add he saith,\add* Thy throne, O God, \add is\add* for ever and ever: a sceptre of righteousness\add is\add* the sceptre of thy kingdom.
\v 9 Thou hast loved righteousness, and hated iniquity; therefore God,\add even\add* thy God, hath anointed thee with the oil of gladness above thy fellows.
\v 10 And, Thou, Lord, in the beginning hast laid the foundation of the earth; and the heavens are the works of thine hands:
\v 11 They shall perish; but thou remainest; and they all shall wax old as doth a garment;
\v 12 And as a vesture shalt thou fold them up, and they shall be changed: but thou art the same, and thy years shall not fail.
\v 13 But to which of the angels said he at any time, Sit on my right hand, until I make thine enemies thy footstool?
\v 14 Are they not all ministering spirits, sent forth to minister for them who shall be heirs of salvation?
\c 2
\p
\v 1 Therefore we ought to give the more earnest heed to the things which we have heard, lest at any time we should let\add them\add* slip.
\v 2 For if the word spoken by angels was stedfast, and every transgression and disobedience received a just recompence of reward;
\v 3 How shall we escape, if we neglect so great salvation; which at the first began to be spoken by the Lord, and wasconfirmed unto us by them that heard \add him; \add*
\v 4 God also bearing \add them\add* witness, both with signs and wonders, and with divers miracles, and gifts of the Holy Ghost, according to his own will?
\p
\v 5 For unto the angels hath he not put in subjection the world to come, whereof we speak.
\v 6 But one in a certain place testified, saying, What is man, that thou art mindful of him? or the son of man, that thou visitest him?
\v 7 Thou madest him a little lower than the angels; thou crownedst him with glory and honour, and didst set him over the works of thy hands:
\v 8 Thou hast put all things in subjection under his feet. For in that he put all in subjection under him, he left nothing\add that is\add* not put under him. But now we see not yet all things put under him.
\v 9 But we see Jesus, who was made a little lower than the angels for the suffering of death, crowned with glory and honour; that he by the grace of God should taste death for every man.
\v 10 For it became him, for whom \add are\add* all things, and by whom\add are\add* all things, in bringing many sons unto glory, to make the captain of their salvation perfect through sufferings.
\v 11 For both he that sanctifieth and they who are sanctified \add are\add* all of one: for which cause he is not ashamed to call them brethren,
\v 12 Saying, I will declare thy name unto my brethren, in the midst of the church will I sing praise unto thee.
\v 13 And again, I will put my trust in him. And again, Behold I and the children which God hath given me.
\v 14 Forasmuch then as the children are partakers of flesh and blood, he also himself likewise took part of the same; that through death he might destroy him that had the power of death, that is, the devil;
\v 15 And deliver them who through fear of death were all their lifetime subject to bondage.
\v 16 For verily he took not on \add him the nature of\add* angels; but he took on\add him\add* the seed of Abraham.
\v 17 Wherefore in all things it behoved him to be made like unto\add his\add* brethren, that he might be a merciful and faithful high priest in things\add pertaining\add* to God, to make reconciliation for the sins of the people.
\v 18 For in that he himself hath suffered being tempted, he is able to succour them that are tempted.
\c 3
\p
\v 1 Wherefore, holy brethren, partakers of the heavenly calling, consider the Apostle and High Priest of our profession, Christ Jesus;
\v 2 Who was faithful to him that appointed him, as also Moses \add wasfaithful\add* in all his house.
\v 3 For this \add man\add* was counted worthy of more glory than Moses, inasmuch as he who hath builded the house hath more honour than the house.
\v 4 For every house is builded by some \add man;\add* but he that built all things\add is\add* God.
\v 5 And Moses verily \add was\add* faithful in all his house, as a servant, for a testimony of those things which were to be spoken after;
\v 6 But Christ as a son over his own house; whose house are we, if we hold fast the confidence and the rejoicing of the hope firm unto the end.
\v 7 Wherefore (as the Holy Ghost saith, To day if ye will hear his voice,
\v 8 Harden not your hearts, as in the provocation, in the day of temptation in the wilderness:
\v 9 When your fathers tempted me, proved me, and saw my works forty years.
\v 10 Wherefore I was grieved with that generation, and said, They do alway err in\add their\add* heart; and they have not known my ways.
\v 11 So I sware in my wrath, They shall not enter into my rest.)
\v 12 Take heed, brethren, lest there be in any of you an evil heart of unbelief, in departing from the living God.
\v 13 But exhort one another daily, while it is called To day; lest any of you be hardened through the deceitfulness of sin.
\v 14 For we are made partakers of Christ, if we hold the beginning of our confidence stedfast unto the end;
\v 15 While it is said, To day if ye will hear his voice, harden not your hearts, as in the provocation.
\v 16 For some, when they had heard, did provoke: howbeit not all that came out of Egypt by Moses.
\v 17 But with whom was he grieved forty years? \add was it\add* not with them that had sinned, whose carcases fell in the wilderness?
\v 18 And to whom sware he that they should not enter into his rest, but to them that believed not?
\v 19 So we see that they could not enter in because of unbelief.
\c 4
\p
\v 1 Let us therefore fear, lest, a promise being left \add us\add* of entering into his rest, any of you should seem to come short of it.
\v 2 For unto us was the gospel preached, as well as unto them: but the word preached did not profit them, not being mixed withfaith in them that heard \add it. \add*
\v 3 For we which have believed do enter into rest, as he said, As I have sworn in my wrath, if they shall enter into my rest: although the works were finished from the foundation of the world.
\v 4 For he spake in a certain place of the seventh \add day\add* on this wise, And God did rest the seventh day from all his works.
\v 5 And in this \add place\add* again, If they shall enter into my rest.
\v 6 Seeing therefore it remaineth that some must enter therein, and they to whom it was first preached entered not in because of unbelief:
\v 7 Again, he limiteth a certain day, saying in David, To day, after so long a time; as it is said, To day if ye will hear his voice, harden not your hearts.
\v 8 For if Jesus had given them rest, then would he not afterward have spoken of another day.
\v 9 There remaineth therefore a rest to the people of God.
\v 10 For he that is entered into his rest, he also hath ceased from his own works, as God\add did\add* from his.
\v 11 Let us labour therefore to enter into that rest, lest any man fall after the same example of unbelief.
\v 12 For the word of God \add is\add* quick, and powerful, and sharper than any twoedged sword, piercing even to the dividingasunder of soul and spirit, and of the joints and marrow, and \add is\add* a discerner of the thoughts and intents of the heart.
\v 13 Neither is there any creature that is not manifest in his sight: but all things\add are\add* naked and opened unto the eyes of him with whom we have to do.
\v 14 Seeing then that we have a great high priest, that is passed into the heavens, Jesus the Son of God, let us hold fast\add our\add* profession.
\v 15 For we have not an high priest which cannot be touched with the feeling of our infirmities; but was in all points temptedlike as \add we are, yet\add* without sin.
\v 16 Let us therefore come boldly unto the throne of grace, that we may obtain mercy, and find grace to help in time of need.
\c 5
\p
\v 1 For every high priest taken from among men is ordained for men in things\add pertaining\add* to God, that he may offer both gifts and sacrifices for sins:
\v 2 Who can have compassion on the ignorant, and on them that are out of the way; for that he himself also is compassed with infirmity.
\v 3 And by reason hereof he ought, as for the people, so also for himself, to offer for sins.
\v 4 And no man taketh this honour unto himself, but he that is called of God, as\add was\add* Aaron.
\v 5 So also Christ glorified not himself to be made an high priest; but he that said unto him, Thou art my Son, to day have I begotten thee.
\v 6 As he saith also in another \add place,\add* Thou \add art\add* a priest for ever after the order of Melchisedec.
\v 7 Who in the days of his flesh, when he had offered up prayers and supplications with strong crying and tears unto him that was able to save him from death, and was heard in that he feared;
\v 8 Though he were a Son, yet learned he obedience by the things which he suffered;
\v 9 And being made perfect, he became the author of eternal salvation unto all them that obey him;
\v 10 Called of God an high priest after the order of Melchisedec.
\p
\v 11 Of whom we have many things to say, and hard to be uttered, seeing ye are dull of hearing.
\v 12 For when for the time ye ought to be teachers, ye have need that one teach you again which\add be\add* the first principles of the oracles of God; and are become such as have need of milk, and not of strong meat.
\v 13 For every one that useth milk \add is\add* unskilful in the word of righteousness: for he is a babe.
\v 14 But strong meat belongeth to them that are of full age, \add even\add* those who by reason of use have their senses exercised to discern both good and evil.
\c 6
\p
\v 1 Therefore leaving the principles of the doctrine of Christ, let us go on unto perfection; not laying again the foundation of repentance from dead works, and of faith toward God,
\v 2 Of the doctrine of baptisms, and of laying on of hands, and of resurrection of the dead, and of eternal judgment.
\v 3 And this will we do, if God permit.
\v 4 For \add it is\add* impossible for those who were once enlightened, and have tasted of the heavenly gift, and were made partakers of the Holy Ghost,
\v 5 And have tasted the good word of God, and the powers of the world to come,
\v 6 If they shall fall away, to renew them again unto repentance; seeing they crucify to themselves the Son of God afresh, andput \add him\add* to an open shame.
\v 7 For the earth which drinketh in the rain that cometh oft upon it, and bringeth forth herbs meet for them by whom it is dressed, receiveth blessing from God:
\v 8 But that which beareth thorns and briers \add is\add* rejected, and\add is\add* nigh unto cursing; whose end \add is\add* to be burned.
\v 9 But, beloved, we are persuaded better things of you, and things that accompany salvation, though we thus speak.
\v 10 For God \add is\add* not unrighteous to forget your work and labour of love, which ye have shewed toward his name, in that ye have ministered to the saints, and do minister.
\v 11 And we desire that every one of you do shew the same diligence to the full assurance of hope unto the end:
\v 12 That ye be not slothful, but followers of them who through faith and patience inherit the promises.
\v 13 For when God made promise to Abraham, because he could swear by no greater, he sware by himself,
\v 14 Saying, Surely blessing I will bless thee, and multiplying I will multiply thee.
\v 15 And so, after he had patiently endured, he obtained the promise.
\v 16 For men verily swear by the greater: and an oath for confirmation\add is\add* to them an end of all strife.
\v 17 Wherein God, willing more abundantly to shew unto the heirs of promise the immutability of his counsel, confirmed\add it\add* by an oath:
\v 18 That by two immutable things, in which \add it was\add* impossible for God to lie, we might have a strong consolation, who have fled for refuge to lay hold upon the hope set before us:
\v 19 Which \add hope\add* we have as an anchor of the soul, both sure and stedfast, and which entereth into that within the veil;
\v 20 Whither the forerunner is for us entered, \add even\add* Jesus, made an high priest for ever after the order of Melchisedec.
\c 7
\p
\v 1 For this Melchisedec, king of Salem, priest of the most high God, who met Abraham returning from the slaughter of the kings, and blessed him;
\v 2 To whom also Abraham gave a tenth part of all; first being by interpretation King of righteousness, and after that also King of Salem, which is, King of peace;
\v 3 Without father, without mother, without descent, having neither beginning of days, nor end of life; but made like unto the Son of God; abideth a priest continually.
\v 4 Now consider how great this man \add was,\add* unto whom even the patriarch Abraham gave the tenth of the spoils.
\v 5 And verily they that are of the sons of Levi, who receive the office of the priesthood, have a commandment to take tithes of the people according to the law, that is, of their brethren, though they come out of the loins of Abraham:
\v 6 But he whose descent is not counted from them received tithes of Abraham, and blessed him that had the promises.
\v 7 And without all contradiction the less is blessed of the better.
\v 8 And here men that die receive tithes; but there he \add receiveththem,\add* of whom it is witnessed that he liveth.
\v 9 And as I may so say, Levi also, who receiveth tithes, payed tithes in Abraham.
\v 10 For he was yet in the loins of his father, when Melchisedec met him.
\v 11 If therefore perfection were by the Levitical priesthood, (for under it the people received the law,) what further need\add was there\add* that another priest should rise after the order of Melchisedec, and not be called after the order of Aaron?
\v 12 For the priesthood being changed, there is made of necessity a change also of the law.
\v 13 For he of whom these things are spoken pertaineth to another tribe, of which no man gave attendance at the altar.
\v 14 For \add it is\add* evident that our Lord sprang out of Juda; of which tribe Moses spake nothing concerning priesthood.
\v 15 And it is yet far more evident: for that after the similitude of Melchisedec there ariseth another priest,
\v 16 Who is made, not after the law of a carnal commandment, but after the power of an endless life.
\v 17 For he testifieth, Thou \add art\add* a priest for ever after the order of Melchisedec.
\v 18 For there is verily a disannulling of the commandment going before for the weakness and unprofitableness thereof.
\v 19 For the law made nothing perfect, but the bringing in of a better hope\add did;\add* by the which we draw nigh unto God.
\v 20 And inasmuch as not without an oath \add he was made priest:\add*
\v 21 (For those priests were made without an oath; but this with an oath by him that said unto him, The Lord sware and willnot repent, Thou \add art\add* a priest for ever after the order of Melchisedec:)
\v 22 By so much was Jesus made a surety of a better testament.
\v 23 And they truly were many priests, because they were not suffered to continue by reason of death:
\v 24 But this \add man,\add* because he continueth ever, hath an unchangeable priesthood.
\v 25 Wherefore he is able also to save them to the uttermost that come unto God by him, seeing he ever liveth to make intercession for them.
\v 26 For such an high priest became us, \add who is\add* holy, harmless, undefiled, separate from sinners, and made higher than the heavens;
\v 27 Who needeth not daily, as those high priests, to offer up sacrifice, first for his own sins, and then for the people’s: for this he did once, when he offered up himself.
\v 28 For the law maketh men high priests which have infirmity; but the word of the oath, which was since the law,\add maketh\add* the Son, who is consecrated for evermore.
\c 8
\p
\v 1 Now of the things which we have spoken \add this is\add* the sum: We have such an high priest, who is set on the right hand of the throne of the Majesty in the heavens;
\v 2 A minister of the sanctuary, and of the true tabernacle, which the Lord pitched, and not man.
\v 3 For every high priest is ordained to offer gifts and sacrifices: wherefore\add it is\add* of necessity that this man have somewhat also to offer.
\v 4 For if he were on earth, he should not be a priest, seeing that there are priests that offer gifts according to the law:
\v 5 Who serve unto the example and shadow of heavenly things, as Moses was admonished of God when he was about to make thetabernacle: for, See, saith he, \add that\add* thou make all things according to the pattern shewed to thee in the mount.
\v 6 But now hath he obtained a more excellent ministry, by how much also he is the mediator of a better covenant, which was established upon better promises.
\v 7 For if that first \add covenant\add* had been faultless, then should no place have been sought for the second.
\v 8 For finding fault with them, he saith, Behold, the days come, saith the Lord, when I will make a new covenant with the house of Israel and with the house of Judah:
\v 9 Not according to the covenant that I made with their fathers in the day when I took them by the hand to lead them out of the land of Egypt; because they continued not in my covenant, and I regarded them not, saith the Lord.
\v 10 For this \add is\add* the covenant that I will make with the house of Israel after those days, saith the Lord; I will put my laws into their mind, and write them in their hearts: and I will be to them a God, and they shall be to me a people:
\v 11 And they shall not teach every man his neighbour, and every man his brother, saying, Know the Lord: for all shall know me, from the least to the greatest.
\v 12 For I will be merciful to their unrighteousness, and their sins and their iniquities will I remember no more.
\v 13 In that he saith, A new \add covenant,\add* he hath made the first old. Now that which decayeth and waxeth old\add is\add* ready to vanish away.
\c 9
\p
\v 1 Then verily the first \add covenant\add* had also ordinances of divine service, and a worldly sanctuary.
\v 2 For there was a tabernacle made; the first, wherein \add was\add* the candlestick, and the table, and the shewbread; which is called the sanctuary.
\v 3 And after the second veil, the tabernacle which is called the Holiest of all;
\v 4 Which had the golden censer, and the ark of the covenant overlaid round about with gold, wherein\add was\add* the golden pot that had manna, and Aaron’s rod that budded, and the tables of the covenant;
\v 5 And over it the cherubims of glory shadowing the mercyseat; of which we cannot now speak particularly.
\v 6 Now when these things were thus ordained, the priests went always into the first tabernacle, accomplishing the service\add of God. \add*
\v 7 But into the second \add went\add* the high priest alone once every year, not without blood, which he offered for himself,and \add for\add* the errors of the people:
\v 8 The Holy Ghost this signifying, that the way into the holiest of all was not yet made manifest, while as the first tabernacle was yet standing:
\v 9 Which \add was\add* a figure for the time then present, in which were offered both gifts and sacrifices, that could not make him that did the service perfect, as pertaining to the conscience;
\v 10  \add Which stood\add* only in meats and drinks, and divers washings, and carnal ordinances, imposed\add on them\add* until the time of reformation.
\v 11 But Christ being come an high priest of good things to come, by a greater and more perfect tabernacle, not made with hands, that is to say, not of this building;
\v 12 Neither by the blood of goats and calves, but by his own blood he entered in once into the holy place, having obtainedeternal redemption \add for us. \add*
\v 13 For if the blood of bulls and of goats, and the ashes of an heifer sprinkling the unclean, sanctifieth to the purifying of the flesh:
\v 14 How much more shall the blood of Christ, who through the eternal Spirit offered himself without spot to God, purge your conscience from dead works to serve the living God?
\v 15 And for this cause he is the mediator of the new testament, that by means of death, for the redemption of thetransgressions \add that were\add* under the first testament, they which are called might receive the promise of eternal inheritance.
\v 16 For where a testament \add is,\add* there must also of necessity be the death of the testator.
\v 17 For a testament \add is\add* of force after men are dead: otherwise it is of no strength at all while the testator liveth.
\v 18 Whereupon neither the first \add testament\add* was dedicated without blood.
\v 19 For when Moses had spoken every precept to all the people according to the law, he took the blood of calves and of goats, with water, and scarlet wool, and hyssop, and sprinkled both the book, and all the people,
\v 20 Saying, This \add is\add* the blood of the testament which God hath enjoined unto you.
\v 21 Moreover he sprinkled with blood both the tabernacle, and all the vessels of the ministry.
\v 22 And almost all things are by the law purged with blood; and without shedding of blood is no remission.
\v 23  \add It was\add* therefore necessary that the patterns of things in the heavens should be purified with these; but the heavenly things themselves with better sacrifices than these.
\v 24 For Christ is not entered into the holy places made with hands,\add which are\add* the figures of the true; but into heaven itself, now to appear in the presence of God for us:
\v 25 Nor yet that he should offer himself often, as the high priest entereth into the holy place every year with blood of others;
\v 26 For then must he often have suffered since the foundation of the world: but now once in the end of the world hath he appeared to put away sin by the sacrifice of himself.
\v 27 And as it is appointed unto men once to die, but after this the judgment:
\v 28 So Christ was once offered to bear the sins of many; and unto them that look for him shall he appear the second time without sin unto salvation.
\c 10
\p
\v 1 For the law having a shadow of good things to come, \add and\add* not the very image of the things, can never with those sacrifices which they offered year by year continually make the comers thereunto perfect.
\v 2 For then would they not have ceased to be offered? because that the worshippers once purged should have had no more conscience of sins.
\v 3 But in those \add sacrifices there is\add* a remembrance again\add made\add* of sins every year.
\v 4 For \add it is\add* not possible that the blood of bulls and of goats should take away sins.
\v 5 Wherefore when he cometh into the world, he saith, Sacrifice and offering thou wouldest not, but a body hast thou prepared me:
\v 6 In burnt offerings and \add sacrifices\add* for sin thou hast had no pleasure.
\v 7 Then said I, Lo, I come (in the volume of the book it is written of me,) to do thy will, O God.
\v 8 Above when he said, Sacrifice and offering and burnt offerings and\add offering\add* for sin thou wouldest not, neither hadst pleasure\add therein;\add* which are offered by the law;
\v 9 Then said he, Lo, I come to do thy will, O God. He taketh away the first, that he may establish the second.
\v 10 By the which will we are sanctified through the offering of the body of Jesus Christ once\add for all. \add*
\v 11 And every priest standeth daily ministering and offering oftentimes the same sacrifices, which can never take away sins:
\v 12 But this man, after he had offered one sacrifice for sins for ever, sat down on the right hand of God;
\v 13 From henceforth expecting till his enemies be made his footstool.
\v 14 For by one offering he hath perfected for ever them that are sanctified.
\v 15  \add Whereof\add* the Holy Ghost also is a witness to us: for after that he had said before,
\v 16 This \add is\add* the covenant that I will make with them after those days, saith the Lord, I will put my laws into their hearts, and in their minds will I write them;
\v 17 And their sins and iniquities will I remember no more.
\v 18 Now where remission of these \add is, there is\add* no more offering for sin.
\p
\v 19 Having therefore, brethren, boldness to enter into the holiest by the blood of Jesus,
\v 20 By a new and living way, which he hath consecrated for us, through the veil, that is to say, his flesh;
\v 21 And \add having\add* an high priest over the house of God;
\v 22 Let us draw near with a true heart in full assurance of faith, having our hearts sprinkled from an evil conscience, and our bodies washed with pure water.
\v 23 Let us hold fast the profession of \add our\add* faith without wavering; (for he is faithful that promised;)
\v 24 And let us consider one another to provoke unto love and to good works:
\v 25 Not forsaking the assembling of ourselves together, as the manner of some\add is;\add* but exhorting \add one another:\add* and so much the more, as ye see the day approaching.
\v 26 For if we sin wilfully after that we have received the knowledge of the truth, there remaineth no more sacrifice for sins,
\v 27 But a certain fearful looking for of judgment and fiery indignation, which shall devour the adversaries.
\v 28 He that despised Moses’ law died without mercy under two or three witnesses:
\v 29 Of how much sorer punishment, suppose ye, shall he be thought worthy, who hath trodden under foot the Son of God, and hath counted the blood of the covenant, wherewith he was sanctified, an unholy thing, and hath done despite unto the Spirit of grace?
\v 30 For we know him that hath said, Vengeance \add belongeth\add* unto me, I will recompense, saith the Lord. And again, The Lord shall judge his people.
\v 31  \add It is\add* a fearful thing to fall into the hands of the living God.
\v 32 But call to remembrance the former days, in which, after ye were illuminated, ye endured a great fight of afflictions;
\v 33 Partly, whilst ye were made a gazingstock both by reproaches and afflictions; and partly, whilst ye became companions of them that were so used.
\v 34 For ye had compassion of me in my bonds, and took joyfully the spoiling of your goods, knowing in yourselves that ye have in heaven a better and an enduring substance.
\v 35 Cast not away therefore your confidence, which hath great recompence of reward.
\v 36 For ye have need of patience, that, after ye have done the will of God, ye might receive the promise.
\v 37 For yet a little while, and he that shall come will come, and will not tarry.
\v 38 Now the just shall live by faith: but if \add any man\add* draw back, my soul shall have no pleasure in him.
\v 39 But we are not of them who draw back unto perdition; but of them that believe to the saving of the soul.
\c 11
\p
\v 1 Now faith is the substance of things hoped for, the evidence of things not seen.
\v 2 For by it the elders obtained a good report.
\v 3 Through faith we understand that the worlds were framed by the word of God, so that things which are seen were not made of things which do appear.
\v 4 By faith Abel offered unto God a more excellent sacrifice than Cain, by which he obtained witness that he was righteous, God testifying of his gifts: and by it he being dead yet speaketh.
\v 5 By faith Enoch was translated that he should not see death; and was not found, because God had translated him: for before his translation he had this testimony, that he pleased God.
\v 6 But without faith \add it is\add* impossible to please \add him:\add* for he that cometh to God must believe that he is,and \add that\add* he is a rewarder of them that diligently seek him.
\v 7 By faith Noah, being warned of God of things not seen as yet, moved with fear, prepared an ark to the saving of his house; by the which he condemned the world, and became heir of the righteousness which is by faith.
\v 8 By faith Abraham, when he was called to go out into a place which he should after receive for an inheritance, obeyed; and he went out, not knowing whither he went.
\v 9 By faith he sojourned in the land of promise, as \add in\add* a strange country, dwelling in tabernacles with Isaac and Jacob, the heirs with him of the same promise:
\v 10 For he looked for a city which hath foundations, whose builder and maker\add is\add* God.
\v 11 Through faith also Sara herself received strength to conceive seed, and was delivered of a child when she was past age, because she judged him faithful who had promised.
\v 12 Therefore sprang there even of one, and him as good as dead,\add so many\add* as the stars of the sky in multitude, and as the sand which is by the sea shore innumerable.
\v 13 These all died in faith, not having received the promises, but having seen them afar off, and were persuaded of\add them,\add* and embraced \add them,\add* and confessed that they were strangers and pilgrims on the earth.
\v 14 For they that say such things declare plainly that they seek a country.
\v 15 And truly, if they had been mindful of that \add country\add* from whence they came out, they might have had opportunity to have returned.
\v 16 But now they desire a better \add country,\add* that is, an heavenly: wherefore God is not ashamed to be called their God: for he hath prepared for them a city.
\v 17 By faith Abraham, when he was tried, offered up Isaac: and he that had received the promises offered up his only begotten\add son, \add*
\v 18 Of whom it was said, That in Isaac shall thy seed be called:
\v 19 Accounting that God \add was\add* able to raise \add him\add* up, even from the dead; from whence also he received him in a figure.
\v 20 By faith Isaac blessed Jacob and Esau concerning things to come.
\v 21 By faith Jacob, when he was a dying, blessed both the sons of Joseph; and worshipped,\add leaning\add* upon the top of his staff.
\v 22 By faith Joseph, when he died, made mention of the departing of the children of Israel; and gave commandment concerning his bones.
\v 23 By faith Moses, when he was born, was hid three months of his parents, because they saw\add he was\add* a proper child; and they were not afraid of the king’s commandment.
\v 24 By faith Moses, when he was come to years, refused to be called the son of Pharaoh’s daughter;
\v 25 Choosing rather to suffer affliction with the people of God, than to enjoy the pleasures of sin for a season;
\v 26 Esteeming the reproach of Christ greater riches than the treasures in Egypt: for he had respect unto the recompence of the reward.
\v 27 By faith he forsook Egypt, not fearing the wrath of the king: for he endured, as seeing him who is invisible.
\v 28 Through faith he kept the passover, and the sprinkling of blood, lest he that destroyed the firstborn should touch them.
\v 29 By faith they passed through the Red sea as by dry \add land:\add* which the Egyptians assaying to do were drowned.
\v 30 By faith the walls of Jericho fell down, after they were compassed about seven days.
\v 31 By faith the harlot Rahab perished not with them that believed not, when she had received the spies with peace.
\v 32 And what shall I more say? for the time would fail me to tell of Gedeon, and\add of\add* Barak, and \add of\add* Samson, and \add of\add* Jephthae;\add of\add* David also, and Samuel, and \add of\add* the prophets:
\v 33 Who through faith subdued kingdoms, wrought righteousness, obtained promises, stopped the mouths of lions,
\v 34 Quenched the violence of fire, escaped the edge of the sword, out of weakness were made strong, waxed valiant in fight, turned to flight the armies of the aliens.
\v 35 Women received their dead raised to life again: and others were tortured, not accepting deliverance; that they might obtain a better resurrection:
\v 36 And others had trial of \add cruel\add* mockings and scourgings, yea, moreover of bonds and imprisonment:
\v 37 They were stoned, they were sawn asunder, were tempted, were slain with the sword: they wandered about in sheepskins and goatskins; being destitute, afflicted, tormented;
\v 38 (Of whom the world was not worthy:) they wandered in deserts, and\add in\add* mountains, and \add in\add* dens and caves of the earth.
\v 39 And these all, having obtained a good report through faith, received not the promise:
\v 40 God having provided some better thing for us, that they without us should not be made perfect.
\c 12
\p
\v 1 Wherefore seeing we also are compassed about with so great a cloud of witnesses, let us lay aside every weight, and the sin which doth so easily beset us, and let us run with patience the race that is set before us,
\v 2 Looking unto Jesus the author and finisher of \add our\add* faith; who for the joy that was set before him endured the cross, despising the shame, and is set down at the right hand of the throne of God.
\v 3 For consider him that endured such contradiction of sinners against himself, lest ye be wearied and faint in your minds.
\v 4 Ye have not yet resisted unto blood, striving against sin.
\v 5 And ye have forgotten the exhortation which speaketh unto you as unto children, My son, despise not thou the chastening of the Lord, nor faint when thou art rebuked of him:
\v 6 For whom the Lord loveth he chasteneth, and scourgeth every son whom he receiveth.
\v 7 If ye endure chastening, God dealeth with you as with sons; for what son is he whom the father chasteneth not?
\v 8 But if ye be without chastisement, whereof all are partakers, then are ye bastards, and not sons.
\v 9 Furthermore we have had fathers of our flesh which corrected\add us,\add* and we gave \add them\add* reverence: shall we not much rather be in subjection unto the Father of spirits, and live?
\v 10 For they verily for a few days chastened \add us\add* after their own pleasure; but he for\add our\add* profit, that \add we\add* might be partakers of his holiness.
\v 11 Now no chastening for the present seemeth to be joyous, but grievous: nevertheless afterward it yieldeth the peaceable fruit of righteousness unto them which are exercised thereby.
\v 12 Wherefore lift up the hands which hang down, and the feeble knees;
\v 13 And make straight paths for your feet, lest that which is lame be turned out of the way; but let it rather be healed.
\v 14 Follow peace with all \add men,\add* and holiness, without which no man shall see the Lord:
\v 15 Looking diligently lest any man fail of the grace of God; lest any root of bitterness springing up trouble\add you,\add* and thereby many be defiled;
\v 16 Lest there \add be\add* any fornicator, or profane person, as Esau, who for one morsel of meat sold his birthright.
\v 17 For ye know how that afterward, when he would have inherited the blessing, he was rejected: for he found no place of repentance, though he sought it carefully with tears.
\v 18 For ye are not come unto the mount that might be touched, and that burned with fire, nor unto blackness, and darkness, and tempest,
\v 19 And the sound of a trumpet, and the voice of words; \add which\add* voice they that heard intreated that the word should not be spoken to them any more:
\v 20 (For they could not endure that which was commanded, And if so much as a beast touch the mountain, it shall be stoned, or thrust through with a dart:
\v 21 And so terrible was the sight, \add that\add* Moses said, I exceedingly fear and quake:)
\v 22 But ye are come unto mount Sion, and unto the city of the living God, the heavenly Jerusalem, and to an innumerable company of angels,
\v 23 To the general assembly and church of the firstborn, which are written in heaven, and to God the Judge of all, and to the spirits of just men made perfect,
\v 24 And to Jesus the mediator of the new covenant, and to the blood of sprinkling, that speaketh better things than\add that of\add* Abel.
\v 25 See that ye refuse not him that speaketh. For if they escaped not who refused him that spake on earth, much more\add shall not\add* we \add escape,\add* if we turn away from him that\add speaketh\add* from heaven:
\v 26 Whose voice then shook the earth: but now he hath promised, saying, Yet once more I shake not the earth only, but also heaven.
\v 27 And this \add word,\add* Yet once more, signifieth the removing of those things that are shaken, as of things that are made, that those things which cannot be shaken may remain.
\v 28 Wherefore we receiving a kingdom which cannot be moved, let us have grace, whereby we may serve God acceptably with reverence and godly fear:
\v 29 For our God \add is\add* a consuming fire.
\c 13
\p
\v 1 Let brotherly love continue.
\v 2 Be not forgetful to entertain strangers: for thereby some have entertained angels unawares.
\v 3 Remember them that are in bonds, as bound with them; \add and\add* them which suffer adversity, as being yourselves also in the body.
\v 4 Marriage \add is\add* honourable in all, and the bed undefiled: but whoremongers and adulterers God will judge.
\v 5  \add Let your\add* conversation \add be\add* without covetousness;\add and be\add* content with such things as ye have: for he hath said, I will never leave thee, nor forsake thee.
\v 6 So that we may boldly say, The Lord \add is\add* my helper, and I will not fear what man shall do unto me.
\p
\v 7 Remember them which have the rule over you, who have spoken unto you the word of God: whose faith follow, considering theend of \add their\add* conversation.
\v 8 Jesus Christ the same yesterday, and to day, and for ever.
\v 9 Be not carried about with divers and strange doctrines. For\add it is\add* a good thing that the heart be established with grace; not with meats, which have not profited them that have been occupied therein.
\v 10 We have an altar, whereof they have no right to eat which serve the tabernacle.
\v 11 For the bodies of those beasts, whose blood is brought into the sanctuary by the high priest for sin, are burned without the camp.
\v 12 Wherefore Jesus also, that he might sanctify the people with his own blood, suffered without the gate.
\v 13 Let us go forth therefore unto him without the camp, bearing his reproach.
\v 14 For here have we no continuing city, but we seek one to come.
\v 15 By him therefore let us offer the sacrifice of praise to God continually, that is, the fruit of\add our\add* lips giving thanks to his name.
\v 16 But to do good and to communicate forget not: for with such sacrifices God is well pleased.
\v 17 Obey them that have the rule over you, and submit yourselves: for they watch for your souls, as they that must give account, that they may do it with joy, and not with grief: for that\add is\add* unprofitable for you.
\p
\v 18 Pray for us: for we trust we have a good conscience, in all things willing to live honestly.
\v 19 But I beseech \add you\add* the rather to do this, that I may be restored to you the sooner.
\v 20 Now the God of peace, that brought again from the dead our Lord Jesus, that great shepherd of the sheep, through the blood of the everlasting covenant,
\v 21 Make you perfect in every good work to do his will, working in you that which is wellpleasing in his sight, through JesusChrist; to whom \add be\add* glory for ever and ever. Amen.
\v 22 And I beseech you, brethren, suffer the word of exhortation: for I have written a letter unto you in few words.
\v 23 Know ye that \add our\add* brother Timothy is set at liberty; with whom, if he come shortly, I will see you.
\p
\v 24 Salute all them that have the rule over you, and all the saints. They of Italy salute you.
\p
\v 25 Grace \add be\add* with you all. Amen.
EOD;
$data [] = $usfm;

foreach ($data as $usfm) {
  $book_chapter_text = Filter_Usfm::import ($usfm, "Standard");
  foreach ($book_chapter_text as $data) {
    $book_number = $data[0];
    $chapter_number = $data[1];
    $chapter_data = $data[2];
    if ($book_number > 0) {
      Bible_Logic::storeChapter ("KJV", $book_number, $chapter_number, $chapter_data);
    }
  }
}


// Clean out nearly empty chapters from the Bibles.
$bibles = $database_bibles->getBibles ();
foreach ($bibles as $bible) {
  $books = $database_bibles->getBooks ($bible);
  foreach ($books as $book) {
    $chapters = $database_bibles->getChapters ($bible, $book);
    foreach ($chapters as $chapter) {
      // Remove chapters, other than 0, that are rather short, as these chapters likely contain no text, but USFM markers only.
      if ($chapter == 0) continue;
      $usfm = $database_bibles->getChapter ($bible, $book, $chapter);
      $length = strlen ($usfm);
      if ($length < 1000) {
        Bible_Logic::deleteChapter ($bible, $book, $chapter);
      }
    }
    // If a book contains chapter 0 only, remove that entire book.
    $chapters = $database_bibles->getChapters ($bible, $book);
    if ($chapters == array (0)) {
      Bible_Logic::deleteBook ($bible, $book);
    }
  }
  // If a Bible contains no books, remove that Bible.
  $books = $database_bibles->getBooks ($bible);
  if (empty ($books)) Bible_Logic::deleteBible ($bible);
}






?>
