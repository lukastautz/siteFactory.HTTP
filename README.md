# siteFactory HTTP server
The siteFactory HTTP server is a very fast and small HTTP/1.1 server who fits into 500 lines of code.<br>
It has an own configuration file syntax who's very simple and self explanatory.<br>
Because of the small size of 500 lines of C code it can only deliver unencrypted, static files.<br>
Static files are delivered at OS level with sendfile.<hr>
## Features:
- **27&#8239;KB** binary
- Only 500 lines of code
- no external depencies (only standard linux libraries)
- fast (sendfile)
- simple configuration syntax
- no CGI
- no SSL
- only HTTP/1.1
## FAQ:
- Can siteFactory HTTP server used for dynamic sites like Wordpress?<br>No, siteFactory can only deliver static files.
- Can it used for sensitive information?<br>No, because of the missing encryption.
- How many websites can delivered from one instance of siteFactory?<br>Default to 100, but it's changeable. You have to edit the size of the hostList object (line 37) and the intenger in line 274 (max sites + 1).
- How many RAM is needed?<br>You need minimal ~1/3MB of RAM, but more is not bad. You can downsize it by downsizing the hostList object (line 37).
- Does sitefactory have to be run as root?<br>Not directly, but if you want to use port 80, then yes.
<hr>

## Installation:
You have to download the .tar.gz or .tar.xz file from the releases. Then run the configure as root. You can simply run this script:<br>
```bash
wget -O sitefactory.tar.xz https://github.com/lukastautz/siteFactory/releases/download/latest/sitefactory.tar.xz && tar -xf sitefactory.tar.xz && sudo ./configure
```
