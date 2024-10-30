import java.io.*;
import java.net.*;

class tcpCalculatorClient 
{
	public static void main(String[] args)
	{
		String calculatorInput;
		String calculatorOutput;

		Socket clientSocket = new Socket(args[0], 6789);
		DataOutputStream outToServer = new DataOutputStream(clientSocket.getInputStream());
		BufferedReader inFromServer = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

		String filePath = "questions.txt";

		try(BufferedReader reader = new BufferedReader(new FileReader(filePath)))
		{
			while((calculatorInput = reader.readLine()) != null)
			{
				System.out.println(calculatorInput);
				outToServer.writeBytes(calculatorInput + '\n');
				calculatorOutput = inFromServer.readLine();
				System.out.println(calculatorOutput);
			}
			clientSocket.close();
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}
	}
}

/*
*	Next, write a new socket client and a socket server that implement the following functions:
*
* 		The client reads out the calculation questions from a text file and sends them line by line
*			to the server by using TCP.
*
*		Once the server receives a question, it converts the received text into a mathematic
*			expression and evaluates it. After that, the server sends the answer back to the client by
*			using TCP.
*
*		Once the client sends over all questions to the server, it sends the last message “DONE\n”
*			to the server. Once the server receives this last message, it replies “CLOSE\n” and then
*			close the connection with the client
*
*		The server should remain open to accept new clients after it is done with one client. To be
*			efficient, the client should only set up a connection with the server once for all questions.
*
*		For debugging purposes, the client and server both display their sent and received
*			messages on the screen. The client also records the turn-around time it takes to send each
*			question and receive the corresponding answer. Once the client receives the “CLOSE”
*			message from the server, it displays the number of characters it has sent and received
*			(including characters in messages “DONE\n” and “CLOSE\n”) and the average turnaround time over all questions. After that, the client terminates its connection with the
*			server.
*
*		The following is an example of the final display at the client side:
*
*			#20----------------------------
*			Question from client: 89.03 - 86.453
*			Answer from server: 2.5769958
*			Q&A END*******************************
*			Total characters sent: 331
*			Total characters received: 183
*			Avg turn-around delay per question-answer: 0.6ms.
*/