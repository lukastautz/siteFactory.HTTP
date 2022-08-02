/*
siteFactory <https://github.com/lukastautz/siteFactory>
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

You can use siteFactory for free in your projects, you can also modify the siteFactory files BUT YOU ARE NOT ALLOWED TO DELETE THIS COMMENT!
*/
#include <ctype.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <alloca.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#define SIMULTANEOUS_CONNECTIONS  10000
#define FILE_NOT_FOUND            struct stat notFoundBuffer; short errorPage = stat(hostList[hostId].error_page, &notFoundBuffer); if (errorPage == 0 && S_ISREG(statBuffer.st_mode)) {int fileInt = open(hostList[hostId].error_page, O_RDONLY); long pos; for (pos = notFoundBuffer.st_size; pos > 0; pos = pos - 268435456) { sendfile(clientfd, fileInt, 0, 268435456); } close(fileInt); char statSize[256]; snprintf(statSize, 256, "%lu", notFoundBuffer.st_size); short length = 80 + strlen(statSize); char header[length]; strcat(header, "HTTP/1.1 404 Not found\nContent-Type: text/html; charset=UTF-8\nContent-Length: "); strcat(header, statSize); strcat(header, "\n\n"); write(clientfd, header, strlen(header)); } else {write(clientfd, "HTTP/1.1 404 Not found\nContent-Type: text/html; charset=UTF-8\n\n<title>404 - Not found</title>Not found.", 103); }
struct {
  char name[128], deny[64], error_page[128], index[256], root_path[128];
  short is_empty;
} hostList[100];
char *buf, *uri, *host;
int listenfd;
int *clients;
char *urlDecode(char *input) {
  int check = 0;
  char hexCode[] = "00";
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
char *strToLower(char *string) {
  int i;
  int len = strlen(string);
  for (i = 0; i < len; i++) {
    if (string[i] >= 65 && string[i] <= 90) {
      string[i] += 32;
    }
  }
  return string;
}
char *getExt(char *filename) {
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
      return filename;
    }
    return dot + 1;
}
char *getMime(char *filename) {
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
    } else if (strcmp(ext, "tar") == 0) {
      mime = "application/x-tar";
    } else if (strcmp(ext, "tif") == 0 || strcmp(ext, "tiff") == 0) {
      mime = "image/tiff";
    } else if (strcmp(ext, "wav") == 0) {
      mime = "audio/wav";
    } else if (strcmp(ext, "wax") == 0) {
      mime = "audio/x-ms-wax";
    } else if (strcmp(ext, "wmv") == 0) {
      mime = "video/x-ms-wmv";
    } else if (strcmp(ext, "wmx") == 0) {
      mime = "video/x-ms-wmx";
    } else if (strcmp(ext, "wvx") == 0) {
      mime = "video/x-ms-wvx";
    } else if (strcmp(ext, "xht") == 0 || strcmp(ext, "xhtml") == 0) {
      mime = "application/xhtml+xml; charset=UTF-8";
    } else if (strcmp(ext, "xls") == 0) {
      mime = "application/vnd.ms-excel";
    } else if (strcmp(ext, "webm") == 0) {
      mime = "video/webm";
    } else {
      mime = "application/octet-stream";
    }
    return mime;
}
char* rmmp(char* str, char ml) {
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
void prepend(char* str, const char* pre) {
  short len = strlen(pre);
  memmove(str + len, str, strlen(str) + 1);
  memcpy(str, pre, len);
}
short hasExt(char* extensions, char* string) {
  string = strToLower(getExt(string));
  extensions = strtok(extensions, "|");
  while (extensions != NULL) {
    if (strcmp(extensions, string) == 0) {
      return 0;
    }
    extensions = strtok(NULL, "|");
  }
  return 1;
}
void strReplace(const char *needle, const char *replacement, char *target) {
    if (strlen(target) < 2) {
      return;
    }
    char *buffer = (char*)alloca(strlen(target));
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
void processQuery(int slot) {
  int rcvd;
  char path[256];
  buf = alloca(10240);
  rcvd = recv(clients[slot], buf, 10240, 0);
  if (rcvd > 0) {
    buf[rcvd] = '\0';
    strReplace("\r\n", "\n", buf);
    strtok(buf, " \t\n");
    uri = strtok(NULL, " \t");
    strtok(NULL, " \t\n");
    uri = urlDecode(uri);
    if (uri[strlen(uri) - 1] == '/') {
        uri[strlen(uri) - 1] = '\0';
    }
    strchr(uri, '?');
    if (strlen(uri) == 0) {
      uri = "/";
    }
    short headerC;
    for (;;) {
      char *key, *val;
      key = strtok(NULL, "\n: \t");
      if (!key) {
        break;
      }
      val = strtok(NULL, "\n");
      while (*val && *val == ' ') {
        val++;
      }
      headerC++;
      if (strcmp(strToLower(key), "host") == 0) {
        host = val;
        break;
      }
      if (headerC > 19) {
        host = "";
        break;
      }
    }
    int clientfd = clients[slot];
    short hostId;
    for (hostId = 0; hostId <= 100; hostId++) {
      if (strcmp(hostList[hostId].name, host) == 0) {
        break;
      }
    }
    if (hostId == 101) {
      write(clientfd, "HTTP/1.1 404 Not found\nContent-Type: text/html; charset=UTF-8\n\n<title>404 - Not found</title>Not found.", 103);
    } else {
      strReplace("..", "", uri);
      uri = rmmp(uri, '/');
      strcat(path, hostList[hostId].root_path);
      strcat(path, uri);
      struct stat statBuffer;
      short exists = stat(path, &statBuffer);
      if (exists == 0 && S_ISDIR(statBuffer.st_mode)) {
        char *tmp = alloca(128);
        strcat(path, "/");
        char *index = strtok(hostList[hostId].index, "|");
        short indexExists = 1;
        while (index) {
          memcpy(tmp, path, 128);
          strcat(tmp, index);
          tmp = rmmp(tmp, '/');
          indexExists = stat(tmp, &statBuffer);
          if (indexExists == 0) {
            memcpy(path, tmp, 128);
            break;
          }
          index = strtok(NULL, "|");
        }
        if (indexExists == 0 && hasExt(hostList[hostId].deny, path) == 1) {
          char statSize[256];
          snprintf(statSize, 256, "%lu", statBuffer.st_size);
          char *mime = getMime(path);
          short length = 49 + strlen(mime) + strlen(statSize);
          char header[length];
          strcat(header, "HTTP/1.1 200 OK\nContent-Type: ");
          strcat(header, mime);
          strcat(header, "\nContent-Length: ");
          strcat(header, statSize);
          strcat(header, "\n\n");
          write(clientfd, header, strlen(header));
          int fileInt = open(path, O_RDONLY);
          long pos;
          for (pos = statBuffer.st_size; pos > 0; pos = pos - 268435456) {
            sendfile(clientfd, fileInt, 0, 268435456);
          }
          close(fileInt);
        } else {
          FILE_NOT_FOUND;
        }
      } else if (exists == 0 && S_ISREG(statBuffer.st_mode) && hasExt(hostList[hostId].deny, path) == 1) {
        char statSize[256];
        snprintf(statSize, 256, "%lu", statBuffer.st_size);
        char *mime = getMime(path);
        short length = 49 + strlen(mime) + strlen(statSize);
        char header[length];
        strcat(header, "HTTP/1.1 200 OK\nContent-Type: ");
        strcat(header, mime);
        strcat(header, "\nContent-Length: ");
        strcat(header, statSize);
        strcat(header, "\n\n");
        write(clientfd, header, strlen(header));
        int fileInt = open(path, O_RDONLY);
        long pos;
        for (pos = statBuffer.st_size; pos > 0; pos = pos - 268435456) {
          sendfile(clientfd, fileInt, 0, 268435456);
        }
        close(fileInt);
      } else {
        FILE_NOT_FOUND;
      }
    }
    close(clientfd);
  }
}
void bindSocket(char *port) {
  struct addrinfo hints, *res, *p;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if (getaddrinfo(NULL, port, &hints, &res) != 0) {
    exit(1);
  }
  for (p = res; p != NULL; p = p->ai_next) {
    int option = 1;
    listenfd = socket(p->ai_family, p->ai_socktype, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    if (listenfd == -1) {
      continue;
    }
    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
      break;
    }
  }
  if (p == NULL) {
    exit(1);
  }
  freeaddrinfo(res);
  if (listen(listenfd, 100000) != 0) {
    exit(1);
  }
}
void main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("sitefactory [CONFIG_FILE]\nStarts the siteFactory HTTP server\n");
    exit(0);
  }
  char *configFile = argv[1];
  short dl;
  for (dl = 0; dl < 100; ++dl) {
    hostList[dl].is_empty = 0;
  }
  FILE *fp = fopen(configFile, "r");
  char *port = malloc(5);
  port = "80";
  char *line = malloc(1024);
  char *part2 = malloc(256);
  char *part3 = malloc(256);
  char *part4 = malloc(256);
  char *buf = malloc(256);
  short lst, sc;
  while (fgets(line, 1024, fp)) {
    strReplace("\r", "", line);
    strReplace("\n", "", line);
    strReplace("*.", "", line);
    line = rmmp(line, ' ');
    strReplace(", ", "|", line);
    strReplace(",", "|", line);
    strReplace(" #", "#", line);
    line = strToLower(line);
    sc = 0;
    if (strlen(line) > 1 && line[0] != '#') {
      line = strtok(line, " ");
      if (strcmp(line, "root") == 0) {
        strtok(NULL, " ");
        part3 = strtok(NULL, " ");
        part4 = strtok(NULL, " ");
        for (;;) {
          if (hostList[sc].is_empty == 0 || strcmp(hostList[sc].name, part3) == 0) {
            break;
          }
          sc++;
        }
        memcpy(hostList[sc].root_path, part4, 128);
        if (hostList[sc].is_empty == 0) {
          memcpy(hostList[sc].name, part3, 128);
          hostList[sc].is_empty = 1;
        }
      } else if (strcmp(line, "deny") == 0) {
        strtok(NULL, " ");
        part3 = strtok(NULL, " ");
        part4 = strtok(NULL, " ");
        for (;;) {
          if (hostList[sc].is_empty == 0 || strcmp(hostList[sc].name, part3) == 0) {
            break;
          }
          sc++;
        }
        memcpy(hostList[sc].deny, part4, 64);
        if (hostList[sc].is_empty == 0) {
          memcpy(hostList[sc].name, part3, 128);
          hostList[sc].is_empty = 1;
        }
      } else if (strcmp(line, "port") == 0) {
        char *port = strtok(NULL, " ");
        strReplace(" ", "", port);
      } else if (strcmp(line, "404") == 0) {
        strtok(NULL, " ");
        part3 = strtok(NULL, " ");
        part4 = strtok(NULL, " ");
        for (;;) {
          if (hostList[sc].is_empty == 0 || strcmp(hostList[sc].name, part3) == 0) {
            break;
          }
          sc++;
        }
        memcpy(hostList[sc].error_page, part4, 128);
        if (hostList[sc].is_empty == 0) {
          memcpy(hostList[sc].name, part3, 128);
          hostList[sc].is_empty = 1;
        }
      } else if (strcmp(line, "index") == 0) {
        strtok(NULL, " ");
        part3 = strtok(NULL, " ");
        part4 = strtok(NULL, " ");
        for (;;) {
          if (hostList[sc].is_empty == 0 || strcmp(hostList[sc].name, part3) == 0) {
            break;
          }
          sc++;
        }
        memcpy(hostList[sc].index, part4, 256);
        if (hostList[sc].is_empty == 0) {
          memcpy(hostList[sc].name, part3, 128);
          hostList[sc].is_empty = 1;
        }
      }
    }
  }
  fclose(fp);
  free(line);
  struct sockaddr_in clientaddr;
  socklen_t addrlen;
  int slot = 0;
  clients = mmap(NULL, sizeof(*clients)*SIMULTANEOUS_CONNECTIONS, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
  int i;
  for (i = 0; i < SIMULTANEOUS_CONNECTIONS; i++) {
    clients[i] = -1;
  }
  bindSocket(port);
  signal(SIGCHLD, SIG_IGN);
  for (;;) {
    addrlen = sizeof(clientaddr);
    clients[slot] = accept(listenfd, (struct sockaddr*)&clientaddr, &addrlen);
    if (clients[slot] >= 0) {
      if (fork() == 0) {
        close(listenfd);
        processQuery(slot);
        close(clients[slot]);
        clients[slot] = -1;
        exit(0);
      } else {
        close(clients[slot]);
      }
    }
    while (clients[slot] != -1) {
      slot = (slot + 1) % SIMULTANEOUS_CONNECTIONS;
    }
  }
}
