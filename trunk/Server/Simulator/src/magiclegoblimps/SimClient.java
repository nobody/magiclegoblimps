/*
The SimClient connects to the SimServer so that a user can control the
simulation by typing commands directly into standard input.
*/

package magiclegoblimps;

import java.io.*;
import java.net.*;

public class SimClient
{
    public static final String DEFAULT_HOST = "localhost";
    public static final int DEFAULT_PORT = 4334;
    
    Socket socket;
    PrintWriter out;
    BufferedReader in;
    
    public static void main(String[] args) {
        String host = DEFAULT_HOST;
        int port = DEFAULT_PORT;
        if (args.length == 2) {
            try {
                host = args[0];
                port = Integer.parseInt(args[1]);
            } catch (NumberFormatException ex) {
                System.err.println("Invalid port number: " + args[0]);
                System.exit(1);
            }
        }
        new SimClient(host, port);
    }
    
    public SimClient(String host, int port) {
        try {
            socket = new Socket(host, port);
            out = new PrintWriter(socket.getOutputStream(), true);
            in = new BufferedReader(
                        new InputStreamReader(socket.getInputStream()));
        } catch (UnknownHostException ex) {
            System.err.println("Bad host: " + ex);
            System.exit(1);
        } catch (IOException ex) {
            System.err.println("I/O ERROR: " + ex);
            System.exit(1);
        }
    
        try {
            String userInput;
           	BufferedReader stdin = new BufferedReader(
                                           new InputStreamReader(System.in));
           	while ((userInput = stdin.readLine()) != null) {
           	    out.println(userInput);
           	    System.out.println(in.readLine());
           	}
          	out.close();
          	in.close();
          	stdin.close();
          	socket.close();
        } catch (IOException ex) {
            System.err.println("I/O ERROR: " + ex);
        }
    }
}
