void ChatPlayersCountAndDisplayName()
{
    array<Man> players = new array<Man>;
    GetGame().GetPlayers(players);
    GetGame().ChatPlayer( 0, "Online players = " + players.Count() );
    GetGame().ChatPlayer( 0, "This server restarts 2AM 8AM 2PM 7PM");
    GetGame().ChatPlayer( 0, "https://discord.gg/U5Df2eU");
}

void CreateChatSpace()
{
    ref array< Man > allPlayers = new ref array< Man >;
    GetGame().GetWorld().GetPlayerList(allPlayers);

    foreach( Man serverMan : allPlayers ) {
        PlayerBase serverPlayer = PlayerBase.Cast(serverMan);
        serverPlayer.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>(""), false, serverPlayer.GetIdentity());
    }
}

private int MsgNB = 0;
void RandomMsg()
{
    switch (MsgNB) {
        case 0:
            GetGame().ChatPlayer( 0, "This server is paid on donations, Please consider donating at Black-Raven.fr/dayz, thank you.");
        break;

        case 1:
            GetGame().ChatPlayer( 0, "Teamspeak : ts.black-raven.fr");
        break;

        case 2:
            GetGame().ChatPlayer( 1, "Modded by zalexki, DannyDog");
        break;

        case 3:
            GetGame().ChatPlayer( 1, "NEW VIP CLASS ! : Black-Raven.fr/dayz");
        break;

        case 4:
            GetGame().ChatPlayer( 1, "Top Donors : @BR_ASGARD");
        break;

        case 5:
            GetGame().ChatPlayer( 0, "This server is hosted by Synhostinger.com");
        break;

        case 6:
            GetGame().ChatPlayer( 0, "Official Partner : twitch.tv/jamaltv");
        break;

        case 7:
            ChatPlayersCountAndDisplayName();
        break;
    }
    CreateChatSpace();

    MsgNB++;
    if (MsgNB > 7) { MsgNB = 0; }
}
