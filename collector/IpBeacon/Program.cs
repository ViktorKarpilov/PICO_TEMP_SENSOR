using System.Net;
using System.Net.Sockets;
using System.Text;
using IpBeacon;
using Microsoft.Extensions.Configuration;

var settings = new ConfigurationBuilder()
    .AddJsonFile("./appsettings.json")
    .Build()
    .GetRequiredSection(nameof(BeaconSettings))
    .Get<BeaconSettings>();

if (settings == null)
{
    throw new Exception("Settings not found");
}

var deviceIp = GetLocalIpAddress();
Console.WriteLine($"Device IP: {deviceIp}");

using var client = new UdpClient(settings.ClientPort);
    
while (true)
{
    try {
        var result = await client.ReceiveAsync();
        Console.WriteLine(Encoding.UTF8.GetString(result.Buffer));
        Console.WriteLine($"Is call message: {Encoding.ASCII.GetString(result.Buffer) == settings.CallMessage}");
        if (Encoding.ASCII.GetString(result.Buffer) == settings.CallMessage) {
            Console.WriteLine($"Received from: {result.RemoteEndPoint}");
            Console.WriteLine($"Sending response to: {result.RemoteEndPoint}");
            var response = $"{{\"broker_ip\":\"{deviceIp}\",\"port\":{settings.BrokerPort}}}";
            await client.SendAsync(Encoding.ASCII.GetBytes(response), response.Length, result.RemoteEndPoint);
            Console.WriteLine($"Response sent!");
        }
    }
    catch (OperationCanceledException) { break; }
}

return;

static string GetLocalIpAddress()
{
    using var socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, 0);
    socket.Connect("8.8.8.8", 65530);

    return socket.LocalEndPoint is not IPEndPoint endPoint
        ? throw new Exception("Local IP address not found")
        : endPoint.Address.ToString();
}
