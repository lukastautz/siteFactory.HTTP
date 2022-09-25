# siteFactory HTTP server
The siteFactory HTTP server is a very fast and small HTTP/1.1 server who fits into 500 lines of code.<br>
Because of the small size of 500 lines of C code it can only deliver unencrypted, static files.<br>
Static files are delivered at OS level with sendfile.<hr>
## Features:
- Only 500 lines of code
- no external depencies (only standard linux libraries)
- fast (sendfile)
- CGI
- no SSL
- only HTTP/1.1
<hr>

## CGI:
CGI support isn't complete yet, siteFactory HTTP server does NOT send environment variables like HTTP_USER_AGENT, and does NOT send any POST data. It sends the request uri as argv[1], and the query string as argv[2].
<hr>

## FAQ:
- Can siteFactory HTTP server used for dynamic sites like Wordpress?<br>No, for none of those CMS, because of the CGI support isn't complete, those systems wouldn't support things like creating big articles or upload files.
- Can it used for sensitive information?<br>No, because of the missing encryption via SSL.
- How many websites can delivered from one instance of siteFactory?<br>There is in most cases no sense for delivering multiple websites, because of there are no vhosts or similar.
- How many RAM is needed?<br>You need minimal 2MB of RAM, but more is not bad.
- Does siteFactory have to be run as root?<br>Not directly, but if you want to use port 80, then yes.
<hr>

## Installation:
### Install binary (only amd64):
```bash#
# Default (without CGI):
sudo wget https://raw.githubusercontent.com/lukastautz/siteFactory/main/binary-amd64/sitefactory.binary -O /bin/sitefactory

# Bash CGI:
sudo wget https://raw.githubusercontent.com/lukastautz/siteFactory/main/binary-amd64/sitefactory.cgi-bash.binary -O /bin/sitefactory

# PHP CGI:
sudo wget https://raw.githubusercontent.com/lukastautz/siteFactory/main/binary-amd64/sitefactory.php-bash.binary -O /bin/sitefactory
```
### Manual compiling:
You have to download the .c source file manually from the folder <a href="https://github.com/lukastautz/siteFactory/tree/main/src">src/</a>.<br>Then compile
- to a dynamically linked executable (``sudo gcc sitefactory.c -o /bin/sitefactory``)
- or, recommended, to a static executable who needs only the half of the RAM needed by a dynamically linked:
```bash
sudo gcc -static -O9 -Ofast sitefactory.c -o /bin/sitefactory
```
<hr>

## Command line options
```bash
sitefactory [OPTIONS]
Starts the siteFactory HTTP server
Options:
  -r     --root         [MANDATORY] Sets the root directory
  -i     --index        Sets the index pages, seperated with ',' (default: index.html)
  -d     --deny         Sets the denied extensions, seperated with ',' (for example 'txt,html')
  -e     --error-page   Sets the 404 error page
  -p     --port         Sets the port (default: 80)
  -h     --help         Prints this page
  -l     --license      Prints the license terms
```