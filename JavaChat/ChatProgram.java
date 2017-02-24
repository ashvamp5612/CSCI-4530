import java.net.*;
import java.io.*;
import java.util.*;

public class ChatProgram {
	
	public static Scanner kbd = new Scanner(System.in);

	public static void main(String[] args) {
		
		int menuOption;
		
		// Allow user to choose server or client operation
		System.out.println();
		System.out.println("1) Host server");
		System.out.println("2) Join server");
		System.out.println("3) Quit");
		
		menuOption = kbd.nextInt();
		System.out.println();
		
		switch(menuOption)
		{
		case 1:
			startServer();
			break;
		case 2:
			startClient();
			break;
		}
		
	}
	
	
	/**
	 * This method enables the program to act in server mode.
	 * Sets up a listening socket, accepts incoming connections, and performs chat.
	 * The method ends when the server types "end" at the keyboard, or when the word
	 * "end" is received from the client.
	 */
	public static void startServer() {
		
		int port;
		ServerSocket listenSocket;
		Socket connection;
		InputStream inStream;
		DataInputStream inDataStream;
		OutputStream outStream;	
		DataOutputStream outDataStream;
		String message = "";
		
		System.out.println("Enter port number: ");
		port = kbd.nextInt();
		kbd.nextLine();
		
		
		try {
			// Try to create socket for listening
			listenSocket = new ServerSocket ( port );
			System.out.println("\nHOSTING...\n");
			
			// Wait for incoming connection
			connection = listenSocket.accept();
			
			// Connected...
			// Set up input and output streams
			outStream = connection.getOutputStream ();
			outDataStream = new DataOutputStream ( outStream );
			inStream = connection.getInputStream ();
		    inDataStream = new DataInputStream ( inStream );

		    try {
				while (!message.equalsIgnoreCase("end")) {
					// Get incoming message from client
					message = receive(inDataStream);
					System.out.println(message + "\n");
					
					// Stop if client sent the "end" message
					if (message.equalsIgnoreCase("end")) break;
					
					// Get message from keyboard and send
					getAndSend(outDataStream);
				}
			} 
		    catch (Exception e) {		    	
		    	// If program exit due to a shutdown, rather than because of "end"
		    	// don't do anything... just exit as normal.
			}
		    finally
		    {
		    	// Clean up... Close all open streams and sockets
		    	inDataStream.close();
				inStream.close();
				outDataStream.close();
				outStream.close();	
				
				connection.close();
				listenSocket.close();
		    }
		    
		    System.out.println("Connection closed.");
		    
			
		}
		catch(IOException e) {
			System.out.println("ERROR: An unknown error occurred.");
		}
		
			
		
	}
	
	
	/**
	 * This function enables the program to act in client mode.
	 * Sets up a communication port, contacts a server, and then performs chat.
	 * The method ends when the server types "end" at the keyboard, or when the word
	 * "end" is received from the client.
	 */
	public static void startClient() {
		
		int port;
		String address, host;
		Socket connection = null;
		InputStream inStream;
		DataInputStream inDataStream;
		OutputStream outStream;	
		DataOutputStream outDataStream;	
		String message = "";
		
		System.out.println("Enter port number: ");
		port = kbd.nextInt();
		kbd.nextLine();
		
		System.out.println("\nEnter IP address (127.0.0.1 is loopback): ");
		address = kbd.nextLine();
		
		try {
			
			// Attempt connection to the server
			System.out.println("\nATTEMPTING TO CONNECT...\n");
			try {
				InetAddress serverAddr = InetAddress.getByName(address);
				host = serverAddr.getHostName ();
				connection = new Socket ( host, port );
				System.out.println( "Connected...\n" );
			}
			catch (Exception e){
				// Server connection could not be made
				System.out.println ("ERROR: Failed to connect\n");
				System.exit(0);
			}
			
			// Set up input and output streams
			outStream = connection.getOutputStream ();
			outDataStream = new DataOutputStream ( outStream );
			inStream = connection.getInputStream ();
		    inDataStream = new DataInputStream ( inStream );
	
			try {
				while (!message.equalsIgnoreCase("end")) {
					
					// Get message from keyboard and send
					getAndSend(outDataStream);
					
					// Get response
					message = receive(inDataStream);
					System.out.println(message + "\n");
				}
			} 
			catch (Exception e) {
		    	// If program exit due to a shutdown, rather than because of "end"
				// or if Client typed "end"
		    	// don't do anything... just exit as normal.
			}
			finally
			{
				// Clean up... Close all open streams and sockets
				inDataStream.close();
				inStream.close();
				outDataStream.close();
				outStream.close();	
				
				connection.close();	
			}
			
			System.out.println("Connection closed.");
						
			
		}
		catch(IOException e) {
			System.out.println("ERROR: An unknown error occurred.");
		}

		
	}
	
	
	/**
	 * This method reads data from the incoming socket
	 * @param inDataStream A stream connected to the incoming socket from which to read
	 * @return The string which was received from the socket
	 * @throws IOException If there was a problem reading the data
	 */
	public static String receive(DataInputStream inDataStream) throws IOException
	{
		String message;
		System.out.println("    ---- WAIT ----\n");
	    message = inDataStream.readUTF ();
	    return message;
	}
	
	
	/**
	 * This method gets a string from the keyboard, and then sends it over an outgoing socket
	 * @param outDataStream A stream connected to the outgoing socket where the data should be sent
	 * @throws IOException If there was a problem sending the data
	 */
	public static void getAndSend(DataOutputStream outDataStream) throws IOException
	{
		String message;		

		System.out.print("> ");
		message = kbd.nextLine();
		outDataStream.writeUTF ( message );
		System.out.println();	
	}

}
