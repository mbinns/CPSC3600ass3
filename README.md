# CPSC3600ass3
Allan Austin, Mackenzie Binnis, Matt Beazley, Tyler Fischer 
HTML Server/Client repo

This file contains a suite of HTTP client server programs. The clients, 
chopper.c and ahsoka.c send TCP GET requests to the server, starLord.c.
The server accepts and parses these incoming requests to make sure that they
are valid requests. After it has parsed the requests, it identifies whether it 
should perform an add or view on the data that it has parsed. If it adds, it 
prepends the host header to the message it received, appends a newline, and then 
places all of that into a local buffer and sends the contents of the buffer as 
a payload in an http response. The chopper.c client can be run two ways, the first
way accepts a string as a parameter at the command line and sends it in the payload 
of an HTTP GET request, and then waits for a response from the server and displays it. 
The second way allows it to make another, seperate GET request. The ahsoka.c client
follows a similar process, but verifies that the server actually does its job 
correctly. It does this by sending a series of add and view requests, and 
then comparing the returned buffer with the expected output
The flags given below MUST be used at runtime in order for the applications to 
run properly, although the order that they are supplied in is entirely up to 
the user. i.e. specifying the message, then the port, then the server will not 
affect the program, provided that each of those are present along with their
corresponding flags.

In order to compile these programs, a makefile has been provided that will run
with the command "make."
After they have been compiled, each executable can be run according to the
syntax given below. They can be run on a single or multiple devices, but each
program will need to be run in its own command line shell.

 ______________________________
|                              |
|          starLord.c          |
|______________________________|
accepts input in the form of:
starLord -p <serverPort>

where:
-p: serverPort: The 16 bit port number the server should use. You should
generally run the server with a port number in the 5K to 10K range.

and will then output:

#Messages<\tab>IPs

12	127.0.0.1, 192.82.1.34,

 ________________________
|                        |
|        chopper.c       |
|________________________|
accepts input in the form of:

chopper -s <serverIP> -p <serverPort> -h <hostHeader> [-a <message>] [-v]

where:
-s,-p,-h,-a, and -v are flags corresponding to
-s: serverIP: is the IP address (in dotted name format) of the host that is
	running the server side.
-p: serverPort: is the 16 bit port number the server is using. You should
	generally run the server with a port number in the 5K to 10K range.
-h: hostHeader: is a string parameter and if it includes whitepaces, should have
the appropriate quotation marks.
-a: message: is a string parameter and if it includes whitepaces, should have
the appropriate quotation marks.
-v: if this flag is included, client will create a view request and the content of the message 
	will not be sent as an add request

and will then output:

attempts<\tab>time<\tab>inputString<\tab>HTTP request status<\tab>outputString

1	0.392020	Hello World!	HTTP OK 200     modified hello might include newlines

 ________________________
|                        |
|        ahsoka.c        |
|________________________|
accepts input in the form of:

ahsoka -s <serverIP> -p <serverPort> -h <hostHeader> -a1 <message>... -aN <messageN>

where:
-s,-p,-h, and -a1..aN are flags corresponding to
-s: serverIP: is the IP address (in dotted name format) of the host that is
running the server side.
-p: serverPort: is the 16 bit port number the server is using. You should
generally run the server with a port number in the 5K to 10K range.
-h: hostHeader: is a string parameter and if it includes whitepaces, should have
the appropriate quotation marks.
-a: message: is a string parameter and if it includes whitepaces, should have
the appropriate quotation marks.


and will then output:

attempts<\tab>time<\tab>totalOutputString<\tab>verificationState

3	5.010020	more modified might include newlines if they’re there 	Verified
