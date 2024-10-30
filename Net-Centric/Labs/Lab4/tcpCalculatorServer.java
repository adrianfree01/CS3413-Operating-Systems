import java.io.*;
import java.net.*;

class tcpCalculatorServer
{
	public static void main(String[] args) throws Exception
	{
		String calculatorInput;
		String calculatorOutput;
		ServerSocket welcomeSocket = new ServerSocket(6789);
		System.out.println("Waiting for connection.....");

		while (true)
		{
			Socket connectionSocket = welcomeSocket.accept();
			System.out.println("Connection successful");
			System.out.println("Waiting for input.....");

			BufferedReader inFromClient = new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
			DataOutputStream outToCli
		}
	}
}