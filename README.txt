****************************
GeSHi Filter (Drupal Module)
****************************


DESCRIPTION
-----------
The GeShi Filter is a Drupal module for syntax highlighting of pieces of
source code. It implements a filter that formats and highlights the syntax of
source code between for example <code>...</code>.


DEPENDENCIES
------------
This module requires a third-party library: GeShi (Generic Syntax Highlighter),
which can be found at http://qbnz.com/highlighter .
See installation procedure for more information.


INSTALLATION
------------
1. Extract the GeSHi Filter module tarball and place the entire geshifilter
  directory into your Drupal setup (e.g. in sites/all/modules).

2. Download the latest supported stable release GeSHi latest release from
  http://qbnz.com/highlighter/
  Although the filter works with the version 1.0.x and the developement version
  1.1.x of GeSHi, it is recommended to use the version 1.0.x at the moment.
  Place the entire extracted 'geshi' engine folder (where geshi.php resides)
  in the geshifilter directory (e.g. as /sites/all/modules/geshifilter/geshi)

3. Enable this module as any other Drupal module by navigating to
    administer > site building > modules


CONFIGURATION
-------------
1. The general GeSHi Filter settings can be found by navigating to:
    administer > site configuration > geshifilter

2. Enable the GeShi filter in one of your input formats in:
     administer > site configuration > input formats

   This filter should play well with other filters, but
   it might need to be placed after the HTML filter ("rearrange" tab of
   your input format).

   You need to configure specific format settings (default language,
    languages allowed...) in the "configure" tab of your input format).

   If HTML filter is enable, don't forget to add <blockcode> tag in
   "Allowed HTML tags" textfield of HTML filter settings.




SETTINGS
--------

	Code Container:
		-Use <pre> container
		-Use <div> container
		-No container

The Code Container has a fundamental effect on the layout of your code
before you even begin to style. What is the Code Container? It's the bit
of markup that goes around your code to contain it. By default your code
is surrounded by a <pre>, but you can also specify a <div>.

The <pre> header is the default. If you're familiar with HTML you'll
know that whitespace is rendered "as is" by a <pre> element. The
advantage for you is that if you use <pre> the whitespace you use will
appear pretty much exactly how it is in the source, and what's more
GeSHi won't have to add a whole lot of <br />'s and non-breaking spaces
(&nbsp;) to your code to indent it. This saves you source code (and your
valuable visitors waiting time and your bandwidth).

But if you don't like <pre> or it looks stupid in your browser no matter
what styles you try to apply to it or something similar, you might want
to use a <div> instead. A <div> will result in more source - GeSHi will
have to insert whitespace markup - but in return you can wrap long lines
of code that would otherwise have your browser's horizontal scrollbar
appear. Of course with <div> you can *not* wrap lines if you please.

The third choice specifies that the code should not be wrapped in
anything at all.

	CSS classes:
		-Use in-line styles
		-Use CSS classes

Using CSS to highlight your code instead of in-lining the styles is a
definate bonus. Not only is it more compliant (the w3c is deprecating
the style attribute in XHTML 2.0) but it results in far less outputted
code - up to a whopping 50% saving - which makes a *huge* difference to
those of us on modems!

GeSHiFilter load geshi.css (in main directory) as default stylesheet but
you can style anything you want in your own theme CSS file.

	Line Numbers:
		-Disable line numbers
		-Use normal line numbering
		-Use fancy line numbering

GeSHi has the ability to add line numbers to your code. Line numbers are
a great way to make your code look professional, especially if you use
the fancy line numbers feature.

Normal line numbers means you specify a style for them, and that style
gets applied to all of them. Fancy line numbers means that you can
specify a different style for each nth line number.

	Fancy number each:
		number

Display a fancy number every n line numbers.

	Allow start param
		true|false

Allow users to make the line numbers start at any number using
start=number param.

If you have not enabled line numbers, this will have no effect.

This feature will break compliancy (however transitional compliancy
remains). This is because the only widely supported way to change the
start value for line numbers is by using the start="number" attribute of
the <ol> tag.

*In-line settings*

Customize here some in-line styles.
If you have not enabled in-line styles, this will have no effect.

USAGE
---
To post pieces of highlighted code, surround them with
<blockcode [type="language"] [start="startLineNumber"]>...</blockcode>.

E.g. highlight actionscript code:

<blockcode type="actionscript">
...
</blockcode>

if inline code is allowed in input format settings, you may also post
highlighted inline code surrounding it with <code [type="language"]>...
</code> tags.

E.g. highlight actionscript inline code:

<code type="actionscript">...</code>

If you don't use type="language" param or specify an unsupported
language, the code will be highlighted according to the default language
set in the input format or will not be highlighted if
"-DO NOT HIGHLIGHT-" is set as default language.

LINE NUMBERS:
When line numbers are enabled, you can specify the start line number
using start="startLineNumber" param.

E.g. highlight actionscript code and start line number at 5:

<code type="actionscript" start="5">
...
</code>




AUTHORS
-------

Original module by:
  Vincent Filby <vfilby at gmail dot com>

Drupal.org hosted version for Drupal 4.7:
  Vincent Filby <vfilby at gmail dot com>
  Michael Hutchinson (http://compsoc.dur.ac.uk/~mjh/contact)
  Damien Pitard <dpdev00 at gmail dot com>

Port to Drupal 5:
  rötzi (http://drupal.org/user/73064)
  Stefaan Lippens (http://drupal.org/user/41478)


GESHI - Generic Syntax Highlighter
----------------------------------
http://qbnz.com/highlighter
written Nigel McNie <nigel at geshi dot org>

A big part of this README is quoted from the GeSHi documentation written
by Nigel McNie.
