PORT = 34911
HOST = flip1.engr.oregonstate.edu
DATAPORT = 34912
COMMAND = -l
FILENAME = server

compile: server client

server: fserver.cpp
		g++ -o server fserver.cpp

client: ftclient.java
		javac ftclient.java

server2: compile
		./server $(PORT)

client2: compile
		#rm test.txt.copy
		java ftclient $(HOST) $(PORT) -g $(FILENAME) $(DATAPORT)
		#diff -s test.txt.copy test.txt

client3: compile
		java ftclient $(HOST) $(PORT) -l $(DATAPORT)

clean:
	$(RM) *.o server client

