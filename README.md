# siteFactory HTTP server
The siteFactory HTTP server is a very fast and small HTTP/1.1 server who fits into 500 lines of code.<br>
Because of the small size of 500 lines of C code it can only deliver unencrypted, static files and simple CGI scripts.<br>
Static files are delivered at OS level with sendfile.<hr>
## Features:
- Only 500 lines of code
- no external depencies (only standard linux libraries)
- fast (sendfile)
- CGI
- no SSL
- only HTTP/1.1
<hr>

## CGI
CGI support isn't complete yet, siteFactory HTTP server does NOT send environment variables like HTTP_USER_AGENT, and does NOT send any POST data. It sends the request uri as argv[1], and the query string as argv[2].<br>
You have to output an complete HTTP/1.1 conforming response, with ``HTTP/1.1 200 OK\nContent-Type: text/html\n\n``!
<hr>

## FAQ
- Can siteFactory HTTP server used for dynamic sites like Wordpress?<br>No, for none of those CMS, because of the CGI support isn't complete, those systems wouldn't support things like creating big articles or upload files.
- Can it used for sensitive information?<br>No, because of the missing encryption via SSL.
- How many websites can delivered from one instance of siteFactory?<br>There is in most cases no sense for delivering multiple websites, because of there are no vhosts or similar.
- How many RAM is needed?<br>You need minimal 2MB of RAM, but more is not bad.
- Does siteFactory have to be run as root?<br>Not directly, but if you want to use port 80, then yes.
- I've set up a CGI server, but there's only a text ouput/not a complete response. How can I fix it?<br>Make sure you're sending an complete HTTP/1.1 conforming response, with ``HTTP/1.1 200 OK\nContent-Type: text/html\n\n``.
<hr>

## Installation
### Install binary (only amd64)
```bash#
# Default (without CGI)
sudo wget https://raw.githubusercontent.com/lukastautz/siteFactory.HTTP/main/binary-amd64/sitefactory.binary -O /bin/sitefactory && sudo chmod 777 /bin/sitefactory

# Bash CGI
sudo wget https://raw.githubusercontent.com/lukastautz/siteFactory.HTTP/main/binary-amd64/sitefactory.cgi-bash.binary -O /bin/sitefactory && sudo chmod 777 /bin/sitefactory

# PHP CGI
sudo wget https://raw.githubusercontent.com/lukastautz/siteFactory.HTTP/main/binary-amd64/sitefactory.cgi-php.binary -O /bin/sitefactory && sudo chmod 777 /bin/sitefactory
```
### Manual compiling
You have to download the .c source file manually from the folder <a href="https://github.com/lukastautz/siteFactory.HTTP/tree/main/src">src/</a>.<br>Then compile
- to a dynamically linked executable (``sudo gcc sitefactory.c -o /bin/sitefactory``)
- or, recommended, to a static executable who needs only the half of the RAM needed by a dynamically linked:
```bash
sudo gcc -static -O9 -Ofast sitefactory.c -o /bin/sitefactory
```
<hr>

## PHP CGI
***Important***: Add this script to each php site! Otherwise you can't use the ***$_GET*** array and the ***$_SERVER['REQUEST_URI']*** variable!
```php
$_SERVER['REQUEST_URI']=$argv[1];parse_str($argv[2],$_GET);
```
- You have to output an complete HTTP response, with ``HTTP/1.1 200 OK\nContent-Type: text/html\n\n``!
<hr>

## Command line switches
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
<hr>

## Benchmark
Benchmark is created with <a href="https://github.com/JoeDog/siege">Siege</a>.<br>
Benchmarked with a virtual machine with Debian 11 (bullseye):
- 2 vCores
- 1024 MB RAM
- 1 Gbit Ethernet
- 10 minutes
### Results
#### PHP CGI
I've used the following short script:
```php
<?php
echo "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" . $argv[1] . '?' . $argv[2] . "\n";
```
- Requests: 6770
- Succeded requests: 100%
- Average response time: 0.93s
- Longest response time: 2.10s
- Shortest response time: 0.32s
- Data transfered: 0.08 MB
#### Bash CGI
I've used the following short script:
```bash
echo "HTTP/1.1 200 OK"
echo "Content-Type: text/html"
echo ""
echo $1?$2
```
- Requests: 9999
- Succeded requests: 100%
- Average response time: 0.16s
- Longest response time: 0.83s
- Shortest response time: 0.00s
- Data transfered: 0.10 MB
#### Static files
I've used the following file:
```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8">
	<title>Example static file</title>
  </head>
  <body>
    This is an example static file<br>Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.<br>Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.<br>Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse
  </body>
</html>
```
- Requests: 240590
- Succeded requests: 100%
- Average response time: 0.09s
- Longest response time: 2.60s
- Shortest response time: 0.00s
- Data transfered: 388.91 MB
<hr>

## Setup a service
Below is an example how you can setup a simple service wich starts on boot. You have to edit the /etc/init.d/sitefactory file and customize things like port, index, denied files with command line switches.
```bash
sudo touch /etc/init.d/sitefactory
sudo chmod 777 /etc/init.d/sitefactory
echo "#!/bin/bash" >> /etc/init.d/sitefactory
echo "### BEGIN INIT INFO" >> /etc/init.d/sitefactory
echo "# Provides:        sitefactory" >> /etc/init.d/sitefactory
echo "# Default-Start:   2 3 4 5" >> /etc/init.d/sitefactory
echo "# Description:     The siteFactory HTTP server" >> /etc/init.d/sitefactory
echo "### END INIT INFO" >> /etc/init.d/sitefactory
echo "NAME=default" >> /etc/init.d/sitefactory
echo "PATH=/bin" >> /etc/init.d/sitefactory
echo "start() {" >> /etc/init.d/sitefactory
echo "    sitefactory -r /var/www > /dev/null &" >> /etc/init.d/sitefactory
echo "}" >> /etc/init.d/sitefactory
echo "stop() {" >> /etc/init.d/sitefactory
echo "    kill \`pidof sitefactory\`" >> /etc/init.d/sitefactory
echo "}" >> /etc/init.d/sitefactory
echo "case \"\$1\" in" >> /etc/init.d/sitefactory
echo "    start)" >> /etc/init.d/sitefactory
echo "       start" >> /etc/init.d/sitefactory
echo "       ;;" >> /etc/init.d/sitefactory
echo "    stop)" >> /etc/init.d/sitefactory
echo "       stop" >> /etc/init.d/sitefactory
echo "       ;;" >> /etc/init.d/sitefactory
echo "    restart)" >> /etc/init.d/sitefactory
echo "       stop" >> /etc/init.d/sitefactory
echo "       start" >> /etc/init.d/sitefactory
echo "       ;;" >> /etc/init.d/sitefactory
echo "    *)" >> /etc/init.d/sitefactory
echo "       echo \"Usage: service sitefactory {start|stop|restart}\"" >> /etc/init.d/sitefactory
echo "esac" >> /etc/init.d/sitefactory
echo "exit 0" >> /etc/init.d/sitefactory
sudo chmod 700 /etc/init.d/sitefactory
sudo chmod +x /etc/init.d/sitefactory
sudo update-rc.d sitefactory defaults
sudo service sitefactory start
```
