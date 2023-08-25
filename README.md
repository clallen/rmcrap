README file for rmcrap v1.16, Copyright (C) 2010  Clint Allen

To make and install:
Open up the Makefile and change CFLAGS, DEBUG, etc. to your taste.
Run "make" then "make install" and you're done.
Do "rmcrap -h" for syntax and usage stuff.

I wrote this program because I downloaded a lot of MP3s from Napster (go Napster!),
got a bunch from friends, etc. and the majority of files are full of spaces,
apostrophes, and other such crap.  This causes no end of problems when trying to
work with them in batches and other ways.  So now I can just run
"rmcrap -R [DIRECTORY]" and have them named all nice-like.
Also you can specify the "-a" option followed by a list of characters that you don't want
changed.  Alphanumeric characters and the characters "-", "+", and "." will always be
left alone.  Ampersands (&) will be turned into "_and_".

Hope you get some use out of it.

--Clint


Version history:

08-07-10	1.19:	Moved "In function X" messages from regular to verbose output.
			Changed the '3' exit code to '0' because that was stupid.  
			Added a "stop on error" arg.
			Added error count in final output.
08-29-09	1.15:	Changed return values for usage(), version() and errors, to make the
			exit codes actually mean something.
			Made the DIRECTORY arg mandatory.
			Updated copyright and tidied up error messages.
04-22-03	1.10:	Wow, almost 3 years between releases, and I've been using it the whole
			time.  Must mean it's very stable. :^)
			In parseFile, I changed
				char buffer[PATH_MAX];
			to
				char *buffer = (char*)calloc(PATH_MAX, sizeof(char));
			which gives a nice clean buffer to work with, and prevents a nasty
			segfault under certain circumstances.  Also added the corresponding
			free(buffer) call.
			Included stdlib.h in the source and stdc++ in the Makefile so the
			program will compile under the new, improved, and picky GCC3 compiler.

10-11-00	0.99:	Added the feature where ampersand characters (&) are replaced with
			the actual word "and".  Pretty cool, huh?  The routine still needs
			some optimization, though.

08-05-00	0.98:	This version is so new and improved I had to bump it up two version
			numbers.  Not really any bug fixes, but I added the "-a" feature,
			which makes it a bit more customizeable (i.e. dangerous :-)

07-25-00	0.96:	Fixed a bug where I was a bit overzealous in the previous release
			(aka "Rename Everything That's Not Alphanumeric").  I forgot in my
			excitement, that periods (.) are not alphanumeric.  So they got
			renamed too.  Not so good when you're using file extensions.
			(like ".mp3" ;-)

07-18-00	0.95:	From the Why-didn't-I-think-of-this-before Dept.:
			Changed the character comparison method to a single
			isalnum() instead of all that other mess.  Simplicity
			is good.  Also updated the -h and -? output to reflect
			this.

07-15-00	0.9:	Initial release upon unsuspecting public.

