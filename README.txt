/*
** Becky Solomon
** CS 372-400
** Fall 2015
** Program 2
**/

To compile:
	make ftclient
	make ftserver

Run server first by typing in a console window:
	ftserver <portnumber>

In a new console window of the SAME server:
	java ftclient <host> <portnumber> -g <filename> <dataportnumber>
OR
	java ftclient <host> <portnumber> -l <dataportnumber>

NOTE: both server and client must be run on same host
(e.g. will not work on flip2 and flip3. must both be flip2)

EXTRA CREDIT: This program will transfer files additional to text files
(e.g. binary files)
