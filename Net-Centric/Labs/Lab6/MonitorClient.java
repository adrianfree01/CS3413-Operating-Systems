import java.io.IOException;
import java.net.*;
import java.util.ArrayList;
import java.util.List;

class MonitorClient
{
    static final int MAX_MSG_LEN = 1024;
    static final int TIMEOUT = 2000; // in milliseconds
    static final int MONITOR_TIMEOUT = 5000; // in milliseconds
    static final double ALPHA = 0.125;
    static final double BETA = 0.25;

    static List<Long> rttSamples = new ArrayList<>();
    static List<Double> estimatedRTTList = new ArrayList<>();
    static List<Double> devRTTList = new ArrayList<>();

    public static void main(String[] args)
    {
        if (args.length != 2)
        {
            System.out.println("Usage: java MonitorClient <Server Hostname/IP> <Server Port>");
            return;
        }

        String serverHost = args[0];
        int serverPort = Integer.parseInt(args[1]);

        DatagramSocket clientSocket = null;

        try
        {
            clientSocket = new DatagramSocket();
            clientSocket.setSoTimeout(TIMEOUT);

            InetAddress serverAddress = InetAddress.getByName(serverHost);
            double estimatedRTT = 0;
            double devRTT = 0;

            System.out.println("Client started. Sending requests to " + serverHost + ":" + serverPort);

            for (int i = 0; i < 40; i++)
            {
                String requestMsg = "Hello " + i + " ";
                byte[] sendData = requestMsg.getBytes();
                DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, serverAddress, serverPort);

                System.out.println("Sending request " + i + ": " + requestMsg.trim());

                long sendTime = System.currentTimeMillis();
                clientSocket.send(sendPacket);

                try
                {
                    byte[] receiveData = new byte[MAX_MSG_LEN];
                    DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
                    clientSocket.receive(receivePacket);

                    long receiveTime = System.currentTimeMillis();
                    String responseMsg = new String(receivePacket.getData(), 0, receivePacket.getLength()).trim();

                    System.out.println("Received reply: " + responseMsg + " for request " + i);

                    long rtt = receiveTime - sendTime;
                    System.out.println("Request " + i + ": RTT = " + rtt + " ms");
                    rttSamples.add(rtt);

                    if (rttSamples.size() == 1)
                    {
                        estimatedRTT = rtt;
                        devRTT = rtt / 2.0;
                    }
                    else
                    {
                        estimatedRTT = (1 - ALPHA) * estimatedRTT + ALPHA * rtt;
                        devRTT = (1 - BETA) * devRTT + BETA * Math.abs(rtt - estimatedRTT);
                    }

                    estimatedRTTList.add(estimatedRTT);
                    devRTTList.add(devRTT);
                }
                catch (SocketTimeoutException e)
                {
                    System.out.println("Request " + i + ": no reply (timed out)");
                }
            }

            System.out.println("Monitoring for delayed replies...");
            clientSocket.setSoTimeout(MONITOR_TIMEOUT);

            try
            {
                while (true)
                {
                    byte[] receiveData = new byte[MAX_MSG_LEN];
                    DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
                    clientSocket.receive(receivePacket);

                    String responseMsg = new String(receivePacket.getData(), 0, receivePacket.getLength()).trim();
                    System.out.println("Delayed reply received: " + responseMsg);
                }
            }
            catch (SocketTimeoutException e)
            {
                System.out.println("No more delayed replies, terminating.");
            }

            System.out.println("\nSummary of RTT Measurements:");

            for (int i = 0; i < rttSamples.size(); i++)
            {
                long rtt = rttSamples.get(i);
                System.out.println("Update " + i + ": SampleRTT = " + rtt + " ms");
            }

            if (!rttSamples.isEmpty())
            {
                System.out.println("\nFinal Calculations:");
                System.out.println("EstimatedRTT = " + String.format("%.2f", estimatedRTT) + " ms");
                System.out.println("DevRTT = " + String.format("%.2f", devRTT) + " ms");
            }
            else
            {
                System.out.println("\nNo successful RTT samples to calculate EstimatedRTT or DevRTT.");
            }
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
        finally
        {
            if (clientSocket != null)
            {
                clientSocket.close();
            }
        }

        // Plot the data
        if (!rttSamples.isEmpty())
        {
            plotRTT.plotData(rttSamples, estimatedRTTList, devRTTList);
        }
    }
}
