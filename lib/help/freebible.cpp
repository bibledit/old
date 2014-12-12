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


#include <help/freebible.h>
#include <assets/view.h>
#include <assets/page.h>
#include <assets/builder.h>
#include <filter/roles.h>
#include <config/logic.h>


const char * help_freebible_url ()
{
  return "help/freebible";
}


bool help_freebible_acl (void * webserver_request)
{
  return Filter_Roles::access_control (webserver_request, Filter_Roles::guest ());
}


string help_freebible (void * webserver_request)
{
  string page;

  page = Assets_Page::header (gettext("The Open Bible"), webserver_request, "");

  Assets_View view = Assets_View (0);
  Assets_Builder builder;

  view.set_variable ("title", gettext("The Free Bible"));

  view.set_variable ("advantages", gettext("While Bibledit may be used for making any Bibles, there are some distinct advantages to releasing Bibles under a free license."));
  view.set_variable ("advantage1", gettext("Others will feel free to publish the open Bible. You don't need to do everything yourself: Others will help spreading the Word of God."));
  view.set_variable ("advantage2", gettext("It becomes easy to publish Bibles in a variety of formats by a variety of people: on paper, phones, the Internet."));
  view.set_variable ("advantage3", gettext("If you think that the Bible you translated is good and reliable, and you release it under an open license, it will rapidly spread over the Internet."));
  view.set_variable ("advantage4", gettext("Compare an open Bible to the Greek minuscules: They were copied freely, and they can now be found all over the place."));
  view.set_variable ("advantage5", gettext("Freely you received, freely give."));

  builder.clear();
  builder.sentence (gettext("Various church people regularly express a desire to have a more accurate Bible than what they have now."));
  builder.sentence (gettext("The years pass by and the desire becomes stronger."));
  builder.sentence (gettext("They pray about the matter, discuss it, and decide to start the work on improving the Bible themselves, with the help of a booklet with directions."));
  builder.sentence (gettext("The booklet is published by the Bible Society."));
  view.set_variable ("paragraph01", builder.result());

  builder.clear();
  builder.sentence (gettext("Over the years some people join the group, and others leave the group."));
  builder.sentence (gettext("The church now has a translation team consisting of four people who by the grace of God work full time on the revision of the Bible."));
  builder.sentence (gettext("Each of the four people bring in their own expertise."));
  builder.sentence (gettext("Two of them focus on the target language."));
  builder.sentence (gettext("They aim at using accurate, faithful, beautiful and contemporary language."));
  builder.sentence (gettext("One of them focuses on the original Greek and Hebrew."));
  builder.sentence (gettext("He ensures that the translated text matches the originals."));
  builder.sentence (gettext("One of them focuses, more than the others, on the recipients."));
  builder.sentence (gettext("She aims at acceptance of the language by the future readers."));
  builder.sentence (gettext("Many proof-readers contribute to the work."));
  builder.sentence (gettext("The translation team regularly consults with language scholars and church representatives about the Bible translation."));
  builder.sentence (gettext("The Bible Society answers queries about how to translate difficult passages."));
  view.set_variable ("paragraph02", builder.result());

  builder.clear();
  builder.sentence (gettext("After several years the New Testament becomes ready."));
  builder.sentence (gettext("The manuscript is submitted to the Bible Society for publishing."));
  builder.sentence (gettext("The manuscript is formatted and uploaded to the church website."));
  builder.sentence (gettext("It can now be used by whoever wants to do so before it would be published in paper form."));
  view.set_variable ("paragraph03", builder.result());

  builder.clear();
  builder.sentence (gettext("The translation team starts to translate the Old Testament."));
  builder.sentence (gettext("The years pass by."));
  builder.sentence (gettext("The team eagerly awaits news from the Bible Society about publishing the New Testament."));
  builder.sentence (gettext("Five years later the news arrives that the Bible Society starts the preparations for publishing it."));
  builder.sentence (gettext("Another year later the team sees the first copies of the New Testament in book form."));
  view.set_variable ("paragraph04", builder.result());

  builder.clear();
  builder.sentence (gettext("During all those years the New Testament has been locked up in the offices of the Bible Society."));
  builder.sentence (gettext("It was not free to go out to the people in book form, but was kept locked up."));
  builder.sentence (gettext("It was enslaved."));
  view.set_variable ("paragraph05", builder.result());

  builder.clear();
  builder.sentence (gettext("The team keeps working on the Old Testament during many years."));
  builder.sentence (gettext("They are enabled to do so by the Lord God."));
  builder.sentence (gettext("The Old Testament becomes ready."));
  builder.sentence (gettext("The manuscript of the whole Bible is submitted to the Bible Society for publishing."));
  builder.sentence (gettext("One member of the translation team suggests to upload the formatted manuscript to the church website, just as was done with the New Testament."));
  builder.sentence (gettext("The team leader decides to not do this, but let the Bible Society be the first to publish the Bible."));
  builder.sentence (gettext("The translator feels disappointed, because the Bible, now complete and accurate, is not free to go out to the people."));
  builder.sentence (gettext("Not in paper form, not in digital form."));
  builder.sentence (gettext("It is enslaved, again."));
  view.set_variable ("paragraph06", builder.result());

  builder.clear();
  builder.sentence (gettext("Over the years the translator heard stories from others who had similar experiences."));
  builder.sentence (gettext("It occurs to him that the current way of doing things is not right."));
  builder.sentence (gettext("It leads to a situation where the Bible is not free to go where it is needed most:"));
  builder.sentence (gettext("To the people."));
  builder.sentence (gettext("The translator begins to see more clearly that Christians should have freedom to publish Bibles or portions of it any time they wish to do so."));
  view.set_variable ("paragraph07", builder.result());

  builder.clear();
  builder.sentence (gettext("The translator becomes the leader of the translation team."));
  builder.sentence (gettext("The team leader is acquainted with open licenses."));
  view.set_variable ("paragraph08", builder.result());
  view.set_variable ("license", gettext("He chooses the GNU Free Documentation License as an appropriate license for the Bible."));
  view.set_variable ("line08", gettext("He is of the view that it is appropriate for the Word of God to be created under an open license to allow it to have free course."));

  builder.clear();
  builder.sentence (gettext("The GNU Free Documentation License allows noncommercial distribution of the Bible by anyone who accepts the license."));
  builder.sentence (gettext("That is a good thing because this enables anyone to print the Bible and give it to someone else."));
  view.set_variable ("paragraph09", builder.result());

  builder.clear();
  builder.sentence (gettext("The GNU Free Documentation License allows commercial distribution of the Bible by any publisher who accepts the license."));
  builder.sentence (gettext("This is good, because it maximizes the distribution potential by allowing sales of printed Bibles."));
  builder.sentence (gettext("It also harnesses the profit motive for distribution."));
  builder.sentence (gettext("It limits overcharging with competitive pressure on prices.") );
  view.set_variable ("paragraph10", builder.result());

  builder.clear();
  builder.sentence (gettext("The GNU Free Documentation License allows to update the Bible, provided the updated version is shared under the same license."));
  builder.sentence (gettext("This is a good thing because it allows legitimate updates, revisions, adaptations with no hassle to the translators and no delay for getting permission."));
  builder.sentence (gettext("A disadvantage is that changes can be made by people who might not be qualified or called to make changes, with possibly heretical results."));
  view.set_variable ("paragraph11", builder.result());

  builder.clear();
  builder.sentence (gettext("There is some fear that granting permission to make revisions would make it easier for somebody to make heretical changes to the Bible."));
  builder.sentence (gettext("But copyright law is not about preventing this."));
  builder.sentence (gettext("If anyone wants to make those changes, he would still do it."));
  builder.sentence (gettext("There is no need for this fear, because it is God who protects His own work."));
  builder.sentence (gettext("He makes clear what is a good translation, and what is not."));
  view.set_variable ("paragraph12", builder.result());

  builder.clear();
  builder.sentence (gettext("If a Bible Society is so protective about a Bible, so as not to grant a license to make revisions, then there is a bigger danger than that of heresy."));
  builder.sentence (gettext("The bigger danger is that the copyright holder holds on to the Bible, or disappears, or dies, or fails to be a good steward of it."));
  builder.sentence (gettext("Nobody would be allowed to improve or update the text."));
  view.set_variable ("paragraph13", builder.result());

  builder.clear();
  builder.sentence (gettext("Trust God."));
  builder.sentence (gettext("He takes care of His Word,"));
  builder.sentence (gettext("He deals with anyone who might abuse it."));
  view.set_variable ("paragraph14", builder.result());

  builder.clear();
  builder.sentence (gettext("The Bible Society says:"));
  builder.sentence (gettext("We own copyright, not in any way to restrict distribution, but to preserve the text."));
  builder.sentence (gettext("Protecting the text may at times restrict distribution."));
  builder.sentence (gettext("For example, the Bible must be distributed by us, or under our supervision."));
  builder.sentence (gettext("We have made a big investment in the Bible."));
  builder.sentence (gettext("Nobody may improve it without our authority."));
  builder.sentence (gettext("If anybody gives us details as to where the Bible can be improved, they can improve it, but only when we agree."));
  builder.sentence (gettext("This Bible is our property."));
  builder.sentence (gettext("We share it."));
  builder.sentence (gettext("It is not entirely ours, so there is a shared property."));
  builder.sentence (gettext("The Free Documentation License is good in an ideal world."));
  builder.sentence (gettext("But the world is evil, so we need a license where we can protect the integrity of the text."));
  view.set_variable ("paragraph15", builder.result());

  builder.clear();
  builder.sentence (gettext("The proponent of the free Bible says:"));
  builder.sentence (gettext("A free license enables spreading the Word of God more quickly."));
  builder.sentence (gettext("The Bible Society has made such a reliable and good Bible."));
  builder.sentence (gettext("Is it not worthy of being spread as far and wide as possible?"));
  builder.sentence (gettext("A restrictive license confines the Word to one publisher, one channel."));
  builder.sentence (gettext("A free license opens up the Word to being spread by more channels."));
  view.set_variable ("paragraph16", builder.result());

  builder.clear();
  builder.sentence (gettext("If the license is restrictive, it is not the bad people who are afraid of illegally copying the Word of God, making changes, then publish it."));
  builder.sentence (gettext("But it's the good people who would be worried to violate copyright law when spreading the Word, thinking that God's blessing cannot be on something illegal."));
  view.set_variable ("paragraph17", builder.result());

  builder.clear();
  builder.sentence (gettext("The free license does not aid bad people much, but it assists good people in many ways."));
  builder.sentence (gettext("A restrictive license does not stop heretics in any way, but it stops good people from spreading the Word."));
  view.set_variable ("paragraph18", builder.result());

  builder.clear();
  builder.sentence (gettext("The restrictive Bible Society says:"));
  builder.sentence (gettext("If you revise our Bible, we'll see you in court."));
  view.set_variable ("paragraph19", builder.result());

  builder.clear();
  builder.sentence (gettext("The free and open Bible Society says:"));
  builder.sentence (gettext("If you revise and spread our Bible, we thank God for opening another channel for distribution."));
  view.set_variable ("paragraph20", builder.result());

  builder.clear();
  builder.sentence (gettext("God wanted property rights to be there for the benefit of humans."));
  builder.sentence (gettext("But did he also want copyright on Bibles?"));
  builder.sentence (gettext("Show from the Word where this is the case."));
  view.set_variable ("paragraph21", builder.result());

  page += view.render ("help", "freebible");

  page += Assets_Page::footer ();

  return page;
}
