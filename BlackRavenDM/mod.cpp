class CfgMods
{
    class BlackRavenDM
    {
        dir = "BlackRavenDM";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "BlackRaven DeathMatch";
        credits = "zalexki, BlackRavenCommunity, DannyDog, DayzModders Discord, DaOne";
        author = "zalexki";
        authorID = "0";
        version = "1.0";
        extra = 0;
        type = "mod";

        dependencies[] = {"World"};

        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"BlackRavenDM/scripts/4_World"};
            }
        }
    }
}