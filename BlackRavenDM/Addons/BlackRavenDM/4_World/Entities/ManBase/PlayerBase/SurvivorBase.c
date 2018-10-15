modded class SurvivorBase
{
    private string PlayerID = "empty";
    private string PlayerFullName = "empty";

    string GetPlayerID()
    {
        return PlayerID;
    }

    void SetPlayerID(string IdentityID)
    {
        PlayerID = IdentityID;
    }

    string GetPlayerFullName()
    {
        return PlayerFullName;
    }

    void SetPlayerFullName(string name)
    {
        PlayerFullName = name;
    }
}
