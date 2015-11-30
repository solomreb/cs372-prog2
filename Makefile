PORT = 34911
HOST = flip1.engr.oregonstate.edu
DATAPORT = 34912
COMMAND = -l
FILENAME = server

compile: server client

ftserver: fserver.cpp
		g++ -o ftserver fserver.cpp

ftclient: ftclient.java
		javac ftclient.java

server2: compile
		./ftserver $(PORT)

clientg: compile
		java ftclient $(HOST) $(PORT) -g $(FILENAME) $(DATAPORT)

#clientl: compile
		#java ftclient $(HOST) $(PORT) -l $(DATAPORT)

clean:
	$(RM) *.o server client

