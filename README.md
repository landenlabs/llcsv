# llcsv
OSX / Linux / DOS  File find and replace text command line tool.

LLReplace searches file text data using regular expression and replaces matches with substitution text.

  [![Build status](https://travis-ci.org/landenlabs/llcsv.svg?branch=master)](https://travis-ci.org/landenlabs/llcsv)
  [![Known Vulnerabilities](https://snyk.io/test/github/landenlabs/llcsv/badge.svg)](https://snyk.io/test/github/landenlabs/llcsv)

  
**Help screen**

<pre>
llcsv  Dennis Lang v1.1 (landenlabs.com) Jan  9 2020

Des: Replace text in files
Use: llcsv [options] directories...

Main options:
   -from=&lt;regExpression>          ; Pattern to find
   -to=&lt;regExpression or string>  ; Optional replacment
   -backupDir=&lt;directory>         ; Optional Path to store backup copy before change
   -includeFiles=&lt;filePattern>    ; Optional files to include in file scan, default=*
   -excludeFiles=&lt;filePattern>    ; Optional files to exclude in file scan, no default
   directories...                 ; Directories to scan
Other options:
   -show=&lt;file|match|pattern>      ; Show file path, text match or patterns
   -hide=&lt;file|match>              ; Hide showing file path or text macth

Examples
 Search only, show patterns and defaults showing file and match:
  llcsv -show=pattern '-from=if [(]MapConfigInfo.DEBUG[)] [{][\r\n ]*Log[.](d|e)([(][^)]*[)];)[\r\n ]*[}]' -hide=match '-include=*.java' src
 Search and replace in-place:
  llcsv '-from=if [(]MapConfigInfo.DEBUG[)] [{][\r\n ]*Log[.](d|e)([(][^)]*[)];)[\r\n ]*[}]' '-to=MapConfigInfo.$1$2$3' '-include=*.java' src

</pre>


Visit home website

[https://landenlabs.com](https://landenlabs.com)

