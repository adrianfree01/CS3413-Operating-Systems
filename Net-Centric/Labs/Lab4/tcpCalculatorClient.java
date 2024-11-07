import java.io.*;
import java.net.*;
import java.util.*;

class tcpCalculatorClient 
{
    public static void main(String[] args) throws Exception
    {
        String calculatorInput;
        String calculatorOutput;
        int totalCharactersSent = 0;
        int totalCharactersReceived = 0;
        long totalTurnaroundTime = 0;
        int questionCount = 0;

        Socket clientSocket = new Socket(args[0], 6789);
        DataOutputStream outToServer = new DataOutputStream(clientSocket.getOutputStream());
        BufferedReader inFromServer = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

        String filePath = "questions.txt";

        try(BufferedReader reader = new BufferedReader(new FileReader(filePath)))
        {
            while((calculatorInput = reader.readLine()) != null)
            {
                System.out.println("Question from client: " + calculatorInput);
                outToServer.writeBytes(calculatorInput + '\n');
                long startTime = System.nanoTime();
                calculatorOutput = inFromServer.readLine();
                long endTime = System.nanoTime();
                totalTurnaroundTime += (endTime - startTime);
                questionCount++;

                System.out.println("Answer from server: " + calculatorOutput);
                totalCharactersSent += calculatorInput.length() + 1;  // +1 for newline
                totalCharactersReceived += calculatorOutput.length();
            }

            outToServer.writeBytes("DONE\n");
            totalCharactersSent += 5;  // Length of "DONE\n"
            calculatorOutput = inFromServer.readLine();
            if ("CLOSE".equals(calculatorOutput))
            {
                totalCharactersReceived += 6;  // Length of "CLOSE\n"
                System.out.println("Connection closed by server.");
            }
            clientSocket.close();

            // Display final statistics
            System.out.println("Total characters sent: " + totalCharactersSent);
            System.out.println("Total characters received: " + totalCharactersReceived);
            System.out.println("Avg turn-around delay per question-answer: " + (totalTurnaroundTime / questionCount / 1e6) + " ms");
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }
}
