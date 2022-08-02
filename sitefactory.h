/*
This file is NOT needed for siteFactory!
It's only for a a general overview.
DO NOT LINK OR INCLUDE THIS FILE TO siteFactory!!
*/

// urlDecode: decodes a url-encoded string
char *urlDecode(char *input);

// strToLower: makes a string lowercase
char *strToLower(char *string);

// getExt: get the file extension of a filename
char *getExt(char *filename);

// getMime: get the mime type for a filename; for example: example.txt => text/plain
char *getMime(char *filename);

// rmmp: removes all multiple occurences of ´ml´ with one in ´str´
char* rmmp(char* str, char ml);

// prepend: prepends ´pre´ to ´str´
void prepend(char* str, const char* pre);

// hasExt: ´extensions´ is a pipe seperated list of file-extensions (like "txt|png|gif");
//    function returns 0 if extension of ´string´ is in ´extensions´, otherwise 0
short hasExt(char* extensions, char* string);

// strReplace: replaces ´needle´ with ´replacement´ in ´target´
void strReplace(const char *needle, const char *replacement, char *target);

// processQuery: works on request with id ´slot´
void processQuery(int slot);

// bindSocket: binds the siteFactory instance on the port ´port´
void bindSocket(char *port);

// main: the main program
void main(int argc, char *argv[]);
