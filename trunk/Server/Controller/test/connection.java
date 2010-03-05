/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Joseph Gebhard
 */
public class connection {
        java.net.Socket socket;
        java.util.Scanner cin = new java.util.Scanner(System.in);
        java.io.PrintWriter cout;
        
        public void connect(java.lang.String host, int port){
            try{
                socket = new java.net.Socket(host, port);
                cout = new java.io.PrintWriter(socket.getOutputStream(), true);
            }catch(java.lang.Exception ex){
                System.out.println("Socket failed to initialize");
                ex.printStackTrace();
            }
            java.lang.String str = "\n";
            while(!str.equalsIgnoreCase("quit")){
                System.out.println("Enter a String: ");
                str = cin.nextLine();
                cout.println(str);
                
            }
        }
        
        
        public static void main(java.lang.String args[]){
            connection connect = new connection();
            connect.connect(args[0], java.lang.Integer.parseInt(args[1]));
        }
}
