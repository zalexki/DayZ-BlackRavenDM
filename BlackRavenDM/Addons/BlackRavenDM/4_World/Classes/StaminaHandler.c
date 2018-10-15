modded class StaminaHandler
{
	void Init()
	{
		//! stamina consumers registration
		m_StaminaConsumers = new StaminaConsumers;
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.HOLD_BREATH, STAMINA_HOLD_BREATH_THRESHOLD);
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.SPRINT, STAMINA_SPRINT_THRESHOLD);
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.JUMP, STAMINA_JUMP_THRESHOLD);
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.MELEE_HEAVY, STAMINA_MELEE_HEAVY_THRESHOLD);
		m_StaminaConsumers.RegisterConsumer(EStaminaConsumers.MELEE_EVADE, STAMINA_MELEE_EVADE_THRESHOLD);

		//! stamina modifiers registration
		m_StaminaModifiers = new StaminaModifiers;
		m_StaminaModifiers.RegisterFixed(EStaminaModifiers.HOLD_BREATH, STAMINA_DRAIN_HOLD_BREATH);
		m_StaminaModifiers.RegisterFixed(EStaminaModifiers.JUMP, 0);
		m_StaminaModifiers.RegisterRandomized(EStaminaModifiers.MELEE_LIGHT, 1, STAMINA_DRAIN_MELEE_LIGHT);
		m_StaminaModifiers.RegisterRandomized(EStaminaModifiers.MELEE_HEAVY, STAMINA_DRAIN_MELEE_LIGHT, STAMINA_DRAIN_MELEE_HEAVY);
		m_StaminaModifiers.RegisterRandomized(EStaminaModifiers.MELEE_EVADE, 3, STAMINA_DRAIN_MELEE_EVADE);
	}

    override void Update(float deltaT)
    {
 #ifdef DEVELOPER
        m_Debug = DiagMenu.GetBool(DiagMenuIDs.DM_CHEATS_STAMINA_DISABLE);
        if( m_Debug ) return;
 #endif
        if( m_Player )
        {
            // Calculates actual max stamina based on player's load
            if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
                m_Stamina = m_Player.GetStatStamina().Get();

            m_PlayerLoad = m_Player.GetPlayerLoad();

            //! StaminaCap calculation starts when PlayerLoad exceeds STAMINA_WEIGHT_LIMIT_THRESHOLD
            if (m_PlayerLoad >= STAMINA_WEIGHT_LIMIT_THRESHOLD)
            {
                m_StaminaCap =  Math.Max((STAMINA_MAX - (((m_PlayerLoad - STAMINA_WEIGHT_LIMIT_THRESHOLD)/STAMINA_KG_TO_GRAMS) * STAMINA_KG_TO_STAMINAPERCENT_PENALTY)),STAMINA_MIN_CAP);
            }
            else
            {
                m_StaminaCap = STAMINA_MAX;
            }

            // Calculates stamina gain/loss based on movement and load
            m_Player.GetMovementState(m_State);
            m_PlayerSpeed = m_State.m_iMovement;
            m_PlayerStance = m_State.m_iStanceIdx;
            switch ( m_PlayerSpeed )
            {
                case DayZPlayerConstants.MOVEMENTIDX_SPRINT: //sprint
                    if ( m_PlayerStance == DayZPlayerConstants.STANCEIDX_ERECT )
                    {
                        m_StaminaDelta = 0;
                        SetCooldown(STAMINA_REGEN_COOLDOWN_DEPLETION);
                        break;
                    }
                    if ( m_PlayerStance == DayZPlayerConstants.STANCEIDX_CROUCH)
                    {
                        m_StaminaDelta = 0;
                        SetCooldown(STAMINA_REGEN_COOLDOWN_DEPLETION);
                        break;
                    }
                    m_StaminaDelta = STAMINA_GAIN_JOG_PER_SEC;
                break;

                case DayZPlayerConstants.MOVEMENTIDX_RUN: //jog
                    if (!m_IsInCooldown)
                    {
                        m_StaminaDelta = (STAMINA_GAIN_JOG_PER_SEC + CalcStaminaGainBonus());
                    }
                break;

                case DayZPlayerConstants.MOVEMENTIDX_WALK: //walk
                    if (!m_IsInCooldown)
                    {
                        m_StaminaDelta = (STAMINA_GAIN_WALK_PER_SEC + CalcStaminaGainBonus());
                    }
                break;

                case DayZPlayerConstants.MOVEMENTIDX_IDLE: //idle
                    if (!m_IsInCooldown)
                    {
                        m_StaminaDelta = (STAMINA_GAIN_IDLE_PER_SEC + CalcStaminaGainBonus());
                    }
                break;

                default:
                    m_StaminaDelta = STAMINA_GAIN_IDLE_PER_SEC;
                break;
            }

            //Sets current stamina & stores + syncs data with client
            m_Stamina = Math.Max(0,Math.Min((m_Stamina + m_StaminaDelta*deltaT),m_StaminaCap));
            m_Stamina = m_Stamina - m_StaminaDepletion;

            if ( GetGame().IsServer() && GetGame().IsMultiplayer() )
            {
                m_Player.GetStatStamina().Set(m_Stamina);
                m_Time += deltaT;
                if ( m_StaminaParams && m_Time >= STAMINA_SYNC_RATE )
                {
                    m_Time = 0;
                    m_StaminaParams.param1 = m_Stamina;
                    m_StaminaParams.param2 = m_StaminaCap;
                    GetGame().RPCSingleParam(m_Player, ERPCs.RPC_STAMINA, m_StaminaParams, true, m_Player.GetIdentity());
                }
            }
            else
            {
                m_Player.SetStamina(m_Stamina, m_StaminaCap);
            }

            //! sets exhaustion look of player based on stamina level
            HumanCommandAdditives ad = m_Player.GetCommandModifier_Additives();
            float exhaustion_value = 1-((m_Stamina/(m_StaminaCap*0.01))*0.01);
            exhaustion_value = Math.Min(1,exhaustion_value);
            if ( ad )
            {
                // do not apply exhaustion on local client if player is in ADS/Optics (camera shakes)
                if ( m_Player.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT && (m_Player.IsInOptics() || m_Player.IsInIronsights()) )
                {
                    ad.SetExhaustion(0, true);
                }
                else
                {
                    ad.SetExhaustion(exhaustion_value, true);
                }
            }

            CheckStaminaState();

            m_StaminaDelta = 0;
            m_StaminaDepletion = 0; // resets depletion modifier
            if(m_StaminaSoundHandler)
            {
                m_StaminaSoundHandler.Update(m_Stamina, deltaT);
            }
        }
    }
}