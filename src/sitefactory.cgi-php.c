/*
siteFactory HTTP server v5.7 <https://github.com/lukastautz/siteFactory>
Copyright (C) 2022 Lukas Tautz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

You can use the siteFactory HTTP server for free in your projects, you can also modify it BUT YOU ARE NOT ALLOWED TO DELETE THIS COMMENT!
*/
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <arpa/inet.h>
#define VERSION                   "siteFactory HTTP server v5.7 compiled with PHP CGI support"
#define VERSION_NUMBER            "5.7"
#define COPYRIGHT                 "Copyright (C) 2022 Lukas Tautz\nLicensed under the GNU General Public License, check out sitefactory --license"
#define SIMULTANEOUS_CONNECTIONS  10000                // Sets the number of the maximal simultaneous connections
//#define ONLY_IPV6
/* START CGI CONFIG */
#define SUPPORT_CGI                                    // Enables CGI support
#define CGI_EXT                   "php"                // Sets the extension of the recognized CGI files
#define CGI_BINARY                "/bin/php"           // Sets the CGI interpreter
#define CGI_NAME                  "php"                // Sets the name of the CGI interpreter (mostly the name of the binary file)
/*
 Important note:
  - Add "$_SERVER['REQUEST_URI']=$argv[1];parse_str($argv[2],$_GET);" to each public PHP file! (without that, you can't use the $_GET array and the reuqest uri)
  - You have to output first "HTTP/1.1 200 OK"
  - Don't forget to send the Content-Type!
*/
/* END CGI CONFIG */
char *buf, *uri, *query;
int listenfd, *clients;
struct {
  char *deny, *root, *index, error_page[128];
  unsigned short port;
} config;
static char *urlDecode(char *input) {
  int check = 0;
  char hexCode[2];
  while (!check) {
    check = 1;
    int count;
    for (count = 0; count < strlen(input); ++count) {
      if (input[count] == '%') {
        if (input[count + 1] == 0) {
          return input;
        }
        if (isxdigit(input[count + 1]) && isxdigit(input[count + 2])) {
          check = 0;
          hexCode[0] = input[count + 1];
          hexCode[1] = input[count + 2];
          long int decimal = strtol(hexCode, NULL, 16);
          memmove(&input[count + 1], &input[count + 3], strlen(&input[count + 3]) + 1);
          input[count] = decimal;
        }
      }
    }
  }
  return input;
}
static char *strToLower(char *string) {
  int len = strlen(string);
  for (int i = 0; i < len; i++) {
    if (string[i] >= 65 && string[i] <= 90) {
      string[i] += 32;
    }
  }
  return string;
}
static char *getExt(char *filename) {
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
      return filename;
    }
    return dot + 1;
}
static char *getMime(char *filename) {
    char *mime;
    char *ext = strToLower(getExt(filename));
    if (strcmp(ext, "html") == 0 || strcmp(ext, "htm") == 0 || strcmp(ext, "php") == 0) {
      mime = "text/html; charset=UTF-8";
    } else if (strcmp(ext, "css") == 0) {
      mime = "text/css; charset=UTF-8";
    } else if (strcmp(ext, "js") == 0) {
      mime = "text/javascript; charset=UTF-8";
    } else if (strcmp(ext, "json") == 0) {
      mime = "application/json; charset=UTF-8";
    } else if (strcmp(ext, "zip") == 0) {
      mime = "application/zip";
    } else if (strcmp(ext, "jfif") == 0 || strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0 || strcmp(ext, "jpe") == 0) {
      mime = "image/jpeg";
    } else if (strcmp(ext, "png") == 0) {
      mime = "image/png";
    } else if (strcmp(ext, "bmp") == 0) {
      mime = "image/bmp";
    } else if (strcmp(ext, "ico") == 0 || strcmp(ext, "icon") == 0) {
      mime = "image/x-icon";
    } else if (strcmp(ext, "txt") == 0 || strcmp(ext, "text") == 0 || strcmp(ext, "conf") == 0 || strcmp(ext, "asc") == 0 || strcmp(ext, "sh") == 0 || strcmp(ext, "c") == 0 || strcmp(ext, "cpp") == 0 || strcmp(ext, "h") == 0 || strcmp(ext, "c++") == 0 || strcmp(ext, "cs") == 0) {
      mime = "text/plain; charset=UTF-8";
    } else if (strcmp(ext, "pdf") == 0) {
      mime = "application/pdf";
    } else if (strcmp(ext, "gif") == 0) {
      mime = "image/gif";
    } else if (strcmp(ext, "mp4") == 0) {
      mime = "video/mp4";
    } else if (strcmp(ext, "mp3") == 0 || strcmp(ext, "mpga") == 0 || strcmp(ext, "mp2") == 0) {
      mime = "audio/mpeg";
    } else if (strcmp(ext, "mpe") == 0 || strcmp(ext, "mpeg") == 0 || strcmp(ext, "mpg") == 0) {
      mime = "video/mpeg";
    } else if (strcmp(ext, "avi") == 0) {
      mime = "video/x-msvideo";
    } else if (strcmp(ext, "doc") == 0 || strcmp(ext, "docx") == 0) {
      mime = "application/msword";
    } else if (strcmp(ext, "dtd") == 0 || strcmp(ext, "xml") == 0 || strcmp(ext, "xsl") == 0) {
      mime = "text/xml; charset=UTF-8";
    } else if (strcmp(ext, "m3u") == 0) {
      mime = "audio/x-mpegurl";
    } else if (strcmp(ext, "mathml") == 0) {
      mime = "application/mathml+xml";
    } else if (strcmp(ext, "mid") == 0 || strcmp(ext, "midi") == 0) {
      mime = "audio/midi";
    } else if (strcmp(ext, "mov") == 0) {
      mime = "video/quicktime";
    } else if (strcmp(ext, "ogg") == 0) {
      mime = "video/ogg";
    } else if (strcmp(ext, "qt") == 0) {
      mime = "video/quicktime";
    } else if (strcmp(ext, "rss") == 0) {
      mime = "application/rss+xml";
    } else if (strcmp(ext, "rtf") == 0) {
      mime = "text/rtf; charset=UTF-8";
    } else if (strcmp(ext, "rtx") == 0) {
      mime = "text/richtext; charset=UTF-8";
    } else if (strcmp(ext, "sgm") == 0 || strcmp(ext, "sgml") == 0) {
      mime = "text/sgml; charset=UTF-8";
    } else if (strcmp(ext, "svg") == 0 || strcmp(ext, "svgz") == 0) {
      mime = "image/svg+xml; charset=UTF-8";
    } else if (strcmp(ext, "tif") == 0 || strcmp(ext, "tiff") == 0) {
      mime = "image/tiff";
    } else if (strcmp(ext, "wav") == 0) {
      mime = "audio/wav";
    } else if (strcmp(ext, "xht") == 0 || strcmp(ext, "xhtml") == 0) {
      mime = "application/xhtml+xml; charset=UTF-8";
    } else if (strcmp(ext, "webm") == 0) {
      mime = "video/webm";
    } else {
      mime = "application/octet-stream";
    }
    return mime;
}
static char* rmmp(char* str, char ml) {
    if (strlen(str) < 2) {
      return str;
    }
    char *from, *to;
    int spc = 0;
    to = from = str;
    for (;;) {
        if (spc && *from == ml && to[-1] == ml) {
          ++from;
        } else {
            spc = (*from == ml) ? 1 : 0;
            *to++ = *from++;
            if (!to[-1]) {
              break;
            }
        }
    }
    return str;
}
static short hasExt(char* extensions, char* string) {
  string = strToLower(getExt(string));
  extensions = strtok(extensions, ",");
  while (extensions != NULL) {
    if (strcmp(extensions, string) == 0) {
      return 0;
    }
    extensions = strtok(NULL, ",");
  }
  return 1;
}
static void strReplace(const char *needle, const char *replacement, char *target) {
    if (strlen(target) < 2) {
      return;
    }
    char buffer[strlen(target)];
    char *insertPoint = &buffer[0];
    const char *tmp = target;
    int needleLen = strlen(needle);
    int replaceLen = strlen(replacement);
    for (;;) {
        const char *p = strstr(tmp, needle);
        if (p == NULL) {
            strcpy(insertPoint, tmp);
            break;
        }
        memcpy(insertPoint, tmp, p - tmp);
        insertPoint += p - tmp;
        memcpy(insertPoint, replacement, replaceLen);
        insertPoint += replaceLen;
        tmp = p + needleLen;
    }
    strcpy(target, buffer);
}
static void notFound(unsigned int slot, char *uri, char *query) {
  if (strlen(config.error_page) != 0) {
    struct stat notFoundBuffer;
    if (stat(config.error_page, &notFoundBuffer) == 0 && (S_ISREG(notFoundBuffer.st_mode) || S_ISLNK(notFoundBuffer.st_mode))) {
#ifdef SUPPORT_CGI
      if (strcmp(strToLower(getExt(config.error_page)), CGI_EXT) == 0) {
        dup2(clients[slot], STDOUT_FILENO);
        close(clients[slot]);
        char *arg[5] = {CGI_NAME, config.error_page, uri, query, NULL};
        execv(CGI_BINARY, arg);
        fflush(stdout);
        shutdown(STDOUT_FILENO, SHUT_WR);
      } else {
#endif
        char statSize[256];
        snprintf(statSize, 256, "%lu", notFoundBuffer.st_size);
        char *mime = getMime(config.error_page);
        short length = 56 + strlen(mime) + strlen(statSize);
        char header[length];
        header[0] = '\0';
        strcat(header, "HTTP/1.1 404 Not found\nContent-Type: ");
        strcat(header, mime);
        strcat(header, "\nContent-Length: ");
        strcat(header, statSize);
        strcat(header, "\n\n");
        write(clients[slot], header, strlen(header));
        int fileInt = open(config.error_page, O_RDONLY);
        for (long pos = notFoundBuffer.st_size; pos > 0; pos = pos - 268435456) {
          sendfile(clients[slot], fileInt, 0, 268435456);
        }
        close(fileInt);
#ifdef SUPPORT_CGI
      }
#endif
    } else {
      write(clients[slot], "HTTP/1.1 500 Internal Server Error\nContent-Type: text/html; charset=UTF-8\n\n<title>Internal Server Error</title>Internal Server Error.", 133);
    }
  } else {
    write(clients[slot], "HTTP/1.1 404 Not found\nContent-Type: text/html; charset=UTF-8\n\n<title>404 - Not found</title>Not found.", 103);
  }
}
static void processQuery(unsigned int slot) {
  unsigned short rcvd;
  char path[128];
  buf = alloca(10240);
  rcvd = read(clients[slot], buf, 10240);
  if (rcvd > 0) {
    buf[rcvd] = '\0';
    strtok(buf, " \t\n\r");
    uri = urlDecode(strtok(NULL, " \t"));
#ifdef SUPPORT_CGI
    query = strchr(uri, '?');
    if (query) *query++ = '\0';
    else query = "";
#else
    query = strchr(uri, '?');
    if (query) *query++ = '\0';
#endif
    if (uri[strlen(uri) - 1] == '/') {
        uri[strlen(uri) - 1] = '\0';
    }
    if (strlen(uri) == 0) {
      uri = "/";
    }
    strReplace("..", "", uri);
    uri = rmmp(uri, '/');
    strcat(path, config.root);
    strcat(path, uri);
    struct stat statBuffer;
    short exists = stat(path, &statBuffer);
    if (exists == 0 && S_ISDIR(statBuffer.st_mode)) {
      char *tmp = alloca(128);
      strcat(path, "/");
      char *index = strtok(config.index, ",");
      short indexExists = 1;
      while (index) {
        memcpy(tmp, path, 128);
        strcat(tmp, config.index);
        tmp = rmmp(tmp, '/');
        indexExists = stat(tmp, &statBuffer);
        if (indexExists == 0) {
          memcpy(path, tmp, 128);
          break;
        }
        index = strtok(NULL, "|");
      }
      if (indexExists == 0 && hasExt(config.deny, path) == 1) {
#ifdef SUPPORT_CGI
        if (strcmp(strToLower(getExt(path)), CGI_EXT) == 0) {
          dup2(clients[slot], STDOUT_FILENO);
          close(clients[slot]);
          char *arg[5] = {CGI_NAME, path, uri, query, NULL};
          execv(CGI_BINARY, arg);
          fflush(stdout);
          shutdown(STDOUT_FILENO, SHUT_WR);
        } else {
#endif
          char statSize[256];
          snprintf(statSize, 256, "%lu", statBuffer.st_size);
          char *mime = getMime(path);
          short length = 49 + strlen(mime) + strlen(statSize);
          char header[length];
          header[0] = '\0';
          strcat(header, "HTTP/1.1 200 OK\nContent-Type: ");
          strcat(header, mime);
          strcat(header, "\nContent-Length: ");
          strcat(header, statSize);
          strcat(header, "\n\n");
          write(clients[slot], header, strlen(header));
          int fileInt = open(path, O_RDONLY);
          for (long pos = statBuffer.st_size; pos > 0; pos = pos - 268435456) {
            sendfile(clients[slot], fileInt, 0, 268435456);
          }
          close(fileInt);
#ifdef SUPPORT_CGI
        }
#endif
      } else {
        notFound(slot, uri, query);
      }
    } else if (exists == 0 && hasExt(config.deny, path) == 1) {
#ifdef SUPPORT_CGI
      if (strcmp(strToLower(getExt(path)), CGI_EXT) == 0) {
          dup2(clients[slot], STDOUT_FILENO);
          close(clients[slot]);
          char *arg[5] = {CGI_NAME, path, uri, query, NULL};
          execv(CGI_BINARY, arg);
          fflush(stdout);
          shutdown(STDOUT_FILENO, SHUT_WR);
      } else {
#endif
          char statSize[256];
          snprintf(statSize, 256, "%lu", statBuffer.st_size);
          char *mime = getMime(path);
          short length = 49 + strlen(mime) + strlen(statSize);
          char header[length];
          header[0] = '\0';
          strcat(header, "HTTP/1.1 200 OK\nContent-Type: ");
          strcat(header, mime);
          strcat(header, "\nContent-Length: ");
          strcat(header, statSize);
          strcat(header, "\n\n");
          write(clients[slot], header, strlen(header));
          int fileInt = open(path, O_RDONLY);
          for (long pos = statBuffer.st_size; pos > 0; pos = pos - 268435456) {
            sendfile(clients[slot], fileInt, 0, 268435456);
          }
          close(fileInt);
#ifdef SUPPORT_CGI
      }
#endif
    } else {
      notFound(slot, uri, query);
    }
    close(clients[slot]);
  }
}
void main(unsigned short argc, char *argv[]) {
  // main program
  if (argc == 1 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
    // Display help
    printf("\033[93msitefactory\033[0m [OPTIONS]\nStarts the siteFactory HTTP server\nOptions:\n  -r     --root         \033[91m[MANDATORY]\033[0m Sets the root directory\n  -i     --index        Sets the index pages, seperated with ',' (default: index.html)\n  -d     --deny         Sets the denied extensions, seperated with ',' (for example 'txt,html')\n  -e     --error-page   Sets the 404 error page\n  -p     --port         Sets the port (default: 80)\n  -h     --help         Prints this page\n  -l     --license      Prints the license terms\n");
#ifdef ONLY_IPV6
    printf("\n\033[92mIPv4\033[0m is disabled!\n");
#else
    printf("\n\033[92mIPv4\033[0m is enabled!\n");
#endif
#ifdef SUPPORT_CGI
    printf("\n\033[92mCGI\033[0m is enabled! (*."CGI_EXT" -> "CGI_BINARY")\n");
#else
    printf("\n\033[92mCGI\033[0m is disabled!\n");
#endif
    printf("\n"VERSION"\n"COPYRIGHT"\n");
    exit(0);
  } else if (strcmp(argv[1], "--license") == 0 || strcmp(argv[1], "-l") == 0) {
    // Display license
    printf("siteFactory HTTP server v"VERSION_NUMBER" <\033[93mhttps://github.com/lukastautz/siteFactory\033[0m>\nCopyright (C) 2022 Lukas Tautz\n\nThis program is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program.  If not, see <\033[93mhttps://www.gnu.org/licenses/\033[0m>.\n");
    exit(0);
  }
  // Temp variable for the error_page
  char _tmp[128];
  _tmp[0] = 0;
  for (unsigned short z = 1; z < argc; z++) {
    // Check the command line switches...
    if (argv[z][0] == '-') {
      if (argv[z][1] == '-') {
        z++;
        switch (argv[z - 1][2]) {
          case 'r': config.root = argv[z]; break;
          case 'i': config.index = argv[z]; break;
          case 'd': config.deny = argv[z]; break;
          case 'e': strcat(_tmp, argv[z]); break;
          case 'p': config.port = atoi(argv[z]);
        }
      } else {
        z++;
        switch (argv[z - 1][1]) {
          case 'r': config.root = argv[z]; break;
          case 'i': config.index = argv[z]; break;
          case 'd': config.deny = argv[z]; break;
          case 'e': strcat(_tmp, argv[z]); break;
          case 'p': config.port = atoi(argv[z]);
        }
      }
    }
  }
  if (config.root == 0) {
    // Root directory missing
    printf("No root directory specified!\n");
    exit(1);
  }
  if (config.index == 0) config.index = "index.html";
  if (config.deny == 0) config.deny = "";
  if (config.port == 0) config.port = 80;
  if (_tmp[0] == '/') {
    // Error page is absolute
    strcat(config.error_page, _tmp);
  } else if (_tmp[0] != 0) {
    // Error page is relative
    strcat(config.error_page, config.root);
    strcat(config.error_page, "/");
    strcat(config.error_page, _tmp);
  }
#ifdef ONLY_IPV6
  // IPv6 socket
  listenfd = socket(AF_INET6, SOCK_STREAM, 0);
  struct sockaddr_in addr_s;
  addr_s.sin_family = AF_INET6;
  addr_s.sin_port = htons(config.port);
  addr_s.sin_addr.s_addr = inet_addr("::1");
  bind(listenfd, (struct sockaddr*)&addr_s, sizeof(addr_s));
#else
  // IPv4 socket
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(config.port);
  addr.sin_addr.s_addr = inet_addr("0.0.0.0");
  bind(listenfd, (struct sockaddr*)&addr, sizeof(addr));
#endif
  // Listen to maximal 100000 connection in queue
  listen(listenfd, 100000);
  struct sockaddr_in clientaddr;
  socklen_t addrlen;
  unsigned int slot = 0;
  clients = mmap(NULL, sizeof(*clients)*SIMULTANEOUS_CONNECTIONS, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
  for (int i = 0; i < SIMULTANEOUS_CONNECTIONS; i++) {
    // Empty the client
    clients[i] = -1;
  }
  signal(SIGCHLD, SIG_IGN);
  for (;;) {
    // Endless loop for connections
    addrlen = sizeof(clientaddr);
    // Accept connection
    clients[slot] = accept(listenfd, (struct sockaddr*)&clientaddr, &addrlen);
    if (clients[slot] >= 0) {
      if (fork() == 0) {
        // fork() succeded, we're the child
        close(listenfd);
        // Respond to the client
        processQuery(slot);
        // Close the connection
        close(clients[slot]);
        // Mark the slot as unused
        clients[slot] = -1;
        // Return to the main process
        exit(0);
      } else {
        // Fork failed, close connection
        close(clients[slot]);
      }
    }
    while (clients[slot] != -1) {
      // Look for unused slots
      slot = (slot + 1) % SIMULTANEOUS_CONNECTIONS;
    }
  }
}