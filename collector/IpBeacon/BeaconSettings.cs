namespace IpBeacon;

public class BeaconSettings
{
    public required string CallMessage { get; init; }
    public required int ClientPort { get; init; }

    public required string BrokerPort { get; init; }
}