# Instruction to run Webserver:

in your terminal:
-> make
-> ./webserver <config_file>
   example: ./webserv configs/default.conf

in your browser:
-> http://localhost:4432/html/index.html
or 
->http://localhost:4432/frontEnd/index.html

Makefile:
	There is a receipt to make in Linux and one for Mac. It detects automatically which one is necessary. You must add your files to both FILES list of files if they are necessary in both 
	operational systems.
	

to test the 404 (Page not found):
	change the name of the html in our browser so it does not match any of the current pages (current pages supported only in the html file as the file paths must match)
	example:
	http://localhost:4432/html/index_NOT_FOUND.html

Ctrl + C : to kill the infinite loop. Handling of the exit still to be implemented.

currently supports Linux with Multiplexed (Clients in Concurrency)
and a simple one client server for Mac
