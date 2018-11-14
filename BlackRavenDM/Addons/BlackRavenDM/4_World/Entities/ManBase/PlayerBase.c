modded class PlayerBase
{
    int kills = 0;
    ref SimpleKillLogger SKL;

    override void OnConnect()
    {
        Debug.Log("Player connected:"+this.ToString(),"Connect");

        ref SurvivorBase sb = SurvivorBase.Cast(this);
        sb.SetPlayerID(this.GetIdentity().GetPlainId());
        sb.SetPlayerFullName(this.GetIdentity().GetName());

        SKL = new SimpleKillLogger;
        SKL.OnConnectHandler(sb.GetPlayerID(), sb.GetPlayerFullName());

        // NEW STATS API
        StatRegister("playtime");
        StatRegister("dist");
    }

    override void EEKilled( Object killer )
    {
        Print("DEBUG: EEKilled, you have died");

        KillFeedChat(killer, this);
        KillStreakHandler(killer);

        ref SurvivorBase sbKilled = SurvivorBase.Cast(this);
        ref SurvivorBase sbKiller = SurvivorBase.Cast(killer);

        if (GetGame().IsServer()) {
            if (killer.IsMan()) {
                if (sbKiller.GetPlayerID() != sbKilled.GetPlayerID()) {
                    SKL.KillHandler(sbKiller.GetPlayerID(), sbKiller.GetPosition());
                }
            }
            SKL.DeathHandler(sbKilled.GetPlayerID(), sbKilled.GetPosition());
        }

        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DeleteEntity, 300000, false, this);
        DayZPlayerSyncJunctures.SendDeath(this, -1, 0);

        if (GetBleedingManagerServer()) delete GetBleedingManagerServer();

        if (GetHumanInventory().GetEntityInHands()) {
            if (CanDropEntity(this)) {
                if (!IsRestrained()) {
                    GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ServerDropEntity, 1000, false, ( GetHumanInventory().GetEntityInHands() ));
                    GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DeleteEntity, 300000, false, ( GetHumanInventory().GetEntityInHands() ));
                }
            }

        }

        // kill character in database
        if (GetHive()) {
            GetHive().CharacterKill(this);
        }

        // disable voice communication
        GetGame().EnableVoN(this, false);


        if (GetSoftSkillManager()) {
            delete GetSoftSkillManager();
        }

        GetSymptomManager().OnPlayerKilled();
    }

    void KillFeedChat(Object killer, PlayerBase pbKilled)
    {
        SurvivorBase sbKilled = SurvivorBase.Cast(pbKilled);

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

    void KillStreakHandler(Object killer)
    {
        if (killer.IsInherited(PlayerBase)) {
            PlayerBase playerKiller = PlayerBase.Cast(killer);
            playerKiller.kills++;

            playerKiller.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>("You have " + playerKiller.kills.ToString() + " kills."), false, playerKiller.GetIdentity());
            playerKiller.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>(""), false, playerKiller.GetIdentity()); // skip line

            string message = "";
            switch (playerKiller.kills) {
                case 5:
                    message = " is on a killing spree!";
                break;
                case 10:
                    message = " is dominating!";
                break;
                case 15:
                    message = " is unstoppable!";
                break;
                case 20:
                    message = " is unstoppable!";
                break;
                case 25:
                    message = " is Godlike!";
                break;
                case 30:
                    message = " is at 30 kills is he cheating ?";
                break;
            }

            if (message != "") {
                ref array< Man > allPlayers = new ref array< Man >;
                GetGame().GetWorld().GetPlayerList(allPlayers);

                foreach( Man man : allPlayers ) {
                    PlayerBase playerBase = PlayerBase.Cast(man);
                    playerBase.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>(playerKiller.GetIdentity().GetName() + message), false, playerBase.GetIdentity());
                    playerBase.RPCSingleParam(ERPCs.RPC_USER_ACTION_MESSAGE, new Param1<string>(""), false, playerBase.GetIdentity()); // skip line
                }
            }

            // PlayerBase seems to be delete/wipe when you die this might not be needed
            kills = 0;
        }
    }

    void DeleteEntity(EntityAI entity)
    {
        ItemBase IBGun = ItemBase.Cast(entity);
        if (IBGun != NULL && IBGun.IsInherited(Weapon)) {
             // If weapon has no parent it's on the ground
            if (IBGun.GetHierarchyParent() == null) { entity.Delete(); }
        } else {
            entity.Delete();
        }
    }
}
