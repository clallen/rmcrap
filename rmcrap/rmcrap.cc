/*{{{				|-- rmcrap.cc --|
    See below in the Usage section for a description of what this thing does.
    This program is not guaranteed to work.  It might even destroy your files
    permanently.  I'm not responsible for that, but I'd probably buy you a
    beer in sympathy.
    Copyright (C) 2010 Clint Allen <clint@olympus.ws>
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
}}}*/

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <limits.h>

//{{{	GLOBALS
const float VERSION = 1.16;
const char COPYRIGHT[] = "Copyright (C) 2010";
const char EMAIL_ADDRESS[] = "Clint Allen <clint@olympus.ws>";
char _allowed_chars[] = ".-+";
int _verbose = 0;
int _prompt = 0;
int _renamed = 0;
int _errors = 0;
int _stop_on_error = 0;
/*}}}*/

void parseFile (const char* filename)
{/*{{{*/
    char oldname[PATH_MAX];
    char newname[PATH_MAX];
    strcpy(oldname, filename);
    strcpy(newname, filename);
    
    // Parse "newname" so "oldname" can be renamed later
    if (_verbose) printf("Scanning filename %s...\n", newname);
    for (int index = 0; newname[index] != '\0'; index++)
    {
	// Remove dashes "-" at the beginning
	if (index == 0 && newname[index] == '-')
	{
	    newname[index] = '_';
	    continue;
	}
	// If it's an ampersand, replace it with "_and_"
	if (newname[index] == '&')
	{
	    // Copy the tail of the string to a buffer (gotta be a better way to do this)
	    char* buffer = (char*)calloc(PATH_MAX, sizeof(char));
	    int bufindex = 0;
	    int newname_len = strlen(newname);
	    for (int strpos = index+1; strpos < newname_len; strpos++)
	    {
		buffer[bufindex] = newname[strpos];
		bufindex++;
	    }
	    // Cap it off, stick an "_and_" on it, then slap the tail back on
	    newname[index] = '\0';
	    strcat(newname, "_and_");
	    strcat(newname, buffer);
	    free(buffer);
	}
	// If the character is not alphanumeric and is not in the allowed list, squash it
	if (!isalnum(newname[index]) && !strchr(_allowed_chars, newname[index]))
	{
	    newname[index] = '_';
	}
    }
    
    // First see if the names are identical.  If they are, skip the rename
    if (strcmp(oldname, newname))
    {
	if (_prompt)
    	{
	    int answer = 'n';
	    puts("Confirm rename:");
	    printf("%s\n", oldname);
	    puts("to");
	    printf("%s\n(y/n)? ", newname);
	    answer = getchar();
	    getchar();
	    if (answer == 'n' || answer == 'N')
		return;
	}
        if (_verbose) printf("renaming to  -->  %s\n", newname);
	// Rename the file
	if (rename(oldname, newname) == -1)
    	{
	    _errors++;
	    if (_verbose) puts("In function parseFile:");
	    printf("Couldn't rename file \"%s\": ", oldname);
	    puts(strerror(errno));
    	    return;
	}
	_renamed++;
    }

    return;
}/*}}}*/
    
int processDir(char* dirname, int recursive)
{/*{{{*/
    dirent* direntry;
    struct stat filestat;
    
    DIR* dirstream = opendir(dirname);
    if (dirstream == NULL)
    {
	if (_verbose) puts("In function processDir:");
        printf("Couldn't open directory \"%s\": ", dirname);
        puts(strerror(errno));
        puts("Check permissions, etc. and try again.");
        return(1);
    }
    if (chdir(dirname) == -1)
    {
	if (_verbose) puts("In function processDir:");
	printf("Unable to change to directory \"%s\": ", dirname);
	puts(strerror(errno));
	return(1);
    }
    for (direntry = readdir(dirstream); direntry != NULL; direntry = readdir(dirstream))
    {
	// Ignore the self and parent dirs
	if (!strcmp(direntry->d_name, ".") || !strcmp(direntry->d_name, ".."))
	    continue;

        if (lstat(direntry->d_name, &filestat) == -1)
        {
	    _errors++;
	    if (_verbose) puts("In function processDir:");
	    printf("Couldn't read \"%s\": ", direntry->d_name);
	    puts(strerror(errno));
	    if (_stop_on_error)
	    {
	    	return(1);
	    }
	    else
	    {
	    	continue;
	    }
	}

	if (recursive)
	{
	    if (S_ISDIR(filestat.st_mode))
	    {
		processDir(direntry->d_name, 1);
		if (chdir("..") == -1)
		{
		    _errors++;
	            if (_verbose) puts("In function processDir:");
		    puts("Unable to change to parent directory: ");
		    puts(strerror(errno));
		    if (_stop_on_error)
		    {
			return(1);
		    }
		    else
		    {
			continue;
		    }
		}
	    }
	}

	parseFile(direntry->d_name);
    }
    if (closedir(dirstream) == -1)
    {
	if (_verbose) puts("In function processDir:");
        puts("Error closing directory stream: ");
        puts(strerror(errno));
        return(1);
    }
    
    return(0);
}/*}}}*/

void usage()
{/*{{{*/
	puts("\nUsage: rmcrap [-a ALLOWED-CHARS] [OPTION]... DIRECTORY");
	puts("Gets rid of irritating crap in filenames that causes problems");
	puts("with many programs and shells.  Replaces all non-alphanumeric");
	puts("characters (except dot '.' minus '-' and plus '+') with underscores.");
	puts("Ampersands (&) are changed to \"_and_\".");
	puts("Operates on all files in the given DIRECTORY.");
	puts("	-a		specify a list of additional allowed characters");
	puts("			(default: ALNUM . - +)");
	puts("	-e		stop on error");
	puts("	-R		recursive; process all files in DIRECTORY and below");
	puts("	-v		verbose; explain what is being done");
	puts("	-p		prompt before renaming files");
	puts("	-h or -?	display this help and return");
	puts("	-V		output version information and return\n");
	puts("Please send bug reports, suggestions, and good beer to");
	printf("%s\n", EMAIL_ADDRESS);
}/*}}}*/

void version()
{/*{{{*/
	printf("\nrmcrap version %1.2f\n", VERSION);
	printf("%s %s\n", COPYRIGHT, EMAIL_ADDRESS);
	puts("This program comes with ABSOLUTELY NO WARRANTY, and is free software.  You");
	puts("are welcome to redistribute it under certain conditions.  Check out the GNU");
	puts("General Public License (http://www.gnu.org/copyleft/gpl.txt) for details.");
}/*}}}*/

int main (int argc, char* argv[])
{/*{{{*/
    char lastarg[PATH_MAX];
    struct stat lastargStat;
    int recursive = 0;

    // If no args are given, print a helpful error and the usage text
    if (argc < 2)
    {
	    if (_verbose) puts("In function main:");
	    puts("Please specify a directory.");
	    usage();
	    return(1);
    }
    // Parse cmdline args
    int opt = 0;
    while ((opt = getopt(argc, argv, "a:eRvph?V")) != -1)
    {
	switch (opt)
	{
	    case 'a':
		strcat(_allowed_chars, optarg);
		break;
	    case 'e':
		_stop_on_error = 1;
		break;
	    case 'R':
		recursive = 1;
		break;
	    case 'v':
		_verbose = 1;
		break;
	    case 'p':
		_prompt = 1;
		break;
	    case 'h':
	    case '?':
		usage();
		return(0);
		break;
	    case 'V':
		version();
		return(1);
		break;
	    default:
		usage();
		return(0);
	}
    }
    // If there's a final argument, assume it's the target directory
    if (argv[optind] != NULL)
    {
	strcpy(lastarg, argv[optind]);
	if (lstat(lastarg, &lastargStat) == -1)
	{
	    if (_verbose) puts("In function main:");
	    printf("Couldn't read \"%s\": ", lastarg);
	    puts(strerror(errno));
	    return(1);
	}
    }
    
    // Check that it is alphanumeric and is a directory
    if (isalnum(lastarg[0]) && !S_ISDIR(lastargStat.st_mode))
    {
	if (_verbose) puts("In function main:");
        printf("\"%s\" is not a directory.\n", lastarg);
	usage();
        return(1);
    }
    
    // Get the absolute path to the target directory
    if (chdir(lastarg) == -1)
    {
	if (_verbose) puts("In function main:");
	printf("Unable to change to directory \"%s\": ", lastarg);
	puts(strerror(errno));
	return(1);
    }
    char* abs_path = getcwd(NULL, 0);
    if (abs_path == NULL)
    {
	if (_verbose) puts("In function main:");
	printf("Unable to read target directory \"%s\": ", lastarg);
	puts(strerror(errno));
	return(1);
    }
    
    // Start the main process loop
    int result = 0;
    if (recursive)
    {
	result = processDir(abs_path, 1);
    }
    else
    {
	result = processDir(abs_path, 0);
    }
    if (result != 0)
    {
    	return(result);
    }
    
    printf("%i error(s).\n", _errors);
    printf("%i file(s) renamed.\n", _renamed);
    
    return(0);
}/*}}}*/

/*
 vim: set foldmethod=marker
*/
