/*
The SimServer aims to provide an interface identical to the server/robot
interface for testing the QoS and AI algorithms as accurately and
realistically as possible. Currently the SimServer only maintains a connection 
with a single client.

The server/robot protocol can be found online at:
    [wiki page]

Usage:
    java magiclegoblimps.SimServer [port]

*/

package magiclegoblimps;

import java.io.*;
import java.net.*;

public class SimServer
{
    public static final int DEFAULT_PORT = 4334;
    
    ServerSocket server;
    Socket client;
    PrintWriter out;
    BufferedReader in;
    SimController sim;
    
    public static void main(String[] args)
    {
        int port = DEFAULT_PORT;
        if (args.length == 1) {
            try {
                port = Integer.parseInt(args[0]);
            } catch (NumberFormatException ex) {
                System.err.println("Invalid port number: " + args[0]);
                System.exit(1);
            }
        }
        new SimServer(port);
    }
    
    public SimServer(int port) {
        // set up server
        try {
            server = new ServerSocket(port);
        } catch (IOException ex) {
            System.err.println("Failed to listen on port: " + port);
            System.exit(1);
        }
        
        // get connection from client
        try {
            client = server.accept();
        } catch (IOException ex) {
            System.err.println("Connection to client failed");
            System.exit(1);
        }
        
        try {
            // open I/O streams for communicating with client
            out = new PrintWriter(client.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(client.getInputStream()));
        
            // Process client requests
            String request = null;
            String response = null;
            sim = new SimController();
            while ((request = in.readLine()) != null) {
                try {
                    response = sim.process(request);
                } catch (Exception ex) {
                    break; // this better be a fatal error
                }
                if (response != null) {
                    out.println(response);
                }
            }
            
            // close I/O streams
            out.close();
            in.close();
            // close connection with client
            client.close();
            // shutdown the server
            server.close();
        } catch (IOException ex) {
            System.err.println("I/O ERROR:\n\n" + ex);
        }
    }
}
