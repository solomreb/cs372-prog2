/*
** Becky Solomon
** CS 372-400
** Fall 2015
** Program 2
** fclient.java 
**
*/

/* adapted from http://www.rgagnon.com/javadetails/java-0542.html */

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.net.ServerSocket;

public class ftclient {

  public final static int FILE_SIZE = 6022386; // file size temporary hard coded
                                               // should bigger than the file to be downloaded

  public static void main (String [] args ) throws IOException {
    int bytesRead;
    int current = 0;
    FileOutputStream fos = null;
    BufferedOutputStream bos = null;
    OutputStream os = null;
    Socket dataSock = null;
    Socket controlSock = null;
    ServerSocket servSock = null;
    String serverHost = null;
    String command = null;
    String filename= null;
    String outCommand = null;
    int serverPort = 0;
    int dataPort = 0;


    if(args.length < 4 || args.length > 5)
    {
        System.out.println("Usage: ftclient.java <serverHost> <serverPort> -g|-l <filename> <dataPort>");
        System.exit(0);
    }
    
    serverHost = args[0];
    serverPort = Integer.parseInt(args[1]);
    command = args[2];
    if (command.equals("-g")) {
        filename = args[3];
        dataPort = Integer.parseInt(args[4]);
    }
    else if (command.equals("-l")){
        filename = null;
        dataPort = Integer.parseInt(args[4]);
    }
    

    
    try {
      System.out.printf("Listening on data port %d\n", dataPort);
      servSock = new ServerSocket(dataPort); //listen
      System.out.printf("Connecting to host %s, port %d\n", serverHost, serverPort);
      controlSock = new Socket(serverHost, serverPort);

      System.out.println("Connected.");

      //send command
      outCommand = command + " " + dataPort + " " + filename;
      os = controlSock.getOutputStream();
      os.write(outCommand.getBytes());
      os.flush();

      //accept connection
      dataSock =servSock.accept();

      // receive file
      byte [] mybytearray  = new byte [FILE_SIZE];
      InputStream is = dataSock.getInputStream();
      fos = new FileOutputStream(filename + ".copy");
      bos = new BufferedOutputStream(fos);

      //bytesRead = is.read(mybytearray);
      //System.out.printf("bytes read: %d\n", bytesRead);
      bytesRead = is.read(mybytearray,0,mybytearray.length);
      current = bytesRead;

      do {
         bytesRead =
            is.read(mybytearray, current, (mybytearray.length-current));
         if(bytesRead >= 0) current += bytesRead;
      } while(bytesRead > -1);

      System.out.printf("current: %s\n", current);
      bos.write(mybytearray, 0 , current);
      //bos.write(mybytearray, 0, bytesRead);
      bos.flush();
      System.out.println("File " + filename
          + " downloaded (" + current + " bytes read)");
    }
    finally {
      if (fos != null) fos.close();
      if (bos != null) bos.close();
      if (servSock != null) servSock.close();
      if (dataSock != null) dataSock.close();
      if (controlSock != null) controlSock.close();
    }
  }

}