PORT = 34911
HOST = flip1.engr.oregonstate.edu
DATAPORT = 34912
COMMAND = -g
FILENAME = test.txt

compile: server client

server: fserver.cpp
		g++ -o server fserver.cpp

client: ftclient.java
		javac ftclient.java

server2: compile
		./server $(PORT)

client2: compile
		java ftclient $(HOST) $(PORT) $(COMMAND) $(FILENAME) $(DATAPORT)

clean:
	$(RM) *.o server client

