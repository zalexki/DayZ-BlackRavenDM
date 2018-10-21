modded class PlayerBase
{
    int kills = 0;
    ref SimpleKillLogger SKL;

    void KillStreakHandler(Object killer)
    {
        if (killer.IsInherited(PlayerBase)) {
            PlayerBase playerKiller = killer;
            playerKiller.kills++;

            playerKiller.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>("You have " + playerKiller.kills.ToString() + " kills."), false, playerKiller.GetIdentity());
            playerKiller.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>(""), false, playerKiller.GetIdentity()); // skip line

            string message = "";
            switch ( playerKiller.kills ) {
                case 5:
                    message = " is on a killing spree!";
                break;
                case 10:
                    message = " is dominating!";
                break;
                case 15:
                    message = " is Unstoppable!";
                break;
                case 20:
                    message = " is Unstoppable!";
                break;
                case 25:
                    message = " is Godlike!";
                break;
            }

            if (message != "") {
                ref array< Man > allPlayers = new ref array< Man >;
                GetGame().GetWorld().GetPlayerList(allPlayers);

                foreach( PlayerBase serverPlayer : allPlayers ) {
                    serverPlayer.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>(playerKiller.GetIdentity().GetName() + message), false, serverPlayer.GetIdentity());
                    serverPlayer.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>(""), false, serverPlayer.GetIdentity()); // skip line
                }
            }

            // PlayerBase seems to be delete/wipe when you die this might not be needed
            kills = 0;
        }
    }

    void KillFeedChat(Object killer, SurvivorBase sbKilled)
    {
        if (killer.IsMan()) {
            Man manKiller = Man.Cast(killer);
            if (sbKilled.GetPlayerFullName() == manKiller.GetIdentity().GetName()) {
                GetGame().ChatPlayer( 0, sbKilled.GetPlayerFullName() + " committed suicide");
            } else {
                SurvivorBase sbKiller = SurvivorBase.Cast(killer);
                GetGame().ChatPlayer( 0, sbKilled.GetPlayerFullName() + " killed By " + sbKiller.GetPlayerFullName());
            }
        } else {
            GetGame().ChatPlayer( 0, sbKilled.GetPlayerFullName() + " killed By zombie/bleedout");
        }
    }

    override void EEKilled(Object killer)
    {
        KillStreakHandler(killer);

        ref SurvivorBase sbKilled = this;
        ref SurvivorBase sbKiller = killer;
        ref Man manKiller = killer;

        if (GetGame().IsServer()) {
            if (manKiller.IsMan() && sbKiller.GetPlayerID() != sbKilled.GetPlayerID()) {
                SKL.KillHandler(sbKiller.GetPlayerID());
            }
            SKL.DeathHandler(sbKilled.GetPlayerID());
        }

        // This seems to increase crash server, anyway it spam too much because of our DeathMatch setup
        // KillFeedChat(killer, sbKilled);

        if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT) {
            // @NOTE: this branch does not happen, EEKilled is called only on server
            if (GetGame().GetPlayer() == this) {
                super.EEKilled(killer);
            }
            if (GetHumanInventory().GetEntityInHands())
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ServerDropEntity,1000,false,( GetHumanInventory().GetEntityInHands() ));
        }
        else if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_SERVER) {
            if (GetBleedingManager()) { delete GetBleedingManager(); };
            // This delete player body and weapon in 60s
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DeleteEntity, 90000, false, this);

            if (GetHumanInventory().GetEntityInHands() ) {
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ServerDropEntity, 1000,false,( GetHumanInventory().GetEntityInHands() ));
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DeleteEntity, 90000, false, ( GetHumanInventory().GetEntityInHands() ));
            }
        }

        if ( GetSoftSkillManager() ) { delete GetSoftSkillManager(); }

        GetStateManager().OnPlayerKilled();

        // kill character in database
        if (GetHive()) { GetHive().CharacterKill(this); }
    }

	void DeleteEntity(EntityAI entity)
    {
        ItemBase IBGun = ItemBase.Cast(entity);
        if (IBGun.IsInherited(Weapon)) {
             // If weapon has no parent it's on the ground
            if (IBGun.GetHierarchyParent() == null) { entity.Delete(); }
        } else {
            entity.Delete();
        }
    }

    override void OnConnect()
    {
        Debug.Log("Player connected:"+this.ToString(),"Connect");

        ref SurvivorBase sb = this;
        sb.SetPlayerID(this.GetIdentity().GetPlainId());
        sb.SetPlayerFullName(this.GetIdentity().GetName());

        SKL = new SimpleKillLogger;
        SKL.OnConnectHandler(sb.GetPlayerID(), sb.GetPlayerFullName());

        // NEW STATS API
        StatRegister("playtime");
        StatRegister("dist");
    }
}