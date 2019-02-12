#include "$CurrentDir:\\mpmissions\\BlackRavenDM.chernarusplus\\Modules\\AdminTool\\AdminTool.c"
#include "$CurrentDir:\\mpmissions\\BlackRavenDM.chernarusplus\\Modules\\BetterAirdrop\\BetterAirdrop.c"
#include "$CurrentDir:\\mpmissions\\BlackRavenDM.chernarusplus\\Modules\\ServerMission\\BlackRavenChat.c"

class BlackRavenMission: MissionServer
{
    ref BetterAirdrop AirDrop;
    ref AdminTool adminTool;

    void BlackRavenMission()
    {
        // Declaring this remove NULL pointer to instance errors when calling Equip methods
        // maybe it's a best practice ?
    }

    override void OnInit()
    {
        super.OnInit();

        // Init deps
        AirDrop = new BetterAirdrop();
        adminTool = new AdminTool(this);
        adminTool.Init();

        // clean up map at airfield and prison every 20min + chat msg every 10min
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CleanupMapAt, 1200000, true, "4652 300 10327", 3000);
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CleanupMapAt, 1200000, true, "2650 300 1330", 1000);
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(RandomMsg, 600000, true);
    }

    override void OnEvent(EventType eventTypeId, Param params)
    {
        super.OnEvent(eventTypeId,params);

        switch(eventTypeId) {
            case ChatMessageEventTypeID:
                 ChatMessageEventParams chat_params = ChatMessageEventParams.Cast(params);
                 if (chat_params.param1 == 0 && chat_params.param2 != "") { //trigger only when channel is Global == 0 and Player Name does not equal to null
                    Param4<int,string,string,string> request_info = new Param4<int,string,string,string>(chat_params.param1, chat_params.param2, chat_params.param3, chat_params.param4);
                    adminTool.RequestHandler(request_info); //Send the param to Admintools
                 }
            break;
        }
    }

    override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
    {
        player.RemoveAllItems();

        // Set max water and stomach
        player.GetStatWater().Set(player.GetStatWater().GetMax());
        player.GetStatEnergy().Set(player.GetStatEnergy().GetMax());
        player.GetStatStomachEnergy().Set(1000);
        player.GetStatStomachWater().Set(1000);

        if (EquipVIP(player)) {
            EquipBaseItems(player);
        } else {
            EquipRandomStuff(player);
            EquipRandomWeapon(player);
            EquipBaseItems(player);
        }
    }

    void EquipRandomStuff(PlayerBase player)
    {
        EntityAI itemEnt;
        ItemBase itemBs;
        HumanInventory humanInventory = player.GetHumanInventory();

        switch (Math.RandomIntInclusive(0, 16)) {
            /* Bob sniper */
            case 0:
                humanInventory.CreateInInventory("BoonieHat_Flecktran");
                humanInventory.CreateInInventory("SportGlasses");
                humanInventory.CreateInInventory("highcapacityvest_olive");
                humanInventory.CreateInInventory("USMCJacket_Woodland");
                humanInventory.CreateInInventory("TacticalGloves_Green");
                humanInventory.CreateInInventory("USMCPants_Woodland");
                humanInventory.CreateInInventory("JungleBoots_Olive");
            break;

            /* Soldat Assault */
            case 1:
                humanInventory.CreateInInventory("BallisticHelmet_Green");
                humanInventory.CreateInInventory("highcapacityvest_olive");
                humanInventory.CreateInInventory("GorkaEJacket_PautRev");
                humanInventory.CreateInInventory("TacticalGloves_Black");
                humanInventory.CreateInInventory("GorkaPants_PautRev");
                humanInventory.CreateInInventory("CombatBoots_Black");
            break;

            /* Fun */
            case 2:
                humanInventory.CreateInInventory("DallasMask");
                humanInventory.CreateInInventory("FurCourierBag");
                humanInventory.CreateInInventory("MiniDress_Pink");
                humanInventory.CreateInInventory("Ballerinas_Blue");
            break;

            /* Soldat Leger */
            case 3:
                humanInventory.CreateInInventory("BalaclavaMask_Blackskull");
                humanInventory.CreateInInventory("highcapacityvest_black");
                humanInventory.CreateInInventory("M65Jacket_Black");
                humanInventory.CreateInInventory("TacticalGloves_Black");
                humanInventory.CreateInInventory("CargoPants_Black");
                humanInventory.CreateInInventory("CombatBoots_Black");
            break;

            /* Pompier */
            case 4:
                humanInventory.CreateInInventory("FirefightersHelmet_Yellow");
                humanInventory.CreateInInventory("LeatherSack_Black");
                humanInventory.CreateInInventory("FirefighterJacket_Black");
                humanInventory.CreateInInventory("TacticalGloves_Black");
                humanInventory.CreateInInventory("FirefightersPants_Black");
                humanInventory.CreateInInventory("CombatBoots_Black");
            break;

            /* Paramedic */
            case 5:
                humanInventory.CreateInInventory("SportGlasses");
                humanInventory.CreateInInventory("LeatherSack_Black");
                humanInventory.CreateInInventory("ParamedicJacket_Blue");
                humanInventory.CreateInInventory("ParamedicPants_Blue");
                humanInventory.CreateInInventory("LeatherShoes_Black");
            break;

            /* medic */
            case 6:
                humanInventory.CreateInInventory("MedicalScrubsHat_White");
                humanInventory.CreateInInventory("LeatherSack_Black");
                humanInventory.CreateInInventory("MedicalScrubsShirt_White");
                humanInventory.CreateInInventory("MedicalScrubsPants_White");
                humanInventory.CreateInInventory("NBCBootsGray");
            break;

            /* Police */
            case 7:
                humanInventory.CreateInInventory("PoliceCap");
                humanInventory.CreateInInventory("PoliceJacket");
                humanInventory.CreateInInventory("PolicePants");
                humanInventory.CreateInInventory("CombatBoots_Grey");
            break;

            /* Prison */
            case 8:
                humanInventory.CreateInInventory("PrisonerCap");
                humanInventory.CreateInInventory("ChildBag_Blue");
                humanInventory.CreateInInventory("PrisonUniformJacket");
                humanInventory.CreateInInventory("PrisonUniformPants");
            break;

            /* Ghillie */
            case 9:
                humanInventory.CreateInInventory("GhillieHood_Woodland");
                humanInventory.CreateInInventory("GhillieSuit_Woodland");
                humanInventory.CreateInInventory("AviatorGlasses");
                humanInventory.CreateInInventory("TacticalGloves_Green");
                humanInventory.CreateInInventory("QuiltedJacket_Green");
                humanInventory.CreateInInventory("HunterPants_Autumn");
                humanInventory.CreateInInventory("CombatBoots_Green");
            break;

            /* Rebelle */
            case 10:
                humanInventory.CreateInInventory("BandanaMask_BlackPattern");
                humanInventory.CreateInInventory("BandanaUntieable_BlackPattern");
                humanInventory.CreateInInventory("FurImprovisedBag");
                humanInventory.CreateInInventory("Jeans_Blue");
                humanInventory.CreateInInventory("TShirt_Black");
                humanInventory.CreateInInventory("AthleticShoes_Black");
            break;

            /* Masque a gaz */
            case 11:
                humanInventory.CreateInInventory("GP5GasMask");
                humanInventory.CreateInInventory("NBCHoodGray");
                humanInventory.CreateInInventory("NBCGlovesGray");
                humanInventory.CreateInInventory("NBCBootsGray");
                humanInventory.CreateInInventory("NBCJacketGray");
                humanInventory.CreateInInventory("BDUPants");
            break;

            /* Cow-Boy */
            case 12:
                humanInventory.CreateInInventory("CowboyHat_Brown");
                humanInventory.CreateInInventory("DesignerGlasses");
                humanInventory.CreateInInventory("FurCourierBag");
                humanInventory.CreateInInventory("ChestHolster");
                humanInventory.CreateInInventory("LeatherPants_Brown");
                humanInventory.CreateInInventory("LeatherShoes_Brown");
                humanInventory.CreateInInventory("LeatherJacket_Brown");
            break;

            /* FUN 2 */
            case 13:
                humanInventory.CreateInInventory("BalaclavaMask_Pink");
                humanInventory.CreateInInventory("Breeches_Pink");
                humanInventory.CreateInInventory("MiniDress_PinkChecker");
                humanInventory.CreateInInventory("Sneakers_White");
                humanInventory.CreateInInventory("ChildBag_Red");
            break;

            /* CRS */
            case 14:
                humanInventory.CreateInInventory("BalaclavaMask_Black");
                humanInventory.CreateInInventory("GorkaHelmetComplete_Black");
                humanInventory.CreateInInventory("CargoPants_Black");
                humanInventory.CreateInInventory("TacticalGloves_Black");
                humanInventory.CreateInInventory("UKAssVest_Black");
                humanInventory.CreateInInventory("TacticalShirt_Black");
                humanInventory.CreateInInventory("CombatBoots_Black");
            break;

            /* Chevalier */
            case 15:
                if (Math.RandomIntInclusive(0,1) > 0) {
                    humanInventory.CreateInInventory("GreatHelm");
                    humanInventory.CreateInHands("Sword");
                    humanInventory.CreateInInventory("NurseDress_Blue");
                    humanInventory.CreateInInventory("Wellies_Black");
                    humanInventory.CreateInInventory("MaleBorisHead");
                } else {
                    EquipRandomStuff(player);
                }
            break;

            /* Bucheron */
            case 16:
                humanInventory.CreateInInventory("HoxtonMask");
                humanInventory.CreateInInventory("WorkingGloves_Black");
                humanInventory.CreateInInventory("BomberJacket_Black");
                humanInventory.CreateInInventory("CargoPants_Black");
                humanInventory.CreateInInventory("Wellies_Black");
                humanInventory.CreateInInventory("MaleBorisHead");
//              disabled because chainsaw is shit, doesnt make sound or even one or two shot ...
//                humanInventory.CreateInHands("Chainsaw").GetInventory().CreateAttachment("Battery9V");
            break;
        }
    }

    void EquipRandomWeapon(PlayerBase player)
    {
        EntityAI itemEnt;
        ItemBase itemBs;
        HumanInventory humanInventory = player.GetHumanInventory();

        switch (Math.RandomInt(0, 8)) {
            case 0:
                itemEnt = humanInventory.CreateInHands( "CZ61" );
                player.SetQuickBarEntityShortcut(itemEnt, 1);
                humanInventory.CreateInInventory("Mag_CZ61_20rnd");
                humanInventory.CreateInInventory("Mag_CZ61_20rnd");
                humanInventory.CreateInInventory("Mag_CZ61_20rnd");
                humanInventory.CreateInInventory("Mag_CZ61_20rnd");
            break;

            case 1:
                itemEnt = humanInventory.CreateInHands("SVD");
                itemEnt.GetInventory().CreateAttachment("PSO11Optic");
                itemEnt.GetInventory().CreateAttachment("GhillieAtt_Woodland");
                player.SetQuickBarEntityShortcut(itemEnt, 1);

                humanInventory.CreateInInventory("Mag_SVD_10Rnd");
                humanInventory.CreateInInventory("Mag_SVD_10Rnd");
                humanInventory.CreateInInventory("Mag_SVD_10Rnd");
                humanInventory.CreateInInventory("Mag_SVD_10Rnd");
                humanInventory.CreateInInventory("Mag_SVD_10Rnd");
            break;

            case 2:
                itemEnt = humanInventory.CreateInHands("Mosin9130");
                itemEnt.GetInventory().CreateAttachment("PUScopeOptic");
                itemEnt.GetInventory().CreateAttachment("Mosin_Compensator");
                player.SetQuickBarEntityShortcut(itemEnt, 0);

                itemEnt = humanInventory.CreateInInventory("Ammo_762x54");
                player.SetQuickBarEntityShortcut(itemEnt, 1);
                itemEnt = humanInventory.CreateInInventory("Ammo_762x54");
                itemBs = ItemBase.Cast(itemEnt);
                itemBs.SetQuantity(5);
                itemEnt = humanInventory.CreateInInventory("Ammo_762x54");
                itemBs = ItemBase.Cast(itemEnt);
                itemBs.SetQuantity(5);
                itemEnt = humanInventory.CreateInInventory("Ammo_762x54");
                itemBs = ItemBase.Cast(itemEnt);
                itemBs.SetQuantity(5);
            break;

            case 3:
                itemEnt = humanInventory.CreateInHands( "M4A1" );
                itemEnt.GetInventory().CreateAttachment( "M4_RISHndgrd_Black" );
                itemEnt.GetInventory().CreateAttachment( "M4_MPBttstck_Black" );
                itemEnt.GetInventory().CreateAttachment( "ACOGOptic" );
                humanInventory.CreateInInventory( "M68Optic" ).GetInventory().CreateAttachment("Battery9V");

                player.SetQuickBarEntityShortcut(itemEnt, 0);

                humanInventory.CreateInInventory("Mag_STANAGCoupled_30Rnd");
                humanInventory.CreateInInventory("Mag_STANAGCoupled_30Rnd");
                humanInventory.CreateInInventory("Mag_STANAGCoupled_30Rnd");
            break;

            case 4:
                itemEnt = humanInventory.CreateInHands("AKM");
                itemEnt.GetInventory().CreateAttachment("PSO1Optic");
                itemEnt.GetInventory().CreateAttachment("AK_WoodBttstck");
                itemEnt.GetInventory().CreateAttachment("AK_WoodHndgrd");
                player.SetQuickBarEntityShortcut(itemEnt, 0);

                humanInventory.CreateInInventory("Mag_AKM_30Rnd");
                humanInventory.CreateInInventory("Mag_AKM_30Rnd");
                humanInventory.CreateInInventory("Mag_AKM_30Rnd");
            break;

            case 5:
                itemEnt = humanInventory.CreateInHands( "UMP45" );
                itemEnt.GetInventory().CreateAttachment( "M68Optic" ).GetInventory().CreateAttachment("Battery9V");
                itemEnt.GetInventory().CreateAttachment("PistolSuppressor");
                player.SetQuickBarEntityShortcut(itemEnt, 0);

                itemEnt = humanInventory.CreateInInventory("Mag_UMP_25Rnd");
                humanInventory.CreateInInventory("Mag_UMP_25Rnd");
                humanInventory.CreateInInventory("Mag_UMP_25Rnd");
                humanInventory.CreateInInventory("Mag_UMP_25Rnd");
            break;

            case 6:
                itemEnt = humanInventory.CreateInHands("MP5K");
                itemEnt.GetInventory().CreateAttachment( "M68Optic" ).GetInventory().CreateAttachment("Battery9V");
                itemEnt.GetInventory().CreateAttachment("MP5_PlasticHndgrd");
                itemEnt.GetInventory().CreateAttachment("MP5k_StockBttstck");
                itemEnt.GetInventory().CreateAttachment("PistolSuppressor");

                player.SetQuickBarEntityShortcut(itemEnt, 0);

                humanInventory.CreateInInventory("Mag_MP5_30Rnd");
                humanInventory.CreateInInventory("Mag_MP5_30Rnd");
                humanInventory.CreateInInventory("Mag_MP5_30Rnd");
                humanInventory.CreateInInventory("Mag_MP5_30Rnd");
            break;

            case 7:
                itemEnt = humanInventory.CreateInHands("Mp133Shotgun");
                player.SetQuickBarEntityShortcut(itemEnt, 0);

                itemEnt = humanInventory.CreateInInventory("Ammo_12gaPellets");
                itemBs = ItemBase.Cast(itemEnt);
                itemBs.SetQuantity(10);
                player.SetQuickBarEntityShortcut(itemEnt, 1);
                itemEnt = humanInventory.CreateInInventory("Ammo_12gaPellets");
                itemBs = ItemBase.Cast(itemEnt);
                itemBs.SetQuantity(10);
                itemEnt = humanInventory.CreateInInventory("Ammo_12gaPellets");
                itemBs = ItemBase.Cast(itemEnt);
                itemBs.SetQuantity(10);
            break;
        }
    }

    void EquipBaseItems(PlayerBase player)
    {
        EntityAI itemEnt;
        ItemBase itemBs;
        HumanInventory humanInventory;
        humanInventory = player.GetHumanInventory();

        itemEnt = humanInventory.CreateInInventory("Rag");
        itemBs = ItemBase.Cast(itemEnt);
        itemBs.SetQuantity(6);
        player.SetQuickBarEntityShortcut(itemEnt, 3);

        player.GetHumanInventory().CreateInInventory("SalineBagIV");
        player.GetHumanInventory().CreateInInventory("Morphine");

        itemEnt = player.GetHumanInventory().CreateInInventory("CZ75");
        player.SetQuickBarEntityShortcut(itemEnt, 2);

        player.GetHumanInventory().CreateInInventory("Mag_CZ75_15Rnd");
        player.GetHumanInventory().CreateInInventory("Mag_CZ75_15Rnd");
    }

    bool EquipVIP(PlayerBase player)
    {
        EntityAI itemEnt;
        ItemBase itemBs;
        bool isVIP = false;

        switch (player.GetIdentity().GetPlainId()) {
            case "76561198050192920":
                player.GetInventory().CreateInInventory("GorkaHelmetComplete_Green");
                player.GetInventory().CreateInInventory("BalaclavaMask_Green");
                player.GetInventory().CreateInInventory("GorkaEJacket_Summer");
                player.GetInventory().CreateInInventory("WorkingGloves_Beige");
                player.GetInventory().CreateInInventory("GorkaPants_Summer");
                player.GetInventory().CreateInInventory("JungleBoots_Green");
                player.GetInventory().CreateInInventory("AviatorGlasses");
                player.GetInventory().CreateInInventory("HuntingBag");
                player.GetInventory().CreateInInventory("BandageDressing");
                player.GetInventory().CreateInInventory("BandageDressing");

                itemEnt = player.GetHumanInventory().CreateInHands( "M4A1" );
                itemEnt.GetInventory().CreateAttachment( "M4_RISHndgrd_Black" );
                itemEnt.GetInventory().CreateAttachment( "M4_MPBttstck_Black" );
                itemEnt.GetInventory().CreateAttachment( "ImprovisedSuppressor" );
                itemEnt.GetInventory().CreateAttachment( "ACOGOptic" );
                player.SetQuickBarEntityShortcut(itemEnt, 0);

                player.GetInventory().CreateInInventory("Mag_STANAGCoupled_30Rnd");
                player.GetInventory().CreateInInventory("Mag_STANAGCoupled_30Rnd");
                player.GetInventory().CreateInInventory("Mag_STANAGCoupled_30Rnd");

                itemEnt = player.GetInventory().CreateInInventory("SVD");
                itemEnt.GetInventory().CreateAttachment( "PSO11Optic" );
                itemEnt.GetInventory().CreateAttachment( "GhillieAtt_Woodland" );
                itemEnt.GetInventory().CreateAttachment( "ImprovisedSuppressor" );
                player.SetQuickBarEntityShortcut(itemEnt, 1);

                player.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
                player.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
                player.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
                player.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
                player.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
                player.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
                player.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
                player.GetInventory().CreateInInventory("Mag_SVD_10Rnd");

                player.GetInventory().CreateInInventory("MakarovIJ70");
                player.GetInventory().CreateInInventory("Mag_IJ70_8Rnd");
                player.GetInventory().CreateInInventory("Mag_IJ70_8Rnd");
                player.GetInventory().CreateInInventory("Mag_IJ70_8Rnd");
                isVIP = true;
            break;
        }

        return isVIP;
    }

    void CleanupMapAt(string pos, float radius)
    {
        Print("DEBUG: CleanupMap");
        array<Object> objects = new array<Object>();
        array<CargoBase> proxyCargos = new array<CargoBase>();
        GetGame().GetObjectsAtPosition(pos.ToVector(), radius, objects, proxyCargos);

        Print("DEBUG: objects count "+objects.Count());
        int i = 0;
        foreach (Object object : objects) {
            ItemBase itemBase = ItemBase.Cast(object);
            if (itemBase != NULL) {
                 // If is itemBase and has no parent it's on the ground
                if (itemBase.GetHierarchyParent() == null) { itemBase.Delete(); i++; }
            }
        }
        Print("DEBUG: "+i+" items deleted");
    }
};
