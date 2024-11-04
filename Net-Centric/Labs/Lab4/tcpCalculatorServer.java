import java.io.*;
import java.net.*;

class tcpCalculatorServer
{
    public static void main(String[] args) throws Exception
    {
        String calculatorInput;
        ServerSocket welcomeSocket = new ServerSocket(6789);
        System.out.println("Waiting for connection.....");

        while (true)
        {
            Socket connectionSocket = welcomeSocket.accept();
            System.out.println("Connection successful");
            System.out.println("Waiting for input.....");

            BufferedReader inFromClient = new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
            DataOutputStream outToClient = new DataOutputStream(connectionSocket.getOutputStream());

            while (true)
            {
                calculatorInput = inFromClient.readLine();
                if (calculatorInput == null || calculatorInput.equals("DONE"))
                {
                    outToClient.writeBytes("CLOSE\n");
                    System.out.println("Session closed with client.");
                    break;
                }

                // Evaluate the mathematical expression
                try {
                    double result = evaluateExpression(calculatorInput);
                    String calculatorOutput = String.valueOf(result);
                    outToClient.writeBytes(calculatorOutput + '\n');
                    System.out.println("Received from client: " + calculatorInput);
                    System.out.println("Sent to client: " + calculatorOutput);
                } catch (Exception e) {
                    outToClient.writeBytes("ERROR\n");
                    System.out.println("Error evaluating expression: " + calculatorInput);
                }
            }

            connectionSocket.close();
        }
    }

    // Simple method to evaluate expressions (additions and subtractions)
    private static double evaluateExpression(String expression) throws Exception {
        // Example: handle basic arithmetic for now
        String[] tokens = expression.split(" ");
        double num1 = Double.parseDouble(tokens[0]);
        String operator = tokens[1];
        double num2 = Double.parseDouble(tokens[2]);

        return switch (operator) {
            case "+" -> num1 + num2;
            case "-" -> num1 - num2;
            case "*" -> num1 * num2;
            case "/" -> num1 / num2;
            default -> throw new UnsupportedOperationException("Unknown operator: " + operator);
        };
    }
}
