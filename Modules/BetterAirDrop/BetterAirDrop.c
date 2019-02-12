/*
    Authored by: philipj
    Modified by: zalexki
*/

class BetterAirdropLocation {
    float x, y;
    string name;
    bool disclose, send_proximity;
    ref TStringArray items;
    void BetterAirdropLocation(float x, float y, string name, bool disclose = false, bool send_proximity = false) {
        this.x = x;
        this.y = y;
        this.name = name;
        this.disclose = disclose;
        this.send_proximity = send_proximity;
    }
};

class BetterAirdrop {
    bool debug_mode = false; // display d2t & port to target

	float interval = 25.0; // Interval in minutes
    float initial = 1.0;
	float plane_height = 100; // relative to ground
    float proximity_warning_distance = 500;

    float speed = 0.3;

	bool display_basic_information = true;
	bool display_coordinates = true;
	bool smoke_signal = true; // not working

    int infected_count = 10;
    int items_to_be_spawned = 0;

    int number_of_drops = 10; // number of drops must match the number amount of airdrop locations
    ref BetterAirdropLocation airdrop_locations[] = {
        new BetterAirdropLocation(4451.0, 10220.0, "NWAF", true, true),
        new BetterAirdropLocation(4060, 10911, "NWAF", true, true),
        new BetterAirdropLocation(4232, 10635, "NWAF", true, true),
        new BetterAirdropLocation(4652, 10320, "NWAF", true, true),
        new BetterAirdropLocation(4535, 10680, "NWAF", true, true),
        new BetterAirdropLocation(4879, 9497, "NWAF", true, true),
        new BetterAirdropLocation(4683, 9843, "NWAF", true, true),
        new BetterAirdropLocation(4923, 9972, "NWAF", true, true),
        new BetterAirdropLocation(4711, 10178, "NWAF", true, true),
        new BetterAirdropLocation(4096, 10648, "NWAF", true, true),
    };

	/*
	 * internals
	 */

	int interval_ms = 0;
    int motionless_ticks = 0;

	Object plane;
	vector plane_pos;
    vector spawn_pos;
    vector drop_pos;
    vector orientation;
    float drop_sim_height_diff = -1;

    Object airdrop_obj;
    Object tmp_airdrop_obj;
    EntityAI airdrop_physics;

    Particle signal;
    Particle explosion;
    Particle smoke;

    bool active = false;
    bool dropped = false;
    bool sent_proximity_warning = false;

    ref BetterAirdropLocation active_drop;

	void BetterAirdrop() {
		Print("DEBUG: BetterAirdrop :: Init()");
		interval_ms = interval * 60 * 1000;
        ResetPlane();
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(InitiateAirdrop, initial*60*1000, false);
	}

    void InitiateAirdrop() {
        ResetPlane();
        Cleanup();
        Spawn();
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(InitiateAirdrop, interval*60*1000, false);
    }

    void ResetPlane() {
        GetGame().ObjectDelete(plane);
        plane = GetGame().CreateObject( "Land_Wreck_Lada_Red", "0 0 0", false, true, false );
        smoke = Particle.Play(ParticleList.SMOKING_HELI_WRECK, plane, Vector(0, 0, -1.0)); // not working
        plane_pos[0] = 0;
        plane_pos[1] = 0;
        plane_pos[2] = 0;
        plane.SetPosition(plane_pos);
        dropped = false;
        active = false;
        sent_proximity_warning = false;
        if (active_drop != NULL && active_drop.send_proximity) {
            SendMessageToPlayers("Harry Potter's car has vanished");
        }
    }

    void MovePlane() {
        float angle = Math.Atan2(drop_pos[1]-spawn_pos[1], drop_pos[0]-spawn_pos[0]);
        float fixed_angle = angle*Math.RAD2DEG;

        plane_pos[0] = Math.Cos(angle);
        plane_pos[2] = Math.Sin(angle);
        vector advanced_plane_pos = plane.GetPosition() + (plane_pos*speed);

        advanced_plane_pos[1] = GetGame().SurfaceY(advanced_plane_pos[0], advanced_plane_pos[2]) + plane_height;

        plane.SetPosition( advanced_plane_pos );
        if(!dropped) {
            float distance = Math.Sqrt(Math.Pow(drop_pos[0] - advanced_plane_pos[0], 2) + Math.Pow(drop_pos[1] - advanced_plane_pos[2], 2));
            if (debug_mode) SendMessageToPlayers("DISTANCE2TARGET: "+distance);
            if (distance <= 10.0) {
                dropped = true;
                Drop();
                if(active_drop.send_proximity) {
                    SendMessageToPlayers("Something fell of the car at " + active_drop.x +" "+ active_drop.y);
                    SendMessageToPlayers(" ");
                }
            } else if (distance <= proximity_warning_distance && !sent_proximity_warning) {
                sent_proximity_warning = true;
                if(active_drop.send_proximity) SendMessageToPlayers("Harry Potter's car was spotted in the sky of "+ active_drop.name);
            }
        } else {
            if (!IsInRect(advanced_plane_pos[0], advanced_plane_pos[1], -10.0, 15470.0, -10.0, 1570.0)) {
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(MovePlane);
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ResetPlane, 1000, false);
            }
        }
    }

    void DropSimulation() {
        float ground = GetGame().SurfaceY(airdrop_obj.GetPosition()[0], airdrop_obj.GetPosition()[2]);
//        Print("<BetterAirdrop> GROUND: "+ground+"; HEIGHT: "+airdrop_physics.GetPosition()[1]+"; DIFF: "+drop_sim_height_diff);
        if(drop_sim_height_diff <= 0.0001) motionless_ticks += 1;
        else motionless_ticks = 0;

        if (airdrop_physics.GetPosition()[1] <= (ground) || motionless_ticks >= 25) { // force drop if stuck for more then 500ms
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(DropSimulation);
            vector tmp_pos = airdrop_obj.GetPosition();
            tmp_pos[1] = ground;
            GetGame().ObjectDelete(airdrop_physics);
            airdrop_obj.SetPosition(tmp_pos);
            airdrop_obj.PlaceOnSurface();
            Particle.Play(ParticleList.RDG2, tmp_pos); // not working
            Particle.Play(ParticleList.ROADFLARE_BURNING_MAIN, tmp_pos); // not working
            AfterDrop();
        } else {
            vector airdrop_obj_pos;
            airdrop_obj_pos[0] = drop_pos[0];
            airdrop_obj_pos[1] = airdrop_physics.GetPosition()[1];
            airdrop_obj_pos[2] = drop_pos[1];
            airdrop_obj.SetPosition(airdrop_obj_pos);
            airdrop_obj.SetOrientation(airdrop_physics.GetOrientation());
        }
    }

    // spawn items/wolves
    void AfterDrop() {
        vector base_pos = airdrop_obj.GetPosition(), dynamic_pos;

        // EntityAI MyV3S;
        // vector NewPosition;
        // vector OldPosition;
        // OldPosition = airdrop_obj.GetPosition();
        // NewPosition[0] = OldPosition[0] + 5.5;
        // NewPosition[1] = OldPosition[1] + 0.2;
        // NewPosition[2] = OldPosition[2] + 5.5;
        // MyV3S = GetGame().CreateObject( "OffroadHatchback", NewPosition, false, true, true );
        // MyV3S.GetInventory().CreateAttachment("HatchbackHood");
        // MyV3S.GetInventory().CreateAttachment("HatchbackTrunk");
        // MyV3S.GetInventory().CreateAttachment("HatchbackDoors_CoDriver");
        // MyV3S.GetInventory().CreateAttachment("HatchbackWheel");
        // MyV3S.GetInventory().CreateAttachment("HatchbackWheel");
        // MyV3S.GetInventory().CreateAttachment("HatchbackWheel");
        // MyV3S.GetInventory().CreateAttachment("HatchbackWheel");
        // MyV3S.GetInventory().CreateAttachment("SparkPlug");
        // MyV3S.GetInventory().CreateAttachment("EngineBelt");
        // MyV3S.GetInventory().CreateAttachment("CarBattery");

//        GetGame().ObjectDelete(airdrop_obj);

        for (int i = 0; i < items_to_be_spawned; i++) {
            float a = Math.RandomFloat(0.4, 1.0) * 2 * Math.PI;
            float r = 5.0 * Math.Sqrt(Math.RandomFloat(0.4, 1.0));
            dynamic_pos = base_pos;
            dynamic_pos[0] = dynamic_pos[0]+(r * Math.Cos(a));
            dynamic_pos[2] = dynamic_pos[2]+(r * Math.Sin(a));
            dynamic_pos[1] = GetGame().SurfaceY(dynamic_pos[0], dynamic_pos[2]) + 0.3;
            string item = GetRandomItem();
            GetGame().CreateObject(item, dynamic_pos, false, true);
        }

        for (int inf = 0; inf < infected_count; inf++) {
            dynamic_pos = base_pos;
            dynamic_pos[0] = dynamic_pos[0]+Math.RandomFloat(-20.0, 20.0);
            dynamic_pos[2] = dynamic_pos[2]+Math.RandomFloat(-20.0, 20.0);
            GetGame().CreateObject( WolvesClasses().GetRandomElement(), dynamic_pos, false, true );
        }
    }

    void Drop() {
        motionless_ticks = 0;
        airdrop_physics = EntityAI.Cast(GetGame().CreateObject( "CivilianSedan", plane.GetPosition(), true, true ));
        airdrop_physics.SetAllowDamage(false);
        airdrop_obj = GetGame().CreateObject( "Land_Container_1Bo", plane.GetPosition(), false, true );
        SetVelocity(airdrop_physics, "10 0 0");
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DropSimulation, 10, true);
    }

	void Spawn() {
        GetGame().CreateSoundOnObject(plane, "powerGeneratorLoop", 2000.0, false); // not working
        plane.PlaySoundLoop("powerGeneratorLoop", 2000.0, false); // not working

        // This define 4 points of spawn on axes
        int side = Math.RandomInt(0,4);
        switch(side) {
            case 0: {
                spawn_pos[0] = 3500;
                spawn_pos[1] = Math.RandomFloat(9500, 11000);
                break;
            }
            case 1: {
                spawn_pos[0] = 5500;
                spawn_pos[1] = Math.RandomFloat(9500, 11000);
                break;
            }
            case 2: {
                spawn_pos[0] = Math.RandomFloat(3500, 5500);
                spawn_pos[1] = 9500;
                break;
            }
            case 3: {
                spawn_pos[0] = Math.RandomFloat(3500, 5500);
                spawn_pos[1] = 11000;
                break;
            }
        }

        vector plane_start_pos;
        plane_start_pos[0] = spawn_pos[0];
        plane_start_pos[2] = spawn_pos[1];
        plane.SetPosition(plane_start_pos);
        plane.SetOrigin(plane_start_pos);

        BetterAirdropLocation default_drop = new BetterAirdropLocation(4665, 10107, "default_drop", true, true);
        if (number_of_drops < 1) {
            active_drop = default_drop;
        } else {
            active_drop = airdrop_locations[Math.RandomInt(0, number_of_drops - 1)];
        }
        vector tmp_pos;
        tmp_pos[0] = active_drop.x;
        tmp_pos[1] = GetGame().SurfaceY(active_drop.x, active_drop.y)+1.0;
        tmp_pos[2] = active_drop.y;
        Particle.Play(ParticleList.SMOKING_HELI_WRECK, tmp_pos); // Does not work on client side
        if(debug_mode) TeleportPlayers(tmp_pos);
        if(active_drop.disclose) SendMessageToPlayers("Something in the air has been spotted heading towards " + active_drop.name);

        drop_pos[0] = active_drop.x;
        drop_pos[1] = active_drop.y;

        float angle = Math.Atan2(drop_pos[1]-spawn_pos[1], drop_pos[0]-spawn_pos[0]);
        float fixed_angle = angle*Math.RAD2DEG;

        vector direction = drop_pos-spawn_pos;
        vector normalized = direction.Normalized();
        float distance = direction.Length();
        Print("BetterAirdrop:: INBOUND " + active_drop.name + " AT [X: "+active_drop.x+"; Y: "+active_drop.y+"] START [X: "+spawn_pos[0]+"; Y: "+spawn_pos[1]+"] DISTANCE2TARGET: "+distance + "; PLANEANGLE: "+fixed_angle +"; V2Y: ");
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MovePlane, 20, true);
	}

	void SendMessageToPlayers(string message) {
		ref array<Man> players = new array<Man>;
		GetGame().GetPlayers( players );
		for ( int i = 0; i < players.Count(); i++ ) {
			PlayerBase player = PlayerBase.Cast(players.Get(i));
			Param1<string> message_param = new Param1<string>(message);
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, message_param, true, player.GetIdentity());
		}
	}

	void TeleportPlayers(vector pos) {
		ref array<Man> players = new array<Man>;
		GetGame().GetPlayers( players );
		for ( int i = 0; i < players.Count(); i++ ) {
			Man player = players.Get(i);
			player.SetPosition(pos);
		}
	}

    bool IsInRect(float x, float y, float min_x, float max_x, float min_y, float max_y) {
        if(x > min_x && x < max_x && y > min_y && y < max_y) return true;
        return false;
    }

    void Cleanup() {
        GetGame().ObjectDelete(airdrop_obj);
    }

    string GetRandomItem() {
        TStringArray loot = {
            "GhillieSuit_Mossy",
        };

        return loot.GetRandomElement();
    }

	TStringArray WolvesClasses()
	{
		return {
            "Animal_CanisLupus_Grey",
            "Animal_CanisLupus_White",
		};
	}
}
