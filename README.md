# To run the current MVP for the Server:

in your terminal:
-> make
-> ./Webserver

in your browser:
-> http://localhost:4432/html/index.html

to test the 404 (Page not found):
	change the name of the html in our browser so it does not match any of the current pages (current pages supported only in the html file as the file paths must match)
	example:
	http://localhost:4432/html/index_NOT_FOUND.html

Ctrl + C : to kill the infinite loop. Handling of the exit still to be implemented.

currently supports Linux with Multiplexed (Clients in Concurrency)

Just run "make" and it compile with the Linux files or with the Mac files