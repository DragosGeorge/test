#include "stdafx.h"

#include "../../common/VnumHelper.h"
#include "../../common/service.h"

#include "char.h"

#include "config.h"
#include "utils.h"
#include "crc32.h"
#include "char_manager.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "item_manager.h"
#include "motion.h"
#include "vector.h"
#include "packet.h"
#include "cmd.h"
#include "fishing.h"
#include "exchange.h"
#include "battle.h"
#include "affect.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "pvp.h"
#include "party.h"
#include "start_position.h"
#include "questmanager.h"
#include "log.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "unique_item.h"
#include "unique_mob.h"
#include "priv_manager.h"
#include "war_map.h"
#include "xmas_event.h"
#include "banword.h"
#include "target.h"
#include "wedding.h"
#include "mob_manager.h"
#include "mining.h"
#include "monarch.h"
#include "castle.h"
#include "arena.h"
#include "dev_log.h"
#include "horsename_manager.h"
#include "pcbang.h"
#include "gm.h"
#include "map_location.h"
#include "BlueDragon_Binder.h"
#include "skill_power.h"
#include "buff_on_attributes.h"
#include "threeway_war.h"
#include "PulseManager.h"

#ifdef __ENABLE_NEW_OFFLINESHOP__
#include "new_offlineshop.h"
#include "new_offlineshop_manager.h"
#endif

#if defined(__PET_SYSTEM__)
#include "PetSystem.h"
#endif

#include "DragonSoul.h"

#ifdef ENABLE_SWITCHBOT
#include "switchbot.h"
#endif

#if defined(__GUILD_DRAGONLAIR__)
#include "MeleyLair.h"
#endif

#ifdef ENABLE_12ZI
#include "zodiac_temple.h"
#endif

#if defined(__TEMPLE_OCHAO__)
#include "TempleOchao.h"
#endif

#if defined(__SHIP_DEFENSE__)
#	include "ShipDefense.h"
#endif

#ifdef ENABLE_WHITE_DRAGON
#include "WhiteDragon.h"
#endif

#ifdef ENABLE_QUEEN_NETHIS
#include "SnakeLair.h"
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
#	include "GrowthPetSystem.h"
#endif

extern const uint16_t g_aBuffOnAttrPoints;	//@fixme532
extern bool RaceToJob(unsigned race, unsigned* ret_job);

extern int g_nPortalLimitTime;
extern int test_server;

bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index)
{
	switch (map_index)
	{
	case 301:
	case 302:
	case 303:
	case 304:
		if (ch->GetLevel() < 90)
			return false;
	}
	return true;
}

bool IS_BLOCKED_PET_SUMMON_MAP(int map_index)
{
	switch (map_index)
	{
	case 113:
		return true;
	}
	return false;
}

bool IS_BLOCKED_PET_DUNGEON_MAP(int map_index)
{
	if (map_index < 10000)
		return false;

	map_index /= 10000;

	switch (map_index)
	{
	default:
		return false;
	}

	return false;
}

bool IS_BLOCKED_MOUNT_SUMMON_MAP(int map_index)
{
	switch (map_index)
	{
	case 113:
		return true;
	}
	return false;
}

bool IS_BLOCKED_MOUNT_DUNGEON_MAP(int map_index)
{
	if (map_index < 10000)
		return false;

	map_index /= 10000;

	switch (map_index)
	{
	case 353: // metin2_map_dawnmist_dungeon_01
#if defined(__SHIP_DEFENSE__)
	case ShipDefense::SHIP_MAP_INDEX:
#endif
		return true;
	}

	return false;
}
// <Factor> DynamicCharacterPtr member function definitions

LPCHARACTER DynamicCharacterPtr::Get() const
{
	LPCHARACTER p = NULL;
	if (is_pc)
		p = CHARACTER_MANAGER::instance().FindByPID(id);
	else
		p = CHARACTER_MANAGER::instance().Find(id);

	return p;
}

DynamicCharacterPtr& DynamicCharacterPtr::operator=(LPCHARACTER character)
{
	if (character == NULL)
	{
		Reset();
		return *this;
	}

	if (character->IsPC())
	{
		is_pc = true;
		id = character->GetPlayerID();
	}
	else
	{
		is_pc = false;
		id = character->GetVID();
	}

	return *this;
}

CHARACTER::CHARACTER() : CEntity{ ENTITY_CHARACTER }
{
	m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateIdle, &CHARACTER::EndStateEmpty);
	m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
	m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateBattle, &CHARACTER::EndStateEmpty);

	Initialize();
}

CHARACTER::~CHARACTER()
{
	Destroy();
}

void CHARACTER::Initialize()
{
#if defined(__MAILBOX__)
	m_pkMailBox = nullptr;
	bMailBoxLoading = false;
	m_iMyMailBoxTime = 0;
#endif

#ifdef ENABLE_TELEPORT_WINDOW
	m_pkTeleportEvent = NULL;
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	m_GrowthPetInfo.clear();
#endif

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
	m_bOpeningGuildstorage = false;
#endif

	m_bNoOpenedShop = true;

	m_bOpeningSafebox = false;
	m_bWarping = false;
	m_fSyncTime = get_float_time() - 3;
	m_dwPlayerID = 0;
	m_dwKillerPID = 0;

#ifdef ENABLE_SWITCHBOT
	use_item_anti_flood_count_ = 0;
	use_item_anti_flood_pulse_ = 0;
#endif

#if defined(__SEND_TARGET_INFO__)
	dwLastTargetInfoPulse = 0;
#endif

	m_iMoveCount = 0;

	m_pkRegen = NULL;
	regen_id_ = 0;
	m_posRegen.x = m_posRegen.y = m_posRegen.z = 0;
	m_posStart.x = m_posStart.y = 0;
	m_posDest.x = m_posDest.y = 0;
	m_fRegenAngle = 0.0f;

	m_pkMobData = NULL;
	m_pkMobInst = NULL;

	m_pkShop = NULL;
	m_pkChrShopOwner = NULL;
	m_pkMyShop = NULL;
	m_pkExchange = NULL;
	m_pkParty = NULL;
	m_pkPartyRequestEvent = NULL;

	m_pGuild = NULL;

#if defined(__BL_67_ATTR__)
	b67Attr = false;
#endif

	m_pkChrTarget = NULL;

#if defined(__CHANGE_LOOK_SYSTEM__)
	m_pkChangeLook = nullptr;
#endif

	m_pkMuyeongEvent = NULL;

#ifdef ENABLE_QUEEN_NETHIS
	m_pkSnakeSkillEvent = NULL;
#endif

#ifdef ENABLE_ELEMENTAL_WORLD
	m_pkElementalWorldEvent = NULL;
#endif

#if defined(__9TH_SKILL__)
	m_pkCheonunEvent = NULL;
#endif

#ifdef ENABLE_ATTR_6TH_7TH_EXTEND
	m_pkHitBuffElementEvent = nullptr;
#endif

	m_pkWarpNPCEvent = NULL;
	m_pkDeadEvent = NULL;
	m_pkStunEvent = NULL;
	m_pkSaveEvent = NULL;
	m_pkRecoveryEvent = NULL;
	m_pkTimedEvent = NULL;
	m_pkFishingEvent = NULL;


	// MINING
	m_pkMiningEvent = NULL;
	// END_OF_MINING

	m_pkPoisonEvent = NULL;
	m_pkBleedingEvent = NULL;
	m_pkFireEvent = NULL;
	m_pkCheckSpeedHackEvent = NULL;
	m_speed_hack_count = 0;

	m_pkAffectEvent = NULL;
	m_afAffectFlag = TAffectFlag(0, 0);

	m_pkDestroyWhenIdleEvent = NULL;

	m_pkChrSyncOwner = NULL;

	memset(&m_points, 0, sizeof(m_points));
	memset(&m_pointsInstant, 0, sizeof(m_pointsInstant));
	memset(&m_quickslot, 0, sizeof(m_quickslot));

#if defined(__MINI_GAME_OKEY__)
	memset(&character_cards, 0, sizeof(character_cards));
	memset(&randomized_cards, 0, sizeof(randomized_cards));
#endif

#ifdef __AURA_SYSTEM__
	m_bAuraRefineWindowType = AURA_WINDOW_TYPE_MAX;
	m_bAuraRefineWindowOpen = false;
	for (BYTE i = AURA_SLOT_MAIN; i < AURA_SLOT_MAX; i++)
		m_pAuraRefineWindowItemSlot[i] = NPOS;

	memset(&m_bAuraRefineInfo, 0, AURA_REFINE_INFO_SLOT_MAX * sizeof(TAuraRefineInfo));
#endif

	m_bCharType = CHAR_TYPE_MONSTER;

	SetPosition(POS_STANDING);

	m_dwPlayStartTime = m_dwLastMoveTime = get_dword_time();

	GotoState(m_stateIdle);
	m_dwStateDuration = 1;

	m_dwLastAttackTime = get_dword_time() - 20000;

	m_bAddChrState = 0;

	m_pkChrStone = NULL;

	m_pkSafebox = NULL;
	m_iSafeboxSize = -1;
	m_iSafeboxLoadTime = 0;

	m_pkMall = NULL;
	m_iMallLoadTime = 0;

#ifdef ENABLE_CUBE_RENEWAL
	m_iCubeTime = 0;
#endif

	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;
	m_lWarpMapIndex = 0;

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;

	m_pSkillLevels = NULL;

	m_dwMoveStartTime = 0;
	m_dwMoveDuration = 0;

	m_dwFlyTargetID = 0;

	m_dwNextStatePulse = 0;

	m_dwLastDeadTime = get_dword_time() - 180000;

	m_bSkipSave = false;

	m_bItemLoaded = false;

	m_bHasPoisoned = false;
	m_bHasBled = false;

	m_pkDungeon = NULL;
	m_iEventAttr = 0;

	m_kAttackLog.dwVID = 0;
	m_kAttackLog.dwTime = 0;

	m_bNowWalking = m_bWalking = false;
	ResetChangeAttackPositionTime();

	m_bDetailLog = false;
	m_bMonsterLog = false;

	m_bDisableCooltime = false;

	m_iAlignment = 0;
	m_iRealAlignment = 0;

	m_iKillerModePulse = 0;
	m_bPKMode = PK_MODE_PEACE;

	m_dwQuestNPCVID = 0;
	m_dwQuestByVnum = 0;
	m_pQuestItem = NULL;
	m_dwQuestItemVID = 0;

	m_dwUnderGuildWarInfoMessageTime = get_dword_time() - 60000;

	m_bUnderRefine = false;

	// REFINE_NPC
	m_dwRefineNPCVID = 0;
	// END_OF_REFINE_NPC

	m_dwPolymorphRace = 0;

	m_bStaminaConsume = false;

	ResetChainLightningIndex();

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
	bPrivateShopSearchState = SHOP_SEARCH_OFF;
#endif

	m_dwMountVnum = 0;
	m_chHorse = NULL;
	m_chRider = NULL;

#if defined(__ENABLE_RIDING_EXTENDED__)
	m_mount_up_grade_exp = 0;
	m_mount_up_grade_fail = 0;
#endif

	m_pWarMap = NULL;
	m_pWeddingMap = NULL;
	m_bChatCounter = 0;
#ifdef __ENABLE_NEW_OFFLINESHOP__
	m_pkOfflineShop = NULL;
	m_pkShopSafebox = NULL;
	m_pkAuction = NULL;
	m_pkAuctionGuest = NULL;
	m_pkOfflineShopGuest = NULL;
	m_bIsLookingOfflineshopOfferList = false;
#endif
	m_bWhisperCounter = 0;

	ResetStopTime();

#if defined(__GEM_MARKET_SYSTEM__)
	LoadGemSystem();
#endif

	m_dwLastVictimSetTime = get_dword_time() - 3000;
	m_iMaxAggro = -100;

	m_bSendHorseLevel = 0;
	m_bSendHorseHealthGrade = 0;
	m_bSendHorseStaminaGrade = 0;

	m_dwLoginPlayTime = 0;

	m_pkChrMarried = NULL;

	m_posSafeboxOpen.x = -1000;
	m_posSafeboxOpen.y = -1000;

	// EQUIP_LAST_SKILL_DELAY
	m_dwLastSkillTime = get_dword_time();
	// END_OF_EQUIP_LAST_SKILL_DELAY

	// MOB_SKILL_COOLTIME
	memset(m_adwMobSkillCooltime, 0, sizeof(m_adwMobSkillCooltime));
	// END_OF_MOB_SKILL_COOLTIME

	m_isinPCBang = false;

	// ARENA
	m_pArena = NULL;
	m_nPotionLimit = quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count");
	// END_ARENA

	// PREVENT_TRADE_WINDOW
	m_isOpenSafebox = 0;
	// END_PREVENT_TRADE_WINDOW

	// PREVENT_REFINE_HACK
	m_iRefineTime = 0;
	// END_PREVENT_REFINE_HACK

	// RESTRICT_USE_SEED_OR_MOONBOTTLE
	m_iSeedTime = 0;
	// END_RESTRICT_USE_SEED_OR_MOONBOTTLE
	// PREVENT_PORTAL_AFTER_EXCHANGE
	m_iExchangeTime = 0;
	// END_PREVENT_PORTAL_AFTER_EXCHANGE

	m_iSafeboxLoadTime = 0;

	m_iMyShopTime = 0;
#if defined(__MYSHOP_DECO__)
	m_bMyShopDecoType = 0;
	m_dwMyShopDecoPolyVnum = 0;
	m_bMyShopTabs = 0;
#endif

#if defined(ENABLE_GROWTH_PET_SYSTEM) && defined(ENABLE_PET_ATTR_DETERMINE)
	m_iGrowthPetDetermineLoadTime = 0;
#endif

	InitMC();

	m_deposit_pulse = 0;

	SET_OVER_TIME(this, OT_NONE);

	m_strNewName = "";

	m_known_guild.clear();

	m_dwLogOffInterval = 0;
	m_dwLastPlay = 0;

	m_bComboSequence = 0;
	m_dwLastComboTime = 0;
	m_bComboIndex = 0;
	m_iComboHackCount = 0;
	m_dwSkipComboAttackByTime = 0;

	m_dwMountTime = 0;

	m_dwLastGoldDropTime = 0;
#if defined(__CHEQUE_SYSTEM__)
	m_dwLastChequeDropTime = 0;
#endif

	m_bIsLoadedAffect = false;
	cannot_dead = false;

	m_newSummonInterval = 0;
	m_lastSummonTime = 0;
#if defined(__EREBUS_DUNGEON__)
	m_iLastHealerSummonDelay = 0;
#endif

#if defined(__PET_SYSTEM__)
	m_petSystem = 0;
	m_bIsPet = false;
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	m_GrowthPetSystem = 0;
	m_bIsGrowthPet = false;
	m_GrowthPetEggVID = 0;
	m_GrowthPetEvolution = 0;
	m_GrowthPetHachWindowIsOpen = false;
	m_GrowthPetWindowType = 0;
#endif

	m_fAttMul = 1.0f;
	m_fDamMul = 1.0f;

	m_pointsInstant.iDragonSoulActiveDeck = -1;

	memset(&m_tvLastSyncTime, 0, sizeof(m_tvLastSyncTime));
	m_iSyncHackCount = 0;

#ifdef ENABLE_REFINE_ELEMENT
	m_OpenElementsSpell = false;
#endif

#if defined(__ACCE_COSTUME_SYSTEM__)
	m_bAcceCombination = false;
	m_bAcceAbsorption = false;
#endif

#if defined(__SORT_INVENTORY_ITEMS__)
	m_sortInventoryPulse = 0;
#if defined(__SPECIAL_INVENTORY_SYSTEM__)
	m_sortSpecialInventoryPulse = 0;
#endif
#endif

#if defined(__HIDE_COSTUME_SYSTEM__)
	m_HideCostumePulse = 0;
	m_bHideBodyCostume = false;
	m_bHideHairCostume = false;
	m_bHideAcceCostume = false;
	m_bHideWeaponCostume = false;
	m_bHideAuraCostume = false;
#endif

#ifdef ENABLE_GUILD_DONATE_ATTENDANCE
	m_dwLastOfferNewExpTime = 0;
#endif

	affectInfo = NULL;

#if defined(__SKILL_COLOR_SYSTEM__)
	memset(&m_dwSkillColor, 0, sizeof(m_dwSkillColor));
#endif

#ifdef ENABLE_12ZI
	/*m_pkBeadPointIdleEvent = nullptr;
	m_bStoneBossCount = 0;*/

	m_pkZodiac = nullptr;
	m_dwZodiacCzLastTime = 0;
	m_dwLastZodiacAttackTime = 0;
	m_dwDeadCount = 0;
#endif
#if defined(ENABLE_12ZI) && defined(ENABLE_12ZI_REWARD_TABLE)
	m_ziRewardTable = nullptr;
#endif
#if defined(ENABLE_12ZI) && defined(ENABLE_12ZI_SHOP_LIMIT)
	m_ShopLimitedPurchaseInfo.clear();
#endif

#if defined(__MINI_GAME_CATCH_KING__)
	m_vecCatchKingFieldCards.clear();
	bCatchKingHandCard = 0;
	bCatchKingHandCardLeft = 0;
	bCatchKingBetSetNumber = 0;
	dwCatchKingTotalScore = 0;
	dwCatchKingGameStatus = false;
#endif

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
	m_pkGuildstorage = nullptr;
	m_iGuildstorageLoadTime = 0;
	m_isOpenGuildstorage = 0;
#endif

#ifdef ENABLE_ANTI_SPAM
	analyze_protect = 0;
	analyze_protect_count = 0;
	ap_pickup = 0;
	apc_pickup = 0;
	ap_open_items = 0;
	apc_open_items = 0;
	ap_attack = 0;
	apc_attack = 0;
	ap_move = 0;
	apc_move = 0;
	ap_click_target = 0;
	apc_click_target = 0;
	ap_drop_target = 0;
	apc_drop_target = 0;
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	m_pkBiologManager = NULL;
#endif

}

void CHARACTER::Create(const char* c_pszName, DWORD vid, bool isPC)
{
	static int s_crc = 172814;

	char crc_string[128 + 1];
	snprintf(crc_string, sizeof(crc_string), "%s%p%d", c_pszName, this, ++s_crc);
	m_vid = VID(vid, GetCRC32(crc_string, strlen(crc_string)));

	if (isPC)
		m_stName = c_pszName;
}

void CHARACTER::Destroy()
{
	CloseMyShop();

	if (m_pkRegen)
	{
		if (m_pkDungeon)
		{
			// Dungeon regen may not be valid at this point
			if (m_pkDungeon->IsValidRegen(m_pkRegen, regen_id_))
			{
				--m_pkRegen->count;
			}
		}
#ifdef ENABLE_12ZI
		else if (m_pkZodiac)
		{
			if (m_pkZodiac->IsValidRegen(m_pkRegen, regen_id_))
				--m_pkRegen->count;
		}
#endif
		else
		{
			// Is this really safe?
			--m_pkRegen->count;
		}
		m_pkRegen = NULL;
	}

	if (m_pkDungeon)
	{
		SetDungeon(NULL);
	}

#ifdef ENABLE_12ZI
	if (m_pkZodiac)
		SetZodiac(nullptr);
#endif

#if defined(__PET_SYSTEM__)
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;

		m_petSystem = 0;
	}
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (m_GrowthPetSystem)
	{
		m_GrowthPetSystem->Destroy();
		delete m_GrowthPetSystem;

		m_GrowthPetSystem = 0;
	}
#endif

#ifdef ENABLE_TELEPORT_WINDOW
	event_cancel(&m_pkTeleportEvent);
#endif

#if defined(__CHANGE_LOOK_SYSTEM__)
	SetChangeLook(nullptr);
#endif

#if defined(__MAILBOX__)
	SetMailBox(nullptr);
#endif

	HorseSummon(false);

	if (GetRider())
		GetRider()->ClearHorseInfo();

	if (GetDesc())
	{
		GetDesc()->BindCharacter(NULL);
		//BindDesc(NULL);
	}

	if (m_pkExchange)
		m_pkExchange->Cancel();

	SetVictim(NULL);

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	ClearStone();
	ClearSync();
	ClearTarget();

	if (NULL == m_pkMobData)
	{
		DragonSoul_CleanUp();
		ClearItem();
	}

	// <Factor> m_pkParty becomes NULL after CParty destructor call!
	LPPARTY party = m_pkParty;
	if (party)
	{
		if (party->GetLeaderPID() == GetVID() && !IsPC())
		{
			M2_DELETE(party);
		}
		else
		{
			party->Unlink(this);

			if (!IsPC())
				party->Quit(GetVID());
		}

		SetParty(NULL); // ï¿½ï¿½ï¿½Øµï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï°ï¿½.
	}

	if (m_pkMobInst)
	{
		M2_DELETE(m_pkMobInst);
		m_pkMobInst = NULL;
	}

	m_pkMobData = NULL;

	if (m_pkSafebox)
	{
		M2_DELETE(m_pkSafebox);
		m_pkSafebox = NULL;
	}

	if (m_pkMall)
	{
		M2_DELETE(m_pkMall);
		m_pkMall = NULL;
	}

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
	if (m_pkGuildstorage)
	{
		M2_DELETE(m_pkGuildstorage);
		m_pkGuildstorage = nullptr;
	}
#endif

	m_set_pkChrSpawnedBy.clear();

	StopMuyeongEvent();
#if defined(__9TH_SKILL__)
	StopCheonunEvent();
#endif

#ifdef ENABLE_ELEMENTAL_WORLD
	StopElementalWorldEvent();
#endif

	event_cancel(&m_pkWarpNPCEvent);
	event_cancel(&m_pkRecoveryEvent);
	event_cancel(&m_pkDeadEvent);
	event_cancel(&m_pkSaveEvent);
	event_cancel(&m_pkTimedEvent);
	event_cancel(&m_pkStunEvent);
	event_cancel(&m_pkFishingEvent);
	event_cancel(&m_pkPoisonEvent);
#ifdef ENABLE_12ZI
	//event_cancel(&m_pkBeadPointIdleEvent);
#endif
	event_cancel(&m_pkBleedingEvent);
	event_cancel(&m_pkFireEvent);
	event_cancel(&m_pkPartyRequestEvent);
	// DELAYED_WARP
	event_cancel(&m_pkCheckSpeedHackEvent);
	// END_DELAYED_WARP

	// RECALL_DELAY
	//event_cancel(&m_pkRecallEvent);
	// END_OF_RECALL_DELAY

	// MINING
	event_cancel(&m_pkMiningEvent);
	// END_OF_MINING

	for (itertype(m_mapMobSkillEvent) it = m_mapMobSkillEvent.begin(); it != m_mapMobSkillEvent.end(); ++it)
	{
		LPEVENT pkEvent = it->second;
		event_cancel(&pkEvent);
	}
	m_mapMobSkillEvent.clear();

	//event_cancel(&m_pkAffectEvent);
	ClearAffect();

	for (itertype(m_map_buff_on_attrs) it = m_map_buff_on_attrs.begin(); it != m_map_buff_on_attrs.end(); ++it)
	{
		if (NULL != it->second)
		{
			M2_DELETE(it->second);
		}
	}
	m_map_buff_on_attrs.clear();

	event_cancel(&m_pkDestroyWhenIdleEvent);

	if (m_pSkillLevels)
	{
		M2_DELETE_ARRAY(m_pSkillLevels);
		m_pSkillLevels = NULL;
	}

#if defined(ENABLE_12ZI) && defined(ENABLE_12ZI_REWARD_TABLE)
	if (m_ziRewardTable)
	{
		M2_DELETE_ARRAY(m_ziRewardTable);
		m_ziRewardTable = nullptr;
	}
#endif

	CEntity::Destroy();

	if (GetSectree())
		GetSectree()->RemoveEntity(this);

	if (m_bMonsterLog)
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);

	if (affectInfo)
	{
		M2_DELETE_ARRAY(affectInfo->data);
		M2_DELETE(affectInfo);
	}

#ifdef ENABLE_ANTI_SPAM	
	analyze_protect = 0;
	analyze_protect_count = 0;
	ap_pickup = 0;
	apc_pickup = 0;
	ap_open_items = 0;
	apc_open_items = 0;
	ap_attack = 0;
	apc_attack = 0;
	ap_move = 0;
	apc_move = 0;
	ap_click_target = 0;
	apc_click_target = 0;
	ap_drop_target = 0;
	apc_drop_target = 0;
#endif

}

const char* CHARACTER::GetName() const
{
	return m_stName.empty() ? (m_pkMobData ? LC_MOB_NAME(GetRaceNum(), GetLanguage()) : "") : m_stName.c_str();
}

void CHARACTER::OpenMyShop(const char* c_pszSign, TShopItemTable* pTable, WORD wItemCount)
{
	if (!CanHandleItem())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use a private shop now."));
		return;
	}

	if (GetPart(PART_MAIN) > 2)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only open the shop if you take off your armour."));
		return;
	}

	if (GetMyShop()) // ï¿½Ì¹ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ý´Â´ï¿½.
	{
		CloseMyShop();
		return;
	}

	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½.
	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());

	// GetPCForceï¿½ï¿½ NULLï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ç·ï¿½ ï¿½ï¿½ï¿½ï¿½ È®ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	if (pPC->IsRunning())
		return;

	if (wItemCount == 0)
		return;

	int64_t nTotalMoney = 0;
#if defined(__CHEQUE_SYSTEM__)
	int64_t nTotalCheque = 0;
#endif

	for (int n = 0; n < wItemCount; ++n)
	{
		nTotalMoney += static_cast<int64_t>((pTable + n)->price);
#if defined(__CHEQUE_SYSTEM__)
		nTotalCheque += static_cast<int64_t>((pTable + n)->price_cheque);
#endif
	}

	nTotalMoney += static_cast<int64_t>(GetGold());
#if defined(__CHEQUE_SYSTEM__)
	nTotalCheque += static_cast<int64_t>(GetCheque());
#endif

	if (g_MaxGold <= nTotalMoney)
	{
		sys_err("[OVERFLOW_GOLD] Overflow (GOLD_MAX) id %u name %s", GetPlayerID(), GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade because you are carrying more than 2 billion Yang."));
		return;
	}

#if defined(__CHEQUE_SYSTEM__)
	if (CHEQUE_MAX <= nTotalCheque)
	{
		sys_err("[OVERFLOW_CHEQUE] Overflow (CHEQUE_MAX) id %u name %s", GetPlayerID(), GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade because you are carrying more than 2 billion Yang."));
		return;
	}
#endif

	char szSign[SHOP_SIGN_MAX_LEN + 1];
	strlcpy(szSign, c_pszSign, sizeof(szSign));

	m_stShopSign = szSign;

	if (m_stShopSign.length() == 0)
		return;

	if (CBanwordManager::instance().CheckString(m_stShopSign.c_str(), m_stShopSign.length()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't give your shop an invalid name."));
		return;
	}

	// MYSHOP_PRICE_LIST
	std::map<DWORD, DWORD> itemkind; // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½, first: vnum, second: ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	// END_OF_MYSHOP_PRICE_LIST

	std::set<TItemPos> cont;
	for (WORD i = 0; i < wItemCount; ++i)
	{
		if (cont.find((pTable + i)->pos) != cont.end())
		{
			sys_err("MYSHOP: duplicate shop item detected! (name: %s)", GetName());
			return;
		}

		// ANTI_GIVE, ANTI_MYSHOP check
		LPITEM pkItem = GetItem((pTable + i)->pos);

		if (pkItem)
		{
			const TItemTable* item_table = pkItem->GetProto();

			if (item_table && (IS_SET(item_table->ullAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't sell Item-Shop items in a private shop."));
				return;
			}

			if (pkItem->IsEquipped() == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Equipped items cannot be sold in the private shop."));
				return;
			}

			if (true == pkItem->isLocked())
			{
				return;
			}

#if defined(__SOUL_BIND_SYSTEM__)
			if (pkItem->IsSealed())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sell soulbound items in a private shop."));
				return;
			}
#endif

			// MYSHOP_PRICE_LIST
			itemkind[pkItem->GetVnum()] = (pTable + i)->price / pkItem->GetCount();
#if defined(__CHEQUE_SYSTEM__)
			itemkind[pkItem->GetVnum()] = (pTable + i)->price_cheque / pkItem->GetCount();
#endif
			// END_OF_MYSHOP_PRICE_LIST
		}

		cont.insert((pTable + i)->pos);
	}

	// MYSHOP_PRICE_LIST
	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ò½ï¿½Å²ï¿½ï¿½.
#if defined(__MYSHOP_DECO__)
	if (CountSpecifyItem(71049) || CountSpecifyItem(KASHMIR_BUNDLE)) // ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ñ´ï¿½.
#else
	if (CountSpecifyItem(71049)) // ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ñ´ï¿½.
#endif
	{
		//
		// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï±ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Å¶ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ DB Ä³ï¿½Ã¿ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½.
		//
		TItemPriceListTable header;
		memset(&header, 0, sizeof(TItemPriceListTable));

		header.dwOwnerID = GetPlayerID();
		header.byCount = itemkind.size();

		size_t idx = 0;

		for (itertype(itemkind) it = itemkind.begin(); it != itemkind.end(); ++it)
		{
			header.aPriceInfo[idx].dwVnum = it->first;
			header.aPriceInfo[idx].dwPrice = it->second;
#if defined(__CHEQUE_SYSTEM__)
			header.aPriceInfo[idx].dwPriceCheque = it->second;
#endif

			idx++;
		}

		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_UPDATE, GetDesc()->GetHandle(), &header, sizeof(TItemPriceListTable));
	}
	// END_OF_MYSHOP_PRICE_LIST
	else if (CountSpecifyItem(50200))
		RemoveSpecifyItem(50200, 1);
	else
		return; // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ß´ï¿½.

	if (m_pkExchange)
		m_pkExchange->Cancel();

	TPacketGCShopSign p;

	p.bHeader = HEADER_GC_SHOP_SIGN;
	p.dwVID = GetVID();
	strlcpy(p.szSign, c_pszSign, sizeof(p.szSign));
#if defined(__MYSHOP_DECO__)
	p.bType = GetMyShopDecoType();
#endif

	PacketAround(&p, sizeof(TPacketGCShopSign));

	m_pkMyShop = CShopManager::instance().CreatePCShop(this, pTable, wItemCount);

	if (IsPolymorphed() == true)
	{
		RemoveAffect(AFFECT_POLYMORPH);
	}

	if (GetHorse())
	{
		HorseSummon(false, true);
	}
	// new mount ï¿½Ì¿ï¿½ ï¿½ß¿ï¿½, ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Úµï¿½ unmount
	// StopRidingï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ï¿½ï¿½ Ã³ï¿½ï¿½ï¿½Ï¸ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½×·ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ø³ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½.
	else if (GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}

#if defined(__MYSHOP_DECO__)
	if (GetMyShopDecoPolyVnum() != 0)
		SetPolymorph(GetMyShopDecoPolyVnum(), true);
	else
		SetPolymorph(30000, true);
#else
	SetPolymorph(30000, true);
#endif
}

void CHARACTER::CloseMyShop()
{
	if (GetMyShop())
	{
		m_stShopSign.clear();
		CShopManager::instance().DestroyPCShop(this);
		m_pkMyShop = NULL;

#if defined(__MYSHOP_DECO__)
		SetMyShopDecoPolyVnum(0);
		SetMyShopDecoType(0);
		SetMyShopTabs(0);
#endif

		TPacketGCShopSign p;

		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
		p.szSign[0] = '\0';
#if defined(__MYSHOP_DECO__)
		p.bType = 0;
#endif

		PacketAround(&p, sizeof(p));

		SetPolymorph(m_points.job, true);
#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
		CTargetManager::instance().DeleteShopSearchTarget(static_cast<DWORD>(GetVID()));
#endif
	}
}

void EncodeMovePacket(TPacketGCMove& pack, DWORD dwVID, BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, BYTE bRot)
{
	pack.bHeader = HEADER_GC_MOVE;
	pack.bFunc = bFunc;
	pack.bArg = bArg;
	pack.dwVID = dwVID;
	pack.dwTime = dwTime ? dwTime : get_dword_time();
	pack.bRot = bRot;
	pack.lX = x;
	pack.lY = y;
	pack.dwDuration = dwDuration;
}

void CHARACTER::Restart(const uint8_t c_uiSubCmd)
{
	if (false == IsDead())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("is_dead_false"));
		ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		StartRecoveryEvent();
		return;
	}

	if (NULL == m_pkDeadEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("m_mkDeadEvent"));
		return;
	}

	int iTimeToDead = (event_time(m_pkDeadEvent) / passes_per_sec);

	if (c_uiSubCmd != SCMD_RESTART_TOWN && (!GetWarMap() || GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		if (!test_server)
		{
			if (IsHack())
			{
				//If it is a holy land map, it is not checked.
				if (false == CThreeWayWar::instance().IsSungZiMapIndex(GetMapIndex()))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot restart yet. Please wait another %d seconds."), iTimeToDead - (180 - g_nPortalLimitTime));
					return;
				}
			}

			if (iTimeToDead > 170)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot restart yet. Please wait another %d seconds."), iTimeToDead - 170);
				return;
			}
		}
	}

	//PREVENT_HACK
	//DESC : It can be used for bugs using the portal after the warehouse and exchange window.
	// add cooldown
	if (c_uiSubCmd == SCMD_RESTART_TOWN)
	{

		if (IsHack())
		{
			//It is not checked on the guild map and the holy land map.
			if ((!GetWarMap() || GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG) ||
				false == CThreeWayWar::instance().IsSungZiMapIndex(GetMapIndex()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot restart yet. Please wait another %d seconds."), iTimeToDead - (180 - g_nPortalLimitTime));
				return;
			}
		}

		if (iTimeToDead > 173)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot restart in the city yet. Wait another %d seconds."), iTimeToDead - 173);
			return;
		}
	}
	//END_PREVENT_HACK

#ifdef ENABLE_12ZI
		// For check how many Prism you need.
	if (c_uiSubCmd == SCMD_RESTART_HERE)
	{
		//if (ch->GetZodiac())
		if (GetMapIndex() >= 3550000 && GetMapIndex() < 3560000)
		{

			uint8_t bNeedPrism;
			if (GetQuestFlag("12zi_temple.IsDead") == 1 || GetQuestFlag("12zi_temple.PrismNeed") > 0)
			{
				if (GetQuestFlag("12zi_temple.PrismNeed") == 1)
					bNeedPrism = 1;
				else if (GetQuestFlag("12zi_temple.PrismNeed") == 2)
					bNeedPrism = 2;
				else if (GetQuestFlag("12zi_temple.PrismNeed") == 3)
					bNeedPrism = 4;
				else if (GetQuestFlag("12zi_temple.PrismNeed") == 4)
					bNeedPrism = 8;
				else if (GetQuestFlag("12zi_temple.PrismNeed") >= 5)
					bNeedPrism = 10;
				else
					bNeedPrism = GetQuestFlag("12zi_temple.PrismNeed");
			}
			else
			{
				if (GetDeadCount() == 3)
					bNeedPrism = 4;
				else if (GetDeadCount() == 4)
					bNeedPrism = 8;
				else if (GetDeadCount() >= 5)
					bNeedPrism = 10;
				else
					bNeedPrism = GetDeadCount();
			}

			ChatPacket(CHAT_TYPE_COMMAND, "OpenReviveDialog %u %u", (uint32_t)GetVID(), bNeedPrism);
			return;
		}
	}
#endif

	ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");

	GetDesc()->SetPhase(PHASE_GAME);
	SetPosition(POS_STANDING);
	StartRecoveryEvent();

	//FORKED_LOAD
	//DESC: If you resurrect during a three-way battle, you will move to the starting point of the three-way battle, not the entrance to the map.
	if (1 == quest::CQuestManager::instance().GetEventFlag("threeway_war"))
	{
		if (c_uiSubCmd == SCMD_RESTART_TOWN || c_uiSubCmd == SCMD_RESTART_HERE)
		{
#ifdef ENABLE_12ZI
			if (c_uiSubCmd == SCMD_RESTART_HERE)
			{
				if (GetMapIndex() >= 3550000 && GetMapIndex() < 3560000)
				{
					sys_log(0, "do_restart: restart here zodiac");

					uint8_t bNeedPrism;
					if (GetQuestFlag("12zi_temple.IsDead") == 1 || GetQuestFlag("12zi_temple.PrismNeed") > 0)
					{
						if (GetQuestFlag("12zi_temple.PrismNeed") == 1)
							bNeedPrism = 1;
						else if (GetQuestFlag("12zi_temple.PrismNeed") == 2)
							bNeedPrism = 2;
						else if (GetQuestFlag("12zi_temple.PrismNeed") == 3)
							bNeedPrism = 4;
						else if (GetQuestFlag("12zi_temple.PrismNeed") == 4)
							bNeedPrism = 8;
						else if (GetQuestFlag("12zi_temple.PrismNeed") >= 5)
							bNeedPrism = 10;
						else
							bNeedPrism = GetQuestFlag("12zi_temple.PrismNeed");
					}
					else
					{
						if (GetDeadCount() == 3)
							bNeedPrism = 4;
						else if (GetDeadCount() == 4)
							bNeedPrism = 8;
						else if (GetDeadCount() >= 5)
							bNeedPrism = 10;
						else
							bNeedPrism = GetDeadCount();
					}

					ChatPacket(CHAT_TYPE_COMMAND, "OpenReviveDialog %u %u", (uint32_t)GetVID(), bNeedPrism);
					return;
				}
			}
#endif
			if (true == CThreeWayWar::instance().IsThreeWayWarMapIndex(GetMapIndex()) &&
				false == CThreeWayWar::instance().IsSungZiMapIndex(GetMapIndex()))
			{
				WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));

				ReviveInvisible(5);
#ifdef ENABLE_PET_SYSTEM
				CheckPet();
#endif
				PointChange(POINT_HP, GetMaxHP() - GetHP());
				PointChange(POINT_SP, GetMaxSP() - GetSP());

				return;
			}

			// holy land
			if (true == CThreeWayWar::instance().IsSungZiMapIndex(GetMapIndex()))
			{
				if (CThreeWayWar::instance().GetReviveTokenForPlayer(GetPlayerID()) <= 0)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The waiting time has expired. You will be revived in the city."));
					WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));
				}
				else
				{
					Show(GetMapIndex(), GetSungziStartX(GetEmpire()), GetSungziStartY(GetEmpire()));
				}

				PointChange(POINT_HP, GetMaxHP() - GetHP());
				PointChange(POINT_SP, GetMaxSP() - GetSP());
				ReviveInvisible(5);
#ifdef ENABLE_PET_SYSTEM
				CheckPet();
#endif
				return;
			}
		}
	}
	// END_FORKED_LOAD

	if (IS_MAZE_DUNGEON(GetMapIndex()))
	{
		if (WarpSet(1147800, 532600))
			return;
	}

	if (GetDungeon())
		GetDungeon()->UseRevive(this);

	if (GetWarMap() && !IsObserverMode())
	{
		CWarMap* pMap = GetWarMap();
		DWORD dwGuildOpponent = pMap ? pMap->GetGuildOpponent(this) : 0;

		if (dwGuildOpponent)
		{
			switch (c_uiSubCmd)
			{
			case SCMD_RESTART_TOWN:
			{
				sys_log(0, "do_restart: restart town");
				PIXEL_POSITION pos;

				if (CWarMapManager::instance().GetStartPosition(GetMapIndex(), GetGuild()->GetID() < dwGuildOpponent ? 0 : 1, pos))
					Show(GetMapIndex(), pos.x, pos.y);
				else
					ExitToSavedLocation();

				PointChange(POINT_HP, GetMaxHP() - GetHP());
				PointChange(POINT_SP, GetMaxSP() - GetSP());
				ReviveInvisible(5);
#ifdef ENABLE_PET_SYSTEM
				CheckPet();
#endif
			} break;

			case SCMD_RESTART_HERE:
			{
#ifdef ENABLE_12ZI
				if (GetMapIndex() >= 3550000 && GetMapIndex() < 3560000)
				{
					sys_log(0, "do_restart: restart here zodiac");

					uint8_t bNeedPrism;
					if (GetQuestFlag("12zi_temple.IsDead") == 1 || GetQuestFlag("12zi_temple.PrismNeed") > 0)
					{
						if (GetQuestFlag("12zi_temple.PrismNeed") == 1)
							bNeedPrism = 1;
						else if (GetQuestFlag("12zi_temple.PrismNeed") == 2)
							bNeedPrism = 2;
						else if (GetQuestFlag("12zi_temple.PrismNeed") == 3)
							bNeedPrism = 4;
						else if (GetQuestFlag("12zi_temple.PrismNeed") == 4)
							bNeedPrism = 8;
						else if (GetQuestFlag("12zi_temple.PrismNeed") >= 5)
							bNeedPrism = 10;
						else
							bNeedPrism = GetQuestFlag("12zi_temple.PrismNeed");
					}
					else
					{
						if (GetDeadCount() == 3)
							bNeedPrism = 4;
						else if (GetDeadCount() == 4)
							bNeedPrism = 8;
						else if (GetDeadCount() >= 5)
							bNeedPrism = 10;
						else
							bNeedPrism = GetDeadCount();
					}

					ChatPacket(CHAT_TYPE_COMMAND, "OpenReviveDialog %u %u", (uint32_t)GetVID(), bNeedPrism);
					return;
				}
#endif
				sys_log(0, "do_restart: restart here");
				RestartAtSamePos();
				//Show(GetMapIndex(), GetX(), GetY());
				PointChange(POINT_HP, GetMaxHP() - GetHP());
				PointChange(POINT_SP, GetMaxSP() - GetSP());
				ReviveInvisible(5);
#ifdef ENABLE_PET_SYSTEM
				CheckPet();
#endif
			} break;
			} // end switch

			return;
		}
	}

#ifdef ENABLE_QUEEN_NETHIS
	if (IsSnakeMap())
	{
		switch (c_uiSubCmd)
		{
		case SCMD_RESTART_TOWN:
			sys_log(0, "do_restart: restart town");
			PIXEL_POSITION pos;

			RestartAtSamePos();

			PointChange(POINT_HP, GetMaxHP() - GetHP());
			PointChange(POINT_SP, GetMaxSP() - GetSP());
			ReviveInvisible(3);
#ifdef ENABLE_PET_SYSTEM
			CheckPet();
#endif
			break;

		case SCMD_RESTART_HERE:
			sys_log(0, "do_restart: restart here");
			RestartAtSamePos();
			PointChange(POINT_HP, GetMaxHP() - GetHP());
			PointChange(POINT_SP, GetMaxSP() - GetSP());
			ReviveInvisible(3);
#ifdef ENABLE_PET_SYSTEM
			CheckPet();
#endif
			break;
		}

		return;
	}

#endif

	switch (c_uiSubCmd)
	{
	case SCMD_RESTART_TOWN:
	{
		sys_log(0, "do_restart: restart town");
		PIXEL_POSITION pos;

		if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), pos))
			WarpSet(pos.x, pos.y);
		else
			WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));

		PointChange(POINT_HP, 50 - GetHP());
		DeathPenalty(1);
	} break;

	case SCMD_RESTART_HERE:
	{
		sys_log(0, "do_restart: restart here");
		RestartAtSamePos();
		//Show(GetMapIndex(), GetX(), GetY());
		//PointChange(POINT_HP, 50 - GetHP());
		PointChange(POINT_HP, (GetMaxHP() - GetHP()) / 2);
		DeathPenalty(0);
		ReviveInvisible(5);
#ifdef ENABLE_PET_SYSTEM
		CheckPet();
#endif
	} break;
	} // end switch
}

void CHARACTER::RestartAtSamePos()
{
	if (m_bIsObserver)
		return;

#if defined(__SET_ITEM__)
	// NOTE : Due to the classic pet disappearing when the character dies,
	// we have to call this function again to check if the pet is spawned.
	RefreshItemSetBonus();
	// NOTE : If the player is cruel, he is likely to drop some equipment,
	// it is necessary to re-check the set bonus.
	RefreshItemSetBonusByValue();
#endif

	ViewReencode();
}

// Entityï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Å¸ï¿½ï¿½ï¿½Ù°ï¿½ ï¿½ï¿½Å¶ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½.
void CHARACTER::EncodeInsertPacket(LPENTITY entity)
{
	LPDESC d;

	if (!(d = entity->GetDesc()))
		return;

	// ï¿½ï¿½ï¿½ï¿½Ì¸ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Úµï¿½
	LPCHARACTER ch = (LPCHARACTER)entity;
	ch->SendGuildName(GetGuild());
	// ï¿½ï¿½ï¿½ï¿½Ì¸ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Úµï¿½

	TPacketGCCharacterAdd pack;

	pack.header = HEADER_GC_CHARACTER_ADD;
	pack.dwVID = m_vid;
#if defined(__WJ_SHOW_MOB_INFO__)
	if (IsMonster() || IsStone())
	{
		pack.dwLevel = GetLevel();
		pack.dwAIFlag = IsMonster() ? GetAIFlag() : 0;
#ifdef ENABLE_12ZI
		if (ch->GetZodiac())
			pack.dwAIFlag = 0;
#endif
	}
	else
	{
		pack.dwLevel = 0;
		pack.dwAIFlag = 0;
	}
#endif
	pack.bType = GetCharType();
	pack.angle = GetRotation();
	pack.x = GetX();
	pack.y = GetY();
	pack.z = GetZ();
	pack.wRaceNum = GetRaceNum();
	if (IsPet()
#ifdef ENABLE_GROWTH_PET_SYSTEM
		|| IsGrowthPet()
#endif
		)
	{
		pack.bMovingSpeed = 150;
	}
	else
	{
		pack.bMovingSpeed = GetLimitPoint(POINT_MOV_SPEED);
	}
	pack.bAttackSpeed = GetLimitPoint(POINT_ATT_SPEED);
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];

	pack.bStateFlag = m_bAddChrState;

	int iDur = 0;

	if (m_posDest.x != pack.x || m_posDest.y != pack.y)
	{
		iDur = (m_dwMoveStartTime + m_dwMoveDuration) - get_dword_time();

		if (iDur <= 0)
		{
			pack.x = m_posDest.x;
			pack.y = m_posDest.y;
		}
	}

	d->Packet(&pack, sizeof(pack));

	if (IsPC() || CMobVnumHelper::IsNPCType(m_bCharType)
#ifdef ENABLE_GROWTH_PET_SYSTEM
		|| m_bCharType == CHAR_TYPE_PET
		|| IsGrowthPet()
#endif
		)
	{
		TPacketGCCharacterAdditionalInfo addPacket;
		memset(&addPacket, 0, sizeof(TPacketGCCharacterAdditionalInfo));

		addPacket.header = HEADER_GC_CHAR_ADDITIONAL_INFO;
		addPacket.dwVID = m_vid;

		addPacket.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
		addPacket.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
		addPacket.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
		addPacket.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#if defined(__ACCE_COSTUME_SYSTEM__)
		addPacket.awPart[CHR_EQUIPPART_ACCE] = GetPart(PART_ACCE);
#endif
#ifdef __AURA_SYSTEM__
		addPacket.awPart[CHR_EQUIPPART_AURA] = GetPart(PART_AURA);
#endif
#if defined(__QUIVER_SYSTEM__)
		addPacket.dwArrow = (IsPC() && GetWear(WEAR_ARROW) != NULL) ? GetWear(WEAR_ARROW)->GetOriginalVnum() : 0;
#endif

		addPacket.bPKMode = m_bPKMode;
		addPacket.dwMountVnum = GetMountVnum();
		addPacket.bEmpire = m_bEmpire;
#ifdef ENABLE_REFINE_ELEMENT
		addPacket.dwElementsEffect = GetElementsEffect();
#endif
		addPacket.dwLevel = 0;
#if defined(__CONQUEROR_LEVEL__)
		addPacket.dwConquerorLevel = IsPC() ? GetConquerorLevel() : 0;
#endif
		addPacket.dwGuildID = 0;
#if defined(__GUILD_LEADER_GRADE_NAME__)
		addPacket.bGuildLeaderGrade = 0;
#endif
#if defined(__GENDER_ALIGNMENT__)
		addPacket.bJob = IsPC() ? m_points.job : 0;
#endif
#if defined(__SKILL_COLOR_SYSTEM__)
		memcpy(addPacket.dwSkillColor, GetSkillColor(), sizeof(addPacket.dwSkillColor));
#endif
		addPacket.bLanguage = GetLanguage();

		strlcpy(addPacket.name, GetName(), sizeof(addPacket.name));

		if (IsPC()
#ifdef ENABLE_GROWTH_PET_SYSTEM
			|| IsGrowthPet()
#endif
			== true)
		{
			addPacket.dwLevel = GetLevel();
		}

		if (GetGuild() != NULL)
		{
			addPacket.dwGuildID = GetGuild()->GetID();
#if defined(__GUILD_LEADER_GRADE_NAME__)
			CGuild* pGuild = this->GetGuild();
			if (pGuild->GetMasterPID() == GetPlayerID())
				addPacket.bGuildLeaderGrade = 3;
			else if (pGuild->GetGeneralPID(GetPlayerID()) == true)
				addPacket.bGuildLeaderGrade = 2;
			else
				addPacket.bGuildLeaderGrade = 1;
#endif
		}

		addPacket.sAlignment = m_iAlignment / 10;

		d->Packet(&addPacket, sizeof(TPacketGCCharacterAdditionalInfo));
	}

	if (iDur)
	{
		TPacketGCMove pack;
		EncodeMovePacket(pack, GetVID(), FUNC_MOVE, 0, m_posDest.x, m_posDest.y, iDur, 0, (BYTE)(GetRotation() / 5));
		d->Packet(&pack, sizeof(pack));

		TPacketGCWalkMode p;
		p.vid = GetVID();
		p.header = HEADER_GC_WALK_MODE;
		p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

		d->Packet(&p, sizeof(p));
	}

	if (entity->IsType(ENTITY_CHARACTER) && GetDesc())
	{
		LPCHARACTER ch = (LPCHARACTER)entity;
		if (ch->IsWalking())
		{
			TPacketGCWalkMode p;
			p.vid = ch->GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = ch->m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;
			GetDesc()->Packet(&p, sizeof(p));
		}
	}

	if (GetMyShop())
	{
		TPacketGCShopSign p;

		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
		strlcpy(p.szSign, m_stShopSign.c_str(), sizeof(p.szSign));
#if defined(__MYSHOP_DECO__)
		p.bType = GetMyShopDecoType();
#endif

		d->Packet(&p, sizeof(TPacketGCShopSign));
	}

	if (entity->IsType(ENTITY_CHARACTER))
	{
		sys_log(3, "EntityInsert %s (RaceNum %d) (%d %d) TO %s",
			GetName(), GetRaceNum(), GetX() / SECTREE_SIZE, GetY() / SECTREE_SIZE, ((LPCHARACTER)entity)->GetName());
	}
}

void CHARACTER::EncodeRemovePacket(LPENTITY entity)
{
	if (entity->GetEntityType() != ENTITY_CHARACTER)
		return;

	LPDESC d;

	if (!(d = entity->GetDesc()))
		return;

	TPacketGCCharacterDelete pack;

	pack.header = HEADER_GC_CHARACTER_DEL;
	pack.id = m_vid;

	d->Packet(&pack, sizeof(TPacketGCCharacterDelete));

	if (entity->IsType(ENTITY_CHARACTER))
		sys_log(3, "EntityRemove %s(%d) FROM %s", GetName(), (DWORD)m_vid, ((LPCHARACTER)entity)->GetName());
}

void CHARACTER::UpdatePacket()
{
	if (IsPC() && (!GetDesc() || !GetDesc()->GetCharacter()))
		return;

	if (GetSectree() == NULL)
		return;

	if (IsWarping())
		return;

	TPacketGCCharacterUpdate pack;
	TPacketGCCharacterUpdate pack2;

	pack.header = HEADER_GC_CHARACTER_UPDATE;
	pack.dwVID = m_vid;

	pack.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
	pack.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
	pack.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
	pack.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#if defined(__ACCE_COSTUME_SYSTEM__)
	pack.awPart[CHR_EQUIPPART_ACCE] = GetPart(PART_ACCE);
#endif
#ifdef __AURA_SYSTEM__
	pack.awPart[CHR_EQUIPPART_AURA] = GetPart(PART_AURA);
#endif

#if defined(__QUIVER_SYSTEM__)
	pack.dwArrow = GetWear(WEAR_ARROW) != NULL ? GetWear(WEAR_ARROW)->GetOriginalVnum() : 0;
#endif

#if defined(__SKILL_COLOR_SYSTEM__)
	memcpy(pack.dwSkillColor, GetSkillColor(), sizeof(pack.dwSkillColor));
#endif

	pack.bMovingSpeed = GetLimitPoint(POINT_MOV_SPEED);
	pack.bAttackSpeed = GetLimitPoint(POINT_ATT_SPEED);
	pack.bStateFlag = m_bAddChrState;
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];
	pack.dwGuildID = GetGuild() ? GetGuild()->GetID() : 0;
	pack.sAlignment = m_iAlignment / 10;
	//pack.dwLevel = IsPC() ? GetLevel() : 0;
	pack.dwLevel = GetLevel();
#if defined(__CONQUEROR_LEVEL__)
	pack.dwConquerorLevel = IsPC() ? GetConquerorLevel() : 0;
#endif
	pack.bPKMode = m_bPKMode;
#ifdef ENABLE_REFINE_ELEMENT
	pack.dwElementsEffect = GetElementsEffect();
#endif
	pack.dwMountVnum = GetMountVnum();
	pack.bLanguage = GetLanguage();

#if defined(__GUILD_LEADER_GRADE_NAME__)
	CGuild* pGuild = this->GetGuild();
	if (pGuild)
	{
		if (pGuild->GetMasterPID() == GetPlayerID())
			pack.bGuildLeaderGrade = 3;
		else if (pGuild->GetGeneralPID(GetPlayerID()) == true)
			pack.bGuildLeaderGrade = 2;
		else
			pack.bGuildLeaderGrade = 1;
	}
	else
	{
		pack.bGuildLeaderGrade = 0;
	}
#endif

	pack2 = pack;
	pack2.dwGuildID = 0;
	pack2.sAlignment = 0;

	if (false)
	{
		if (m_bIsObserver != true)
		{
			for (auto iter = m_map_view.begin(); iter != m_map_view.end(); iter++)
			{
				LPENTITY pEntity = iter->first;

				if (pEntity != NULL)
				{
					if (pEntity->IsType(ENTITY_CHARACTER) == true)
					{
						if (pEntity->GetDesc() != NULL)
						{
							LPCHARACTER pChar = (LPCHARACTER)pEntity;

							if (GetEmpire() == pChar->GetEmpire() || pChar->GetGMLevel() > GM_PLAYER)
							{
								pEntity->GetDesc()->Packet(&pack, sizeof(pack));
							}
							else
							{
								pEntity->GetDesc()->Packet(&pack2, sizeof(pack2));
							}
						}
					}
					else
					{
						if (pEntity->GetDesc() != NULL)
						{
							pEntity->GetDesc()->Packet(&pack, sizeof(pack));
						}
					}
				}
			}
		}

		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&pack, sizeof(pack));
		}
	}
	else
	{
		PacketAround(&pack, sizeof(pack));
	}
}

LPCHARACTER CHARACTER::FindCharacterInView(const char* c_pszName, bool bFindPCOnly)
{
	auto it = m_map_view.begin();

	for (; it != m_map_view.end(); ++it)
	{
		if (!it->first->IsType(ENTITY_CHARACTER))
			continue;

		LPCHARACTER tch = (LPCHARACTER)it->first;

		if (bFindPCOnly && tch->IsNPC())
			continue;

		if (!strcasecmp(tch->GetName(), c_pszName))
			return (tch);
	}

	return NULL;
}

void CHARACTER::SetPosition(int pos)
{
	if (pos == POS_STANDING)
	{
		REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

		event_cancel(&m_pkDeadEvent);
		event_cancel(&m_pkStunEvent);
	}
	else if (pos == POS_DEAD)
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);

	if (!IsStone())
	{
		switch (pos)
		{
		case POS_FIGHTING:
			if (!IsState(m_stateBattle))
				MonsterLog("BATTLE");

			GotoState(m_stateBattle);
			break;

		default:
			if (!IsState(m_stateIdle))
				MonsterLog("IDLE");

			GotoState(m_stateIdle);
			break;
		}
	}

	m_pointsInstant.position = pos;
}

void CHARACTER::Save()
{
	if (!m_bSkipSave)
		CHARACTER_MANAGER::instance().DelayedSave(this);
}

void CHARACTER::CreatePlayerProto(TPlayerTable& tab)
{
	memset(&tab, 0, sizeof(TPlayerTable));

	if (GetNewName().empty())
	{
		strlcpy(tab.name, GetName(), sizeof(tab.name));
	}
	else
	{
		strlcpy(tab.name, GetNewName().c_str(), sizeof(tab.name));
	}

	strlcpy(tab.ip, GetDesc()->GetHostName(), sizeof(tab.ip));

	tab.id = m_dwPlayerID;
	tab.voice = GetPoint(POINT_VOICE);
	tab.level = GetLevel();
	tab.level_step = GetPoint(POINT_LEVEL_STEP);
	tab.exp = GetExp();
	tab.gold = GetGold();
#if defined(__CHEQUE_SYSTEM__)
	tab.cheque = GetCheque();
#endif
#if defined(__GEM_SYSTEM__)
	tab.gem = GetGem();
#endif
	tab.job = m_points.job;
	tab.part_base = m_pointsInstant.bBasePart;
	tab.skill_group = m_points.skill_group;

#if defined(__ENABLE_RIDING_EXTENDED__)
	tab.mount_up_grade_exp = GetMountUpGradeExp();
	tab.mount_up_grade_fail = IsMountUpGradeFail();
#endif

#ifdef ENABLE_MEDAL_OF_HONOR
	tab.medal_honor = GetMedalHonor();
	// tab.aiShopExUsablePoint[SHOP_COIN_TYPE_MEDAL_OF_HONOR] = GetShopExUsablePoint(EX_SHOP_COIN_TYPE_MEDAL_OF_HONOR);
	// tab.aiShopExDailyUse[SHOP_COIN_TYPE_MEDAL_OF_HONOR] = GetShopExDailyTimePoint(EX_SHOP_COIN_TYPE_MEDAL_OF_HONOR);
	tab.aiShopExUsablePoint[SHOP_COIN_TYPE_MEDAL_OF_HONOR] = GetShopExUsablePoint(SHOP_COIN_TYPE_MEDAL_OF_HONOR);
	tab.aiShopExDailyUse[SHOP_COIN_TYPE_MEDAL_OF_HONOR] = GetShopExDailyTimePoint(SHOP_COIN_TYPE_MEDAL_OF_HONOR);
#endif

#ifdef ENABLE_12ZI
	/*tab.bead_point = GetBeadPoint();
	tab.bead_next_time = GetBeadNextTime();*/
#endif
#if defined(ENABLE_12ZI) && defined(ENABLE_12ZI_REWARD_TABLE)
	thecore_memcpy(tab.ziRewardTable, m_ziRewardTable, sizeof(TZiRewardTable) * 2);
#endif

	DWORD dwPlayedTime = (get_dword_time() - m_dwPlayStartTime);

	if (dwPlayedTime > 60000)
	{
		if (GetSectree() && !GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
		{
			if (GetRealAlignment() < 0)
			{
				if (IsEquipUniqueItem(UNIQUE_ITEM_FASTER_ALIGNMENT_UP_BY_TIME))
					UpdateAlignment(120 * (dwPlayedTime / 60000));
				else
					UpdateAlignment(60 * (dwPlayedTime / 60000));
			}
			else
				UpdateAlignment(5 * (dwPlayedTime / 60000));
		}

		SetRealPoint(POINT_PLAYTIME, GetRealPoint(POINT_PLAYTIME) + dwPlayedTime / 60000);
		ResetPlayTime(dwPlayedTime % 60000);
	}

	tab.playtime = GetRealPoint(POINT_PLAYTIME);
	tab.lAlignment = m_iRealAlignment;

	if (m_posWarp.x != 0 || m_posWarp.y != 0)
	{
		tab.x = m_posWarp.x;
		tab.y = m_posWarp.y;
		tab.z = 0;
		tab.lMapIndex = m_lWarpMapIndex;
	}
	else
	{
		tab.x = GetX();
		tab.y = GetY();
		tab.z = GetZ();
		tab.lMapIndex = GetMapIndex();
	}

	if (m_lExitMapIndex == 0)
	{
		tab.lExitMapIndex = tab.lMapIndex;
		tab.lExitX = tab.x;
		tab.lExitY = tab.y;
	}
	else
	{
		tab.lExitMapIndex = m_lExitMapIndex;
		tab.lExitX = m_posExit.x;
		tab.lExitY = m_posExit.y;
	}

	sys_log(0, "SAVE: %s %dx%d", GetName(), tab.x, tab.y);

	tab.st = GetRealPoint(POINT_ST);
	tab.ht = GetRealPoint(POINT_HT);
	tab.dx = GetRealPoint(POINT_DX);
	tab.iq = GetRealPoint(POINT_IQ);

	tab.stat_point = GetPoint(POINT_STAT);
	tab.skill_point = GetPoint(POINT_SKILL);
	tab.sub_skill_point = GetPoint(POINT_SUB_SKILL);
	tab.horse_skill_point = GetPoint(POINT_HORSE_SKILL);

	tab.stat_reset_count = GetPoint(POINT_STAT_RESET_COUNT);

	tab.hp = GetHP();
	tab.sp = GetSP();

	tab.stamina = GetStamina();

	tab.sRandomHP = m_points.iRandomHP;
	tab.sRandomSP = m_points.iRandomSP;

	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
		tab.quickslot[i] = m_quickslot[i];

	thecore_memcpy(tab.parts, m_pointsInstant.parts, sizeof(tab.parts));

	// REMOVE_REAL_SKILL_LEVLES
	thecore_memcpy(tab.skills, m_pSkillLevels, sizeof(TPlayerSkill) * SKILL_MAX_NUM);
	// END_OF_REMOVE_REAL_SKILL_LEVLES

	tab.horse = GetHorseData();

#if defined(__CONQUEROR_LEVEL__)
	tab.conqueror_level = GetConquerorLevel();
	tab.conqueror_level_step = GetPoint(POINT_CONQUEROR_LEVEL_STEP);
	tab.conqueror_exp = GetConquerorExp();

	tab.sungma_str = GetRealPoint(POINT_SUNGMA_STR);
	tab.sungma_hp = GetRealPoint(POINT_SUNGMA_HP);
	tab.sungma_move = GetRealPoint(POINT_SUNGMA_MOVE);
	tab.sungma_immune = GetRealPoint(POINT_SUNGMA_IMMUNE);

	tab.conqueror_point = GetPoint(POINT_CONQUEROR_POINT);
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	tab.m_BiologActualMission = GetBiologMissions();
	tab.m_BiologCollectedItems = GetBiologCollectedItems();
	tab.m_BiologCooldownReminder = GetBiologCooldownReminder();
	tab.m_BiologCooldown = GetBiologCooldown();
#endif

}

void CHARACTER::SaveReal()
{
	if (m_bSkipSave)
		return;

	if (!GetDesc())
	{
		sys_err("Character::Save : no descriptor when saving (name: %s)", GetName());
		return;
	}

	TPlayerTable table;
	CreatePlayerProto(table);

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_SAVE, GetDesc()->GetHandle(), &table, sizeof(TPlayerTable));

	quest::PC* pkQuestPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());

	if (!pkQuestPC)
		sys_err("CHARACTER::Save : null quest::PC pointer! (name %s)", GetName());
	else
	{
		pkQuestPC->Save();
	}

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
	if (pMarriage)
		pMarriage->Save();
}

void CHARACTER::FlushDelayedSaveItem()
{
	// ï¿½ï¿½ï¿½ï¿½ ï¿½Èµï¿½ ï¿½ï¿½ï¿½ï¿½Ç°ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Å²ï¿½ï¿½.
	LPITEM item;

	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
		if ((item = GetInventoryItem(i)))
			ITEM_MANAGER::instance().FlushDelayedSave(item);
}

void CHARACTER::Disconnect(const char* c_pszReason, int type)
{
	assert(GetDesc() != NULL);

	sys_log(0, "DISCONNECT: %s (%s)", GetName(), c_pszReason ? c_pszReason : "unset");

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
	if (GetGuild() && IsOpenGuildstorage())
	{
		GetGuild()->SetStorageState(false, 0);
	}
#endif

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	if (GetArena() != NULL)
	{
		GetArena()->OnDisconnect(GetPlayerID());
	}

	if (GetParty() != NULL)
	{
		GetParty()->UpdateOfflineState(GetPlayerID());
	}

	marriage::CManager::instance().Logout(this);

	// P2P Logout
	TPacketGGLogout p;
	p.bHeader = HEADER_GG_LOGOUT;
	strlcpy(p.szName, GetName(), sizeof(p.szName));
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogout));

	char buf[51];
#if defined(__CHEQUE_SYSTEM__) && defined(__GEM_SYSTEM__)
	snprintf(buf, sizeof(buf), "%lld %d %d %d %d %d",
		GetGold(), GetCheque(), GetGem(), g_bChannel, GetMapIndex(), GetAlignment());
	//snprintf(buf, sizeof(buf), "%s %lld %d %d %d %ld %d",
		//inet_ntoa(GetDesc()->GetAddr().sin_addr), GetGold(), GetCheque(), GetGem(), g_bChannel, GetMapIndex(), GetAlignment());
#elif defined(__CHEQUE_SYSTEM__)
	snprintf(buf, sizeof(buf), "%s %lld %d %d %d %d",
		GetGold(), GetCheque(), g_bChannel, GetMapIndex(), GetAlignment());
	//snprintf(buf, sizeof(buf), "%s %lld %d %d %ld %d",
	//	inet_ntoa(GetDesc()->GetAddr().sin_addr), GetGold(), GetCheque(), g_bChannel, GetMapIndex(), GetAlignment());
#else
	snprintf(buf, sizeof(buf), "%lld %d %d %d",
		GetGold(), g_bChannel, GetMapIndex(), GetAlignment());
	//snprintf(buf, sizeof(buf), "%s %lld %d %ld %d",
	//	inet_ntoa(GetDesc()->GetAddr().sin_addr), GetGold(), g_bChannel, GetMapIndex(), GetAlignment());
#endif

	LogManager::instance().CharLog(this, 0, "LOGOUT", buf);

#if defined(__GUILD_DRAGONLAIR__)
	if (MeleyLair::CMgr::instance().IsMeleyMap(GetMapIndex()))
		MeleyLair::CMgr::instance().Leave(GetGuild(), this, false);
#endif

	if (LC_IsYMIR() || LC_IsKorea() || LC_IsBrazil())
	{
		long playTime = GetRealPoint(POINT_PLAYTIME) - m_dwLoginPlayTime;
		LogManager::instance().LoginLog(false, GetDesc()->GetAccountTable().id, GetPlayerID(), GetLevel(), GetJob(), playTime);

		if (LC_IsBrazil() != true)
			CPCBangManager::instance().Log(GetDesc()->GetHostName(), GetPlayerID(), playTime);
	}

	if (m_pWarMap)
		SetWarMap(NULL);

	if (m_pWeddingMap)
	{
		SetWeddingMap(NULL);
	}

	if (GetGuild())
		GetGuild()->LogoutMember(this);

	quest::CQuestManager::instance().LogoutPC(this);

	if (GetParty())
		GetParty()->Unlink(this);

	// ï¿½×¾ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½Ó²ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Ä¡ ï¿½Ù°ï¿½ ï¿½Ï±ï¿½
	if (IsStun() || IsDead())
	{
		DeathPenalty(0);
		PointChange(POINT_HP, 50 - GetHP());
	}

	if (!CHARACTER_MANAGER::instance().FlushDelayedSave(this))
	{
		SaveReal();
	}

#ifdef __ENABLE_NEW_OFFLINESHOP__
	offlineshop::GetManager().RemoveSafeboxFromCache(GetPlayerID());
	offlineshop::GetManager().RemoveGuestFromShops(this);

	if (m_pkAuctionGuest)
	{
		m_pkAuctionGuest->RemoveGuest(this);
	}

	if (GetOfflineShop())
	{
		SetOfflineShop(NULL);
	}

	SetShopSafebox(NULL);

	m_pkAuction = NULL;
	m_pkAuctionGuest = NULL;
	m_bIsLookingOfflineshopOfferList = false;
#endif

	FlushDelayedSaveItem();

	SaveAffect();
	m_bIsLoadedAffect = false;

#ifdef __ENABLE_BIOLOG_SYSTEM__
	if (m_pkBiologManager)
	{
		delete m_pkBiologManager;
		m_pkBiologManager = NULL;
	}
#endif

	m_bSkipSave = true; // ï¿½ï¿½ ï¿½ï¿½ï¿½Ä¿ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ì»ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï¸ï¿½ ï¿½ÈµÈ´ï¿½.

	quest::CQuestManager::instance().DisconnectPC(this);

	CloseSafebox();

	CloseMall();

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
	CloseGuildstorage();
#endif

#ifdef __AURA_SYSTEM__
	if (IsAuraRefineWindowOpen())
		AuraRefineWindowClose();
#endif

	CPVPManager::instance().Disconnect(this);

	CTargetManager::instance().Logout(GetPlayerID());

	MessengerManager::instance().Logout(GetName());

	if (GetDesc())
	{
#if defined(__IMPROVED_LOGOUT_POINTS__)
		packet_point_change pack;
		pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
		pack.dwVID = m_vid;
		pack.type = POINT_PLAYTIME;
		pack.value = GetRealPoint(POINT_PLAYTIME) + (get_dword_time() - m_dwPlayStartTime) / 60000;
		pack.amount = 0;
		GetDesc()->Packet(&pack, sizeof(struct packet_point_change));
#endif
		GetDesc()->BindCharacter(NULL);
		GetDesc()->SendRequestLogout(type);
		// BindDesc(NULL);
	}

	M2_DESTROY_CHARACTER(this);
}

#if defined(__WJ_SHOW_MOB_INFO__)
bool CHARACTER::Show(long lMapIndex, long x, long y, long z, bool bShowSpawnMotion, bool bAggressive)
#else
bool CHARACTER::Show(long lMapIndex, long x, long y, long z, bool bShowSpawnMotion/* = false */)
#endif
{
	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "cannot find sectree by %dx%d mapindex %d", x, y, lMapIndex);
		return false;
	}

	SetMapIndex(lMapIndex);

	bool bChangeTree = false;

	if (!GetSectree() || GetSectree() != sectree)
		bChangeTree = true;

	if (bChangeTree)
	{
		if (GetSectree())
			GetSectree()->RemoveEntity(this);

#ifdef ENABLE_GOTO_LAG_FIX
		ViewCleanup(IsPC());
#else
		ViewCleanup();
#endif
	}

	if (!IsNPC())
	{
		sys_log(0, "SHOW: %s %dx%dx%d", GetName(), x, y, z);
		if (GetStamina() < GetMaxStamina())
			StartAffectEvent();
	}
	else if (m_pkMobData)
	{
		m_pkMobInst->m_posLastAttacked.x = x;
		m_pkMobInst->m_posLastAttacked.y = y;
		m_pkMobInst->m_posLastAttacked.z = z;
	}

	if (bShowSpawnMotion)
	{
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
		m_afAffectFlag.Set(AFF_SPAWN);
	}

#if defined(__WJ_SHOW_MOB_INFO__)
	if (bAggressive)
		SetAggressive();
#endif

	SetXYZ(x, y, z);

	m_posDest.x = x;
	m_posDest.y = y;
	m_posDest.z = z;

	m_posStart.x = x;
	m_posStart.y = y;
	m_posStart.z = z;

	if (bChangeTree)
	{
		EncodeInsertPacket(this);
		sectree->InsertEntity(this);

		UpdateSectree();
	}
	else
	{
		ViewReencode();
		sys_log(0, "      in same sectree");
	}

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	SetValidComboInterval(0);
	return true;
}

// BGM_INFO
struct BGMInfo
{
	std::string name;
	float vol;
};

typedef std::map<unsigned, BGMInfo> BGMInfoMap;

static BGMInfoMap gs_bgmInfoMap;
static bool gs_bgmVolEnable = false;

void CHARACTER_SetBGMVolumeEnable()
{
	gs_bgmVolEnable = true;
	sys_log(0, "bgm_info.set_bgm_volume_enable");
}

void CHARACTER_AddBGMInfo(unsigned mapIndex, const char* name, float vol)
{
	BGMInfo newInfo;
	newInfo.name = name;
	newInfo.vol = vol;

	gs_bgmInfoMap[mapIndex] = newInfo;

	sys_log(0, "bgm_info.add_info(%d, '%s', %f)", mapIndex, name, vol);
}

const BGMInfo& CHARACTER_GetBGMInfo(unsigned mapIndex)
{
	BGMInfoMap::iterator f = gs_bgmInfoMap.find(mapIndex);
	if (gs_bgmInfoMap.end() == f)
	{
		static BGMInfo s_empty = { "", 0.0f };
		return s_empty;
	}
	return f->second;
}

bool CHARACTER_IsBGMVolumeEnable()
{
	return gs_bgmVolEnable;
}
// END_OF_BGM_INFO

void CHARACTER::MainCharacterPacket()
{
	const unsigned mapIndex = GetMapIndex();
	const BGMInfo& bgmInfo = CHARACTER_GetBGMInfo(mapIndex);

	// SUPPORT_BGM
	if (!bgmInfo.name.empty())
	{
		if (CHARACTER_IsBGMVolumeEnable())
		{
			sys_log(1, "bgm_info.play_bgm_vol(%d, name='%s', vol=%f)", mapIndex, bgmInfo.name.c_str(), bgmInfo.vol);
			TPacketGCMainCharacter4_BGM_VOL mainChrPacket;
			mainChrPacket.header = HEADER_GC_MAIN_CHARACTER4_BGM_VOL;
			mainChrPacket.dwVID = m_vid;
			mainChrPacket.wRaceNum = GetRaceNum();
			mainChrPacket.lx = GetX();
			mainChrPacket.ly = GetY();
			mainChrPacket.lz = GetZ();
			mainChrPacket.empire = GetDesc()->GetEmpire();
			mainChrPacket.skill_group = GetSkillGroup();
			strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));

			mainChrPacket.fBGMVol = bgmInfo.vol;
			strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter4_BGM_VOL));
		}
		else
		{
			sys_log(1, "bgm_info.play(%d, '%s')", mapIndex, bgmInfo.name.c_str());
			TPacketGCMainCharacter3_BGM mainChrPacket;
			mainChrPacket.header = HEADER_GC_MAIN_CHARACTER3_BGM;
			mainChrPacket.dwVID = m_vid;
			mainChrPacket.wRaceNum = GetRaceNum();
			mainChrPacket.lx = GetX();
			mainChrPacket.ly = GetY();
			mainChrPacket.lz = GetZ();
			mainChrPacket.empire = GetDesc()->GetEmpire();
			mainChrPacket.skill_group = GetSkillGroup();
			strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));
			strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter3_BGM));
		}
	}
	// END_OF_SUPPORT_BGM
	else
	{
		sys_log(0, "bgm_info.play(%d, DEFAULT_BGM_NAME)", mapIndex);

		TPacketGCMainCharacter pack;
		pack.header = HEADER_GC_MAIN_CHARACTER;
		pack.dwVID = m_vid;
		pack.wRaceNum = GetRaceNum();
		pack.lx = GetX();
		pack.ly = GetY();
		pack.lz = GetZ();
		pack.empire = GetDesc()->GetEmpire();
		pack.skill_group = GetSkillGroup();
		strlcpy(pack.szName, GetName(), sizeof(pack.szName));
		GetDesc()->Packet(&pack, sizeof(TPacketGCMainCharacter));
	}
}

void CHARACTER::PointsPacket()
{
	if (!GetDesc())
		return;

	TPacketGCPoints pack;

	pack.header = HEADER_GC_CHARACTER_POINTS;

	pack.points[POINT_LEVEL] = GetLevel();
	pack.points[POINT_EXP] = GetExp();
	pack.points[POINT_NEXT_EXP] = GetNextExp();
	pack.points[POINT_HP] = GetHP();
	pack.points[POINT_MAX_HP] = GetMaxHP();
	pack.points[POINT_SP] = GetSP();
	pack.points[POINT_MAX_SP] = GetMaxSP();
	pack.points[POINT_GOLD] = GetGold();
	pack.points[POINT_STAMINA] = GetStamina();
	pack.points[POINT_MAX_STAMINA] = GetMaxStamina();

	for (int i = POINT_ST; i < POINT_MAX_NUM; ++i)
		pack.points[i] = GetPoint(i);

#ifdef ENABLE_12ZI
	//pack.points[POINT_BEAD] = GetBeadPoint();
#endif

#if defined(__CONQUEROR_LEVEL__)
	pack.points[POINT_CONQUEROR_LEVEL] = GetConquerorLevel();
	pack.points[POINT_CONQUEROR_EXP] = GetConquerorExp();
	pack.points[POINT_CONQUEROR_NEXT_EXP] = GetConquerorNextExp();

	pack.points[POINT_MOV_SPEED] = GetLimitPoint(POINT_MOV_SPEED);
#endif

#if defined(__CHEQUE_SYSTEM__)
	pack.points[POINT_CHEQUE] = GetCheque();
#endif
#if defined(__GEM_SYSTEM__)
	pack.points[POINT_GEM] = GetGem();
#endif

#ifdef ENABLE_MEDAL_OF_HONOR
	pack.points[POINT_MEDAL_OF_HONOR] = GetMedalHonor();
#endif

	GetDesc()->Packet(&pack, sizeof(TPacketGCPoints));
}

void CHARACTER::UpdatePointsPacket(BYTE type, long long val, long long amount, bool bAmount, bool bBroadcast)
{
	//if (GetDesc())
	if (GetDesc() && !IsWarping())
	{
		struct packet_point_change pack;

#if defined(__CONQUEROR_LEVEL__)
		switch (type)
		{
		case POINT_MOV_SPEED:
			if (GetSungMaWill(POINT_SUNGMA_MOVE) > GetPoint(POINT_SUNGMA_MOVE))
				val /= 2;
			break;
		}
#endif

		pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
		pack.dwVID = m_vid;
		pack.type = type;
		pack.value = val;

		if (bAmount)
			pack.amount = amount;
		else
			pack.amount = 0;

		if (!bBroadcast)
			GetDesc()->Packet(&pack, sizeof(struct packet_point_change));
		else
			PacketAround(&pack, sizeof(pack));
	}
}

bool CHARACTER::ChangeSex()
{
	int src_race = GetRaceNum();

	switch (src_race)
	{
	case MAIN_RACE_WARRIOR_M:
		m_points.job = MAIN_RACE_WARRIOR_W;
		break;

	case MAIN_RACE_WARRIOR_W:
		m_points.job = MAIN_RACE_WARRIOR_M;
		break;

	case MAIN_RACE_ASSASSIN_M:
		m_points.job = MAIN_RACE_ASSASSIN_W;
		break;

	case MAIN_RACE_ASSASSIN_W:
		m_points.job = MAIN_RACE_ASSASSIN_M;
		break;

	case MAIN_RACE_SURA_M:
		m_points.job = MAIN_RACE_SURA_W;
		break;

	case MAIN_RACE_SURA_W:
		m_points.job = MAIN_RACE_SURA_M;
		break;

	case MAIN_RACE_SHAMAN_M:
		m_points.job = MAIN_RACE_SHAMAN_W;
		break;

	case MAIN_RACE_SHAMAN_W:
		m_points.job = MAIN_RACE_SHAMAN_M;
		break;

	case MAIN_RACE_WOLFMAN_M:
		m_points.job = MAIN_RACE_WOLFMAN_M;
		break;

	default:
		sys_err("CHANGE_SEX: %s unknown race %d", GetName(), src_race);
		return false;
	}

	sys_log(0, "CHANGE_SEX: %s (%d -> %d)", GetName(), src_race, m_points.job);
	return true;
}

bool CHARACTER::IsMale() const
{
	switch (GetRaceNum())
	{
	case MAIN_RACE_WARRIOR_M:
	case MAIN_RACE_SURA_M:
	case MAIN_RACE_ASSASSIN_M:
	case MAIN_RACE_SHAMAN_M:
	case MAIN_RACE_WOLFMAN_M:
		return true;
	}
	return false;
}

bool CHARACTER::IsFemale() const
{
	switch (GetRaceNum())
	{
	case MAIN_RACE_ASSASSIN_W:
	case MAIN_RACE_SHAMAN_W:
	case MAIN_RACE_WARRIOR_W:
	case MAIN_RACE_SURA_W:
		return true;
	}
	return false;
}

WORD CHARACTER::GetRaceNum() const
{
	if (m_dwPolymorphRace)
		return m_dwPolymorphRace;

	if (m_pkMobData)
		return m_pkMobData->m_table.dwVnum;

	return m_points.job;
}

void CHARACTER::SetRace(BYTE race)
{
	if (race >= MAIN_RACE_MAX_NUM)
	{
		sys_err("CHARACTER::SetRace(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
		return;
	}

	m_points.job = race;
}

BYTE CHARACTER::GetJob() const
{
	unsigned race = m_points.job;
	unsigned job;

	if (RaceToJob(race, &job))
		return job;

	sys_err("CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
	return JOB_WARRIOR;
}

void CHARACTER::SetLevel(BYTE level)
{
	m_points.level = level;

	if (IsPC())
	{
		if (level < PK_PROTECT_LEVEL)
			SetPKMode(PK_MODE_PROTECT);
		else if (GetGMLevel() != GM_PLAYER)
			SetPKMode(PK_MODE_PROTECT);
		else if (m_bPKMode == PK_MODE_PROTECT)
			SetPKMode(PK_MODE_PEACE);
	}
}

void CHARACTER::SetBasicCsotume(LPCHARACTER ch)
{
	int race = GetRaceNum();
	BYTE job = ch->GetJob();
	LPITEM item;
	item = ch->GetWear(WEAR_COSTUME_BODY);
	if (item != NULL)
		ch->UnequipItem(item);
	switch (race)
	{
	case MAIN_RACE_WARRIOR_M:
	{
		item = ITEM_MANAGER::instance().CreateItem(41856);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_WARRIOR_W:
	{
		item = ITEM_MANAGER::instance().CreateItem(41858);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_ASSASSIN_M:
	{
		item = ITEM_MANAGER::instance().CreateItem(41856);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_ASSASSIN_W:
	{
		item = ITEM_MANAGER::instance().CreateItem(41858);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_SURA_M:
	{
		item = ITEM_MANAGER::instance().CreateItem(41856);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_SURA_W:
	{
		item = ITEM_MANAGER::instance().CreateItem(41858);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_SHAMAN_M:
	{
		item = ITEM_MANAGER::instance().CreateItem(41856);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_SHAMAN_W:
	{
		item = ITEM_MANAGER::instance().CreateItem(41858);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_WOLFMAN_M:
	{
		item = ITEM_MANAGER::instance().CreateItem(41856);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	}
}

void CHARACTER::SetBasicCsotumeHair(LPCHARACTER ch)
{
	int race = GetRaceNum();
	BYTE job = ch->GetJob();
	LPITEM item;
	item = ch->GetWear(WEAR_COSTUME_HAIR);
	if (item != NULL)
		ch->UnequipItem(item);
	switch (race)
	{
	case MAIN_RACE_WARRIOR_M:
	{
		item = ITEM_MANAGER::instance().CreateItem(45596);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_WARRIOR_W:
	{
		item = ITEM_MANAGER::instance().CreateItem(45598);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_ASSASSIN_M:
	{
		item = ITEM_MANAGER::instance().CreateItem(45596);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_ASSASSIN_W:
	{
		item = ITEM_MANAGER::instance().CreateItem(45598);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_SURA_M:
	{
		item = ITEM_MANAGER::instance().CreateItem(45596);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_SURA_W:
	{
		item = ITEM_MANAGER::instance().CreateItem(45598);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_SHAMAN_M:
	{
		item = ITEM_MANAGER::instance().CreateItem(45596);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_SHAMAN_W:
	{
		item = ITEM_MANAGER::instance().CreateItem(45598);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	case MAIN_RACE_WOLFMAN_M:
	{
		item = ITEM_MANAGER::instance().CreateItem(45596);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
			M2_DESTROY_ITEM(item);
	}
	break;
	}
}

void CHARACTER::SetEmpire(BYTE bEmpire)
{
	m_bEmpire = bEmpire;
}

void CHARACTER::SetPlayerProto(const TPlayerTable* t)
{
	if (!GetDesc() || !*GetDesc()->GetHostName())
		sys_err("cannot get desc or hostname");
	else
		SetGMLevel();

	m_bCharType = CHAR_TYPE_PC;

	m_dwPlayerID = t->id;

	m_iAlignment = t->lAlignment;
	m_iRealAlignment = t->lAlignment;

	m_points.voice = t->voice;

	m_points.skill_group = t->skill_group;

	m_pointsInstant.bBasePart = t->part_base;
	SetPart(PART_HAIR, t->parts[PART_HAIR]);
#if defined(__ACCE_COSTUME_SYSTEM__)
	SetPart(PART_ACCE, t->parts[PART_ACCE]);
#endif
#ifdef __AURA_SYSTEM__
	SetPart(PART_AURA, t->parts[PART_AURA]);
#endif

	m_points.iRandomHP = t->sRandomHP;
	m_points.iRandomSP = t->sRandomSP;

	// REMOVE_REAL_SKILL_LEVLES
	if (m_pSkillLevels)
		M2_DELETE_ARRAY(m_pSkillLevels);

	m_pSkillLevels = M2_NEW TPlayerSkill[SKILL_MAX_NUM];
	thecore_memcpy(m_pSkillLevels, t->skills, sizeof(TPlayerSkill) * SKILL_MAX_NUM);
	// END_OF_REMOVE_REAL_SKILL_LEVLES

#if defined(ENABLE_12ZI) && defined(ENABLE_12ZI_REWARD_TABLE)
	if (m_ziRewardTable)
		M2_DELETE_ARRAY(m_ziRewardTable);

	m_ziRewardTable = M2_NEW TZiRewardTable[2];
	thecore_memcpy(m_ziRewardTable, t->ziRewardTable, sizeof(TZiRewardTable) * 2);
#endif

	if (t->lMapIndex >= 10000)
	{
		bool bExitPosition = true;
#ifdef ENABLE_12ZI
		if (CZodiacManager::Instance().IsZiStageMapIndex(t->lMapIndex))
			bExitPosition = false;
#endif
		// #ifdef ENABLE_GUILD_DRAGONLAIR_SYSTEM
		// 		if (CMeleyLairManager::Instance().IsMeleyMap(t->lMapIndex))
		// 			bExitPosition = false;
		// #endif
		if (bExitPosition)
		{
			m_posWarp.x = t->lExitX;
			m_posWarp.y = t->lExitY;
			m_lWarpMapIndex = t->lExitMapIndex;
		}
	}

	SetRealPoint(POINT_PLAYTIME, t->playtime);
	m_dwLoginPlayTime = t->playtime;
	SetRealPoint(POINT_ST, t->st);
	SetRealPoint(POINT_HT, t->ht);
	SetRealPoint(POINT_DX, t->dx);
	SetRealPoint(POINT_IQ, t->iq);

	SetPoint(POINT_ST, t->st);
	SetPoint(POINT_HT, t->ht);
	SetPoint(POINT_DX, t->dx);
	SetPoint(POINT_IQ, t->iq);

	SetPoint(POINT_STAT, t->stat_point);
	SetPoint(POINT_SKILL, t->skill_point);
	SetPoint(POINT_SUB_SKILL, t->sub_skill_point);
	SetPoint(POINT_HORSE_SKILL, t->horse_skill_point);

	SetPoint(POINT_STAT_RESET_COUNT, t->stat_reset_count);

	SetPoint(POINT_LEVEL_STEP, t->level_step);
	SetRealPoint(POINT_LEVEL_STEP, t->level_step);

	SetRace(t->job);

	SetLevel(t->level);
	SetExp(t->exp);
	SetGold(t->gold);
#if defined(__ENABLE_RIDING_EXTENDED__)
	SetMountUpGradeExp(t->mount_up_grade_exp);
	SetMountUpGradeFail(t->mount_up_grade_fail);
#endif

#ifdef ENABLE_MEDAL_OF_HONOR
	SetMedalHonor(t->medal_honor);
	// SetShopExUsablePoint(SHOP_COIN_TYPE_MEDAL_OF_HONOR, t->aiShopExUsablePoint[EX_SHOP_COIN_TYPE_MEDAL_OF_HONOR]);
	// SetShopExDailyTimePoint(SHOP_COIN_TYPE_MEDAL_OF_HONOR, t->aiShopExDailyUse[EX_SHOP_COIN_TYPE_MEDAL_OF_HONOR]);
	SetShopExUsablePoint(SHOP_COIN_TYPE_MEDAL_OF_HONOR, t->aiShopExUsablePoint[SHOP_COIN_TYPE_MEDAL_OF_HONOR]);
	SetShopExDailyTimePoint(SHOP_COIN_TYPE_MEDAL_OF_HONOR, t->aiShopExDailyUse[SHOP_COIN_TYPE_MEDAL_OF_HONOR]);
#endif

#if defined(__CHEQUE_SYSTEM__)
	SetCheque(t->cheque);
#endif
#if defined(__GEM_SYSTEM__)
	SetGem(t->gem);
#endif

#ifdef ENABLE_12ZI
	/*SetBeadPoint(t->bead_point);
	SetBeadNextTime(t->bead_next_time);*/
#endif

#if defined(__CONQUEROR_LEVEL__)
	SetRealPoint(POINT_SUNGMA_STR, t->sungma_str);
	SetRealPoint(POINT_SUNGMA_HP, t->sungma_hp);
	SetRealPoint(POINT_SUNGMA_MOVE, t->sungma_move);
	SetRealPoint(POINT_SUNGMA_IMMUNE, t->sungma_immune);

	SetPoint(POINT_SUNGMA_STR, t->sungma_str);
	SetPoint(POINT_SUNGMA_HP, t->sungma_hp);
	SetPoint(POINT_SUNGMA_MOVE, t->sungma_move);
	SetPoint(POINT_SUNGMA_IMMUNE, t->sungma_immune);

	SetPoint(POINT_CONQUEROR_POINT, t->conqueror_point);

	SetPoint(POINT_CONQUEROR_LEVEL_STEP, t->conqueror_level_step);
	SetRealPoint(POINT_CONQUEROR_LEVEL_STEP, t->conqueror_level_step);

	SetConquerorLevel(t->conqueror_level);
	SetConquerorExp(t->conqueror_exp);
#endif

	SetMapIndex(t->lMapIndex);
	SetXYZ(t->x, t->y, t->z);

	ComputePoints();

	SetHP(t->hp);
	SetSP(t->sp);
	SetStamina(t->stamina);

	if (GetGMLevel() >= 3 && GetGMLevel() <= 5)
		m_afAffectFlag.Set(AFF_YMIR);

	if (GetLevel() < PK_PROTECT_LEVEL)
		m_bPKMode = PK_MODE_PROTECT;

	SetHorseData(t->horse);

	if (GetHorseLevel() > 0)
		UpdateHorseDataByLogoff(t->logoff_interval);

	thecore_memcpy(m_aiPremiumTimes, t->aiPremiumTimes, sizeof(t->aiPremiumTimes));

	m_dwLogOffInterval = t->logoff_interval;
	m_dwLastAttackTime = t->last_play;

	sys_log(0, "PLAYER_LOAD: %s PREMIUM %d %d, LOGGOFF_INTERVAL %u PTR: %p", t->name, m_aiPremiumTimes[0], m_aiPremiumTimes[1], t->logoff_interval, this);

	if (GetGMLevel() != GM_PLAYER)
	{
		LogManager::instance().CharLog(this, GetGMLevel(), "GM_LOGIN", "");
		sys_log(0, "GM_LOGIN(gmlevel=%d, name=%s(%d), pos=(%d, %d)", GetGMLevel(), GetName(), GetPlayerID(), GetX(), GetY());
	}

#if defined(__PET_SYSTEM__)
	// NOTE: ï¿½Ï´ï¿½ Ä³ï¿½ï¿½ï¿½Í°ï¿½ PCï¿½ï¿½ ï¿½ï¿½ì¿¡ï¿½ï¿½ PetSystemï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½. ï¿½ï¿½ï¿½ï¿½ ï¿½Ó½Å´ï¿½ ï¿½Þ¸ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ NPCï¿½ï¿½ï¿½ï¿½ ï¿½Ï±ï¿½ ï¿½ï¿½..
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;
	}

	m_petSystem = M2_NEW CPetSystem(this);
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (m_GrowthPetSystem)
	{
		m_GrowthPetSystem->Destroy();
		delete m_GrowthPetSystem;
	}

	m_GrowthPetSystem = M2_NEW CGrowthPetSystem(this);
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	if (m_pkBiologManager)
	{
		sys_err("Biolog manager already exist for owner %u %s", GetPlayerID(), GetName());
		delete m_pkBiologManager;
	}

	m_pkBiologManager = M2_NEW CBiologSystem(this);

	SetBiologMissions(t->m_BiologActualMission);
	SetBiologCollectedItems(t->m_BiologCollectedItems);
	SetBiologCooldownReminder(t->m_BiologCooldownReminder);
	SetBiologCooldown(t->m_BiologCooldown);
#endif

}

EVENTFUNC(kill_ore_load_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("kill_ore_load_even> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) // <Factor>
		return 0;

	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::SetProto(const CMob* pkMob)
{
	if (m_pkMobInst)
		M2_DELETE(m_pkMobInst);

	m_pkMobData = pkMob;
	m_pkMobInst = M2_NEW CMobInstance;

	m_bPKMode = PK_MODE_FREE;

	const TMobTable* t = &m_pkMobData->m_table;

	m_bCharType = t->bType;

	SetLevel(t->bLevel);
	SetEmpire(t->bEmpire);

	SetExp(t->dwExp);
	SetRealPoint(POINT_ST, t->bStr);
	SetRealPoint(POINT_DX, t->bDex);
	SetRealPoint(POINT_HT, t->bCon);
	SetRealPoint(POINT_IQ, t->bInt);

	ComputePoints();

	SetHP(GetMaxHP());
	SetSP(GetMaxSP());

	////////////////////
	m_pointsInstant.dwAIFlag = t->dwAIFlag;
	SetImmuneFlag(t->dwImmuneFlag);

	AssignTriggers(t);

	ApplyMobAttribute(t);

	if (IsStone())
	{
		DetermineDropMetinStone();
	}

	if (IsWarp() || IsGoto())
	{
		StartWarpNPCEvent();
	}

	CHARACTER_MANAGER::instance().RegisterRaceNumMap(this);

#if defined(__TEMPLE_OCHAO__)
	/*
	if (GetRaceNum() == TempleOchao::PORTAL)
		m_dwPlayStartTime = get_dword_time() + 60 * 1000;
	*/
#endif

	// XXX X-mas santa hardcoding
	if (GetRaceNum() == xmas::MOB_SANTA_VNUM)
	{
		SetPoint(POINT_ATT_GRADE_BONUS, 10);
		if (g_iUseLocale)
			SetPoint(POINT_DEF_GRADE_BONUS, 6);
		else
			SetPoint(POINT_DEF_GRADE_BONUS, 15);

		//ï¿½ï¿½Å¸ï¿½ï¿½
		//m_dwPlayStartTime = get_dword_time() + 10 * 60 * 1000;
		//ï¿½Å¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ 
		m_dwPlayStartTime = get_dword_time() + 30 * 1000;
		if (test_server)
			m_dwPlayStartTime = get_dword_time() + 30 * 1000;
	}

	// XXX CTF GuildWar hardcoding
	if (warmap::IsWarFlag(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
	}

	if (warmap::IsWarFlagBase(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
	}

	if (IsHorse() ||
		GetRaceNum() == 20101 ||
		GetRaceNum() == 20102 ||
		GetRaceNum() == 20103 ||
		GetRaceNum() == 20104 ||
		GetRaceNum() == 20105 ||
		GetRaceNum() == 20106 ||
		GetRaceNum() == 20107 ||
		GetRaceNum() == 20108 ||
		GetRaceNum() == 20109
#if defined(__ENABLE_RIDING_EXTENDED__)
		|| GetRaceNum() == 20149
		|| GetRaceNum() == 20150
		|| GetRaceNum() == 20151
#endif
		)
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
	}

	// MINING
	if (mining::IsVeinOfOre(GetRaceNum()))
	{
		char_event_info* info = AllocEventInfo<char_event_info>();

		info->ch = this;

		m_pkMiningEvent = event_create(kill_ore_load_event, info, PASSES_PER_SEC(number(7 * 60, 15 * 60)));
	}
	// END_OF_MINING
}

const TMobTable& CHARACTER::GetMobTable() const
{
	return m_pkMobData->m_table;
}

bool CHARACTER::IsRaceFlag(DWORD dwBit) const
{
	return m_pkMobData ? IS_SET(m_pkMobData->m_table.dwRaceFlag, dwBit) : 0;
}

DWORD CHARACTER::GetMobDamageMin() const
{
	return m_pkMobData->m_table.dwDamageRange[0];
}

DWORD CHARACTER::GetMobDamageMax() const
{
	return m_pkMobData->m_table.dwDamageRange[1];
}

float CHARACTER::GetMobDamageMultiply() const
{
	float fDamMultiply = GetMobTable().fDamMultiply;

	if (IsBerserk())
		fDamMultiply = fDamMultiply * 2.0f; // BALANCE: ï¿½ï¿½ï¿½ï¿½È­ ï¿½ï¿½ ï¿½Î¹ï¿½

	return fDamMultiply;
}

#if defined(__ELEMENT_SYSTEM__)
int CHARACTER::GetMobElement(BYTE bElement) const
{
	if (bElement >= 0 && bElement < MOB_ELEMENT_MAX_NUM)
		return m_pkMobData->m_table.cElements[bElement];
	return 0;
}
#endif

DWORD CHARACTER::GetMobDropItemVnum() const
{
	return m_pkMobData->m_table.dwDropItemVnum;
}

bool CHARACTER::IsSummonMonster() const
{
	return GetSummonVnum() != 0;
}

bool CHARACTER::CanSummonMonster() const
{
#if defined(__EREBUS_DUNGEON__)
	if (GetRaceNum() == THUNDER_BOSS)
	{
		if (thecore_pulse() > m_iLastHealerSummonDelay)
			return true;

		return false;
	}
#endif

	if (!IsSummonMonster())
		return false;

	return (thecore_pulse() > m_lastSummonTime + PASSES_PER_SEC(m_newSummonInterval));
}

void CHARACTER::MarkSummonedMonster()
{
	m_lastSummonTime = thecore_pulse();
	m_newSummonInterval = number(g_npcGroupRespawnRange[0], g_npcGroupRespawnRange[1]);
}

DWORD CHARACTER::GetSummonVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwSummonVnum : 0;
}

DWORD CHARACTER::GetPolymorphItemVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwPolymorphItemVnum : 0;
}

DWORD CHARACTER::GetMonsterDrainSPPoint() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwDrainSP : 0;
}

BYTE CHARACTER::GetMobRank() const
{
	if (!m_pkMobData)
		return MOB_RANK_KNIGHT; // PCï¿½ï¿½ ï¿½ï¿½ï¿½ KNIGHTï¿½ï¿½

	return m_pkMobData->m_table.bRank;
}

BYTE CHARACTER::GetMobSize() const
{
	if (!m_pkMobData)
		return MOBSIZE_MEDIUM;

	return m_pkMobData->m_table.bSize;
}

WORD CHARACTER::GetMobAttackRange() const
{
	switch (GetMobBattleType())
	{
	case BATTLE_TYPE_RANGE:
	case BATTLE_TYPE_MAGIC:
#if defined(__SHIP_DEFENSE__)
		if (CShipDefenseManager::Instance().IsHydra(m_pkMobData->m_table.dwVnum))
			return m_pkMobData->m_table.wAttackRange + GetPoint(POINT_BOW_DISTANCE) + 4000;
#endif
		return m_pkMobData->m_table.wAttackRange + GetPoint(POINT_BOW_DISTANCE);
	default:
#if defined(__SHIP_DEFENSE__)
		if (CShipDefenseManager::Instance().IsMinion(m_pkMobData->m_table.dwVnum))
			return m_pkMobData->m_table.wAttackRange + 300;
#endif
		return m_pkMobData->m_table.wAttackRange;
	}
}

BYTE CHARACTER::GetMobBattleType() const
{
	if (!m_pkMobData)
		return BATTLE_TYPE_MELEE;

	return (m_pkMobData->m_table.bBattleType);
}

void CHARACTER::ComputeBattlePoints()
{
	if (IsPolymorphed())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob* pMob = CMobManager::instance().Get(dwMobVnum);
		int iAtt = 0;
		int iDef = 0;

		if (pMob)
		{
			iAtt = GetLevel() * 2 + GetPolymorphPoint(POINT_ST) * 2;
			// lev + con
			iDef = GetLevel() + GetPolymorphPoint(POINT_HT) + pMob->m_table.wDef;
		}

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
	else if (IsPC())
	{
		SetPoint(POINT_ATT_GRADE, 0);
		SetPoint(POINT_DEF_GRADE, 0);
		SetPoint(POINT_CLIENT_DEF_GRADE, 0);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));

		//
		// ï¿½âº» ATK = 2lev + 2str, ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ 2strï¿½ï¿½ ï¿½Ù²ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
		//
		int iAtk = GetLevel() * 2;
		int iStatAtk = 0;

		switch (GetJob())
		{
		case JOB_WARRIOR:
		case JOB_SURA:
			iStatAtk = (2 * GetPoint(POINT_ST));
			break;

		case JOB_ASSASSIN:
			iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_DX)) / 3;
			break;

		case JOB_SHAMAN:
			iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_IQ)) / 3;
			break;

		case JOB_WOLFMAN:
			iStatAtk = (2 * GetPoint(POINT_ST));
			break;

		default:
			sys_err("invalid job %d", GetJob());
			iStatAtk = (2 * GetPoint(POINT_ST));
			break;
		}

		// ï¿½ï¿½ï¿½ï¿½ Å¸ï¿½ï¿½ ï¿½Ö°ï¿½, ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ý·ï¿½ï¿½ï¿½ ST*2 ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ST*2ï¿½ï¿½ ï¿½Ñ´ï¿½.
		// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ß¸ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ý·ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ê°ï¿½ ï¿½Ï±ï¿½ ï¿½ï¿½ï¿½Ø¼ï¿½ï¿½ï¿½.
		if (GetMountVnum() && iStatAtk < 2 * GetPoint(POINT_ST))
			iStatAtk = (2 * GetPoint(POINT_ST));

		iAtk += iStatAtk;

		// ï¿½Â¸ï¿½(ï¿½ï¿½) : ï¿½Ë¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
		if (GetMountVnum())
		{
			if (GetJob() == JOB_SURA && GetSkillGroup() == 1)
			{
				iAtk += (iAtk * GetHorseLevel()) / 60;
			}
			else
			{
				iAtk += (iAtk * GetHorseLevel()) / 30;
			}
		}

		//
		// ATK Setting
		//
		iAtk += GetPoint(POINT_ATT_GRADE_BONUS);

		PointChange(POINT_ATT_GRADE, iAtk);

		// DEF = LEV + CON + ARMOR
		int iShowDef = GetLevel() + GetPoint(POINT_HT); // For Ymir(Ãµï¿½ï¿½)
		int iDef = GetLevel() + (int)(GetPoint(POINT_HT) / 1.25); // For Other
		int iArmor = 0;

		LPITEM pkItem;

		for (int i = 0; i < WEAR_MAX_NUM; ++i)
		{
			if (((pkItem = GetWear(i))) && pkItem->GetType() == ITEM_ARMOR)
			{

				if (pkItem->GetSubType() == ARMOR_BODY
					|| pkItem->GetSubType() == ARMOR_HEAD
					|| pkItem->GetSubType() == ARMOR_FOOTS
					|| pkItem->GetSubType() == ARMOR_SHIELD
#if defined(__PENDANT_SYSTEM__)
					|| pkItem->GetSubType() == ARMOR_PENDANT
#endif
#if defined(__GLOVE_SYSTEM__)
					|| pkItem->GetSubType() == ARMOR_GLOVE
#endif
					)
				{
					iArmor += pkItem->GetValue(1);
					iArmor += (2 * pkItem->GetValue(5));
				}
			}
#ifdef __AURA_SYSTEM__
			else if (pkItem && pkItem->GetType() == ITEM_COSTUME && pkItem->GetSubType() == COSTUME_AURA)
			{
				const long c_lLevelSocket = pkItem->GetSocket(ITEM_SOCKET_AURA_CURRENT_LEVEL);
				const long c_lDrainSocket = pkItem->GetSocket(ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM);
				const long c_lBoostSocket = pkItem->GetSocket(ITEM_SOCKET_AURA_BOOST);

				BYTE bCurLevel = (c_lLevelSocket / 100000) - 1000;
				BYTE bBoostIndex = c_lBoostSocket / 100000000;

				TItemTable* pBoosterProto = ITEM_MANAGER::instance().GetTable(ITEM_AURA_BOOST_ITEM_VNUM_BASE + bBoostIndex);
				float fAuraDrainPer = (1.0f * bCurLevel / 10.0f) / 100.0f;
				if (pBoosterProto)
					fAuraDrainPer += 1.0f * pBoosterProto->alValues[ITEM_AURA_BOOST_PERCENT_VALUE] / 100.0f;

				TItemTable* pDrainedItem = NULL;
				if (c_lDrainSocket != 0)
					pDrainedItem = ITEM_MANAGER::instance().GetTable(c_lDrainSocket);
				if (pDrainedItem != NULL && pDrainedItem->bType == ITEM_ARMOR && pDrainedItem->bSubType == ARMOR_SHIELD)
				{
					float fValue = (pDrainedItem->alValues[1] + (2 * pDrainedItem->alValues[5])) * fAuraDrainPer;
					iArmor += static_cast<int>((fValue < 1.0f) ? ceilf(fValue) : truncf(fValue));;
				}
			}
#endif
		}

		// ï¿½ï¿½ Å¸ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Âºï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
		if (true == IsHorseRiding())
		{
			if (iArmor < GetHorseArmor())
				iArmor = GetHorseArmor();

			const char* pHorseName = CHorseNameManager::instance().GetHorseName(GetPlayerID());

			if (pHorseName != NULL && strlen(pHorseName))
			{
				iArmor += 20;
			}
		}

		iArmor += GetPoint(POINT_DEF_GRADE_BONUS);
		iArmor += GetPoint(POINT_PARTY_DEFENDER_BONUS);

		PointChange(POINT_DEF_GRADE, iDef + iArmor);
		PointChange(POINT_CLIENT_DEF_GRADE, (iShowDef + iArmor) - GetPoint(POINT_DEF_GRADE));
		PointChange(POINT_MAGIC_ATT_GRADE, GetLevel() * 2 + GetPoint(POINT_IQ) * 2 + GetPoint(POINT_MAGIC_ATT_GRADE_BONUS));
		PointChange(POINT_MAGIC_DEF_GRADE, GetLevel() + (GetPoint(POINT_IQ) * 3 + GetPoint(POINT_HT)) / 3 + iArmor / 2 + GetPoint(POINT_MAGIC_DEF_GRADE_BONUS));
	}
	else
	{
		// 2lev + str * 2
		int iAtt = GetLevel() * 2 + GetPoint(POINT_ST) * 2;
		// lev + con
		int iDef = GetLevel() + GetPoint(POINT_HT) + GetMobTable().wDef;

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
}

void CHARACTER::ComputePoints()
{
	long lStat = GetPoint(POINT_STAT);
	long lStatResetCount = GetPoint(POINT_STAT_RESET_COUNT);
	long lSkillActive = GetPoint(POINT_SKILL);
	long lSkillSub = GetPoint(POINT_SUB_SKILL);
	long lSkillHorse = GetPoint(POINT_HORSE_SKILL);
	long lLevelStep = GetPoint(POINT_LEVEL_STEP);

	long lAttackerBonus = GetPoint(POINT_PARTY_ATTACKER_BONUS);
	long lTankerBonus = GetPoint(POINT_PARTY_TANKER_BONUS);
	long lBufferBonus = GetPoint(POINT_PARTY_BUFFER_BONUS);
	long lSkillMasterBonus = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
	long lHasteBonus = GetPoint(POINT_PARTY_HASTE_BONUS);
	long lDefenderBonus = GetPoint(POINT_PARTY_DEFENDER_BONUS);

	long lHPRecovery = GetPoint(POINT_HP_RECOVERY);
	long lSPRecovery = GetPoint(POINT_SP_RECOVERY);

#if defined(__CONQUEROR_LEVEL__)
	long lConquerorPoint = GetPoint(POINT_CONQUEROR_POINT);
	long lConquerorLevelStep = GetPoint(POINT_CONQUEROR_LEVEL_STEP);
#endif

	memset(m_pointsInstant.points, 0, sizeof(m_pointsInstant.points));
	BuffOnAttr_ClearAll();
	m_SkillDamageBonus.clear();

	SetPoint(POINT_STAT, lStat);
	SetPoint(POINT_SKILL, lSkillActive);
	SetPoint(POINT_SUB_SKILL, lSkillSub);
	SetPoint(POINT_HORSE_SKILL, lSkillHorse);
	SetPoint(POINT_LEVEL_STEP, lLevelStep);
	SetPoint(POINT_STAT_RESET_COUNT, lStatResetCount);

	SetPoint(POINT_ST, GetRealPoint(POINT_ST));
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

#if defined(__CONQUEROR_LEVEL__)
	SetPoint(POINT_CONQUEROR_POINT, lConquerorPoint);
	SetPoint(POINT_CONQUEROR_LEVEL_STEP, lConquerorLevelStep);

	SetPoint(POINT_SUNGMA_STR, GetRealPoint(POINT_SUNGMA_STR));
	SetPoint(POINT_SUNGMA_HP, GetRealPoint(POINT_SUNGMA_HP));
	SetPoint(POINT_SUNGMA_MOVE, GetRealPoint(POINT_SUNGMA_MOVE));
	SetPoint(POINT_SUNGMA_IMMUNE, GetRealPoint(POINT_SUNGMA_IMMUNE));
#endif

	SetPart(PART_MAIN, GetOriginalPart(PART_MAIN));
	SetPart(PART_WEAPON, GetOriginalPart(PART_WEAPON));
	SetPart(PART_HEAD, GetOriginalPart(PART_HEAD));
	SetPart(PART_HAIR, GetOriginalPart(PART_HAIR));
#if defined(__ACCE_COSTUME_SYSTEM__)
	SetPart(PART_ACCE, GetOriginalPart(PART_ACCE));
#endif
#ifdef __AURA_SYSTEM__
	SetPart(PART_AURA, GetOriginalPart(PART_AURA));
#endif

	SetPoint(POINT_PARTY_ATTACKER_BONUS, lAttackerBonus);
	SetPoint(POINT_PARTY_TANKER_BONUS, lTankerBonus);
	SetPoint(POINT_PARTY_BUFFER_BONUS, lBufferBonus);
	SetPoint(POINT_PARTY_SKILL_MASTER_BONUS, lSkillMasterBonus);
	SetPoint(POINT_PARTY_HASTE_BONUS, lHasteBonus);
	SetPoint(POINT_PARTY_DEFENDER_BONUS, lDefenderBonus);

	SetPoint(POINT_HP_RECOVERY, lHPRecovery);
	SetPoint(POINT_SP_RECOVERY, lSPRecovery);

	// PC_BANG_ITEM_ADD
	SetPoint(POINT_PC_BANG_EXP_BONUS, 0);
	SetPoint(POINT_PC_BANG_DROP_BONUS, 0);
	// END_PC_BANG_ITEM_ADD

	int64_t iMaxHP, iMaxSP;
	int iMaxStamina;

	if (IsPC())
	{
		// ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½/ï¿½ï¿½ï¿½Å·ï¿½
		iMaxHP = JobInitialPoints[GetJob()].max_hp + m_points.iRandomHP + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].hp_per_ht;
		iMaxSP = JobInitialPoints[GetJob()].max_sp + m_points.iRandomSP + GetPoint(POINT_IQ) * JobInitialPoints[GetJob()].sp_per_iq;
		iMaxStamina = JobInitialPoints[GetJob()].max_stamina + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].stamina_per_con;

		{
			CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_ADD_HP);

			if (NULL != pkSk)
			{
				pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_ADD_HP) / 100.0f);

				iMaxHP += static_cast<int64_t>(pkSk->kPointPoly.Eval());
			}
		}

		// ï¿½âº» ï¿½ï¿½ï¿½ï¿½
		SetPoint(POINT_MOV_SPEED, 100);
		SetPoint(POINT_ATT_SPEED, 100);
		PointChange(POINT_ATT_SPEED, GetPoint(POINT_PARTY_HASTE_BONUS));
		SetPoint(POINT_CASTING_SPEED, 100);
	}
	else
	{
		iMaxHP = m_pkMobData->m_table.dwMaxHP;
		iMaxSP = 0;
		iMaxStamina = 0;

		SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		SetPoint(POINT_MOV_SPEED, m_pkMobData->m_table.sMovingSpeed);
		SetPoint(POINT_CASTING_SPEED, m_pkMobData->m_table.sAttackSpeed);
	}

	if (IsPC())
	{
		// ï¿½ï¿½ Å¸ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½âº» ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Èºï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½.
		// ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ì¹Ç·ï¿½, ï¿½ï¿½ï¿½ï¿½/ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Ã¼ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
		// ï¿½ï¿½Ã¤ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Ã¶ó°¡°ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½Ì´ï¿½.
		if (GetMountVnum())
		{
			if (GetHorseST() > GetPoint(POINT_ST))
				PointChange(POINT_ST, GetHorseST() - GetPoint(POINT_ST));

			if (GetHorseDX() > GetPoint(POINT_DX))
				PointChange(POINT_DX, GetHorseDX() - GetPoint(POINT_DX));

			if (GetHorseHT() > GetPoint(POINT_HT))
				PointChange(POINT_HT, GetHorseHT() - GetPoint(POINT_HT));

			if (GetHorseIQ() > GetPoint(POINT_IQ))
				PointChange(POINT_IQ, GetHorseIQ() - GetPoint(POINT_IQ));

#if defined(__CONQUEROR_LEVEL__)
			if (GetHorseSungMaST() > GetPoint(POINT_SUNGMA_STR))
				PointChange(POINT_SUNGMA_STR, GetHorseSungMaST() - GetPoint(POINT_SUNGMA_STR));

			if (GetHorseSungMaDX() > GetPoint(POINT_SUNGMA_MOVE))
				PointChange(POINT_SUNGMA_MOVE, GetHorseSungMaDX() - GetPoint(POINT_SUNGMA_MOVE));

			if (GetHorseSungMaHT() > GetPoint(POINT_SUNGMA_HP))
				PointChange(POINT_SUNGMA_HP, GetHorseSungMaHT() - GetPoint(POINT_SUNGMA_HP));

			if (GetHorseSungMaIQ() > GetPoint(POINT_SUNGMA_IMMUNE))
				PointChange(POINT_SUNGMA_IMMUNE, GetHorseSungMaIQ() - GetPoint(POINT_SUNGMA_IMMUNE));
#endif
		}
	}

	ComputeBattlePoints();

	// ï¿½âº» HP/SP ï¿½ï¿½ï¿½ï¿½
	if (iMaxHP != GetMaxHP())
	{
		SetRealPoint(POINT_MAX_HP, iMaxHP); // ï¿½âº»HPï¿½ï¿½ RealPointï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Â´ï¿½.
	}
	PointChange(POINT_MAX_HP, 0);

	if (iMaxSP != GetMaxSP())
	{
		SetRealPoint(POINT_MAX_SP, iMaxSP); // ï¿½âº»SPï¿½ï¿½ RealPointï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Â´ï¿½.
	}
	PointChange(POINT_MAX_SP, 0);

	SetMaxStamina(iMaxStamina);

	// @fixme118 part1
	// const auto iCurHP = this->GetHP();
	// const auto iCurSP = this->GetSP();

	int64_t iCurHP = this->GetHP();
	int64_t iCurSP = this->GetSP();

	m_pointsInstant.dwImmuneFlag = 0;

	for (int i = 0; i < WEAR_MAX_NUM; i++)
	{
		LPITEM pItem = GetWear(i);
		if (pItem)
		{
			pItem->ModifyPoints(true);
			SET_BIT(m_pointsInstant.dwImmuneFlag, GetWear(i)->GetImmuneFlag());
		}
	}
#ifdef ENABLE_EVENT_MANAGER
	CHARACTER_MANAGER::Instance().CheckBonusEvent(this);
#endif
	// ï¿½ï¿½È¥ï¿½ï¿½ ï¿½Ã½ï¿½ï¿½ï¿½
	// ComputePointsï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½É¸ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ ï¿½Ó¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê±ï¿½È­ï¿½Ï°ï¿½,
	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½, ï¿½ï¿½ï¿½ï¿½ ï¿½î¿¡ ï¿½ï¿½ï¿½Ãµï¿½ ï¿½ï¿½ï¿½ ï¿½Ó¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï±ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½,
	// ï¿½ï¿½È¥ï¿½ï¿½ ï¿½Ã½ï¿½ï¿½Ûµï¿½ ActiveDeckï¿½ï¿½ ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½ ï¿½ï¿½È¥ï¿½ï¿½ï¿½ï¿½ ï¿½Ó¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ù½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ñ¾ï¿½ ï¿½Ñ´ï¿½.
	if (DragonSoul_IsDeckActivated())
	{
		for (int i = WEAR_MAX_NUM + DS_SLOT_MAX * DragonSoul_GetActiveDeck();
			i < WEAR_MAX_NUM + DS_SLOT_MAX * (DragonSoul_GetActiveDeck() + 1); i++)
		{
			LPITEM pItem = GetWear(i);
			if (pItem)
			{
				if (DSManager::instance().IsTimeLeftDragonSoul(pItem))
					pItem->ModifyPoints(true);
			}
		}

#if defined(__DS_SET__)
		DragonSoul_HandleSetBonus();
#endif
	}

	ComputeSkillPoints();

	RefreshAffect();

	if (GetHP() > GetMaxHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());

	if (GetSP() > GetMaxSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());

#ifndef ENABLE_PET_SYSTEM	//@fixme520
	CPetSystem* pPetSystem = GetPetSystem();
	if (nullptr != pPetSystem)
		pPetSystem->RefreshBuff();
#endif

	// @fixme118 part2 (after petsystem stuff)
	// if (IsPC())
	// {
	// 	if (this->GetHP() != iCurHP)
	// 		this->PointChange(POINT_HP, iCurHP - this->GetHP());
	// 	if (this->GetSP() != iCurSP)
	// 		this->PointChange(POINT_SP, iCurSP - this->GetSP());
	// }

	PointChange(POINT_MAX_HP, 0);
	PointChange(POINT_MAX_SP, 0);

	UpdatePacket();
}

// m_dwPlayStartTimeï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ milisecondï¿½ï¿½. ï¿½ï¿½ï¿½ï¿½ï¿½Íºï¿½ï¿½Ì½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ð´ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Ï±ï¿½
// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ã·ï¿½ï¿½Ì½Ã°ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ / 60000 ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ï´Âµï¿½, ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
// ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½â¿¡ dwTimeRemainï¿½ï¿½ï¿½ï¿½ ï¿½Ö¾î¼­ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Çµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ö¾ï¿½ï¿½ ï¿½Ñ´ï¿½.
void CHARACTER::ResetPlayTime(DWORD dwTimeRemain)
{
	m_dwPlayStartTime = get_dword_time() - dwTimeRemain;
}

const int aiRecoveryPercents[10] = { 1, 5, 5, 5, 5, 5, 5, 5, 5, 5 };

EVENTFUNC(recovery_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("recovery_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) // <Factor>
		return 0;

	if (!ch->IsPC())
	{
		//
		// ï¿½ï¿½ï¿½ï¿½ È¸ï¿½ï¿½
		//
		if (ch->IsAffectFlag(AFF_POISON))
			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));

		if (ch->IsAffectFlag(AFF_BLEEDING))
			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));

		DWORD vnum = ch->GetMobTable().dwVnum;
		if (vnum == 2493) // Aqua Dragon
		{
			int regenPct = BlueDragon_GetRangeFactor("hp_regen", ch->GetHPPct());
			regenPct += ch->GetMobTable().bRegenPercent;

			for (int i = 1; i <= 4; ++i)
			{
				if (REGEN_PECT_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap(ch->GetMapIndex(), dwDragonStoneID);

					regenPct += (val * cnt);

					break;
				}
			}

			ch->MonsterLog("AQUA_HP_REGEN +%d", MAX(1, (ch->GetMaxHP() * regenPct) / 100));
			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * regenPct) / 100));
		}
		else if (!ch->IsDoor())
		{
			ch->MonsterLog("HP_REGEN +%d", MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
		}

		if (ch->GetHP() >= ch->GetMaxHP())
		{
			ch->m_pkRecoveryEvent = NULL;
			return 0;
		}

		if (2493 == ch->GetMobTable().dwVnum)
		{
			for (int i = 1; i <= 4; ++i)
			{
				if (REGEN_TIME_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap(ch->GetMapIndex(), dwDragonStoneID);

					return PASSES_PER_SEC(MAX(1, (ch->GetMobTable().bRegenCycle - (val * cnt))));
				}
			}
		}

		return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
	}
	else
	{
		//
		// PC È¸ï¿½ï¿½
		//
		ch->CheckTarget();
		// ch->UpdateSectree(); // ï¿½ï¿½ï¿½â¼­ ï¿½Ì°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½?
		ch->UpdateKillerMode();

		if (ch->IsAffectFlag(AFF_POISON) == true)
		{
			// Prohibit automatic recovery in case of poisoning
			// Disable auto-recovery in case of divination
			return 3;
		}

		if (ch->IsAffectFlag(AFF_BLEEDING))
			return 3;

#ifdef ENABLE_12ZI
		/*if (ch->FindAffect(AFFECT_NO_RECOVERY))
			return 3;*/
#endif

		int iSec = (get_dword_time() - ch->GetLastMoveTime()) / 3000;

		// SP È¸ï¿½ï¿½ ï¿½ï¿½Æ¾.
		// ï¿½ï¿½ ï¿½Ì°É·ï¿½ ï¿½Ø¼ï¿½ ï¿½Ô¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Â°ï¿½ ?!
		ch->DistributeSP(ch);

		if (ch->GetMaxHP() <= ch->GetHP())
			return PASSES_PER_SEC(3);

		int iPercent = 0;
		int64_t iAmount = 0;

		{
			iPercent = aiRecoveryPercents[MIN(9, iSec)];
			iAmount = 15 + (ch->GetMaxHP() * iPercent) / 100;
		}

		iAmount += (iAmount * ch->GetPoint(POINT_HP_REGEN)) / 100;

		sys_log(1, "RECOVERY_EVENT: %s %d HP_REGEN %d HP +%lld", ch->GetName(), iPercent, ch->GetPoint(POINT_HP_REGEN), iAmount);

		ch->PointChange(POINT_HP, iAmount, false);
		return PASSES_PER_SEC(3);
	}
}

void CHARACTER::StartRecoveryEvent()
{
	if (m_pkRecoveryEvent)
		return;

	if (IsDead() || IsStun())
		return;

	if (IsNPC() && GetHP() >= GetMaxHP()) // ï¿½ï¿½ï¿½Í´ï¿½ Ã¼ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ñ´ï¿½.
		return;

#if defined(__GUILD_DRAGONLAIR__)
	if ((MeleyLair::CMgr::instance().IsMeleyMap(GetMapIndex())) && ((GetRaceNum() == (WORD)(MeleyLair::STATUE_VNUM)) || ((GetRaceNum() == (WORD)(MeleyLair::BOSS_VNUM)))))
		return;
#endif

#if defined(__SHIP_DEFENSE__)
	if (CShipDefenseManager::Instance().IsMast(this->GetRaceNum()))
		return;
#endif

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	int iSec = IsPC() ? 3 : (MAX(1, GetMobTable().bRegenCycle));
	m_pkRecoveryEvent = event_create(recovery_event, info, PASSES_PER_SEC(iSec));
}

#ifdef ENABLE_12ZI
/*EVENTFUNC(bead_point_idle_event)
{
	const auto* info = dynamic_cast<char_event_info*>(event->info);
	if (info == nullptr)
	{
		sys_err("bead_point_idle_event> <Factor> Null pointer");
		return 0;
	}

	auto ch = static_cast<LPCHARACTER>(info->ch);
	if (ch == nullptr) // <Factor>
		return 0;

	if (!ch->IsPC())
		return 0;

	if (ch->GetBeadPoint() < 36 && (ch->GetBeadNextTime() < get_global_time()))
	{
		ch->PointChange(POINT_BEAD, 1, true);
		if (ch->GetBeadPoint() < 36)
		{
			const time_t next_time = get_global_time() + 3600;
			ch->SetBeadNextTime(next_time);

			const time_t remain_time = (next_time - get_global_time());
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RefreshBeadTimer %d", remain_time);
		}
	}

	return PASSES_PER_SEC(3);
}

void CHARACTER::StartBeadPointIdleEvent()
{
	if (m_pkBeadPointIdleEvent)
		return;

	if (!IsPC())
		return;

	if (GetBeadPoint() == 0 && GetBeadNextTime() == 0)
	{
		PointChange(POINT_BEAD, 36, true);
	}
	else if (GetBeadPoint() < 36)
	{
		const time_t remain_time = (GetBeadNextTime() - get_global_time());
		ChatPacket(CHAT_TYPE_COMMAND, "RefreshBeadTimer %d", remain_time);
		UpdatePoint(POINT_BEAD, GetBeadPoint());
	}
	else
	{
		UpdatePoint(POINT_BEAD, GetBeadPoint());
	}

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkBeadPointIdleEvent = event_create(bead_point_idle_event, info, PASSES_PER_SEC(3));
}*/
#endif

void CHARACTER::Standup()
{
	struct packet_position pack_position;

	if (!IsPosition(POS_SITTING))
		return;

	SetPosition(POS_STANDING);

	sys_log(1, "STANDUP: %s", GetName());

	pack_position.header = HEADER_GC_CHARACTER_POSITION;
	pack_position.vid = GetVID();
	pack_position.position = POSITION_GENERAL;

	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::Sitdown(int is_ground)
{
	struct packet_position pack_position;

	if (IsPosition(POS_SITTING))
		return;

	SetPosition(POS_SITTING);
	sys_log(1, "SITDOWN: %s", GetName());

	pack_position.header = HEADER_GC_CHARACTER_POSITION;
	pack_position.vid = GetVID();
	pack_position.position = POSITION_SITTING_GROUND;
	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::SetRotation(float fRot)
{
	m_pointsInstant.fRot = fRot;
}

// x, y ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½.
void CHARACTER::SetRotationToXY(long x, long y)
{
	SetRotation(GetDegreeFromPositionXY(GetX(), GetY(), x, y));
}

bool CHARACTER::CannotMoveByAffect() const
{
	return (IsAffectFlag(AFF_STUN));
}

bool CHARACTER::CanMove() const
{
	if (CannotMoveByAffect())
		return false;

	if (GetMyShop()) // ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½Â¿ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
		return false;

	// 0.2ï¿½ï¿½ ï¿½ï¿½ï¿½Ì¶ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½.
	/*
	if (get_float_time() - m_fSyncTime < 0.2f)
		return false;
	*/
	return true;
}

// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ x, y ï¿½ï¿½Ä¡ï¿½ï¿½ ï¿½Ìµï¿½ ï¿½ï¿½Å²ï¿½ï¿½.
bool CHARACTER::Sync(long x, long y)
{
	if (!GetSectree())
		return false;

	LPSECTREE new_tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), x, y);

	if (!new_tree)
	{
		if (GetDesc())
		{
			sys_err("cannot find tree at %d %d (name: %s)", x, y, GetName());
			GetDesc()->SetPhase(PHASE_CLOSE);
		}
		else
		{
			sys_err("no tree: %s %d %d %d", GetName(), x, y, GetMapIndex());
			Dead();
		}

		return false;
	}

	SetRotationToXY(x, y);
	SetXYZ(x, y, 0);

	if (GetDungeon())
	{
		// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ìºï¿½Æ® ï¿½Ó¼ï¿½ ï¿½ï¿½È­
		int iLastEventAttr = m_iEventAttr;
		m_iEventAttr = new_tree->GetEventAttribute(x, y);

		if (m_iEventAttr != iLastEventAttr)
		{
			if (GetParty())
			{
				quest::CQuestManager::instance().AttrOut(GetParty()->GetLeaderPID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetParty()->GetLeaderPID(), this, m_iEventAttr);
			}
			else
			{
				quest::CQuestManager::instance().AttrOut(GetPlayerID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetPlayerID(), this, m_iEventAttr);
			}
		}
	}

	if (GetSectree() != new_tree)
	{
		if (test_server)
			if (!IsNPC())
			{
				SECTREEID id = new_tree->GetID();
				SECTREEID old_id = GetSectree()->GetID();

				const float fDist = DISTANCE_SQRT(id.coord.x - old_id.coord.x, id.coord.y - old_id.coord.y);
				sys_log(0, "SECTREE DIFFER: %s %dx%d was %dx%d dist %.1fm",
					GetName(),
					id.coord.x,
					id.coord.y,
					old_id.coord.x,
					old_id.coord.y,
					fDist);
			}

		new_tree->InsertEntity(this);
	}

	return true;
}

void CHARACTER::Stop()
{
	if (!IsState(m_stateIdle))
		MonsterLog("[IDLE] ï¿½ï¿½ï¿½ï¿½");

	GotoState(m_stateIdle);

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
}

bool CHARACTER::Goto(long x, long y)
{
#if defined(__GUILD_DRAGONLAIR__)
	// NOTE: Prevent Dragon Queen Meley from changing position
	// when warping out of the dungeon.
	if (GetRaceNum() == static_cast<WORD>(MeleyLair::BOSS_VNUM))
		return false;
#endif

	// TODO ï¿½Å¸ï¿½Ã¼Å© ï¿½Ê¿ï¿½
	// ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Ä¡ï¿½ï¿½ ï¿½Ìµï¿½ï¿½ï¿½ ï¿½Ê¿ï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½Úµï¿½ ï¿½ï¿½ï¿½ï¿½)
	if (GetX() == x && GetY() == y)
		return false;

	if (m_posDest.x == x && m_posDest.y == y)
	{
		if (!IsState(m_stateMove))
		{
			m_dwStateDuration = 4;
			GotoState(m_stateMove);
		}
		return false;
	}

	m_posDest.x = x;
	m_posDest.y = y;

	CalculateMoveDuration();

	m_dwStateDuration = 4;

	if (!IsState(m_stateMove))
	{
		MonsterLog("[MOVE] %s", GetVictim() ? "To victim" : "Free");

		if (GetVictim())
		{
			//MonsterChat(MONSTER_CHAT_CHASE);
			//MonsterChat(MONSTER_CHAT_ATTACK);
		}
	}

	GotoState(m_stateMove);

	return true;
}

DWORD CHARACTER::GetMotionMode() const
{
	DWORD dwMode = MOTION_MODE_GENERAL;

	if (IsPolymorphed())
		return dwMode;

	LPITEM pkItem;

	if ((pkItem = GetWear(WEAR_WEAPON)))
	{
		switch (pkItem->GetProto()->bSubType)
		{
		case WEAPON_SWORD:
			dwMode = MOTION_MODE_ONEHAND_SWORD;
			break;

		case WEAPON_TWO_HANDED:
			dwMode = MOTION_MODE_TWOHAND_SWORD;
			break;

		case WEAPON_DAGGER:
			dwMode = MOTION_MODE_DUALHAND_SWORD;
			break;

		case WEAPON_BOW:
			dwMode = MOTION_MODE_BOW;
			break;

		case WEAPON_BELL:
			dwMode = MOTION_MODE_BELL;
			break;

		case WEAPON_FAN:
			dwMode = MOTION_MODE_FAN;
			break;

		case WEAPON_CLAW:
			dwMode = MOTION_MODE_CLAW;
			break;

		}
	}
	return dwMode;
}

float CHARACTER::GetMoveMotionSpeed() const
{
	DWORD dwMode = GetMotionMode();

	const CMotion* pkMotion = NULL;

	if (!GetMountVnum())
		pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(dwMode, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	else
	{
		pkMotion = CMotionManager::instance().GetMotion(GetMountVnum(), MAKE_MOTION_KEY(MOTION_MODE_GENERAL, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));

		if (!pkMotion)
			pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(MOTION_MODE_HORSE, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	}

	if (pkMotion)
		return -pkMotion->GetAccumVector().y / pkMotion->GetDuration();
	else
	{
		//sys_err("cannot find motion (name %s race %d mode %d)", GetName(), GetRaceNum(), dwMode);
		return 300.0f;
	}
}

float CHARACTER::GetMoveSpeed() const
{
	return GetMoveMotionSpeed() * 10000 / CalculateDuration(GetLimitPoint(POINT_MOV_SPEED), 10000);
}

void CHARACTER::CalculateMoveDuration()
{
	m_posStart.x = GetX();
	m_posStart.y = GetY();

	float fDist = DISTANCE_SQRT(m_posStart.x - m_posDest.x, m_posStart.y - m_posDest.y);

	float motionSpeed = GetMoveMotionSpeed();

	m_dwMoveDuration = CalculateDuration(GetLimitPoint(POINT_MOV_SPEED),
		(int)((fDist / motionSpeed) * 1000.0f));

	if (IsNPC())
		sys_log(1, "%s: GOTO: distance %f, spd %u, duration %u, motion speed %f pos %d %d -> %d %d",
			GetName(), fDist, GetLimitPoint(POINT_MOV_SPEED), m_dwMoveDuration, motionSpeed,
			m_posStart.x, m_posStart.y, m_posDest.x, m_posDest.y);

	m_dwMoveStartTime = get_dword_time();
}

// x y ï¿½ï¿½Ä¡ï¿½ï¿½ ï¿½Ìµï¿½ ï¿½Ñ´ï¿½. (ï¿½Ìµï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Ö´ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ È®ï¿½ï¿½ ï¿½Ï°ï¿½ Sync ï¿½Þ¼Òµï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ìµï¿½ ï¿½Ñ´ï¿½)
// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ charï¿½ï¿½ x, y ï¿½ï¿½ï¿½ï¿½ ï¿½Ù·ï¿½ ï¿½Ù²ï¿½ï¿½ï¿½ï¿½ï¿½,
// Å¬ï¿½ó¿¡¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Ä¡ï¿½ï¿½ï¿½ï¿½ ï¿½Ù²ï¿½ x, yï¿½ï¿½ï¿½ï¿½ interpolationï¿½Ñ´ï¿½.
// ï¿½È°Å³ï¿½ ï¿½Ù´ï¿½ ï¿½ï¿½ï¿½ï¿½ charï¿½ï¿½ m_bNowWalkingï¿½ï¿½ ï¿½Þ·ï¿½ï¿½Ö´ï¿½.
// Warpï¿½ï¿½ ï¿½Çµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ì¶ï¿½ï¿½ Showï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½.
bool CHARACTER::Move(long x, long y)
{
	// ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Ä¡ï¿½ï¿½ ï¿½Ìµï¿½ï¿½ï¿½ ï¿½Ê¿ï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½Úµï¿½ ï¿½ï¿½ï¿½ï¿½)
	if (GetX() == x && GetY() == y)
		return true;

	if (test_server)
		if (m_bDetailLog)
			sys_log(0, "%s position %u %u", GetName(), x, y);

	OnMove();
	return Sync(x, y);
}

void CHARACTER::SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, int iRot)
{
	TPacketGCMove pack;

	if (bFunc == FUNC_WAIT)
	{
		x = m_posDest.x;
		y = m_posDest.y;
		dwDuration = m_dwMoveDuration;
	}

	EncodeMovePacket(pack, GetVID(), bFunc, bArg, x, y, dwDuration, dwTime, iRot == -1 ? (int)GetRotation() / 5 : iRot);
	PacketView(&pack, sizeof(TPacketGCMove), this);
}

int64_t CHARACTER::GetRealPoint(uint16_t type) const //@fixme532
{
	return m_points.points[type];
}

void CHARACTER::SetRealPoint(uint16_t type, int64_t val) //@fixme532
{
	m_points.points[type] = val;
}

int64_t CHARACTER::GetPolymorphPoint(uint16_t type) const //@fixme532
{
	if (IsPolymorphed() && !IsPolyMaintainStat())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob* pMob = CMobManager::instance().Get(dwMobVnum);
		int iPower = GetPolymorphPower();

		if (pMob)
		{
			switch (type)
			{
			case POINT_ST:
				if (GetJob() == JOB_SHAMAN || GetJob() == JOB_SURA && GetSkillGroup() == 2)
					return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_IQ);
				return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_ST);

			case POINT_HT:
				return pMob->m_table.bCon * iPower / 100 + GetPoint(POINT_HT);

			case POINT_IQ:
				return pMob->m_table.bInt * iPower / 100 + GetPoint(POINT_IQ);

			case POINT_DX:
				return pMob->m_table.bDex * iPower / 100 + GetPoint(POINT_DX);
			}
		}
	}

	return GetPoint(type);
}

int64_t CHARACTER::GetPoint(uint16_t type) const // @fixme532
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}

	int64_t val = m_pointsInstant.points[type];
	int64_t max_val = LLONG_MAX;

	switch (type)
	{
	case POINT_STEAL_HP:
	case POINT_STEAL_SP:
		max_val = 50;
		break;
	}

	if (val > max_val)
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), val, max_val);

	return (val);
}

int64_t CHARACTER::GetLimitPoint(uint16_t type) const // @fixme532
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}

	int64_t val = m_pointsInstant.points[type];
	int64_t max_val = LLONG_MAX;
	int64_t limit = LLONG_MAX;
	int64_t min_limit = -LLONG_MAX;

	switch (type)
	{
	case POINT_ATT_SPEED:
		min_limit = 0;

		if (IsPC())
			limit = 170;
		else
			limit = 250;
		break;

	case POINT_MOV_SPEED:
		min_limit = 0;

		if (IsPC())
			limit = 200;
		else
			limit = 250;
		if (FindAffect(AFFECT_WAR_FLAG))
			limit = 50;
		break;

	case POINT_STEAL_HP:
	case POINT_STEAL_SP:
		limit = 50;
		max_val = 50;
		break;

	case POINT_MALL_ATTBONUS:
	case POINT_MALL_DEFBONUS:
		limit = 20;
		max_val = 50;
		break;
	}

#if defined(__CONQUEROR_LEVEL__)
	if (IsPC())
	{
		switch (type)
		{
		case POINT_MOV_SPEED:
			if (GetSungMaWill(POINT_SUNGMA_MOVE) > GetPoint(POINT_SUNGMA_MOVE))
				val /= 2;
			break;
		}
	}
#endif

	if (val > max_val)
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), val, max_val);

	if (val > limit)
		val = limit;

	if (val < min_limit)
		val = min_limit;

	return (val);
}

void CHARACTER::SetPoint(uint16_t type, int64_t val) //@fixme532
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return;
	}

	m_pointsInstant.points[type] = val;

	// ï¿½ï¿½ï¿½ï¿½ ï¿½Ìµï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½È³ï¿½ï¿½ï¿½ï¿½Ù¸ï¿½ ï¿½Ìµï¿½ ï¿½Ã°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ù½ï¿½ ï¿½Ø¾ï¿½ ï¿½Ñ´ï¿½.
	if (type == POINT_MOV_SPEED && get_dword_time() < m_dwMoveStartTime + m_dwMoveDuration)
	{
		CalculateMoveDuration();
	}
}

long long CHARACTER::GetAllowedGold() const
{
	if (GetLevel() <= 10)
		return 100000;
	else if (GetLevel() <= 20)
		return 500000;
	else
		return 50000000;
}

void CHARACTER::CheckMaximumPoints()
{
	if (GetMaxHP() < GetHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());

	if (GetMaxSP() < GetSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());
}

void CHARACTER::PointChange(uint16_t type, int64_t amount, bool bAmount, bool bBroadcast) //@fixme532
{
	int64_t val = 0;

	//sys_log(0, "PointChange %d %d | %d -> %d cHP %d mHP %d", type, amount, GetPoint(type), GetPoint(type)+amount, GetHP(), GetMaxHP());

	switch (type)
	{
	case POINT_NONE:
		return;

#if defined(__CONQUEROR_LEVEL__)
	case POINT_CONQUEROR_LEVEL:
	{
		if ((GetConquerorLevel() + amount) > gPlayerMaxConquerorLevel)
			return;

		SetConquerorLevel(GetConquerorLevel() + amount);
		val = GetConquerorLevel();

		sys_log(0, "CONQUEROR_LEVELUP: %s %d NEXT EXP %d", GetName(), GetConquerorLevel(), GetConquerorNextExp());

		PointChange(POINT_CONQUEROR_NEXT_EXP, GetConquerorNextExp(), false);
	} break;

	case POINT_CONQUEROR_NEXT_EXP:
	{
		val = GetConquerorNextExp();
		bAmount = false; // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ bAmountï¿½ï¿½ false ï¿½ï¿½ï¿½ï¿½ ï¿½Ñ´ï¿½.
	} break;

	case POINT_CONQUEROR_EXP:
	{
		DWORD exp = GetConquerorExp();
		DWORD next_exp = GetConquerorNextExp();

		// expï¿½ï¿½ 0 ï¿½ï¿½ï¿½Ï·ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Êµï¿½ï¿½ï¿½ ï¿½Ñ´ï¿½
		if (amount < 0 && exp <= -amount)
		{
			sys_log(0, "%s - Reduce EXP by %d, CUR EXP: %d (setting to zero)", GetName(), -amount, exp);
			amount = -exp;

			SetConquerorExp(0);
			val = GetExp();
		}
		else
		{
			if (GetConquerorLevel() >= gPlayerMaxConquerorLevel)
				return;

			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %lu EXP for your Champion Level."), amount);

			DWORD iExpBalance = 0;

			// ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½!
			if (exp + amount >= next_exp)
			{
				iExpBalance = (exp + amount) - next_exp;
				amount = next_exp - exp;

				SetConquerorExp(0);
				exp = next_exp;
			}
			else
			{
				SetConquerorExp(exp + amount);
				exp = GetConquerorExp();
			}

			DWORD q = DWORD(next_exp / 4.0f);
			int iLevStep = GetRealPoint(POINT_CONQUEROR_LEVEL_STEP);

			// iLevStepï¿½ï¿½ 4 ï¿½Ì»ï¿½ï¿½Ì¸ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ã¶ï¿½ï¿½ï¿½ï¿½ ï¿½Ï¹Ç·ï¿½ ï¿½ï¿½ï¿½â¿¡ ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ì´ï¿½.
			if (iLevStep >= 4)
			{
				sys_err("%s CONQUEROR_LEVEL_STEP bigger than 4! (%d)", GetName(), iLevStep);
				iLevStep = 4;
			}

			if (exp >= next_exp && iLevStep < 4)
			{
				for (int i = 0; i < 4 - iLevStep; ++i)
					PointChange(POINT_CONQUEROR_LEVEL_STEP, 1, false, true);
			}
			else if (exp >= q * 3 && iLevStep < 3)
			{
				for (int i = 0; i < 3 - iLevStep; ++i)
					PointChange(POINT_CONQUEROR_LEVEL_STEP, 1, false, true);
			}
			else if (exp >= q * 2 && iLevStep < 2)
			{
				for (int i = 0; i < 2 - iLevStep; ++i)
					PointChange(POINT_CONQUEROR_LEVEL_STEP, 1, false, true);
			}
			else if (exp >= q && iLevStep < 1)
				PointChange(POINT_CONQUEROR_LEVEL_STEP, 1);

			if (iExpBalance)
			{
				PointChange(POINT_CONQUEROR_EXP, iExpBalance);
			}

			val = GetConquerorExp();
		}
	} break;

	case POINT_CONQUEROR_LEVEL_STEP:
	{
		if (amount > 0)
		{
			val = GetPoint(POINT_CONQUEROR_LEVEL_STEP) + amount;
			switch (val)
			{
			case 1:
			case 2:
			case 3:
			{
				if ((GetConquerorLevel() <= gPlayerMaxLevelStats) && (GetConquerorLevel() <= gPlayerMaxConquerorLevel))
					PointChange(POINT_CONQUEROR_POINT, 1);
			} break;
			case 4:
			{
				PointChange(POINT_CONQUEROR_POINT, 1);
				PointChange(POINT_CONQUEROR_LEVEL, 1, false, true);
				val = 0;
			} break;
			}

			SetPoint(POINT_CONQUEROR_LEVEL_STEP, val);
			SetRealPoint(POINT_CONQUEROR_LEVEL_STEP, val);

			Save();
		}
		else
			val = GetPoint(POINT_CONQUEROR_LEVEL_STEP);
	} break;

#ifdef ENABLE_ATTR_6TH_7TH_EXTEND
	case POINT_RESIST_FIST:
	case POINT_SKILL_DAMAGE_SAMYEON:
	case POINT_SKILL_DAMAGE_TANHWAN:
	case POINT_SKILL_DAMAGE_PALBANG:
	case POINT_SKILL_DAMAGE_GIGONGCHAM:
	case POINT_SKILL_DAMAGE_GYOKSAN:
	case POINT_SKILL_DAMAGE_GEOMPUNG:
	case POINT_SKILL_DAMAGE_AMSEOP:
	case POINT_SKILL_DAMAGE_GUNGSIN:
	case POINT_SKILL_DAMAGE_CHARYUN:
	case POINT_SKILL_DAMAGE_SANGONG:
	case POINT_SKILL_DAMAGE_YEONSA:
	case POINT_SKILL_DAMAGE_KWANKYEOK:
	case POINT_SKILL_DAMAGE_GIGUNG:
	case POINT_SKILL_DAMAGE_HWAJO:
	case POINT_SKILL_DAMAGE_SWAERYUNG:
	case POINT_SKILL_DAMAGE_YONGKWON:
	case POINT_SKILL_DAMAGE_PABEOB:
	case POINT_SKILL_DAMAGE_MARYUNG:
	case POINT_SKILL_DAMAGE_HWAYEOMPOK:
	case POINT_SKILL_DAMAGE_MAHWAN:
	case POINT_SKILL_DAMAGE_BIPABU:
	case POINT_SKILL_DAMAGE_YONGBI:
	case POINT_SKILL_DAMAGE_PAERYONG:
	case POINT_SKILL_DAMAGE_NOEJEON:
	case POINT_SKILL_DAMAGE_BYEURAK:
	case POINT_SKILL_DAMAGE_CHAIN:
	case POINT_SKILL_DAMAGE_CHAYEOL:
	case POINT_SKILL_DAMAGE_SALPOONG:
	case POINT_SKILL_DAMAGE_GONGDAB:
	case POINT_SKILL_DAMAGE_PASWAE:
	case POINT_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE:
	case POINT_SKILL_DEFEND_BONUS_BOSS_OR_MORE:
	case POINT_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE:
	case POINT_SKILL_DAMAGE_BONUS_BOSS_OR_MORE:
	case POINT_HIT_BUFF_ENCHANT_FIRE:
	case POINT_HIT_BUFF_ENCHANT_ICE:
	case POINT_HIT_BUFF_ENCHANT_ELEC:
	case POINT_HIT_BUFF_ENCHANT_WIND:
	case POINT_HIT_BUFF_ENCHANT_DARK:
	case POINT_HIT_BUFF_ENCHANT_EARTH:
	case POINT_HIT_BUFF_RESIST_FIRE:
	case POINT_HIT_BUFF_RESIST_ICE:
	case POINT_HIT_BUFF_RESIST_ELEC:
	case POINT_HIT_BUFF_RESIST_WIND:
	case POINT_HIT_BUFF_RESIST_DARK:
	case POINT_HIT_BUFF_RESIST_EARTH:
	case POINT_USE_SKILL_CHEONGRANG_MOV_SPEED:
	case POINT_USE_SKILL_CHEONGRANG_CASTING_SPEED:
	case POINT_USE_SKILL_CHAYEOL_CRITICAL_PCT:
	case POINT_USE_SKILL_SANGONG_ATT_GRADE_BONUS:
	case POINT_USE_SKILL_GIGUNG_ATT_GRADE_BONUS:
	case POINT_USE_SKILL_JEOKRANG_DEF_BONUS:
	case POINT_USE_SKILL_GWIGEOM_DEF_BONUS:
	case POINT_USE_SKILL_TERROR_ATT_GRADE_BONUS:
	case POINT_USE_SKILL_MUYEONG_ATT_GRADE_BONUS:
	case POINT_USE_SKILL_MANASHILED_CASTING_SPEED:
	case POINT_USE_SKILL_HOSIN_DEF_BONUS:
	case POINT_USE_SKILL_GICHEON_ATT_GRADE_BONUS:
	case POINT_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS:
	case POINT_USE_SKILL_JEUNGRYEOK_DEF_BONUS:
	case POINT_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS:
	case POINT_USE_SKILL_CHUNKEON_CASTING_SPEED:
	case POINT_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS:
	case POINT_SKILL_DURATION_INCREASE_EUNHYUNG:
	case POINT_SKILL_DURATION_INCREASE_GYEONGGONG:
	case POINT_SKILL_DURATION_INCREASE_GEOMKYUNG:
	case POINT_SKILL_DURATION_INCREASE_JEOKRANG:
	case POINT_USE_SKILL_PALBANG_HP_ABSORB:
	case POINT_USE_SKILL_AMSEOP_HP_ABSORB:
	case POINT_USE_SKILL_YEONSA_HP_ABSORB:
	case POINT_USE_SKILL_YONGBI_HP_ABSORB:
	case POINT_USE_SKILL_CHAIN_HP_ABSORB:
	case POINT_USE_SKILL_PASWAE_SP_ABSORB:
	case POINT_USE_SKILL_GIGONGCHAM_STUN:
	case POINT_USE_SKILL_CHARYUN_STUN:
	case POINT_USE_SKILL_PABEOB_STUN:
	case POINT_USE_SKILL_MAHWAN_STUN:
	case POINT_USE_SKILL_GONGDAB_STUN:
	case POINT_USE_SKILL_SAMYEON_STUN:
	case POINT_USE_SKILL_GYOKSAN_KNOCKBACK:
	case POINT_USE_SKILL_SEOMJEON_KNOCKBACK:
	case POINT_USE_SKILL_SWAERYUNG_KNOCKBACK:
	case POINT_USE_SKILL_HWAYEOMPOK_KNOCKBACK:
	case POINT_USE_SKILL_GONGDAB_KNOCKBACK:
	case POINT_USE_SKILL_KWANKYEOK_KNOCKBACK:
	case POINT_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER:
	case POINT_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER:
	case POINT_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER:
	case POINT_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER:
	case POINT_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER:
	case POINT_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER:
	case POINT_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER:
	case POINT_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER:
	case POINT_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER:
	case POINT_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER:
	case POINT_DAMAGE_HP_RECOVERY:
	case POINT_DAMAGE_SP_RECOVERY:
	case POINT_ALIGNMENT_DAMAGE_BONUS:
	case POINT_NORMAL_DAMAGE_GUARD:
	case POINT_MORE_THEN_HP90_DAMAGE_REDUCE:
	case POINT_USE_SKILL_TUSOK_HP_ABSORB:
	case POINT_USE_SKILL_PAERYONG_HP_ABSORB:
	case POINT_USE_SKILL_BYEURAK_HP_ABSORB:
	case POINT_FIRST_ATTRIBUTE_BONUS:
	case POINT_SECOND_ATTRIBUTE_BONUS:
	case POINT_THIRD_ATTRIBUTE_BONUS:
	case POINT_FOURTH_ATTRIBUTE_BONUS:
	case POINT_FIFTH_ATTRIBUTE_BONUS:
	case POINT_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER:
	case POINT_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER:
	case POINT_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER:
	case POINT_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER:
	case POINT_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER:
	case POINT_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER:
	case POINT_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER:
	case POINT_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER:
	case POINT_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER:
	case POINT_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER:
	case POINT_USE_SKILL_CHAYEOL_HP_ABSORB:
	case POINT_HIT_PCT:
	case POINT_ATTBONUS_PER_HUMAN:
	case POINT_ATTBONUS_PER_ANIMAL:
	case POINT_ATTBONUS_PER_ORC:
	case POINT_ATTBONUS_PER_MILGYO:
	case POINT_ATTBONUS_PER_UNDEAD:
	case POINT_ATTBONUS_PER_DEVIL:
	case POINT_ENCHANT_PER_ELECT:
	case POINT_ENCHANT_PER_FIRE:
	case POINT_ENCHANT_PER_ICE:
	case POINT_ENCHANT_PER_WIND:
	case POINT_ENCHANT_PER_EARTH:
	case POINT_ENCHANT_PER_DARK:
	case POINT_ATTBONUS_PER_CZ:
	case POINT_ATTBONUS_PER_INSECT:
	case POINT_ATTBONUS_PER_DESERT:
	case POINT_ATTBONUS_PER_STONE:
	case POINT_ATTBONUS_PER_MONSTER:
	case POINT_RESIST_PER_HUMAN:
	case POINT_RESIST_PER_ICE:
	case POINT_RESIST_PER_DARK:
	case POINT_RESIST_PER_EARTH:
	case POINT_RESIST_PER_FIRE:
	case POINT_RESIST_PER_ELEC:
	case POINT_RESIST_PER_MAGIC:
	case POINT_RESIST_PER_WIND:
	case POINT_HIT_BUFF_SUNGMA_STR:
	case POINT_HIT_BUFF_SUNGMA_MOVE:
	case POINT_HIT_BUFF_SUNGMA_HP:
	case POINT_HIT_BUFF_SUNGMA_IMMUNE:
	case POINT_MOUNT_MELEE_MAGIC_ATTBONUS_PER:
	case POINT_DISMOUNT_MOVE_SPEED_BONUS_PER:
	case POINT_HIT_AUTO_HP_RECOVERY:
	case POINT_HIT_AUTO_SP_RECOVERY:
	case POINT_USE_SKILL_COOLTIME_DECREASE_ALL:
	case POINT_HIT_STONE_ATTBONUS_STONE:
	case POINT_HIT_STONE_DEF_GRADE_BONUS:
	case POINT_KILL_BOSS_ITEM_BONUS:
	case POINT_MOB_HIT_MOB_AGGRESSIVE:
	case POINT_NO_DEATH_AND_HP_RECOVERY30:
	case POINT_AUTO_PICKUP:
	case POINT_MOUNT_NO_KNOCKBACK:
	case POINT_IMMUNE_POISON100:
	case POINT_IMMUNE_BLEEDING100:
	case POINT_MONSTER_DEFEND_BONUS:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		break;
#endif

#ifdef ENABLE_REFINE_ELEMENT
	case POINT_ATT_ELEMENTS:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		break;
#endif

	case POINT_SUNGMA_STR:
	case POINT_SUNGMA_HP:
	case POINT_SUNGMA_MOVE:
	case POINT_SUNGMA_IMMUNE:
	{
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
	} break;

	case POINT_CONQUEROR_POINT:
	{
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);

		SetRealPoint(type, val);
	} break;
#endif

	case POINT_LEVEL:
	{
		if ((GetLevel() + amount) > gPlayerMaxLevel)
			return;

		SetLevel(GetLevel() + amount);
		val = GetLevel();

		sys_log(0, "LEVELUP: %s %d NEXT EXP %d", GetName(), GetLevel(), GetNextExp());

		// WOLFMAN ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Æ¯ï¿½ï¿½Ã³ï¿½ï¿½ (ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ï³ï¿½ï¿½Ì¹Ç·ï¿½, 5ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ç¸ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ 1ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½. ï¿½Ïµï¿½ï¿½Úµï¿½ ï¿½ï¿½ï¿½ï¿½)
		if (GetJob() == JOB_WOLFMAN)
		{
			if ((5 <= val) && (GetSkillGroup() != 1))
			{
				ClearSkill();
				SetSkillGroup(1);
				SetRealPoint(POINT_SKILL, GetLevel() - 1);
				SetPoint(POINT_SKILL, GetRealPoint(POINT_SKILL));
				PointChange(POINT_SKILL, 0);
			}
		}

#if defined(__CONQUEROR_LEVEL__)
		if ((GetConquerorLevel() > 0) && (val < gPlayerMaxLevel))
			SetConqueror(false);
#endif

		PointChange(POINT_NEXT_EXP, GetNextExp(), false);

		if (amount)
		{
			quest::CQuestManager::instance().LevelUp(GetPlayerID());

			LogManager::instance().LevelLog(this, val, GetRealPoint(POINT_PLAYTIME) + (get_dword_time() - m_dwPlayStartTime) / 60000);

			if (GetGuild())
				GetGuild()->LevelChange(GetPlayerID(), GetLevel());

			if (GetParty())
				GetParty()->RequestSetMemberLevel(GetPlayerID(), GetLevel());
		}

		PointsPacket();
	} break;

	case POINT_NEXT_EXP:
	{
		val = GetNextExp();
		bAmount = false; // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ bAmountï¿½ï¿½ false ï¿½ï¿½ï¿½ï¿½ ï¿½Ñ´ï¿½.
	} break;

	case POINT_EXP:
	{
#if defined(__ANTI_EXP_RING__)
		if (HasFrozenEXP())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have frozen your experience."));
			return;
		}
#endif

		DWORD exp = GetExp();
		DWORD next_exp = GetNextExp();

		// expï¿½ï¿½ 0 ï¿½ï¿½ï¿½Ï·ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Êµï¿½ï¿½ï¿½ ï¿½Ñ´ï¿½
		if (amount < 0 && exp <= -amount)
		{
			sys_log(0, "%s - Reduce EXP by %d, CUR EXP: %d (setting to zero)", GetName(), -amount, exp);
			amount = -(long)exp;

			SetExp(0);
			val = GetExp();
		}
		else
		{
#if defined(__CONQUEROR_LEVEL__)
			DWORD conqueror_exp = next_exp / 4;
			if (GetLevel() >= gPlayerMaxLevel && exp >= conqueror_exp)
				return;
#else
			if (gPlayerMaxLevel <= GetLevel())
				return;
#endif

			ChatPacket(CHAT_TYPE_EXP_INFO, LC_TEXT("You have received %d experience points."), amount);

			DWORD iExpBalance = 0;

			// ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½!
			if (exp + amount >= next_exp)
			{
				iExpBalance = (exp + amount) - next_exp;
				amount = next_exp - exp;

				SetExp(0);
				exp = next_exp;
			}
#if defined(__CONQUEROR_LEVEL__)
			else if (GetLevel() >= gPlayerMaxLevel)
			{
				if (exp + amount >= conqueror_exp)
				{
					SetExp(conqueror_exp);
					exp = GetExp();
				}
				else
				{
					SetExp(exp + amount);
					exp = GetExp();
				}
			}
#endif
			else
			{
				SetExp(exp + amount);
				exp = GetExp();
			}

			DWORD q = DWORD(next_exp / 4.0f);
			int iLevStep = GetRealPoint(POINT_LEVEL_STEP);

			// iLevStepï¿½ï¿½ 4 ï¿½Ì»ï¿½ï¿½Ì¸ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ã¶ï¿½ï¿½ï¿½ï¿½ ï¿½Ï¹Ç·ï¿½ ï¿½ï¿½ï¿½â¿¡ ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ì´ï¿½.
			if (iLevStep >= 4)
			{
				sys_err("%s LEVEL_STEP bigger than 4! (%d)", GetName(), iLevStep);
				iLevStep = 4;
			}

			if (exp >= next_exp && iLevStep < 4)
			{
				for (int i = 0; i < 4 - iLevStep; ++i)
					PointChange(POINT_LEVEL_STEP, 1, false, true);
			}
			else if (exp >= q * 3 && iLevStep < 3)
			{
				for (int i = 0; i < 3 - iLevStep; ++i)
					PointChange(POINT_LEVEL_STEP, 1, false, true);
			}
			else if (exp >= q * 2 && iLevStep < 2)
			{
				for (int i = 0; i < 2 - iLevStep; ++i)
					PointChange(POINT_LEVEL_STEP, 1, false, true);
			}
			else if (exp >= q && iLevStep < 1)
			{
				PointChange(POINT_LEVEL_STEP, 1);
			}

			if (iExpBalance)
			{
				PointChange(POINT_EXP, iExpBalance);
			}

			val = GetExp();
		}
	}
	break;

	case POINT_LEVEL_STEP:
	{
		if (amount > 0)
		{
			val = GetPoint(POINT_LEVEL_STEP) + amount;

			switch (val)
			{
			case 1:
			case 2:
			case 3:
				if ((GetLevel() <= gPlayerMaxLevelStats) && (GetLevel() <= gPlayerMaxLevel))
					PointChange(POINT_STAT, 1);
				break;

			case 4:
			{
				int iHP = number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end);
				int iSP = number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end);

				m_points.iRandomHP += iHP;
				m_points.iRandomSP += iSP;

				if (GetSkillGroup())
				{
					if (GetLevel() >= 5)
						PointChange(POINT_SKILL, 1);

					if (GetLevel() >= 9)
						PointChange(POINT_SUB_SKILL, 1);
				}

				PointChange(POINT_MAX_HP, iHP);
				PointChange(POINT_MAX_SP, iSP);
				PointChange(POINT_LEVEL, 1, false, true);

				val = 0;
			}
			break;
			}

			if (GetLevel() <= 10)
				AutoGiveItem(27001, 2);
			else if (GetLevel() <= 30)
				AutoGiveItem(27002, 2);
			else
			{
				AutoGiveItem(27002, 2);
				// AutoGiveItem(27003, 2);
			}

			PointChange(POINT_HP, GetMaxHP() - GetHP());
			PointChange(POINT_SP, GetMaxSP() - GetSP());
			PointChange(POINT_STAMINA, GetMaxStamina() - GetStamina());

			SetPoint(POINT_LEVEL_STEP, val);
			SetRealPoint(POINT_LEVEL_STEP, val);

			Save();
		}
		else
			val = GetPoint(POINT_LEVEL_STEP);
	} break;

	case POINT_HP:
	{
		if (IsDead() || IsStun())
			return;

		int64_t prev_hp = GetHP();

		amount = MIN(GetMaxHP() - GetHP(), amount);
		SetHP(GetHP() + amount);
		val = GetHP();

		BroadcastTargetPacket();

		if (GetParty() && IsPC() && val != prev_hp)
			GetParty()->SendPartyInfoOneToAll(this);
	}
	break;

	case POINT_SP:
	{
		if (IsDead() || IsStun())
			return;

		amount = MIN(GetMaxSP() - GetSP(), amount);
		SetSP(GetSP() + amount);
		val = GetSP();
	}
	break;

	case POINT_STAMINA:
	{
		if (IsDead() || IsStun())
			return;

		int prev_val = GetStamina();
		amount = MIN(GetMaxStamina() - GetStamina(), amount);
		SetStamina(GetStamina() + amount);
		val = GetStamina();

		if (val == 0)
		{
			// Staminaï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½!
			SetNowWalking(true);
		}
		else if (prev_val == 0)
		{
			// ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½×¹Ì³ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
			ResetWalking();
		}

		if (amount < 0 && val != 0) // ï¿½ï¿½ï¿½Ò´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê´Â´ï¿½.
			return;
	}
	break;

	case POINT_MAX_HP:
	{
		SetPoint(type, GetPoint(type) + amount);

		//SetMaxHP(GetMaxHP() + amount);
		// ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ = (ï¿½âº» ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ + ï¿½ß°ï¿½) * ï¿½Ö´ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½%
		int64_t curMaxHP = GetMaxHP();
		int64_t hp = GetRealPoint(POINT_MAX_HP);
		int64_t add_hp = MIN(3500, hp * GetPoint(POINT_MAX_HP_PCT) / 100);
		add_hp += GetPoint(POINT_MAX_HP);
		add_hp += GetPoint(POINT_PARTY_TANKER_BONUS);

		SetMaxHP(hp + add_hp);

		float fRatio = (float)GetMaxHP() / (float)curMaxHP;
		PointChange(POINT_HP, GetHP() * fRatio - GetHP());

		val = GetMaxHP();
	}
	break;

	case POINT_MAX_SP:
	{
		SetPoint(type, GetPoint(type) + amount);

		//SetMaxSP(GetMaxSP() + amount);
		// ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½Å·ï¿½ = (ï¿½âº» ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½Å·ï¿½ + ï¿½ß°ï¿½) * ï¿½Ö´ï¿½ï¿½ï¿½ï¿½Å·ï¿½%
		int curMaxSP = GetMaxSP();
		int sp = GetRealPoint(POINT_MAX_SP);
		int add_sp = MIN(800, sp * GetPoint(POINT_MAX_SP_PCT) / 100);
		add_sp += GetPoint(POINT_MAX_SP);
		add_sp += GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);

		SetMaxSP(sp + add_sp);

		float fRatio = (float)GetMaxSP() / (float)curMaxSP;
		PointChange(POINT_SP, GetSP() * fRatio - GetSP());

		val = GetMaxSP();
	}
	break;

	case POINT_MAX_HP_PCT:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);

		PointChange(POINT_MAX_HP, 0);
		break;

	case POINT_MAX_SP_PCT:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);

		PointChange(POINT_MAX_SP, 0);
		break;

	case POINT_MAX_STAMINA:
		SetMaxStamina(GetMaxStamina() + amount);
		val = GetMaxStamina();
		break;

	case POINT_GOLD:
	{
		const int64_t nTotalGold = static_cast<int64_t>(GetGold()) + static_cast<int64_t>(amount);

		if (g_MaxGold <= nTotalGold)
		{
			sys_err("[OVERFLOW_GOLD] OriGold %lld AddedGold %lld id %u Name %s ", GetGold(), amount, GetPlayerID(), GetName());
			LogManager::instance().CharLog(this, GetGold() + amount, "OVERFLOW_GOLD", "");
			return;
		}

		SetGold(GetGold() + amount);
		val = GetGold();
	}
	break;

#if defined(__CHEQUE_SYSTEM__)
	case POINT_CHEQUE:
	{
		const int64_t nTotalCheque = static_cast<int64_t>(GetCheque()) + static_cast<int64_t>(amount);

		if (CHEQUE_MAX <= nTotalCheque)
		{
			sys_err("[OVERFLOW_CHEQUE] OriCheque %d AddedCheque %lld id %u Name %s ", GetCheque(), amount, GetPlayerID(), GetName());
			LogManager::instance().CharLog(this, GetCheque() + amount, "OVERFLOW_CHEQUE", "");
			return;
		}

		SetCheque(GetCheque() + amount);
		val = GetCheque();
	}
	break;
#endif

#ifdef ENABLE_MEDAL_OF_HONOR
	case POINT_MEDAL_OF_HONOR:
	{
		const int64_t nTotalMedalHonor = static_cast<int64_t>(GetMedalHonor()) + static_cast<int64_t>(amount);

		if ((MEDAL_OF_HONOR_MAX <= nTotalMedalHonor) || (nTotalMedalHonor < 0))
			return;

		SetMedalHonor(GetMedalHonor() + amount);
		val = GetMedalHonor();
	}
	break;
#endif

#ifdef ENABLE_12ZI
	/*case POINT_BEAD:
	{
		const int64_t nTotalBeadPoints = static_cast<int64_t>(GetBeadPoint()) + static_cast<int64_t>(amount);

		if ((BEAD_POINT_MAX <= nTotalBeadPoints) || (nTotalBeadPoints < 0))
			return;

		SetBeadPoint(GetBeadPoint() + amount);
		amount = GetBeadPoint();
	}
	break;*/
#endif

#if defined(__GEM_SYSTEM__)
	case POINT_GEM:
	{
		const int64_t nTotalGem = static_cast<int64_t>(GetGem()) + static_cast<int64_t>(amount);

		if (GEM_MAX <= nTotalGem)
		{
			sys_err("[OVERFLOW_GEM] OriGem %d AddedGem %lld id %u Name %s ", GetGem(), amount, GetPlayerID(), GetName());
			LogManager::instance().CharLog(this, GetGem() + amount, "OVERFLOW_GEM", "");
			return;
		}

		SetGem(GetGem() + amount);
		val = GetGem();
	}
	break;
#endif

	case POINT_SKILL:
	case POINT_STAT:
	case POINT_SUB_SKILL:
	case POINT_STAT_RESET_COUNT:
	case POINT_HORSE_SKILL:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);

		SetRealPoint(type, val);
		break;

	case POINT_DEF_GRADE:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);

		PointChange(POINT_CLIENT_DEF_GRADE, amount);
		break;

	case POINT_CLIENT_DEF_GRADE:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		break;

	case POINT_ST:
	case POINT_HT:
	case POINT_DX:
	case POINT_IQ:
	case POINT_HP_REGEN:
	case POINT_SP_REGEN:
	case POINT_ATT_SPEED:
	case POINT_ATT_GRADE:
	case POINT_MOV_SPEED:
	case POINT_CASTING_SPEED:
	case POINT_MAGIC_ATT_GRADE:
	case POINT_MAGIC_DEF_GRADE:
	case POINT_BOW_DISTANCE:
	case POINT_HP_RECOVERY:
	case POINT_SP_RECOVERY:

	case POINT_ATTBONUS_HUMAN: // 42 ï¿½Î°ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	case POINT_ATTBONUS_ANIMAL: // 43 ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ % ï¿½ï¿½ï¿½ï¿½
	case POINT_ATTBONUS_ORC: // 44 ï¿½ï¿½ï¿½Í¿ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ % ï¿½ï¿½ï¿½ï¿½
	case POINT_ATTBONUS_MILGYO: // 45 ï¿½Ð±ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ % ï¿½ï¿½ï¿½ï¿½
	case POINT_ATTBONUS_UNDEAD: // 46 ï¿½ï¿½Ã¼ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ % ï¿½ï¿½ï¿½ï¿½
	case POINT_ATTBONUS_DEVIL: // 47 ï¿½ï¿½ï¿½ï¿½(ï¿½Ç¸ï¿½)ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ % ï¿½ï¿½ï¿½ï¿½

	case POINT_ATTBONUS_MONSTER:
	case POINT_ATTBONUS_SURA:
	case POINT_ATTBONUS_ASSASSIN:
	case POINT_ATTBONUS_WARRIOR:
	case POINT_ATTBONUS_SHAMAN:
	case POINT_ATTBONUS_WOLFMAN:

	case POINT_POISON_PCT:
	case POINT_BLEEDING_PCT:

	case POINT_STUN_PCT:
	case POINT_SLOW_PCT:

	case POINT_BLOCK:
	case POINT_DODGE:

	case POINT_CRITICAL_PCT:
	case POINT_RESIST_CRITICAL:
	case POINT_PENETRATE_PCT:
	case POINT_RESIST_PENETRATE:
	case POINT_CURSE_PCT:

	case POINT_STEAL_HP: // 48 ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	case POINT_STEAL_SP: // 49 ï¿½ï¿½ï¿½Å·ï¿½ ï¿½ï¿½ï¿½ï¿½

	case POINT_MANA_BURN_PCT: // 50 ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
	case POINT_DAMAGE_SP_RECOVER: // 51 ï¿½ï¿½ï¿½Ý´ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½Å·ï¿½ È¸ï¿½ï¿½ È®ï¿½ï¿½
	case POINT_RESIST_NORMAL_DAMAGE:
	case POINT_RESIST_SWORD:
	case POINT_RESIST_TWOHAND:
	case POINT_RESIST_DAGGER:
	case POINT_RESIST_BELL:
	case POINT_RESIST_FAN:
	case POINT_RESIST_BOW:
	case POINT_RESIST_CLAW:

	case POINT_RESIST_FIRE:
	case POINT_RESIST_ELEC:
	case POINT_RESIST_MAGIC:
#if defined(__MAGIC_REDUCTION__)
	case POINT_RESIST_MAGIC_REDUCTION:
#endif
	case POINT_RESIST_WIND:
	case POINT_RESIST_ICE:
	case POINT_RESIST_EARTH:
	case POINT_RESIST_DARK:
	case POINT_REFLECT_MELEE: // 67 ï¿½ï¿½ï¿½ï¿½ ï¿½Ý»ï¿½
	case POINT_REFLECT_CURSE: // 68 ï¿½ï¿½ï¿½ï¿½ ï¿½Ý»ï¿½
	case POINT_POISON_REDUCE: // 69 ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	case POINT_BLEEDING_REDUCE:

	case POINT_KILL_SP_RECOVER: // 70 ï¿½ï¿½ ï¿½Ò¸ï¿½ï¿½ MP È¸ï¿½ï¿½
	case POINT_KILL_HP_RECOVERY: // 75
	case POINT_HIT_HP_RECOVERY:
	case POINT_HIT_SP_RECOVERY:
	case POINT_MANASHIELD:
	case POINT_ATT_BONUS:
	case POINT_DEF_BONUS:
	case POINT_SKILL_DAMAGE_BONUS:
	case POINT_NORMAL_HIT_DAMAGE_BONUS:
	case POINT_SKILL_DEFEND_BONUS:
	case POINT_NORMAL_HIT_DEFEND_BONUS:

#if defined(__ELEMENT_SYSTEM__)
	case POINT_ENCHANT_ELECT:
	case POINT_ENCHANT_FIRE:
	case POINT_ENCHANT_ICE:
	case POINT_ENCHANT_WIND:
	case POINT_ENCHANT_EARTH:
	case POINT_ENCHANT_DARK:
	{
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		BroadcastTargetPacket();
	} break;

	case POINT_ATTBONUS_CZ:
	case POINT_ATTBONUS_INSECT:
	case POINT_ATTBONUS_DESERT:

	case POINT_ATTBONUS_SWORD:
	case POINT_ATTBONUS_TWOHAND:
	case POINT_ATTBONUS_DAGGER:
	case POINT_ATTBONUS_BELL:
	case POINT_ATTBONUS_FAN:
	case POINT_ATTBONUS_BOW:
	case POINT_ATTBONUS_CLAW:

	case POINT_RESIST_HUMAN:
#endif
	case POINT_ATTBONUS_STONE:
		// DEPEND_BONUS_ATTRIBUTES
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		break;
		// END_OF_DEPEND_BONUS_ATTRIBUTES

	case POINT_PARTY_ATTACKER_BONUS:
	case POINT_PARTY_TANKER_BONUS:
	case POINT_PARTY_BUFFER_BONUS:
	case POINT_PARTY_SKILL_MASTER_BONUS:
	case POINT_PARTY_HASTE_BONUS:
	case POINT_PARTY_DEFENDER_BONUS:

	case POINT_RESIST_WARRIOR:
	case POINT_RESIST_ASSASSIN:
	case POINT_RESIST_SURA:
	case POINT_RESIST_SHAMAN:
	case POINT_RESIST_WOLFMAN:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		break;

	case POINT_MALL_ATTBONUS:
	case POINT_MALL_DEFBONUS:
	case POINT_MALL_EXPBONUS:
	case POINT_MALL_ITEMBONUS:
	case POINT_MALL_GOLDBONUS:
	case POINT_MELEE_MAGIC_ATT_BONUS_PER:
		if (GetPoint(type) + amount > 100)
		{
			sys_err("MALL_BONUS exceeded over 100!! point type: %d name: %s amount %lld", type, GetName(), amount);
			amount = 100 - GetPoint(type);
		}

		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		break;

		// PC_BANG_ITEM_ADD
	case POINT_PC_BANG_EXP_BONUS:
	case POINT_PC_BANG_DROP_BONUS:
	case POINT_RAMADAN_CANDY_BONUS_EXP:
		SetPoint(type, amount);
		val = GetPoint(type);
		break;
		// END_PC_BANG_ITEM_ADD

	case POINT_EXP_DOUBLE_BONUS: // 71
	case POINT_GOLD_DOUBLE_BONUS: // 72
	case POINT_ITEM_DROP_BONUS: // 73
	case POINT_POTION_BONUS: // 74
		if (GetPoint(type) + amount > 100)
		{
			sys_err("BONUS exceeded over 100!! point type: %d name: %s amount %lld", type, GetName(), amount);
			amount = 100 - GetPoint(type);
		}

		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		break;

	case POINT_IMMUNE_STUN: // 76
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		if (val)
		{
			SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
		}
		else
		{
			REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
		}
		break;

	case POINT_IMMUNE_SLOW: // 77
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		if (val)
		{
			SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
		}
		else
		{
			REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
		}
		break;

	case POINT_IMMUNE_FALL: // 78
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		if (val)
		{
			SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
		}
		else
		{
			REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
		}
		break;

	case POINT_ATT_GRADE_BONUS:
		SetPoint(type, GetPoint(type) + amount);
		PointChange(POINT_ATT_GRADE, amount);
		val = GetPoint(type);
		break;

	case POINT_DEF_GRADE_BONUS:
		SetPoint(type, GetPoint(type) + amount);
		PointChange(POINT_DEF_GRADE, amount);
		val = GetPoint(type);
		break;

	case POINT_MAGIC_ATT_GRADE_BONUS:
		SetPoint(type, GetPoint(type) + amount);
		PointChange(POINT_MAGIC_ATT_GRADE, amount);
		val = GetPoint(type);
		break;

	case POINT_MAGIC_DEF_GRADE_BONUS:
		SetPoint(type, GetPoint(type) + amount);
		PointChange(POINT_MAGIC_DEF_GRADE, amount);
		val = GetPoint(type);
		break;

	case POINT_VOICE:
	case POINT_EMPIRE_POINT:
		//sys_err("CHARACTER::PointChange: %s: point cannot be changed. use SetPoint instead (type: %d)", GetName(), type);
		val = GetRealPoint(type);
		break;

	case POINT_POLYMORPH:
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
		SetPolymorph(val);
		break;

	case POINT_MOUNT:
	{
		SetPoint(type, GetPoint(type) + amount);
		val = GetPoint(type);
#if !defined(__MOUNT_COSTUME_SYSTEM__)
		MountVnum(val);
#endif
	}
	break;

	case POINT_ENERGY:
	case POINT_COSTUME_ATTR_BONUS:
	{
		int old_val = GetPoint(type);
		SetPoint(type, old_val + amount);
		val = GetPoint(type);
		BuffOnAttr_ValueChange(type, old_val, val);
	}
	break;

	default:
		sys_err("CHARACTER::PointChange: %s: unknown point change type %d", GetName(), type);
		return;
	}

	switch (type)
	{
	case POINT_LEVEL:
	case POINT_ST:
	case POINT_DX:
	case POINT_IQ:
	case POINT_HT:
#if defined(__CONQUEROR_LEVEL__)
	case POINT_CONQUEROR_LEVEL:
	case POINT_SUNGMA_STR:
	case POINT_SUNGMA_HP:
	case POINT_SUNGMA_MOVE:
	case POINT_SUNGMA_IMMUNE:
#endif
		ComputeBattlePoints();
		break;
	case POINT_MAX_HP:
	case POINT_MAX_SP:
	case POINT_MAX_STAMINA:
		break;
	}

	if (type == POINT_HP && amount == 0)
		return;

	UpdatePointsPacket(type, val, amount, bAmount, bBroadcast);
}

#ifdef ENABLE_12ZI
/*void CHARACTER::UpdatePoint(uint8_t point_type, int real_value)
{
	if (GetDesc())
	{
		TPacketGCPointChange pack{};

		pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
		pack.dwVID = m_vid;
		pack.type = point_type;
		pack.value = 0;
		pack.amount = !real_value ? GetPoint(point_type) : real_value;

		GetDesc()->Packet(&pack, sizeof(TPacketGCPointChange));
	}
}*/
#endif

void CHARACTER::ApplyPoint(uint16_t bApplyType, int iVal) //@fixme532
{
	switch (bApplyType)
	{
	case APPLY_NONE:						// 0
		break;

		// NOTE: ï¿½ï¿½ï¿½ï¿½ï¿½Û¿ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ö´ï¿½HP ï¿½ï¿½ï¿½Ê½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ® ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ê½ï¿½ï¿½ï¿½ ï¿½È°ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Ï¹Ç·ï¿½
		// ï¿½×³ï¿½ MAX_HPï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Ï¸ï¿½ ï¿½ï¿½ï¿½ï¿½Æ® ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½. ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Õ¸ï¿½ï¿½ï¿½ï¿½Ì±âµµ ï¿½Ï°ï¿½..
		// ï¿½Ù²ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ö´ï¿½ hpï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ hpï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Ù²ï¿½ ï¿½Ö´ï¿½ hpï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ hpï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ñ´ï¿½.
		// ï¿½ï¿½ï¿½ï¿½ PointChangeï¿½ï¿½ï¿½ï¿½ ï¿½Ï´Â°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ skip..
		// SPï¿½ï¿½ ï¿½È°ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Ñ´ï¿½.
		// Mantis : 101460 ~ ity ~
	case APPLY_MAX_HP:						// 1
	{
		if (GetMaxHP() == 0) break;
		PointChange(aApplyInfo[bApplyType].bPointType, iVal);
		break;
	}
	case APPLY_MAX_SP:						// 2
	{
		if (GetMaxSP() == 0) break;
		PointChange(aApplyInfo[bApplyType].bPointType, iVal);
		break;
	}

	case APPLY_CON:							// 3
	{
		PointChange(POINT_HT, iVal);
		PointChange(POINT_MAX_HP, (iVal * JobInitialPoints[GetJob()].hp_per_ht));
		PointChange(POINT_MAX_STAMINA, (iVal * JobInitialPoints[GetJob()].stamina_per_con));
		break;
	}

	case APPLY_INT:							// 4
	{
		PointChange(POINT_IQ, iVal);
		PointChange(POINT_MAX_SP, (iVal * JobInitialPoints[GetJob()].sp_per_iq));
		break;
	}

	case APPLY_STR:							// 5
	case APPLY_DEX:							// 6
	case APPLY_ATT_SPEED:					// 7
	case APPLY_MOV_SPEED:					// 8
	case APPLY_CAST_SPEED:					// 9
	case APPLY_HP_REGEN:					// 10
	case APPLY_SP_REGEN:					// 11
	case APPLY_POISON_PCT:					// 12
	case APPLY_STUN_PCT:					// 13
	case APPLY_SLOW_PCT:					// 14
	case APPLY_CRITICAL_PCT:				// 15
	case APPLY_PENETRATE_PCT:				// 16
	case APPLY_ATTBONUS_HUMAN:				// 17
	case APPLY_ATTBONUS_ANIMAL:				// 18
	case APPLY_ATTBONUS_ORC:				// 19
	case APPLY_ATTBONUS_MILGYO:				// 20
	case APPLY_ATTBONUS_UNDEAD:				// 21
	case APPLY_ATTBONUS_DEVIL:				// 22
	case APPLY_STEAL_HP:					// 23
	case APPLY_STEAL_SP:					// 24
	case APPLY_MANA_BURN_PCT:				// 25
	case APPLY_DAMAGE_SP_RECOVER:			// 26
	case APPLY_BLOCK:						// 27
	case APPLY_DODGE:						// 28
	case APPLY_RESIST_SWORD:				// 29
	case APPLY_RESIST_TWOHAND:				// 30
	case APPLY_RESIST_DAGGER:				// 31
	case APPLY_RESIST_BELL:					// 32
	case APPLY_RESIST_FAN:					// 33
	case APPLY_RESIST_BOW:					// 34
	case APPLY_RESIST_FIRE:					// 35
	case APPLY_RESIST_ELEC:					// 36
	case APPLY_RESIST_MAGIC:				// 37
	case APPLY_RESIST_WIND:					// 38
	case APPLY_REFLECT_MELEE:				// 39
	case APPLY_REFLECT_CURSE:				// 40
	case APPLY_POISON_REDUCE:				// 41
	case APPLY_KILL_SP_RECOVER:				// 42
	case APPLY_EXP_DOUBLE_BONUS:			// 43
	case APPLY_GOLD_DOUBLE_BONUS:			// 44
	case APPLY_ITEM_DROP_BONUS:				// 45
	case APPLY_POTION_BONUS:				// 46
	case APPLY_KILL_HP_RECOVER:				// 47
	case APPLY_IMMUNE_STUN:					// 48
	case APPLY_IMMUNE_SLOW:					// 49
	case APPLY_IMMUNE_FALL:					// 50
		PointChange(aApplyInfo[bApplyType].bPointType, iVal);
		break;

	case APPLY_SKILL:						// 51
		// SKILL_DAMAGE_BONUS
	{
		// ï¿½Ö»ï¿½ï¿½ï¿½ ï¿½ï¿½Æ® ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ 8ï¿½ï¿½Æ® vnum, 9ï¿½ï¿½Æ® add, 15ï¿½ï¿½Æ® change
		// 00000000 00000000 00000000 00000000
		// ^^^^^^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^
		// vnum		^ add		change
		BYTE bSkillVnum = (BYTE)(((DWORD)iVal) >> 24);
		int iAdd = iVal & 0x00800000;
		int iChange = iVal & 0x007fffff;

		sys_log(1, "APPLY_SKILL skill %d add? %d change %d", bSkillVnum, iAdd ? 1 : 0, iChange);

		if (0 == iAdd)
			iChange = -iChange;

		boost::unordered_map<BYTE, int>::iterator iter = m_SkillDamageBonus.find(bSkillVnum);

		if (iter == m_SkillDamageBonus.end())
			m_SkillDamageBonus.insert(std::make_pair(bSkillVnum, iChange));
		else
			iter->second += iChange;
	}
	// END_OF_SKILL_DAMAGE_BONUS
	break;

	case APPLY_BOW_DISTANCE:				// 52
	case APPLY_ATT_GRADE_BONUS:				// 53
	case APPLY_DEF_GRADE_BONUS:				// 54
	case APPLY_MAGIC_ATT_GRADE:				// 55
	case APPLY_MAGIC_DEF_GRADE:				// 56
	case APPLY_CURSE_PCT:					// 57
	case APPLY_MAX_STAMINA:					// 58

	case APPLY_ATT_BONUS_TO_WARRIOR:		// 59
	case APPLY_ATT_BONUS_TO_ASSASSIN:		// 60
	case APPLY_ATT_BONUS_TO_SURA:			// 61
	case APPLY_ATT_BONUS_TO_SHAMAN:			// 62
	case APPLY_ATT_BONUS_TO_MONSTER:		// 63

	case APPLY_MALL_ATTBONUS:				// 64
	case APPLY_MALL_DEFBONUS:				// 65
	case APPLY_MALL_EXPBONUS:				// 66 
	case APPLY_MALL_ITEMBONUS:				// 67
	case APPLY_MALL_GOLDBONUS:				// 68
		PointChange(aApplyInfo[bApplyType].bPointType, iVal);
		break;

	case APPLY_MAX_HP_PCT:					// 69
	{
		if (GetMaxHP() == 0) break;
		PointChange(aApplyInfo[bApplyType].bPointType, iVal);
	}
	break;
	case APPLY_MAX_SP_PCT:					// 70
	{
		if (GetMaxSP() == 0) break;
		PointChange(aApplyInfo[bApplyType].bPointType, iVal);
	}
	break;

	case APPLY_SKILL_DAMAGE_BONUS:			// 71
	case APPLY_NORMAL_HIT_DAMAGE_BONUS:		// 72

		// DEPEND_BONUS_ATTRIBUTES
	case APPLY_SKILL_DEFEND_BONUS:			// 73
	case APPLY_NORMAL_HIT_DEFEND_BONUS:		// 74
		// END_OF_DEPEND_BONUS_ATTRIBUTES

	case APPLY_PC_BANG_EXP_BONUS:			// 75
	case APPLY_PC_BANG_DROP_BONUS:			// 76

		// case APPLY_EXTRACT_HP_PCT:			// 77 ï¿½ï¿½ï¿½ï¿½ HP ï¿½Ò¸ï¿½

	case APPLY_RESIST_WARRIOR:				// 78
	case APPLY_RESIST_ASSASSIN:				// 79
	case APPLY_RESIST_SURA:					// 80
	case APPLY_RESIST_SHAMAN:				// 81

	case APPLY_ENERGY:						// 82 ï¿½ï¿½ï¿½
	case APPLY_DEF_GRADE:					// 83 ï¿½ï¿½ï¿½ï¿½. DEF_GRADE_BONUSï¿½ï¿½ Å¬ï¿½ó¿¡¼ï¿½ ï¿½Î¹ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Çµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½(...)ï¿½ï¿½ ï¿½Ö´ï¿½.
	case APPLY_COSTUME_ATTR_BONUS:			// 84 ï¿½Ú½ï¿½Æ¬ ï¿½ï¿½ï¿½ï¿½ï¿½Û¿ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ó¼ï¿½Ä¡ ï¿½ï¿½ï¿½Ê½ï¿½
	case APPLY_MAGIC_ATTBONUS_PER:			// 85 ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ý·ï¿½ +x%
	case APPLY_MELEE_MAGIC_ATTBONUS_PER:	// 86 ï¿½ï¿½ï¿½ï¿½ + ï¿½Ð¸ï¿½ ï¿½ï¿½ï¿½Ý·ï¿½ +x%

	case APPLY_RESIST_ICE:					// 87 ï¿½Ã±ï¿½ ï¿½ï¿½ï¿½ï¿½
	case APPLY_RESIST_EARTH:				// 88 ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	case APPLY_RESIST_DARK:					// 89 ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½

	case APPLY_ANTI_CRITICAL_PCT:			// 90 Å©ï¿½ï¿½Æ¼ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	case APPLY_ANTI_PENETRATE_PCT:			// 91 ï¿½ï¿½ï¿½ï¿½Å¸ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½

	case APPLY_BLEEDING_REDUCE:				// 92
	case APPLY_BLEEDING_PCT:				// 93
	case APPLY_ATT_BONUS_TO_WOLFMAN:		// 94
	case APPLY_RESIST_WOLFMAN:				// 95
	case APPLY_RESIST_CLAW:					// 96

#if defined(__MOUNT_COSTUME_SYSTEM__)
	case APPLY_MOUNT:						// 97
#endif

#if defined(__ACCE_COSTUME_SYSTEM__)
	case APPLY_ACCEDRAIN_RATE:				// 98
#endif

#if defined(__MAGIC_REDUCTION__)
	case APPLY_RESIST_MAGIC_REDUCTION:		// 99
#endif

#if defined(__ELEMENT_SYSTEM__)
	case APPLY_ENCHANT_ELECT:				// 100
	case APPLY_ENCHANT_FIRE:				// 101
	case APPLY_ENCHANT_ICE:					// 102
	case APPLY_ENCHANT_WIND:				// 103
	case APPLY_ENCHANT_EARTH:				// 104
	case APPLY_ENCHANT_DARK:				// 105
	case APPLY_ATTBONUS_CZ:					// 106
	case APPLY_ATTBONUS_INSECT:				// 107
	case APPLY_ATTBONUS_DESERT:				// 108

	case APPLY_ATTBONUS_SWORD:				// 109
	case APPLY_ATTBONUS_TWOHAND:			// 110
	case APPLY_ATTBONUS_DAGGER:				// 111
	case APPLY_ATTBONUS_BELL:				// 112
	case APPLY_ATTBONUS_FAN:				// 113
	case APPLY_ATTBONUS_BOW:				// 114
	case APPLY_ATTBONUS_CLAW:				// 115

	case APPLY_RESIST_HUMAN:				// 116
#endif
	case APPLY_ATTBONUS_STONE:				// 117
#if defined(__CONQUEROR_LEVEL__)
	case APPLY_SUNGMA_STR:					// 118
	case APPLY_SUNGMA_HP:					// 119
	case APPLY_SUNGMA_MOVE:					// 120
	case APPLY_SUNGMA_IMMUNE:				// 121
#endif
#ifdef ENABLE_ATTR_6TH_7TH_EXTEND
	case APPLY_SKILL_DAMAGE_SAMYEON:
	case APPLY_SKILL_DAMAGE_TANHWAN:
	case APPLY_SKILL_DAMAGE_PALBANG:
	case APPLY_SKILL_DAMAGE_GIGONGCHAM:
	case APPLY_SKILL_DAMAGE_GYOKSAN:
	case APPLY_SKILL_DAMAGE_GEOMPUNG:
	case APPLY_SKILL_DAMAGE_AMSEOP:
	case APPLY_SKILL_DAMAGE_GUNGSIN:
	case APPLY_SKILL_DAMAGE_CHARYUN:
	case APPLY_SKILL_DAMAGE_SANGONG:
	case APPLY_SKILL_DAMAGE_YEONSA:
	case APPLY_SKILL_DAMAGE_KWANKYEOK:
	case APPLY_SKILL_DAMAGE_GIGUNG:
	case APPLY_SKILL_DAMAGE_HWAJO:
	case APPLY_SKILL_DAMAGE_SWAERYUNG:
	case APPLY_SKILL_DAMAGE_YONGKWON:
	case APPLY_SKILL_DAMAGE_PABEOB:
	case APPLY_SKILL_DAMAGE_MARYUNG:
	case APPLY_SKILL_DAMAGE_HWAYEOMPOK:
	case APPLY_SKILL_DAMAGE_MAHWAN:
	case APPLY_SKILL_DAMAGE_BIPABU:
	case APPLY_SKILL_DAMAGE_YONGBI:
	case APPLY_SKILL_DAMAGE_PAERYONG:
	case APPLY_SKILL_DAMAGE_NOEJEON:
	case APPLY_SKILL_DAMAGE_BYEURAK:
	case APPLY_SKILL_DAMAGE_CHAIN:
	case APPLY_SKILL_DAMAGE_CHAYEOL:
	case APPLY_SKILL_DAMAGE_SALPOONG:
	case APPLY_SKILL_DAMAGE_GONGDAB:
	case APPLY_SKILL_DAMAGE_PASWAE:
	case APPLY_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE:
	case APPLY_SKILL_DEFEND_BONUS_BOSS_OR_MORE:
	case APPLY_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE:
	case APPLY_SKILL_DAMAGE_BONUS_BOSS_OR_MORE:
	case APPLY_HIT_BUFF_ENCHANT_FIRE:
	case APPLY_HIT_BUFF_ENCHANT_ICE:
	case APPLY_HIT_BUFF_ENCHANT_ELEC:
	case APPLY_HIT_BUFF_ENCHANT_WIND:
	case APPLY_HIT_BUFF_ENCHANT_DARK:
	case APPLY_HIT_BUFF_ENCHANT_EARTH:
	case APPLY_HIT_BUFF_RESIST_FIRE:
	case APPLY_HIT_BUFF_RESIST_ICE:
	case APPLY_HIT_BUFF_RESIST_ELEC:
	case APPLY_HIT_BUFF_RESIST_WIND:
	case APPLY_HIT_BUFF_RESIST_DARK:
	case APPLY_HIT_BUFF_RESIST_EARTH:
	case APPLY_USE_SKILL_CHEONGRANG_MOV_SPEED:
	case APPLY_USE_SKILL_CHEONGRANG_CASTING_SPEED:
	case APPLY_USE_SKILL_CHAYEOL_CRITICAL_PCT:
	case APPLY_USE_SKILL_SANGONG_ATT_GRADE_BONUS:
	case APPLY_USE_SKILL_GIGUNG_ATT_GRADE_BONUS:
	case APPLY_USE_SKILL_JEOKRANG_DEF_BONUS:
	case APPLY_USE_SKILL_GWIGEOM_DEF_BONUS:
	case APPLY_USE_SKILL_TERROR_ATT_GRADE_BONUS:
	case APPLY_USE_SKILL_MUYEONG_ATT_GRADE_BONUS:
	case APPLY_USE_SKILL_MANASHILED_CASTING_SPEED:
	case APPLY_USE_SKILL_HOSIN_DEF_BONUS:
	case APPLY_USE_SKILL_GICHEON_ATT_GRADE_BONUS:
	case APPLY_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS:
	case APPLY_USE_SKILL_JEUNGRYEOK_DEF_BONUS:
	case APPLY_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS:
	case APPLY_USE_SKILL_CHUNKEON_CASTING_SPEED:
	case APPLY_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS:
	case APPLY_SKILL_DURATION_INCREASE_EUNHYUNG:
	case APPLY_SKILL_DURATION_INCREASE_GYEONGGONG:
	case APPLY_SKILL_DURATION_INCREASE_GEOMKYUNG:
	case APPLY_SKILL_DURATION_INCREASE_JEOKRANG:
	case APPLY_USE_SKILL_PALBANG_HP_ABSORB:
	case APPLY_USE_SKILL_AMSEOP_HP_ABSORB:
	case APPLY_USE_SKILL_YEONSA_HP_ABSORB:
	case APPLY_USE_SKILL_YONGBI_HP_ABSORB:
	case APPLY_USE_SKILL_CHAIN_HP_ABSORB:
	case APPLY_USE_SKILL_PASWAE_SP_ABSORB:
	case APPLY_USE_SKILL_GIGONGCHAM_STUN:
	case APPLY_USE_SKILL_CHARYUN_STUN:
	case APPLY_USE_SKILL_PABEOB_STUN:
	case APPLY_USE_SKILL_MAHWAN_STUN:
	case APPLY_USE_SKILL_GONGDAB_STUN:
	case APPLY_USE_SKILL_SAMYEON_STUN:
	case APPLY_USE_SKILL_GYOKSAN_KNOCKBACK:
	case APPLY_USE_SKILL_SEOMJEON_KNOCKBACK:
	case APPLY_USE_SKILL_SWAERYUNG_KNOCKBACK:
	case APPLY_USE_SKILL_HWAYEOMPOK_KNOCKBACK:
	case APPLY_USE_SKILL_GONGDAB_KNOCKBACK:
	case APPLY_USE_SKILL_KWANKYEOK_KNOCKBACK:
	case APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER:
	case APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER:
	case APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER:
	case APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER:
	case APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER:
	case APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER:
	case APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER:
	case APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER:
	case APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER:
	case APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER:
	case APPLY_DAMAGE_HP_RECOVERY:
	case APPLY_DAMAGE_SP_RECOVERY:
	case APPLY_ALIGNMENT_DAMAGE_BONUS:
	case APPLY_NORMAL_DAMAGE_GUARD:
	case APPLY_MORE_THEN_HP90_DAMAGE_REDUCE:
	case APPLY_USE_SKILL_TUSOK_HP_ABSORB:
	case APPLY_USE_SKILL_PAERYONG_HP_ABSORB:
	case APPLY_USE_SKILL_BYEURAK_HP_ABSORB:
	case APPLY_FIRST_ATTRIBUTE_BONUS:
	case APPLY_SECOND_ATTRIBUTE_BONUS:
	case APPLY_THIRD_ATTRIBUTE_BONUS:
	case APPLY_FOURTH_ATTRIBUTE_BONUS:
	case APPLY_FIFTH_ATTRIBUTE_BONUS:
	case APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER:
	case APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER:
	case APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER:
	case APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER:
	case APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER:
	case APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER:
	case APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER:
	case APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER:
	case APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER:
	case APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER:
	case APPLY_USE_SKILL_CHAYEOL_HP_ABSORB:
	case APPLY_HIT_PCT:
	case APPLY_ATTBONUS_PER_HUMAN:
	case APPLY_ATTBONUS_PER_ANIMAL:
	case APPLY_ATTBONUS_PER_ORC:
	case APPLY_ATTBONUS_PER_MILGYO:
	case APPLY_ATTBONUS_PER_UNDEAD:
	case APPLY_ATTBONUS_PER_DEVIL:
	case APPLY_ENCHANT_PER_ELECT:
	case APPLY_ENCHANT_PER_FIRE:
	case APPLY_ENCHANT_PER_ICE:
	case APPLY_ENCHANT_PER_WIND:
	case APPLY_ENCHANT_PER_EARTH:
	case APPLY_ENCHANT_PER_DARK:
	case APPLY_ATTBONUS_PER_CZ:
	case APPLY_ATTBONUS_PER_INSECT:
	case APPLY_ATTBONUS_PER_DESERT:
	case APPLY_ATTBONUS_PER_MONSTER:
	case APPLY_RESIST_PER_HUMAN:
	case APPLY_RESIST_PER_ICE:
	case APPLY_RESIST_PER_DARK:
	case APPLY_RESIST_PER_EARTH:
	case APPLY_RESIST_PER_FIRE:
	case APPLY_RESIST_PER_ELEC:
	case APPLY_RESIST_PER_MAGIC:
	case APPLY_RESIST_PER_WIND:
	case APPLY_HIT_BUFF_SUNGMA_STR:
	case APPLY_HIT_BUFF_SUNGMA_MOVE:
	case APPLY_HIT_BUFF_SUNGMA_HP:
	case APPLY_HIT_BUFF_SUNGMA_IMMUNE:
	case APPLY_MOUNT_MELEE_MAGIC_ATTBONUS_PER:
	case APPLY_DISMOUNT_MOVE_SPEED_BONUS_PER:
	case APPLY_HIT_AUTO_HP_RECOVERY:
	case APPLY_HIT_AUTO_SP_RECOVERY:
	case APPLY_USE_SKILL_COOLTIME_DECREASE_ALL:
	case APPLY_HIT_STONE_ATTBONUS_STONE:
	case APPLY_HIT_STONE_DEF_GRADE_BONUS:
	case APPLY_KILL_BOSS_ITEM_BONUS:
	case APPLY_MOB_HIT_MOB_AGGRESSIVE:
	case APPLY_NO_DEATH_AND_HP_RECOVERY30:
	case APPLY_AUTO_PICKUP:
	case APPLY_MOUNT_NO_KNOCKBACK:
	case APPLY_SUNGMA_PER_STR:
	case APPLY_SUNGMA_PER_HP:
	case APPLY_SUNGMA_PER_MOVE:
	case APPLY_SUNGMA_PER_IMMUNE:
	case APPLY_IMMUNE_POISON100:
	case APPLY_IMMUNE_BLEEDING100:
	case APPLY_MONSTER_DEFEND_BONUS:
#endif
		PointChange(aApplyInfo[bApplyType].bPointType, iVal);
		break;

#if defined(__ITEM_APPLY_RANDOM__)
	case APPLY_RANDOM:
		break;
#endif

	default:
		sys_err("Unknown apply type %d name %s", bApplyType, GetName());
		break;
	}
}

void CHARACTER::MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion* packet)
{
	packet->header = HEADER_GC_MOTION;
	packet->vid = m_vid;
	packet->motion = motion;

	if (victim)
		packet->victim_vid = victim->GetVID();
	else
		packet->victim_vid = 0;
}

void CHARACTER::Motion(BYTE motion, LPCHARACTER victim)
{
	struct packet_motion pack_motion;
	MotionPacketEncode(motion, victim, &pack_motion);
	PacketAround(&pack_motion, sizeof(struct packet_motion));
}

EVENTFUNC(save_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("save_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) // <Factor>
		return 0;

	sys_log(1, "SAVE_EVENT: %s", ch->GetName());
	ch->Save();
	ch->FlushDelayedSaveItem();
	return (save_event_second_cycle);
}

void CHARACTER::StartSaveEvent()
{
	if (m_pkSaveEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;
	m_pkSaveEvent = event_create(save_event, info, save_event_second_cycle);
}

void CHARACTER::MonsterLog(const char* format, ...)
{
	if (!test_server)
		return;

	if (IsPC())
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	int len = snprintf(chatbuf, sizeof(chatbuf), "%u)", (DWORD)GetVID());

	if (len < 0 || len >= (int)sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	va_list args;

	va_start(args, format);

	int len2 = vsnprintf(chatbuf + len, sizeof(chatbuf) - len, format, args);

	if (len2 < 0 || len2 >= (int)sizeof(chatbuf) - len)
		len += (sizeof(chatbuf) - len) - 1;
	else
		len += len2;

	// \0 ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	++len;

	va_end(args);

	TPacketGCChat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(TPacketGCChat) + len;
	pack_chat.type = CHAT_TYPE_TALKING;
	pack_chat.id = (DWORD)GetVID();
	pack_chat.bEmpire = 0;

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(TPacketGCChat));
	buf.write(chatbuf, len);

	CHARACTER_MANAGER::instance().PacketMonsterLog(this, buf.read_peek(), buf.size());
}

void CHARACTER::ChatPacket(BYTE type, const char* format, ...)
{
	LPDESC d = GetDesc();

	if (!d || !format)
		return;

	if (IsWarping())
		return;

	const char* localeFormat;
	if (type != CHAT_TYPE_COMMAND || type != CHAT_TYPE_NOTICE)
		localeFormat = LC_STRING(format, d->GetLanguage());
	else
		localeFormat = format;

	if (!localeFormat)
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	va_list args;

	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), localeFormat, args);
	va_end(args);

	struct packet_chat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(struct packet_chat) + len;
	pack_chat.type = type;
	pack_chat.id = 0;
	pack_chat.bEmpire = d->GetEmpire();

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(chatbuf, len);

	d->Packet(buf.read_peek(), buf.size());

	if (type == CHAT_TYPE_COMMAND && test_server)
		sys_log(0, "SEND_COMMAND %s %s", GetName(), chatbuf);
}

// MINING
void CHARACTER::mining_take()
{
	m_pkMiningEvent = NULL;
}

void CHARACTER::mining_cancel()
{
	if (m_pkMiningEvent)
	{
		sys_log(0, "XXX MINING CANCEL");
		event_cancel(&m_pkMiningEvent);
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Mining is finished."));
	}
}

void CHARACTER::mining(LPCHARACTER chLoad)
{
	if (m_pkMiningEvent)
	{
		mining_cancel();
		return;
	}

	if (!chLoad)
		return;

	if (IsRiding())
		return;

	if (GetMapIndex() != chLoad->GetMapIndex() || DISTANCE_APPROX(GetX() - chLoad->GetX(), GetY() - chLoad->GetY()) > 1000)
		return;

	if (mining::GetRawOreFromLoad(chLoad->GetRaceNum()) == 0)
		return;

	LPITEM pick = GetWear(WEAR_WEAPON);

	if (!pick || pick->GetType() != ITEM_PICK)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need a Pickaxe in order to extract ore!"));
		return;
	}

	int count = number(5, 15); // ï¿½ï¿½ï¿½ï¿½ È½ï¿½ï¿½, ï¿½ï¿½ ï¿½ï¿½ï¿½Û´ï¿½ 2ï¿½ï¿½

	// Ã¤ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	TPacketGCDigMotion p;
	p.header = HEADER_GC_DIG_MOTION;
	p.vid = GetVID();
	p.target_vid = chLoad->GetVID();
	p.count = count;

	PacketAround(&p, sizeof(p));

	m_pkMiningEvent = mining::CreateMiningEvent(this, chLoad, count);
}
// END_OF_MINING

void CHARACTER::fishing()
{
	if (m_pkFishingEvent)
	{
		fishing_take();
		return;
	}

	// ï¿½ï¿½ï¿½ï¿½ ï¿½Ó¼ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ã¸ï¿½ ï¿½Ãµï¿½ï¿½Ñ´ï¿½?
	{
		LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

		int x = GetX();
		int y = GetY();

		LPSECTREE tree = pkSectreeMap->Find(x, y);
		DWORD dwAttr = tree->GetAttribute(x, y);

		if (IS_SET(dwAttr, ATTR_BLOCK))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot fish here."));
			return;
		}
	}

	LPITEM rod = GetWear(WEAR_WEAPON);

	// ï¿½ï¿½ï¿½Ã´ï¿½ ï¿½ï¿½ï¿½ï¿½
	if (!rod || rod->GetType() != ITEM_ROD)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please choose a Fishing Pole."));
		return;
	}

	if (0 == rod->GetSocket(2))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Place the Bait on the Hook first."));
		return;
	}

	float fx, fy;
	GetDeltaByDegree(GetRotation(), 400.0f, &fx, &fy);

	m_pkFishingEvent = fishing::CreateFishingEvent(this);
}

void CHARACTER::fishing_take()
{
	LPITEM rod = GetWear(WEAR_WEAPON);
	if (rod && rod->GetType() == ITEM_ROD)
	{
		using fishing::fishing_event_info;
		if (m_pkFishingEvent)
		{
			struct fishing_event_info* info = dynamic_cast<struct fishing_event_info*>(m_pkFishingEvent->info);

			if (info)
				fishing::Take(info, this);
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't go fishing without a fishing pole!"));
	}

	event_cancel(&m_pkFishingEvent);
}

bool CHARACTER::StartStateMachine(int iNextPulse)
{
	if (CHARACTER_MANAGER::instance().AddToStateList(this))
	{
		m_dwNextStatePulse = thecore_heart->pulse + iNextPulse;
		return true;
	}

	return false;
}

void CHARACTER::StopStateMachine()
{
	CHARACTER_MANAGER::instance().RemoveFromStateList(this);
}

void CHARACTER::UpdateStateMachine(DWORD dwPulse)
{
	if (dwPulse < m_dwNextStatePulse)
		return;

	if (IsDead())
		return;

	Update();
	m_dwNextStatePulse = dwPulse + m_dwStateDuration;
}

void CHARACTER::SetNextStatePulse(int iNextPulse)
{
	CHARACTER_MANAGER::instance().AddToStateList(this);
	m_dwNextStatePulse = iNextPulse;

	if (iNextPulse < 10)
		MonsterLog("Wait 10 sec");
}

// Character instance update function.
void CHARACTER::UpdateCharacter(DWORD dwPulse)
{
	CFSM::Update();
}

#if defined(ENABLE_12ZI) && defined(ENABLE_12ZI_SHOP_LIMIT)
void CHARACTER::BroadcastUpdateLimitedPurchase(uint32_t dwVnum, uint32_t dwLimitedCount, uint32_t dwLimitedPurchaseCount)
{
	TPacketGCShop pack{};
	pack.header = HEADER_GC_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_UNK_21;
	pack.size = sizeof(TPacketGCShop) + sizeof(TPacketGCShopUpdateLimitedPurchase);

	TPacketGCShopUpdateLimitedPurchase pack2{};
	pack2.vnum = dwVnum;
	pack2.limited_count = dwLimitedCount;
	pack2.limited_purchase_count = dwLimitedPurchaseCount;

	pack.size = sizeof(TPacketGCShop) + sizeof(TPacketGCShopUpdateLimitedPurchase);

	if (GetDesc())
	{
		GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCShop));
		GetDesc()->Packet(&pack2, sizeof(TPacketGCShopUpdateLimitedPurchase));
	}
}

void CHARACTER::SetPurchaseItemLimit(LPCHARACTER ch, uint32_t dwVnum, uint32_t dwLimitedCount, uint32_t dwLimitedPurchaseCount)
{
	if (!ch)
		return;

	const auto dwPlayerID = ch->GetPlayerID();
	const auto it = m_ShopLimitedPurchaseInfo.find(dwPlayerID);
	if (it == m_ShopLimitedPurchaseInfo.end())
	{
		TShopPriceLimitCount::TPurchaseData purchaseData;
		purchaseData.dwLimitedCount = dwLimitedCount;
		purchaseData.dwLimitedPurchaseCount = dwLimitedPurchaseCount;

		TShopPriceLimitCount purchaseInfo;
		purchaseInfo.data.emplace(dwVnum, purchaseData);

		m_ShopLimitedPurchaseInfo.emplace(dwPlayerID, purchaseInfo);
		ch->BroadcastUpdateLimitedPurchase(dwVnum, dwLimitedCount, dwLimitedPurchaseCount);
	}
	else
	{
		const auto it2 = it->second.data.find(dwVnum);
		if (it2 == it->second.data.end())
		{
			TShopPriceLimitCount::TPurchaseData purchaseData;
			purchaseData.dwLimitedCount = dwLimitedCount;
			purchaseData.dwLimitedPurchaseCount = dwLimitedPurchaseCount;

			it->second.data.emplace(dwVnum, purchaseData);

			ch->BroadcastUpdateLimitedPurchase(dwVnum, dwLimitedCount, dwLimitedPurchaseCount);
			return;
		}

		ch->BroadcastUpdateLimitedPurchase(dwVnum, it2->second.dwLimitedCount, it2->second.dwLimitedPurchaseCount);
	}
}

void CHARACTER::ShopPurchaseLimitItem(LPCHARACTER ch, uint32_t dwVnum)
{
	if (!ch)
		return;

	const auto dwPlayerID = ch->GetPlayerID();
	const auto it = m_ShopLimitedPurchaseInfo.find(dwPlayerID);
	if (it != m_ShopLimitedPurchaseInfo.end())
	{
		const auto it2 = it->second.data.find(dwVnum);
		if (it2 != it->second.data.end())
		{
			it2->second.dwLimitedPurchaseCount++;
			ch->BroadcastUpdateLimitedPurchase(dwVnum, it2->second.dwLimitedCount, it2->second.dwLimitedPurchaseCount);
		}
	}
}

bool CHARACTER::CanShopPurchaseLimitedItem(LPCHARACTER ch, uint32_t dwVnum)
{
	if (!ch)
		return false;

	const auto dwPlayerID = ch->GetPlayerID();
	const auto it = m_ShopLimitedPurchaseInfo.find(dwPlayerID);
	if (it != m_ShopLimitedPurchaseInfo.end())
	{
		const auto it2 = it->second.data.find(dwVnum);
		if (it2 != it->second.data.end())
		{
			if (it2->second.dwLimitedPurchaseCount >= it2->second.dwLimitedCount)
				return false;
		}
	}

	return true;
}

uint32_t CHARACTER::GetShopLimitedCount(uint32_t dwPlayerID, uint32_t dwVnum)
{
	const auto it = m_ShopLimitedPurchaseInfo.find(dwPlayerID);
	if (it != m_ShopLimitedPurchaseInfo.end())
	{
		const auto it2 = it->second.data.find(dwVnum);
		if (it2 != it->second.data.end())
			return it2->second.dwLimitedCount;
	}

	return 0;
}

uint32_t CHARACTER::GetShopLimitedPurchaseCount(uint32_t dwPlayerID, uint32_t dwVnum)
{
	const auto it = m_ShopLimitedPurchaseInfo.find(dwPlayerID);
	if (it != m_ShopLimitedPurchaseInfo.end())
	{
		const auto it2 = it->second.data.find(dwVnum);
		if (it2 != it->second.data.end())
			return it2->second.dwLimitedPurchaseCount;
	}

	return 0;
}
#endif

void CHARACTER::SetShop(LPSHOP pkShop)
{
	if ((m_pkShop = pkShop))
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
	else
	{
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
		SetShopOwner(NULL);
	}
}

void CHARACTER::SetExchange(CExchange* pkExchange)
{
	m_pkExchange = pkExchange;
}

void CHARACTER::SetPart(BYTE bPartPos, WORD wVal)
{
	assert(bPartPos < PART_MAX_NUM);
	m_pointsInstant.parts[bPartPos] = wVal;
}

DWORD CHARACTER::GetPart(BYTE bPartPos) const
{
	assert(bPartPos < PART_MAX_NUM);
#if defined(__HIDE_COSTUME_SYSTEM__)
	if (bPartPos == PART_MAIN && GetWear(WEAR_COSTUME_BODY) && IsBodyCostumeHidden() == true)
	{
		if (const LPITEM pArmor = GetWear(WEAR_BODY))
#if defined(__CHANGE_LOOK_SYSTEM__)
			return pArmor->GetTransmutationVnum() != 0 ? pArmor->GetTransmutationVnum() : pArmor->GetVnum();
#else
			return pArmor->GetVnum();
#endif
		else
			return 0;
	}
	else if (bPartPos == PART_HAIR && GetWear(WEAR_COSTUME_HAIR) && IsHairCostumeHidden() == true)
		return 0;
	else if (bPartPos == PART_ACCE && GetWear(WEAR_COSTUME_ACCE) && IsAcceCostumeHidden() == true)
		return 0;
	else if (bPartPos == PART_WEAPON && GetWear(WEAR_COSTUME_WEAPON) && IsWeaponCostumeHidden() == true)
	{
		if (const LPITEM pWeapon = GetWear(WEAR_WEAPON))
#if defined(__CHANGE_LOOK_SYSTEM__)
			return pWeapon->GetTransmutationVnum() != 0 ? pWeapon->GetTransmutationVnum() : pWeapon->GetVnum();
#else
			return pWeapon->GetVnum();
#endif
		else
			return 0;
	}
#endif
	else if (bPartPos == PART_AURA && GetWear(WEAR_COSTUME_AURA) && IsAuraCostumeHidden() == true)
		return 0;
	return m_pointsInstant.parts[bPartPos];
}

DWORD CHARACTER::GetOriginalPart(BYTE bPartPos) const
{
	switch (bPartPos)
	{
	case PART_MAIN:
#if defined(__HIDE_COSTUME_SYSTEM__)
		if (GetWear(WEAR_COSTUME_BODY) && IsBodyCostumeHidden() == true)
			if (const LPITEM pArmor = GetWear(WEAR_BODY))
				return pArmor->GetVnum();
#endif
		if (!IsPC()) // PCï¿½ï¿½ ï¿½Æ´ï¿½ ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½×´ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
			return GetPart(PART_MAIN);
		else
			return m_pointsInstant.bBasePart;

	case PART_HAIR:
#if defined(__HIDE_COSTUME_SYSTEM__)
		if (GetWear(WEAR_COSTUME_HAIR) && IsHairCostumeHidden() == true)
			return 0;
#endif
		return GetPart(PART_HAIR);

#if defined(__ACCE_COSTUME_SYSTEM__)
	case PART_ACCE:
#if defined(__HIDE_COSTUME_SYSTEM__)
		if (GetWear(WEAR_COSTUME_ACCE) && IsAcceCostumeHidden() == true)
			return 0;
#endif
		return GetPart(PART_ACCE);
#endif

#if defined(__WEAPON_COSTUME_SYSTEM__)
	case PART_WEAPON:
#if defined(__HIDE_COSTUME_SYSTEM__)
		if (GetWear(WEAR_COSTUME_WEAPON) && IsWeaponCostumeHidden() == true)
			if (const LPITEM pWeapon = GetWear(WEAR_WEAPON))
				return pWeapon->GetVnum();
#endif
		return GetWear(WEAR_COSTUME_WEAPON) ? GetPart(PART_WEAPON) : 0;
#endif

	case PART_AURA:
#if defined(__HIDE_COSTUME_SYSTEM__)
		if (GetWear(WEAR_COSTUME_AURA) && IsAuraCostumeHidden() == true)
			return 0;
#endif
		return GetPart(PART_AURA);

	default:
		return 0;
	}
}

BYTE CHARACTER::GetCharType() const
{
	return m_bCharType;
}

void CHARACTER::WonExchange(BYTE bOption, WORD wValue)
{
	if (!CanWarp())
		return;

	float fMul = (bOption == WON_EXCHANGE_CG_SUBHEADER_BUY) ? 1.0f + (static_cast<float>(TAX_MUL) / 100.0f) : 1.0f;
	WORD wVal = abs(wValue);
	if (bOption == WON_EXCHANGE_CG_SUBHEADER_SELL)
	{
		if (GetCheque() < wVal)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough Won."));
			return;
		}
		if (GetGold() + static_cast<long long>(wVal) * static_cast<long long>(CHEQUE_NAME_VALUE * fMul) > g_MaxGold)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have more than 2 Billion Yang with you. You cannot trade."));
			return;
		}

		PointChange(POINT_GOLD, static_cast<long long>(wVal) * static_cast<long long>(CHEQUE_NAME_VALUE * fMul), true);
		PointChange(POINT_CHEQUE, -wVal, true);
	}
	else if (bOption == WON_EXCHANGE_CG_SUBHEADER_BUY)
	{
		if (GetGold() < static_cast<long long>(wVal) * static_cast<long long>(CHEQUE_NAME_VALUE * fMul))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough Yang."));
			return;
		}
		if (GetCheque() + wVal > CHEQUE_MAX - 1)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have more than 999 Won with you. You cannot trade."));
			return;
		}

		PointChange(POINT_GOLD, -(static_cast<long long>(wVal) * static_cast<long long>(CHEQUE_NAME_VALUE * fMul)), true);
		PointChange(POINT_CHEQUE, wVal, true);
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("An error has occurred."));
	}
}

bool CHARACTER::SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList)
{
	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
		return false;
	// END_OF_TRENT_MONSTER

	if (ch)
	{
		if (!battle_is_attackable(ch, this))
		{
			SendDamagePacket(ch, 0, DAMAGE_BLOCK);
			return false;
		}
	}

	if (ch == this)
	{
		sys_err("SetSyncOwner owner == this (%p)", this);
		return false;
	}

	if (!ch)
	{
		if (bRemoveFromList && m_pkChrSyncOwner)
		{
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
		}

		if (m_pkChrSyncOwner)
			sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());

		// ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê´ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Í´ï¿½ NULLï¿½ï¿½ ï¿½ï¿½ï¿½ÃµÇ¾ï¿½ï¿½ ï¿½Ñ´ï¿½.
		m_pkChrSyncOwner = NULL;
	}
	else
	{
		if (!IsSyncOwner(ch))
			return false;

		// ï¿½Å¸ï¿½ï¿½ï¿½ 200 ï¿½Ì»ï¿½ï¿½Ì¸ï¿½ SyncOwnerï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½.
		if (DISTANCE_APPROX(GetX() - ch->GetX(), GetY() - ch->GetY()) > 250)
		{
			sys_log(1, "SetSyncOwner distance over than 250 %s %s", GetName(), ch->GetName());

			// SyncOwnerï¿½ï¿½ ï¿½ï¿½ï¿½ Ownerï¿½ï¿½ Ç¥ï¿½ï¿½ï¿½Ñ´ï¿½.
			if (m_pkChrSyncOwner == ch)
				return true;

			return false;
		}

		if (m_pkChrSyncOwner != ch)
		{
			if (m_pkChrSyncOwner)
			{
				sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());
				m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
			}

			m_pkChrSyncOwner = ch;
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.push_back(this);

			// SyncOwnerï¿½ï¿½ ï¿½Ù²ï¿½ï¿½ LastSyncTimeï¿½ï¿½ ï¿½Ê±ï¿½È­ï¿½Ñ´ï¿½.
			static const timeval zero_tv = { 0, 0 };
			SetLastSyncTime(zero_tv);

			sys_log(1, "SetSyncOwner set %s %p to %s", GetName(), this, ch->GetName());
		}

		m_fSyncTime = get_float_time();
	}

	// TODO: Sync Ownerï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ ï¿½ï¿½Å¶ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ç·ï¿½,
	// ï¿½ï¿½ï¿½ï¿½È­ ï¿½ï¿½ ï¿½Ã°ï¿½ï¿½ï¿½ 3ï¿½ï¿½ ï¿½Ì»ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ Ç®ï¿½ï¿½ï¿½Ö´ï¿½ ï¿½ï¿½Å¶ï¿½ï¿½
	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ï¸ï¿½ ï¿½ï¿½Å¶ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Ö´ï¿½.
	TPacketGCOwnership pack;

	pack.bHeader = HEADER_GC_OWNERSHIP;
	pack.dwOwnerVID = ch ? ch->GetVID() : 0;
	pack.dwVictimVID = GetVID();

	PacketAround(&pack, sizeof(TPacketGCOwnership));
	return true;
}

struct FuncClearSync
{
	void operator () (LPCHARACTER ch)
	{
		assert(ch != NULL);
		ch->SetSyncOwner(NULL, false); // false ï¿½Ã·ï¿½ï¿½×·ï¿½ ï¿½Ø¾ï¿½ for_each ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½.
	}
};

void CHARACTER::ClearSync()
{
	SetSyncOwner(NULL);

	// In the below for_each, the pointers of those who have me as m_pkChrSyncOwner are set to NULL.
	std::for_each(m_kLst_pkChrSyncOwned.begin(), m_kLst_pkChrSyncOwned.end(), FuncClearSync());
	m_kLst_pkChrSyncOwned.clear();
}

bool CHARACTER::IsSyncOwner(LPCHARACTER ch) const
{
	if (m_pkChrSyncOwner == ch)
		return true;

	// If the last sync time has passed more than 3 seconds, no one has ownership
	// none. So anyone is a SyncOwner, so it returns true
	if (get_float_time() - m_fSyncTime >= 3.0f)
		return true;

	return false;
}

void CHARACTER::SetParty(LPPARTY pkParty)
{
	if (pkParty == m_pkParty)
		return;

	if (pkParty && m_pkParty)
		sys_err("%s is trying to reassigning party (current %p, new party %p)", GetName(), get_pointer(m_pkParty), get_pointer(pkParty));

	sys_log(1, "PARTY set to %p", get_pointer(pkParty));

	//if (m_pkDungeon && IsPC())
	//	SetDungeon(NULL);

#ifdef ENABLE_12ZI
	if (m_pkZodiac && IsPC() && !pkParty)
		SetZodiac(nullptr);
#endif

	m_pkParty = pkParty;

	if (IsPC())
	{
		if (m_pkParty)
			SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);
		else
			REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);

		UpdatePacket();
	}
}

// PARTY_JOIN_BUG_FIX
/// ï¿½ï¿½Æ¼ ï¿½ï¿½ï¿½ï¿½ event ï¿½ï¿½ï¿½ï¿½
EVENTINFO(TPartyJoinEventInfo)
{
	DWORD dwGuestPID; ///< ï¿½ï¿½Æ¼ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Ä³ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ PID
	DWORD dwLeaderPID; ///< ï¿½ï¿½Æ¼ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ PID

	TPartyJoinEventInfo()
		: dwGuestPID(0)
		, dwLeaderPID(0)
	{
	}
};

EVENTFUNC(party_request_event)
{
	TPartyJoinEventInfo* info = dynamic_cast<TPartyJoinEventInfo*>(event->info);

	if (info == NULL)
	{
		sys_err("party_request_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(info->dwGuestPID);

	if (ch)
	{
		sys_log(0, "PartyRequestEvent %s", ch->GetName());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
		ch->SetPartyRequestEvent(NULL);
	}

	return 0;
}

bool CHARACTER::RequestToParty(LPCHARACTER leader)
{
	if (leader->GetParty())
		leader = leader->GetParty()->GetLeaderCharacter();

	if (!leader)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot make a request because the group leader is not online."));
		return false;
	}

	if (m_pkPartyRequestEvent)
		return false;

	if (!IsPC() || !leader->IsPC())
		return false;

	if (leader->IsBlockMode(BLOCK_PARTY_REQUEST))
		return false;

	PartyJoinErrCode errcode = IsPartyJoinableCondition(leader, this);

	switch (errcode)
	{
	case PERR_NONE:
		break;

	case PERR_SERVER:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The server cannot execute this group request."));
		return false;

	case PERR_DIFFEMPIRE:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot form a group with players from another kingdom."));
		return false;

	case PERR_DUNGEON:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite players while you are in a dungeon."));
		return false;

	case PERR_OBSERVER:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite a player while you are in observer mode."));
		return false;

	case PERR_LVBOUNDARY:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] Only players with a level difference of -30 to +30 can be invited."));
		return false;

	case PERR_LOWLEVEL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too low."));
		return false;

	case PERR_HILEVEL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too high."));
		return false;

	case PERR_ALREADYJOIN:
		return false;

	case PERR_PARTYISFULL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite any more players into your group."));
		return false;

	default:
		sys_err("Do not process party join error(%d)", errcode);
		return false;
	}

	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = GetPlayerID();
	info->dwLeaderPID = leader->GetPlayerID();

	SetPartyRequestEvent(event_create(party_request_event, info, PASSES_PER_SEC(10)));

#if defined(__WJ_NEW_USER_CARE__)
	leader->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequest %u %s", (DWORD)GetVID(), GetName());
#else
	leader->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequest %u", (DWORD)GetVID());
#endif
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You want to join %s's group."), leader->GetName());
	return true;
}

void CHARACTER::DenyToParty(LPCHARACTER member)
{
	sys_log(1, "DenyToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo* info = dynamic_cast<TPartyJoinEventInfo*>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err("CHARACTER::DenyToParty> <Factor> Null pointer");
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

void CHARACTER::AcceptToParty(LPCHARACTER member)
{
	sys_log(1, "AcceptToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo* info = dynamic_cast<TPartyJoinEventInfo*>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err("CHARACTER::AcceptToParty> <Factor> Null pointer");
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	if (!GetParty())
		member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This player is not in this group."));
	else
	{
		if (GetPlayerID() != GetParty()->GetLeaderPID())
			return;

		PartyJoinErrCode errcode = IsPartyJoinableCondition(this, member);
		switch (errcode)
		{
		case PERR_NONE: member->PartyJoin(this); return;
		case PERR_SERVER: member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The server cannot execute this group request.")); break;
		case PERR_DUNGEON: member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite players while you are in a dungeon.")); break;
		case PERR_OBSERVER: member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite a player while you are in observer mode.")); break;
		case PERR_LVBOUNDARY: member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] Only players with a level difference of -30 to +30 can be invited.")); break;
		case PERR_LOWLEVEL: member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too low.")); break;
		case PERR_HILEVEL: member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too high.")); break;
		case PERR_ALREADYJOIN: break;
		case PERR_PARTYISFULL: {
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite any more players into your group."));
			member->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot join this group because it is already full."));
			break;
		}
		default: sys_err("Do not process party join error(%d)", errcode);
		}
	}

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

/**
* ï¿½ï¿½Æ¼ ï¿½Ê´ï¿½ event callback ï¿½Ô¼ï¿½.
* event ï¿½ï¿½ ï¿½ßµï¿½ï¿½Ï¸ï¿½ ï¿½Ê´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Ã³ï¿½ï¿½ï¿½Ñ´ï¿½.
**/
EVENTFUNC(party_invite_event)
{
	TPartyJoinEventInfo* pInfo = dynamic_cast<TPartyJoinEventInfo*>(event->info);

	if (pInfo == NULL)
	{
		sys_err("party_invite_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER pchInviter = CHARACTER_MANAGER::instance().FindByPID(pInfo->dwLeaderPID);

	if (pchInviter)
	{
		sys_log(1, "PartyInviteEvent %s", pchInviter->GetName());
		pchInviter->PartyInviteDeny(pInfo->dwGuestPID);
	}

	return 0;
}

void CHARACTER::PartyInvite(LPCHARACTER pchInvitee)
{
	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You do not have the right to invite someone."));
		return;
	}
	else if (pchInvitee->IsBlockMode(BLOCK_PARTY_INVITE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] %s has declined your group invitation."), pchInvitee->GetName());
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableCondition(this, pchInvitee);

	switch (errcode)
	{
	case PERR_NONE:
		break;

	case PERR_SERVER:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The server cannot execute this group request."));
		return;

	case PERR_DIFFEMPIRE:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot form a group with players from another kingdom."));
		return;

	case PERR_DUNGEON:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite players while you are in a dungeon."));
		return;

	case PERR_OBSERVER:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite a player while you are in observer mode."));
		return;

	case PERR_LVBOUNDARY:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] Only players with a level difference of -30 to +30 can be invited."));
		return;

	case PERR_LOWLEVEL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too low."));
		return;

	case PERR_HILEVEL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too high."));
		return;

	case PERR_ALREADYJOIN:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] %s is already in the group."), pchInvitee->GetName());
		return;

	case PERR_PARTYISFULL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite any more players into your group."));
		return;

	default:
		sys_err("Do not process party join error(%d)", errcode);
		return;
	}

	if (m_PartyInviteEventMap.end() != m_PartyInviteEventMap.find(pchInvitee->GetPlayerID()))
		return;

	//
	// EventMap ï¿½ï¿½ ï¿½Ìºï¿½Æ® ï¿½ß°ï¿½
	//
	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = pchInvitee->GetPlayerID();
	info->dwLeaderPID = GetPlayerID();

	m_PartyInviteEventMap.insert(EventMap::value_type(pchInvitee->GetPlayerID(), event_create(party_invite_event, info, PASSES_PER_SEC(10))));

	//
	// ï¿½Ê´ï¿½ ï¿½Þ´ï¿½ character ï¿½ï¿½ï¿½ï¿½ ï¿½Ê´ï¿½ ï¿½ï¿½Å¶ ï¿½ï¿½ï¿½ï¿½
	//

	TPacketGCPartyInvite p;
	p.header = HEADER_GC_PARTY_INVITE;
	p.leader_vid = GetVID();
	pchInvitee->GetDesc()->Packet(&p, sizeof(p));
}

void CHARACTER::PartyInviteAccept(LPCHARACTER pchInvitee)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(pchInvitee->GetPlayerID());

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteAccept from not invited character(%s)", pchInvitee->GetName());
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You do not have the right to invite someone."));
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableMutableCondition(this, pchInvitee);

	switch (errcode)
	{
	case PERR_NONE:
		break;

	case PERR_SERVER:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The server cannot execute this group request."));
		return;

	case PERR_DUNGEON:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot accept an invitation into a dungeon."));
		return;

	case PERR_OBSERVER:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite a player while you are in observer mode."));
		return;

	case PERR_LVBOUNDARY:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] Only players with a level difference of -30 to +30 can be invited."));
		return;

	case PERR_LOWLEVEL:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too low."));
		return;

	case PERR_HILEVEL:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite this player, as their level is too high."));
		return;

	case PERR_ALREADYJOIN:
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot accept the invitation."));
		return;

	case PERR_PARTYISFULL:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot invite any more players into your group."));
		pchInvitee->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot join this group because it is already full."));
		return;

	default:
		sys_err("ignore party join error(%d)", errcode);
		return;
	}

	//
	// ï¿½ï¿½Æ¼ ï¿½ï¿½ï¿½ï¿½ Ã³ï¿½ï¿½
	//

	if (GetParty())
		pchInvitee->PartyJoin(this);
	else
	{
		LPPARTY pParty = CPartyManager::instance().CreateParty(this);

		pParty->Join(pchInvitee->GetPlayerID());
		pParty->Link(pchInvitee);
		pParty->SendPartyInfoAllToOne(this);
	}
}

void CHARACTER::PartyInviteDeny(DWORD dwPID)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(dwPID);

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteDeny to not exist event(inviter PID: %d, invitee PID: %d)", GetPlayerID(), dwPID);
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	LPCHARACTER pchInvitee = CHARACTER_MANAGER::instance().FindByPID(dwPID);
	if (pchInvitee)
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] %s has declined your invitation."), pchInvitee->GetName());
}

void CHARACTER::PartyJoin(LPCHARACTER pLeader)
{
	pLeader->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] %s has joined your group."), GetName());
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] %s has joined your Group._2"), pLeader->GetName());

	pLeader->GetParty()->Join(GetPlayerID());
	pLeader->GetParty()->Link(this);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (pchLeader->GetEmpire() != pchGuest->GetEmpire())
		return PERR_DIFFEMPIRE;

	return IsPartyJoinableMutableCondition(pchLeader, pchGuest);
}

static bool __party_can_join_by_level(LPCHARACTER leader, LPCHARACTER quest)
{
	int level_limit = 30;

	if (LC_IsCanada())
		level_limit = 15;
	else if (LC_IsBrazil() == true)
	{
		level_limit = 10;
	}
	else
		level_limit = 30;

	return (abs(leader->GetLevel() - quest->GetLevel()) <= level_limit);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (!CPartyManager::instance().IsEnablePCParty())
		return PERR_SERVER;
	else if (pchLeader->GetDungeon())
		return PERR_DUNGEON;
	else if (pchGuest->IsObserverMode())
		return PERR_OBSERVER;
	else if (false == __party_can_join_by_level(pchLeader, pchGuest))
		return PERR_LVBOUNDARY;
	else if (pchGuest->GetParty())
		return PERR_ALREADYJOIN;
	else if (pchLeader->GetParty())
	{
		if (pchLeader->GetParty()->GetMemberCount() == PARTY_MAX_MEMBER)
			return PERR_PARTYISFULL;
	}

	return PERR_NONE;
}
// END_OF_PARTY_JOIN_BUG_FIX

void CHARACTER::SetDungeon(LPDUNGEON pkDungeon)
{
	if (pkDungeon && m_pkDungeon)
		sys_err("%s is trying to reassigning dungeon (current %p, new party %p)", GetName(), get_pointer(m_pkDungeon), get_pointer(pkDungeon));

	if (m_pkDungeon == pkDungeon)
		return;

	if (m_pkDungeon)
	{
		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->DecPartyMember(GetParty(), this);
			else
				m_pkDungeon->DecMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->DecMonster();
		}
	}

	m_pkDungeon = pkDungeon;

	if (pkDungeon)
	{
		sys_log(0, "%s DUNGEON set to %p, PARTY is %p", GetName(), get_pointer(pkDungeon), get_pointer(m_pkParty));

		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->IncPartyMember(GetParty(), this);
			else
				m_pkDungeon->IncMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->IncMonster();
		}
	}
}

#ifdef ENABLE_12ZI
void CHARACTER::SetZodiac(LPZODIAC pkZodiac)
{
	if (pkZodiac && m_pkZodiac)
		sys_err("%s is trying to reassigning zodiac (current %p, new party %p)", GetName(), get_pointer(m_pkZodiac), get_pointer(pkZodiac));

	if (m_pkZodiac == pkZodiac)
		return;

	if (m_pkZodiac)
	{
		if (IsPC())
		{
			if (GetParty())
				m_pkZodiac->DecPartyMember(GetParty(), this);
			else
				m_pkZodiac->DecMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkZodiac->DecMonster();
		}
	}

	m_pkZodiac = pkZodiac;

	if (pkZodiac)
	{
		sys_log(0, "%s ZODIAC set to %p, PARTY is %p", GetName(), get_pointer(pkZodiac), get_pointer(m_pkParty));

		if (IsPC())
		{
			if (GetParty())
				m_pkZodiac->IncPartyMember(GetParty(), this);
			else
				m_pkZodiac->IncMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkZodiac->IncMonster();
		}
	}
}
#endif

void CHARACTER::SetWarMap(CWarMap* pWarMap)
{
	if (m_pWarMap)
		m_pWarMap->DecMember(this);

	m_pWarMap = pWarMap;

	if (m_pWarMap)
		m_pWarMap->IncMember(this);
}

void CHARACTER::SetWeddingMap(marriage::WeddingMap* pMap)
{
	if (m_pWeddingMap)
		m_pWeddingMap->DecMember(this);

	m_pWeddingMap = pMap;

	if (m_pWeddingMap)
		m_pWeddingMap->IncMember(this);
}

bool CHARACTER::IsWearingDress() const
{
	LPITEM pkItem = GetWear(WEAR_BODY);
	if (pkItem && (pkItem->GetVnum() > 11900 && pkItem->GetVnum() < 11915))
		return true;
	return false;
}

void CHARACTER::SetRegen(LPREGEN pkRegen)
{
	m_pkRegen = pkRegen;
	if (pkRegen != NULL)
	{
		regen_id_ = pkRegen->id;
	}
	m_fRegenAngle = GetRotation();
	m_posRegen = GetXYZ();
}

bool CHARACTER::OnIdle()
{
	return false;
}

void CHARACTER::OnMove(bool bIsAttack)
{
	m_dwLastMoveTime = get_dword_time();

	if (bIsAttack)
	{
		m_dwLastAttackTime = m_dwLastMoveTime;

		if (IsAffectFlag(AFF_REVIVE_INVISIBLE))
			RemoveAffect(AFFECT_REVIVE_INVISIBLE);

		if (IsAffectFlag(AFF_EUNHYUNG))
		{
			RemoveAffect(SKILL_EUNHYUNG);
			SetAffectedEunhyung();
		}
		else
		{
			ClearAffectedEunhyung();
		}

		/*
		if (IsAffectFlag(AFF_JEONSIN))
			RemoveAffect(SKILL_JEONSINBANGEO);
		*/
	}

	/*
	if (IsAffectFlag(AFF_GUNGON))
		RemoveAffect(SKILL_GUNGON);
	*/

	// MINING
	mining_cancel();
	// END_OF_MINING
}

void CHARACTER::OnClick(LPCHARACTER pkChrCauser)
{
	if (!pkChrCauser)
	{
		sys_err("OnClick %s by NULL", GetName());
		return;
	}

	DWORD vid = GetVID();
	sys_log(0, "OnClick %s[vnum %d ServerUniqueID %d, pid %d] by %s", GetName(), GetRaceNum(), vid, GetPlayerID(), pkChrCauser->GetName());

#ifdef ENABLE_12ZI
	if (pkChrCauser != this && IsPC() && pkChrCauser->IsPC() && IsPolymorphed() && (GetMapIndex() >= 3550000 && GetMapIndex() < 3560000) && (pkChrCauser->GetMapIndex() >= 3550000 && pkChrCauser->GetMapIndex() < 3560000))
	{
		if (GetPolymorphVnum() >= 20452 && GetPolymorphVnum() <= 20463)
		{
			SetPolymorph(0);
			return;
		}
	}
#endif

#ifdef ENABLE_QUEEN_NETHIS
	if ((IsNPC()) && (GetRaceNum() == (WORD)(SnakeLair::PORTAL_VNUM)) && (SnakeLair::CSnk::instance().IsSnakeMap(pkChrCauser->GetMapIndex())))
	{
		SnakeLair::CSnk::instance().Start(pkChrCauser);
		return;
	}
#endif

	if (mining::IsVeinOfOre(this->GetRaceNum()))
	{
		pkChrCauser->mining(this);
		return;
	}

#if defined(__GUILD_DRAGONLAIR__)
	if ((IsNPC()) && (GetRaceNum() == (WORD)(MeleyLair::GATE_VNUM)) && (MeleyLair::CMgr::instance().IsMeleyMap(pkChrCauser->GetMapIndex())))
	{
		MeleyLair::CMgr::instance().Start(pkChrCauser, pkChrCauser->GetGuild());
		return;
	}
#endif

	// ï¿½ï¿½È¯ï¿½ï¿½ï¿½Ï¶ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½.
	{
		if (pkChrCauser->GetExchange())
		{
			if (test_server)
				sys_err("OnClick Fail (%s->%s) - pc is exchanging", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Â·ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½.
	{
		// ï¿½ï¿½, ï¿½Ú½ï¿½ï¿½ï¿½ ï¿½Ú½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Å¬ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Ö´ï¿½.
		if (pkChrCauser->GetMyShop() && pkChrCauser != this)
		{
			if (test_server)
				sys_err("OnClick Fail (%s->%s) - pc has shop", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	if (IsPC())
	{
		// Å¸ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ PCï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ Å¬ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ Ã³ï¿½ï¿½ï¿½Ïµï¿½ï¿½ï¿½ ï¿½Õ´Ï´ï¿½.
		if (!CTargetManager::instance().GetTargetInfo(pkChrCauser->GetPlayerID(), TARGET_TYPE_VID, GetVID()))
		{
			// 2005.03.17.myevan.Å¸ï¿½ï¿½ï¿½ï¿½ ï¿½Æ´ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ Ã³ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ûµï¿½ï¿½ï¿½Å²ï¿½ï¿½.
			if (GetMyShop())
			{
				if (pkChrCauser->IsDead() == true) return;

				// PREVENT_TRADE_WINDOW
				if (pkChrCauser == this) // ï¿½Ú±ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
				{
					if (PreventTradeWindow(WND_MYSHOP, true/*except*/))
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use a private shop now."));
						return;
					}
				}
				else // ï¿½Ù¸ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ Å¬ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
				{
					// Å¬ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½È¯/Ã¢ï¿½ï¿½/ï¿½ï¿½ï¿½Î»ï¿½ï¿½ï¿½/ï¿½ï¿½ï¿½ï¿½ï¿½Ì¿ï¿½ï¿½ï¿½ï¿½Ì¶ï¿½ï¿½ ï¿½Ò°ï¿½
					if (pkChrCauser->PreventTradeWindow(WND_ALL))
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't use a private shop now."));
						return;
					}

					// Å¬ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½È¯/Ã¢ï¿½ï¿½/ï¿½ï¿½ï¿½ï¿½ï¿½Ì¿ï¿½ï¿½ï¿½ï¿½Ì¶ï¿½ï¿½ ï¿½Ò°ï¿½
					//if ((GetExchange() || IsOpenSafebox() || GetShopOwner()))
					if (PreventTradeWindow(WND_MYSHOP | WND_SHOPOWNER, true/*except*/))
					{
						pkChrCauser->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This player is already trading with another player."));
						return;
					}
				}
				// END_PREVENT_TRADE_WINDOW

				if (pkChrCauser->GetShop())
				{
					pkChrCauser->GetShop()->RemoveGuest(pkChrCauser);
					pkChrCauser->SetShop(NULL);
				}

				GetMyShop()->AddGuest(pkChrCauser, GetVID(), false);
				pkChrCauser->SetShopOwner(this);
#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
				if (CTargetManager::instance().GetTargetInfo(pkChrCauser->GetPlayerID(), TARGET_TYPE_VID_SHOP_SEARCH, static_cast<DWORD>(GetVID())))
					CTargetManager::instance().DeleteTarget(pkChrCauser->GetPlayerID(), SHOP_SEARCH_INDEX, "__SHOPSEARCH_TARGET__");
#endif
				return;
			}

			//if (test_server)
			//	sys_err("%s.OnClickFailure(%s) - target is PC", pkChrCauser->GetName(), GetName());

			return;
		}
	}

	pkChrCauser->SetQuestNPCID(GetVID());

	if (quest::CQuestManager::instance().Click(pkChrCauser->GetPlayerID(), this))
	{
		return;
	}

	// NPC ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ : ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
	if (!IsPC())
	{
		if (!m_triggerOnClick.pFunc)
		{
			// NPC Æ®ï¿½ï¿½ï¿½ï¿½ ï¿½Ã½ï¿½ï¿½ï¿½ ï¿½Î±ï¿½ ï¿½ï¿½ï¿½ï¿½
			/*
			sys_err("%s.OnClickFailure(%s) : triggerOnClick.pFunc is EMPTY(pid=%d)",
				pkChrCauser->GetName(),
				GetName(),
				pkChrCauser->GetPlayerID());
			*/
			return;
		}

		m_triggerOnClick.pFunc(this, pkChrCauser);
	}
}

BYTE CHARACTER::GetGMLevel() const
{
	if (test_server)
		return GM_IMPLEMENTOR;
	return m_pointsInstant.gm_level;
}

void CHARACTER::SetGMLevel()
{
	if (GetDesc())
	{
		BYTE myLevel = gm_get_level(GetName(), GetDesc()->GetHostName(), GetDesc()->GetAccountTable().login);
		m_pointsInstant.gm_level = myLevel;
	}
	else
	{
		m_pointsInstant.gm_level = GM_PLAYER;
	}
}

BOOL CHARACTER::IsGM() const
{
	if (test_server)
		return true;

	if (GetGMLevel() != GM_PLAYER)
		return true;

	return false;
}

void CHARACTER::SetStone(LPCHARACTER pkChrStone)
{
	m_pkChrStone = pkChrStone;

	if (m_pkChrStone)
	{
		/*#ifdef ENABLE_12ZI
				if (GetZodiac())
					m_pkFromStone = true;
		#endif*/
		if (pkChrStone->m_set_pkChrSpawnedBy.find(this) == pkChrStone->m_set_pkChrSpawnedBy.end())
			pkChrStone->m_set_pkChrSpawnedBy.insert(this);
	}
}

struct FuncDeadSpawnedByStone
{
	void operator () (LPCHARACTER ch)
	{
		ch->Dead(NULL);
		ch->SetStone(NULL);
	}
};

void CHARACTER::ClearStone()
{
	if (!m_set_pkChrSpawnedBy.empty())
	{
		// ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Å² ï¿½ï¿½ï¿½Íµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Î´ï¿½.
		FuncDeadSpawnedByStone f;
		std::for_each(m_set_pkChrSpawnedBy.begin(), m_set_pkChrSpawnedBy.end(), f);
		m_set_pkChrSpawnedBy.clear();
	}

	if (!m_pkChrStone)
		return;

	m_pkChrStone->m_set_pkChrSpawnedBy.erase(this);
	m_pkChrStone = NULL;
}

void CHARACTER::ClearTarget()
{
	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
		m_pkChrTarget = NULL;
	}

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;
	p.dwVID = 0;
	p.bHPPercent = 0;
#if defined(__VIEW_TARGET_DECIMAL_HP__)
	p.iMinHP = 0;
	p.iMaxHP = 0;
#endif
#if defined(__ELEMENT_SYSTEM__)
	memset(&p.bElement, 0, sizeof(p.bElement));
#endif
#if defined(__SHIP_DEFENSE__)
	p.bAlliance = false;
#endif

	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();

	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *(it++);
		pkChr->m_pkChrTarget = NULL;

		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}

		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}

	m_set_pkChrTargetedBy.clear();
}

void CHARACTER::SetTarget(LPCHARACTER pkChrTarget)
{
	if (m_pkChrTarget == pkChrTarget)
		return;

	// CASTLE
	if (IS_CASTLE_MAP(GetMapIndex()) && !IsGM())
		return;
	// CASTLE

	if (m_pkChrTarget)
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);

	m_pkChrTarget = pkChrTarget;

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;

	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.insert(this);

		p.dwVID = m_pkChrTarget->GetVID();

#if defined(__VIEW_TARGET_PLAYER_HP__)
		if ((m_pkChrTarget->GetMaxHP() <= 0))
		{
			p.bHPPercent = 0;
#if defined(__VIEW_TARGET_DECIMAL_HP__)
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
		}
		else if (m_pkChrTarget->IsPC() && !m_pkChrTarget->IsPolymorphed())
		{
			p.bHPPercent = MINMAX(0, (m_pkChrTarget->GetHP() * 100) / m_pkChrTarget->GetMaxHP(), 100);
#if defined(__VIEW_TARGET_DECIMAL_HP__)
			p.iMinHP = m_pkChrTarget->GetHP();
			p.iMaxHP = m_pkChrTarget->GetMaxHP();
#endif
		}
#else
		if ((m_pkChrTarget->IsPC() && !m_pkChrTarget->IsPolymorphed()) || (m_pkChrTarget->GetMaxHP() <= 0))
		{
			p.bHPPercent = 0;
#if defined(__VIEW_TARGET_DECIMAL_HP__)
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
		}
#endif
		else
		{
			if (m_pkChrTarget->GetRaceNum() == 20101 ||
				m_pkChrTarget->GetRaceNum() == 20102 ||
				m_pkChrTarget->GetRaceNum() == 20103 ||
				m_pkChrTarget->GetRaceNum() == 20104 ||
				m_pkChrTarget->GetRaceNum() == 20105 ||
				m_pkChrTarget->GetRaceNum() == 20106 ||
				m_pkChrTarget->GetRaceNum() == 20107 ||
				m_pkChrTarget->GetRaceNum() == 20108 ||
				m_pkChrTarget->GetRaceNum() == 20109
#if defined(__ENABLE_RIDING_EXTENDED__)
				|| m_pkChrTarget->GetRaceNum() == 20149
				|| m_pkChrTarget->GetRaceNum() == 20150
				|| m_pkChrTarget->GetRaceNum() == 20151
#endif
				)
			{
				LPCHARACTER owner = m_pkChrTarget->GetVictim();

				if (owner)
				{
					int iHorseHealth = owner->GetHorseHealth();
					int iHorseMaxHealth = owner->GetHorseMaxHealth();

					if (iHorseMaxHealth)
					{
						p.bHPPercent = MINMAX(0, iHorseHealth * 100 / iHorseMaxHealth, 100);
#if defined(__VIEW_TARGET_DECIMAL_HP__)
						p.iMinHP = 100;
						p.iMaxHP = 100;
#endif
					}
					else
					{
						p.bHPPercent = 100;
#if defined(__VIEW_TARGET_DECIMAL_HP__)
						p.iMinHP = 100;
						p.iMaxHP = 100;
#endif
					}
				}
				else
				{
					p.bHPPercent = 100;
#if defined(__VIEW_TARGET_DECIMAL_HP__)
					p.iMinHP = 100;
					p.iMaxHP = 100;
#endif
				}
			}
			else
			{
				if (m_pkChrTarget->GetMaxHP() <= 0)
				{
					p.bHPPercent = 0;
#if defined(__VIEW_TARGET_DECIMAL_HP__)
					p.iMinHP = 0;
					p.iMaxHP = 0;
#endif
				}
				else
				{
					p.bHPPercent = MINMAX(0, (m_pkChrTarget->GetHP() * 100) / m_pkChrTarget->GetMaxHP(), 100);
#if defined(__VIEW_TARGET_DECIMAL_HP__)
					p.iMinHP = m_pkChrTarget->GetHP();
					p.iMaxHP = m_pkChrTarget->GetMaxHP();
#endif
				}
			}
		}
	}
	else
	{
		p.dwVID = 0;
		p.bHPPercent = 0;
#if defined(__VIEW_TARGET_DECIMAL_HP__)
		p.iMinHP = 0;
		p.iMaxHP = 0;
#endif
	}

#if defined(__ELEMENT_SYSTEM__)
	memset(&p.bElement, 0, sizeof(p.bElement));
	if (m_pkChrTarget)
	{
		if (m_pkChrTarget->IsPC())
		{
			p.bElement[MOB_ELEMENT_ELECT] = m_pkChrTarget->GetPoint(POINT_ENCHANT_ELECT);
			p.bElement[MOB_ELEMENT_FIRE] = m_pkChrTarget->GetPoint(POINT_ENCHANT_FIRE);
			p.bElement[MOB_ELEMENT_ICE] = m_pkChrTarget->GetPoint(POINT_ENCHANT_ICE);
			p.bElement[MOB_ELEMENT_WIND] = m_pkChrTarget->GetPoint(POINT_ENCHANT_WIND);
			p.bElement[MOB_ELEMENT_EARTH] = m_pkChrTarget->GetPoint(POINT_ENCHANT_EARTH);
			p.bElement[MOB_ELEMENT_DARK] = m_pkChrTarget->GetPoint(POINT_ENCHANT_DARK);
		}
		else
		{
			p.bElement[MOB_ELEMENT_ELECT] = m_pkChrTarget->GetMobElement(MOB_ELEMENT_ELECT);
			p.bElement[MOB_ELEMENT_FIRE] = m_pkChrTarget->GetMobElement(MOB_ELEMENT_FIRE);
			p.bElement[MOB_ELEMENT_ICE] = m_pkChrTarget->GetMobElement(MOB_ELEMENT_ICE);
			p.bElement[MOB_ELEMENT_WIND] = m_pkChrTarget->GetMobElement(MOB_ELEMENT_WIND);
			p.bElement[MOB_ELEMENT_EARTH] = m_pkChrTarget->GetMobElement(MOB_ELEMENT_EARTH);
			p.bElement[MOB_ELEMENT_DARK] = m_pkChrTarget->GetMobElement(MOB_ELEMENT_DARK);
		}
	}
#endif

#if defined(__SHIP_DEFENSE__)
	p.bAlliance = false;
#endif

	GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
}

void CHARACTER::BroadcastTargetPacket()
{
	if (m_set_pkChrTargetedBy.empty())
		return;

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;
	p.dwVID = GetVID();

	if (GetMaxHP() <= 0)
	{
		p.bHPPercent = 0;
#if defined(__VIEW_TARGET_DECIMAL_HP__)
		p.iMinHP = 0;
		p.iMaxHP = 0;
#endif
	}
	else
	{
#if defined(__VIEW_TARGET_PLAYER_HP__)
		p.bHPPercent = MINMAX(0, (GetHP() * 100) / GetMaxHP(), 100);
#if defined(__VIEW_TARGET_DECIMAL_HP__)
		p.iMinHP = GetHP();
		p.iMaxHP = GetMaxHP();
#endif
#else
		if (IsPC())
		{
			p.bHPPercent = 0;
#if defined(__VIEW_TARGET_DECIMAL_HP__)
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
		}
		else
		{
			p.bHPPercent = MINMAX(0, (GetHP() * 100) / GetMaxHP(), 100);
#if defined(__VIEW_TARGET_DECIMAL_HP__)
			p.iMinHP = GetHP();
			p.iMaxHP = GetMaxHP();
#endif
		}
#endif
	}

#if defined(__ELEMENT_SYSTEM__)
	memset(&p.bElement, 0, sizeof(p.bElement));

	if (IsPC())
	{
		p.bElement[MOB_ELEMENT_ELECT] = GetPoint(POINT_ENCHANT_ELECT);
		p.bElement[MOB_ELEMENT_FIRE] = GetPoint(POINT_ENCHANT_FIRE);
		p.bElement[MOB_ELEMENT_ICE] = GetPoint(POINT_ENCHANT_ICE);
		p.bElement[MOB_ELEMENT_WIND] = GetPoint(POINT_ENCHANT_WIND);
		p.bElement[MOB_ELEMENT_EARTH] = GetPoint(POINT_ENCHANT_EARTH);
		p.bElement[MOB_ELEMENT_DARK] = GetPoint(POINT_ENCHANT_DARK);
	}
	else
	{
		p.bElement[MOB_ELEMENT_ELECT] = GetMobElement(MOB_ELEMENT_ELECT);
		p.bElement[MOB_ELEMENT_FIRE] = GetMobElement(MOB_ELEMENT_FIRE);
		p.bElement[MOB_ELEMENT_ICE] = GetMobElement(MOB_ELEMENT_ICE);
		p.bElement[MOB_ELEMENT_WIND] = GetMobElement(MOB_ELEMENT_WIND);
		p.bElement[MOB_ELEMENT_EARTH] = GetMobElement(MOB_ELEMENT_EARTH);
		p.bElement[MOB_ELEMENT_DARK] = GetMobElement(MOB_ELEMENT_DARK);
	}
#endif

#if defined(__SHIP_DEFENSE__)
	p.bAlliance = false;
#endif

	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();

	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *it++;

		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}

		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}
}

void CHARACTER::CheckTarget()
{
	if (!m_pkChrTarget)
		return;

	if (DISTANCE_APPROX(GetX() - m_pkChrTarget->GetX(), GetY() - m_pkChrTarget->GetY()) >= 4800)
		SetTarget(NULL);
}

void CHARACTER::SetWarpLocation(long lMapIndex, long x, long y)
{
	m_posWarp.x = x * 100;
	m_posWarp.y = y * 100;
	m_lWarpMapIndex = lMapIndex;
}

void CHARACTER::SaveExitLocation()
{
	m_posExit = GetXYZ();
	m_lExitMapIndex = GetMapIndex();
}

void CHARACTER::ExitToSavedLocation()
{
	sys_log(0, "ExitToSavedLocation");
	WarpSet(m_posWarp.x, m_posWarp.y, m_lWarpMapIndex);

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;
}

// fixme
// ï¿½ï¿½ï¿½Ý±ï¿½ï¿½ï¿½ privateMapIndex ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Îµï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Ã¼Å© ï¿½Ï´ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ÜºÎ¿ï¿½ï¿½ï¿½ ï¿½Ï°ï¿½,
// ï¿½Ù¸ï¿½ï¿½ï¿½ warpsetï¿½ï¿½ ï¿½Ò·ï¿½ï¿½Âµï¿½
// ï¿½Ì¸ï¿½ warpset ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½.
bool CHARACTER::WarpSet(long x, long y, long lPrivateMapIndex)
{
	if (!IsPC())
		return false;

	if (IsWarping())
		return false;

	long lAddr;
	long lMapIndex;
	WORD wPort;

	if (!CMapLocation::instance().Get(x, y, lMapIndex, lAddr, wPort))
	{
		sys_err("cannot find map location index %d x %d y %d name %s", lMapIndex, x, y, GetName());
		return false;
	}

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
	if (GetGuild() && IsOpenGuildstorage())
		CloseGuildstorage();
#endif

	// Send Supplementary Data Block if new map requires security packages in loading this map
	{
		long lCurAddr;
		long lCurMapIndex = 0;
		WORD wCurPort;

		CMapLocation::instance().Get(GetX(), GetY(), lCurMapIndex, lCurAddr, wCurPort);

		// Do not send SDB files if char is in the same map
		if (lCurMapIndex != lMapIndex)
		{
			const TMapRegion* rMapRgn = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);
			{
				DESC_MANAGER::instance().SendClientPackageSDBToLoadMap(GetDesc(), rMapRgn->strMapName.c_str());
			}
		}
	}

	if (lPrivateMapIndex >= 10000)
	{
		if (lPrivateMapIndex / 10000 != lMapIndex)
		{
			sys_err("Invalid map index %d, must be child of %d", lPrivateMapIndex, lMapIndex);
			return false;
		}

		lMapIndex = lPrivateMapIndex;
	}

#if defined(__SORT_INVENTORY_ITEMS__)
	if (GetSortInventoryPulse() > get_global_time())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot warp after sorting your inventory."));
		return false;;
	}

#if defined(__SPECIAL_INVENTORY_SYSTEM__)
	if (GetSortSpecialInventoryPulse() > get_global_time())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot warp after sorting your inventory."));
		return false;;
	}
#endif
#endif


	m_bWarping = true;

	Stop();
	Save();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
		ViewCleanup();

		EncodeRemovePacket(this);
	}

	m_lWarpMapIndex = lMapIndex;
	m_posWarp.x = x;
	m_posWarp.y = y;

	sys_log(0, "WarpSet %s %d %d current map %d target map %d", GetName(), x, y, GetMapIndex(), lMapIndex);

	TPacketGCWarp p;

	p.bHeader = HEADER_GC_WARP;
	p.lX = x;
	p.lY = y;
	p.lAddr = lAddr;
	p.wPort = wPort;

#ifdef ENABLE_SWITCHBOT
	CSwitchbotManager::Instance().SetIsWarping(GetPlayerID(), true);

	if (p.wPort != mother_port)
	{
		CSwitchbotManager::Instance().P2PSendSwitchbot(GetPlayerID(), p.wPort);
	}
#endif

	//GetDesc()->Packet(&p, sizeof(TPacketGCWarp));

	char buf[256];
	snprintf(buf, sizeof(buf), "%s MapIdx %ld DestMapIdx%ld DestX%ld DestY%ld Empire%d", GetName(), GetMapIndex(), lPrivateMapIndex, x, y, GetEmpire());
	LogManager::instance().CharLog(this, 0, "WARP", buf);

	GetDesc()->WarpPacket(p);

	return true;
}

void CHARACTER::WarpEnd()
{
	if (test_server)
		sys_log(0, "WarpEnd %s", GetName());

	if (m_posWarp.x == 0 && m_posWarp.y == 0)
		return;

	int index = m_lWarpMapIndex;

	if (index > 10000)
		index /= 10000;

	if (!map_allow_find(index))
	{
		sys_err("location %d %d not allowed to login this server", m_posWarp.x, m_posWarp.y);
		if (g_bIgnoreDisallowedMap) //< 2020.08.07.Owsap - Go home if map index isn't allowed.
			GoHome();
		else
			GetDesc()->SetPhase(PHASE_CLOSE);

		return;
	}

	sys_log(0, "WarpEnd %s %d %u %u", GetName(), m_lWarpMapIndex, m_posWarp.x, m_posWarp.y);

	Show(m_lWarpMapIndex, m_posWarp.x, m_posWarp.y, 0);
	Stop();

	m_lWarpMapIndex = 0;
	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;

	{
		// P2P Login
		TPacketGGLogin p;

		p.bHeader = HEADER_GG_LOGIN;
		strlcpy(p.szName, GetName(), sizeof(p.szName));
		p.dwPID = GetPlayerID();
		p.bEmpire = GetEmpire();
		p.lMapIndex = SECTREE_MANAGER::instance().GetMapIndex(GetX(), GetY());
		p.bChannel = g_bChannel;
		p.bLanguage = GetLanguage();

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogin));
	}
}

bool CHARACTER::Return()
{
	if (!IsNPC())
		return false;

#ifdef ENABLE_12ZI
	if (IsZodiacBoss())
		return false;
#endif

	int x, y;
	/*
	float fDist = DISTANCE_SQRT(m_pkMobData->m_posLastAttacked.x - GetX(), m_pkMobData->m_posLastAttacked.y - GetY());
	float fx, fy;
	GetDeltaByDegree(GetRotation(), fDist, &fx, &fy);
	x = GetX() + (int)fx;
	y = GetY() + (int)fy;
	*/
	SetVictim(NULL);

	x = m_pkMobInst->m_posLastAttacked.x;
	y = m_pkMobInst->m_posLastAttacked.y;

	SetRotationToXY(x, y);

	if (!Goto(x, y))
		return false;

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	if (test_server)
		sys_log(0, "%s %p -- %d %d", GetName(), this, x, y);

	if (GetParty())
		GetParty()->SendMessage(this, PM_RETURN, x, y);

	return true;
}

bool CHARACTER::Follow(LPCHARACTER pkChr, float fMinDistance)
{
	if (IsPC())
	{
		sys_err("CHARACTER::Follow : PC cannot use this method", GetName());
		return false;
	}

	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
	{
		if (pkChr->IsPC()) // ï¿½Ñ¾Æ°ï¿½ï¿½ï¿½ ï¿½ï¿½ë°¡ PCï¿½ï¿½ ï¿½ï¿½
		{
			// If i'm in a party. I must obey party leader's AI.
			if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
			{
				if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000) // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ý¹ï¿½ï¿½ï¿½ï¿½ï¿½ 15ï¿½Ê°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
				{
					// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ 50ï¿½ï¿½ï¿½ï¿½ ï¿½Ì»ï¿½ ï¿½ï¿½ï¿½Ì³ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï°ï¿½ ï¿½ï¿½ï¿½Æ°ï¿½ï¿½ï¿½.
					if (m_pkMobData->m_table.wAttackRange < DISTANCE_APPROX(pkChr->GetX() - GetX(), pkChr->GetY() - GetY()))
						if (Return())
							return true;
				}
			}
		}
		return false;
	}
	// END_OF_TRENT_MONSTER

	long x = pkChr->GetX();
	long y = pkChr->GetY();

	if (pkChr->IsPC()) // ï¿½Ñ¾Æ°ï¿½ï¿½ï¿½ ï¿½ï¿½ë°¡ PCï¿½ï¿½ ï¿½ï¿½
	{
		// If i'm in a party. I must obey party leader's AI.
		if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
		{
			if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000) // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ý¹ï¿½ï¿½ï¿½ï¿½ï¿½ 15ï¿½Ê°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
			{
				// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ 50ï¿½ï¿½ï¿½ï¿½ ï¿½Ì»ï¿½ ï¿½ï¿½ï¿½Ì³ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï°ï¿½ ï¿½ï¿½ï¿½Æ°ï¿½ï¿½ï¿½.
				if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
					if (Return())
						return true;
			}
		}
	}

	if (IsGuardNPC())
	{
		if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
			if (Return())
				return true;
	}

	if (pkChr->IsState(pkChr->m_stateMove) &&
		GetMobBattleType() != BATTLE_TYPE_RANGE &&
		GetMobBattleType() != BATTLE_TYPE_MAGIC &&
		false == IsPet()
#ifdef ENABLE_GROWTH_PET_SYSTEM
		&& false == IsGrowthPet()
		&& false == IsPetPay()
#endif
		)
	{
		// ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ìµï¿½ï¿½ï¿½ï¿½Ì¸ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ìµï¿½ï¿½ï¿½ ï¿½Ñ´ï¿½
		// ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Óµï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Å¸ï¿½ï¿½Îºï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ã°ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
		// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½Ã°ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ìµï¿½ï¿½Ñ´Ù°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï¿ï¿½ ï¿½Å±ï¿½ï¿½ ï¿½Ìµï¿½ï¿½Ñ´ï¿½.
		float rot = pkChr->GetRotation();
		float rot_delta = GetDegreeDelta(rot, GetDegreeFromPositionXY(GetX(), GetY(), pkChr->GetX(), pkChr->GetY()));

		float yourSpeed = pkChr->GetMoveSpeed();
		float mySpeed = GetMoveSpeed();

		float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());
		float fFollowSpeed = mySpeed - yourSpeed * cos(rot_delta * M_PI / 180);

		if (fFollowSpeed >= 0.1f)
		{
			float fMeetTime = fDist / fFollowSpeed;
			float fYourMoveEstimateX, fYourMoveEstimateY;

			if (fMeetTime * yourSpeed <= 100000.0f)
			{
				GetDeltaByDegree(pkChr->GetRotation(), fMeetTime * yourSpeed, &fYourMoveEstimateX, &fYourMoveEstimateY);

				x += (long)fYourMoveEstimateX;
				y += (long)fYourMoveEstimateY;

				float fDistNew = sqrt(((double)x - GetX()) * (x - GetX()) + ((double)y - GetY()) * (y - GetY()));
				if (fDist < fDistNew)
				{
					x = (long)(GetX() + (x - GetX()) * fDist / fDistNew);
					y = (long)(GetY() + (y - GetY()) * fDist / fDistNew);
				}
			}
		}
	}

	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Ä¡ï¿½ï¿½ ï¿½Ù¶ï¿½ï¿½ï¿½ï¿½ ï¿½Ñ´ï¿½.
	SetRotationToXY(x, y);

	float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());

	if (fDist <= fMinDistance)
		return false;

	float fx, fy;

	if (IsChangeAttackPosition(pkChr) && GetMobRank() < MOB_RANK_BOSS)
	{
		// ï¿½ï¿½ï¿½ï¿½ ï¿½Öºï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ìµï¿½
		SetChangeAttackPositionTime();

		int retry = 16;
		int dx, dy;
		int rot = (int)GetDegreeFromPositionXY(x, y, GetX(), GetY());

		while (--retry)
		{
			if (fDist < 500.0f)
				GetDeltaByDegree((rot + number(-90, 90) + number(-90, 90)) % 360, fMinDistance, &fx, &fy);
			else
				GetDeltaByDegree(number(0, 359), fMinDistance, &fx, &fy);

			dx = x + (int)fx;
			dy = y + (int)fy;

			LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), dx, dy);

			if (NULL == tree)
				break;

			if (0 == (tree->GetAttribute(dx, dy) & (ATTR_BLOCK | ATTR_OBJECT)))
				break;
		}

		//sys_log(0, "ï¿½ï¿½Ã³ ï¿½ï¿½ò°¡·ï¿½ ï¿½Ìµï¿½ %s retry %d", GetName(), retry);
		if (!Goto(dx, dy))
			return false;
	}
	else
	{
		// ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ó°¡±ï¿½
		float fDistToGo = fDist - fMinDistance;
		GetDeltaByDegree(GetRotation(), fDistToGo, &fx, &fy);

		//sys_log(0, "ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ìµï¿½ %s", GetName());
		if (!Goto(GetX() + (int)fx, GetY() + (int)fy))
			return false;
	}

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	//MonsterLog("ï¿½Ñ¾Æ°ï¿½ï¿½ï¿½; %s", pkChr->GetName());
	return true;
}

float CHARACTER::GetDistanceFromSafeboxOpen() const
{
	return DISTANCE_APPROX(GetX() - m_posSafeboxOpen.x, GetY() - m_posSafeboxOpen.y);
}

void CHARACTER::SetSafeboxOpenPosition()
{
	m_posSafeboxOpen = GetXYZ();
}

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
float CHARACTER::GetDistanceFromGuildstorageOpen() const
{
	return DISTANCE_APPROX(GetX() - m_posGuildstorageOpen.x, GetY() - m_posGuildstorageOpen.y);
}

void CHARACTER::SetGuildstorageOpenPosition()
{
	m_posGuildstorageOpen = GetXYZ();
}
#endif

CSafebox* CHARACTER::GetSafebox() const
{
	return m_pkSafebox;
}

void CHARACTER::ReqSafeboxLoad(const char* pszPassword)
{
	if (!*pszPassword || strlen(pszPassword) > SAFEBOX_PASSWORD_MAX_LEN)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You have entered an incorrect password."));
		return;
	}
	else if (m_pkSafebox)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] The Storeroom is already open."));
		return;
	}

#if defined(ENABLE_GROWTH_PET_SYSTEM) && defined(ENABLE_PET_ATTR_DETERMINE)
	if (IsGrowthPetDetermineWindow())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] The Storeroom is open."));
		return;
	}
#endif

	int iPulse = thecore_pulse();

	if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(10))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You have to wait 10 seconds before you can open the Storeroom again."));
		return;
	}
	/*else if (GetDistanceFromSafeboxOpen() > 1000)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You are too far away to open the storeroom."));
		return;
	}*/
	else if (m_bOpeningSafebox)
	{
		sys_log(0, "Overlapped safebox load request from %s", GetName());
		return;
	}

	SetSafeboxLoadTime();
	m_bOpeningSafebox = true;

	TSafeboxLoadPacket p;
	p.dwID = GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, pszPassword, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_LOAD, GetDesc()->GetHandle(), &p, sizeof(p));
}

void CHARACTER::LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem* pItems)
{
	bool bLoaded = false;

	// PREVENT_TRADE_WINDOW
	SetOpenSafebox(true);
	// END_PREVENT_TRADE_WINDOW

	if (m_pkSafebox)
		bLoaded = true;

	if (!m_pkSafebox)
		m_pkSafebox = M2_NEW CSafebox(this, iSize, dwGold);
	else
		m_pkSafebox->ChangeSize(iSize);

	m_iSafeboxSize = iSize;

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = iSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkSafebox->IsValidPosition(pItems->pos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
#if defined(__ITEM_APPLY_RANDOM__)
			item->SetRandomApplies(pItems->aApplyRandom);
#endif
			// #ifdef ENABLE_SET_ITEM
			// 			item->SetItemSetValue(pItems->set_value);
			// #endif

#if defined(__SET_ITEM__)
			item->SetItemSetValue(pItems->bSetValue);
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
			item->SetGrowthPetItemInfo(pItems->aPetInfo);

			if ((item->GetType() == ITEM_PET) && ((item->GetSubType() == PET_UPBRINGING) || (item->GetSubType() == PET_BAG)))
			{
				const long lPetDuration = (item->GetSocket(0) - get_global_time()) / 60;
				if ((lPetDuration <= 0) && (item->GetSubType() == PET_UPBRINGING))
					//LocaleChatPacket(CHAT_TYPE_INFO, ELocaleStrings::SCHLAEFT_GERADE_TIEF_UND_FEST, LC_LOCALE_ITEM_TEXT(item->GetVnum(), GetLanguage()));
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s is currently fast asleep."), item->GetLocaleName());
				SetGrowthPetInfo(pItems->aPetInfo);
				if (test_server)
					sys_err("%d %d %d %d", pItems->owner, pItems->aPetInfo.pet_id, pItems->aPetInfo.exp_monster, pItems->aPetInfo.exp_item);
			}
#endif

			item->SetAttributes(pItems->aAttr);
#if defined(__SOUL_BIND_SYSTEM__)
			item->SetSoulBind(pItems->soulbind);
#endif
#if defined(__CHANGE_LOOK_SYSTEM__)
			item->SetTransmutationVnum(pItems->dwTransmutationVnum);
#endif

#ifdef ENABLE_REFINE_ELEMENT
			item->SetElement(pItems->grade_element, pItems->attack_element, pItems->element_type_bonus, pItems->elements_value_bonus);
#endif

			if (!m_pkSafebox->Add(pItems->pos, item))
			{
				M2_DESTROY_ITEM(item);
			}
			else
			{
				item->OnAfterCreatedItem();
				item->SetSkipSave(false);
			}
		}
#ifdef ENABLE_GROWTH_PET_SYSTEM
		SendGrowthPetInfoPacket();
#endif
	}
}

void CHARACTER::ChangeSafeboxSize(BYTE bSize)
{
	//if (!m_pkSafebox)
	//	return;

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = bSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (m_pkSafebox)
		m_pkSafebox->ChangeSize(bSize);

	m_iSafeboxSize = bSize;
}

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
void CHARACTER::ReqGuildstorageLoad()
{
	CGuild* pGuild = GetGuild();
	if (!pGuild) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] It does not belong to the guild.!"));
		return;
	}

	if (pGuild->GetGuildstorage() == 0) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open a Storeroom while another window is open."));
		return;
	}

	if (GetExchange() || GetMyShop() ||
#ifdef ENABLE_PREMIUM_PRIVATE_SHOP
		GetViewingShopOwner() ||
#else
		GetShopOwner() ||
#endif
#ifdef ENABLE_MAILBOX
		GetMailBox() ||
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		IsChangeLookWindowOpen() ||
#endif
		IsOpenSafebox() || IsCubeOpen()) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open a Storeroom while another window is open."));
		return;
	}

#	ifdef ENABLE_GUILDRENEWAL_SYSTEM
	const TGuildMember* m = pGuild->GetMember(GetPlayerID());
	if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_BANK)) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You do not have permission to use Guild Storage."));
		return;
	}
#	endif

	if (pGuild->IsStorageOpen())
	{
		LPCHARACTER pChar = CHARACTER_MANAGER::Instance().FindByPID(pGuild->GetStoragePid());
		if (pChar) {
			pChar->ChatPacket(7, "player %s want to open storage.", GetName());
			ChatPacket(CHAT_TYPE_INFO, "guild is already open by %s", pChar->GetName());
		}
		else
			ChatPacket(CHAT_TYPE_INFO, "guild is already open");
		return;
	}

#if defined(ENABLE_GROWTH_PET_SYSTEM) && defined(ENABLE_PET_ATTR_DETERMINE)
	if (IsGrowthPetDetermineWindow())
	{
		ChatPacket(CHAT_TYPE_INFO, "While modifying your pet's stats, you cannot trade, sell items, use the storeroom etc.");
		return;
	}
#endif

	if (GetGuildstorage())
	{
		ChatPacket(CHAT_TYPE_INFO, "[Storeroom] The Storeroom is already open.");
		return;
	}

	if (!PulseManager::Instance().IncreaseClock(GetPlayerID(), ePulse::SafeboxLoad, std::chrono::milliseconds(1000)))
	{
		ChatPacket(CHAT_TYPE_INFO, "[Storeroom] You have to wait 10 seconds before you can open the Storeroom again.");
		return;
	}
	else if (GetDistanceFromGuildstorageOpen() > 1000)
	{
		ChatPacket(CHAT_TYPE_INFO, "[Storeroom] You are too far away to open the storeroom.");
		//LocaleChatPacket(CHAT_TYPE_INFO, ELocaleStrings::LAGER_DU_BIST_ZU_WEIT_ENTFERNT_UM_DAS_LAGER_ZU_OEFFNEN);
		return;
	}
	else if (m_bOpeningGuildstorage)
	{
		sys_log(0, "Overlapped guildstorage load request from %s", GetName());
		return;
	}

	SetGuildstorageLoadTime();
	m_bOpeningGuildstorage = true;

	TSafeboxLoadPacket p;
	p.dwID = GetGuild() ? GetGuild()->GetID() : 0;
	strlcpy(p.szLogin, GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, "000000", sizeof(p.szPassword));

	if (!p.dwID)
		return;

	db_clientdesc->DBPacket(HEADER_GD_GUILDSTORAGE_LOAD, GetDesc()->GetHandle(), &p, sizeof(p));
	pGuild->SetStorageState(true, GetPlayerID());
}

void CHARACTER::ChangeGuildstorageSize(uint8_t bSize)
{
	TPacketGCGuildstorageOpen p{};

	p.bHeader = HEADER_GC_GUILDSTORAGE_OPEN;
	p.bSize = bSize;

	GetDesc()->Packet(&p, sizeof(TPacketGCGuildstorageOpen));

	if (m_pkGuildstorage)
		m_pkGuildstorage->ChangeSize(bSize);

	m_iSafeboxSize = bSize;
}
#endif

void CHARACTER::CloseSafebox()
{
	if (!m_pkSafebox)
		return;

	// PREVENT_TRADE_WINDOW
	SetOpenSafebox(false);
	// END_PREVENT_TRADE_WINDOW

	m_pkSafebox->Save();

	M2_DELETE(m_pkSafebox);
	m_pkSafebox = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseSafebox");

	SetSafeboxLoadTime();
	m_bOpeningSafebox = false;

	Save();
}

void CHARACTER::LoadSafeboxBuff()
{
	if (!g_bSafeboxChangePasswordBuff)
		return;

	if (FindAffect(AFFECT_SAFE_BOX_BUFF))
		return;

	char szQuery[254 + 1];
	snprintf(szQuery, sizeof(szQuery), "SELECT `buff_point`, `buff_value`, `buff_duration` FROM safebox%s WHERE `account_id` = %u",
		get_table_postfix(), GetDesc()->GetAccountTable().id);

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	SQLResult* pRes = pMsg->Get();
	if (pRes->uiNumRows > 0)
	{
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

		BYTE bPoint = POINT_NONE;
		str_to_number(bPoint, row[0]);

		long lValue = 0;
		str_to_number(lValue, row[1]);

		long lDuration = 0;
		str_to_number(lDuration, row[2]);

		if (lDuration > std::time(nullptr))
		{
			long lRemainingTime = lDuration - std::time(nullptr);
			if (lRemainingTime > 300)
				AddAffect(AFFECT_SAFE_BOX_BUFF, bPoint, lValue, AFF_NONE, lRemainingTime, 0, true);
		}
	}
}

void CHARACTER::SetSafeboxBuff()
{
	if (!g_bSafeboxChangePasswordBuff)
		return;

	if (FindAffect(AFFECT_SAFE_BOX_BUFF))
		return;

	//
	// NOTE: Safebox Change Password Buff
	//

	long lDuration = 60 * 60 * 24 * 90;
	using SafeboxBuff = struct {
		BYTE bPoint;
		long lValue;
		std::string stText;
	};
	using SafeboxBuffVec = std::vector<SafeboxBuff>;

	static SafeboxBuffVec s_vSafeboxBuf{
		{ POINT_MALL_DEFBONUS, 20, "You have received a storage bonus of %d defence.", },
		{ POINT_MAX_HP_PCT, 1, "You have received a storage bonus of %d%% HP.", },
		{ POINT_MALL_EXPBONUS, 5, "You have received a storage bonus of %d%% EXP.", },
		{ POINT_ATTBONUS_MONSTER, 3, "You have received a storage bonus of %d%% strength against monsters." },
	};

	SafeboxBuffVec::const_iterator it = s_vSafeboxBuf.begin();
	std::advance(it, std::rand() % s_vSafeboxBuf.size());

	char szQuery[254 + 1];
	snprintf(szQuery, sizeof(szQuery), "UPDATE safebox%s SET `buff_point` = %d, `buff_value` = %d, `buff_duration` = %d WHERE `account_id` = %u",
		get_table_postfix(), it->bPoint, it->lValue, std::time(nullptr) + lDuration, GetDesc()->GetAccountTable().id);
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiAffectedRows > 0)
	{
		AddAffect(AFFECT_SAFE_BOX_BUFF, it->bPoint, it->lValue, AFF_NONE, lDuration, 0, true);
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT(it->stText.c_str()), it->lValue);
	}
}

CSafebox* CHARACTER::GetMall() const
{
	return m_pkMall;
}

void CHARACTER::LoadMall(int iItemCount, TPlayerItem* pItems)
{
	bool bLoaded = false;

	if (m_pkMall)
		bLoaded = true;

	if (!m_pkMall)
		m_pkMall = M2_NEW CSafebox(this, 3 * SAFEBOX_PAGE_SIZE, 0);
	else
		m_pkMall->ChangeSize(3 * SAFEBOX_PAGE_SIZE);

	m_pkMall->SetWindowMode(MALL);

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_MALL_OPEN;
	p.bSize = 3 * SAFEBOX_PAGE_SIZE;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkMall->IsValidPosition(pItems->pos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
#if defined(__ITEM_APPLY_RANDOM__)
			item->SetRandomApplies(pItems->aApplyRandom);
#endif
			// #ifdef ENABLE_SET_ITEM
			// 			item->SetItemSetValue(pItems->set_value);
			// #endif

#if defined(__SET_ITEM__)
			item->SetItemSetValue(pItems->bSetValue);
#endif

#ifdef ENABLE_REFINE_ELEMENT
			item->SetElement(pItems->grade_element, pItems->attack_element, pItems->element_type_bonus, pItems->elements_value_bonus);
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
			item->SetGrowthPetItemInfo(pItems->aPetInfo);

			if ((item->GetType() == ITEM_PET) && ((item->GetSubType() == PET_UPBRINGING) || (item->GetSubType() == PET_BAG)))
			{
				SetGrowthPetInfo(pItems->aPetInfo);
				//sys_err("GuildBank: %d %d %d %d", pItems->owner, pItems->aPetInfo.pet_id, pItems->aPetInfo.exp_monster, pItems->aPetInfo.exp_item);
			}
#endif

			item->SetAttributes(pItems->aAttr);
#if defined(__SOUL_BIND_SYSTEM__)
			item->SetSoulBind(pItems->soulbind);
#endif
#if defined(__CHANGE_LOOK_SYSTEM__)
			item->SetTransmutationVnum(pItems->dwTransmutationVnum);
#endif

			if (!m_pkMall->Add(pItems->pos, item))
				M2_DESTROY_ITEM(item);
			else
				item->SetSkipSave(false);
		}
#ifdef ENABLE_GROWTH_PET_SYSTEM
		SendGrowthPetInfoPacket();
#endif
	}
}

void CHARACTER::CloseMall()
{
	if (!m_pkMall)
		return;

	m_pkMall->Save();

	M2_DELETE(m_pkMall);
	m_pkMall = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseMall");
}

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
CSafebox* CHARACTER::GetGuildstorage() const
{
	return m_pkGuildstorage;
}

void CHARACTER::LoadGuildstorage(int iSize, int iItemCount, TPlayerItem* pItems)
{
	bool bLoaded = false;

	//PREVENT_TRADE_WINDOW
	SetOpenGuildstorage(true);
	//END_PREVENT_TRADE_WINDOW

	if (m_pkGuildstorage)
		bLoaded = true;

	if (!m_pkGuildstorage)
		m_pkGuildstorage = M2_NEW CSafebox(this, iSize, 0);
	else
		m_pkGuildstorage->ChangeSize(iSize);

	m_iGuildstorageSize = iSize;

	m_pkGuildstorage->SetWindowMode(GUILDBANK);

	TPacketGCGuildstorageOpen p{};

	p.bHeader = HEADER_GC_GUILDSTORAGE_OPEN;
	p.bSize = iSize;

	GetDesc()->Packet(&p, sizeof(TPacketGCGuildstorageOpen));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkGuildstorage->IsValidPosition(pItems->pos))
				continue;

			LPITEM item = ITEM_MANAGER::Instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
			item->SetAttributes(pItems->aAttr);
#ifdef ENABLE_YOHARA_SYSTEM
			item->SetRandomAttrs(pItems->aApplyRandom);
			item->SetRandomDefaultAttrs(pItems->alRandomValues);
#endif
#ifdef ENABLE_SEALBIND_SYSTEM
			item->SetSealDate(pItems->nSealDate);
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			item->SetChangeLookVnum(pItems->dwTransmutationVnum);
#endif
#ifdef ENABLE_GIVE_BASIC_ITEM
			item->SetBasic(pItems->is_basic);
#endif
#ifdef ENABLE_REFINE_ELEMENT
			item->SetElement(pItems->grade_element, pItems->attack_element, pItems->element_type_bonus, pItems->elements_value_bonus);
#endif
#ifdef ENABLE_SET_ITEM
			item->SetItemSetValue(pItems->bSetValue);
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
			item->SetGrowthPetItemInfo(pItems->aPetInfo);

			if ((item->GetType() == ITEM_PET) && ((item->GetSubType() == PET_UPBRINGING) || (item->GetSubType() == PET_BAG)))
			{
				SetGrowthPetInfo(pItems->aPetInfo);
				//sys_err("GuildBank: %d %d %d %d", pItems->owner, pItems->aPetInfo.pet_id, pItems->aPetInfo.exp_monster, pItems->aPetInfo.exp_item);
			}
#endif

			if (!m_pkGuildstorage->Add(pItems->pos, item))
				M2_DESTROY_ITEM(item);
			else
				item->SetSkipSave(false);
		}

#ifdef ENABLE_GROWTH_PET_SYSTEM
		SendGrowthPetInfoPacket();
#endif
	}
}

void CHARACTER::CloseGuildstorage()
{
	if (!m_pkGuildstorage)
		return;

	//PREVENT_TRADE_WINDOW
	SetOpenGuildstorage(false);
	GetGuild()->SetStorageState(false, 0);
	//END_PREVENT_TRADE_WINDOW

	m_pkGuildstorage->Save();

	M2_DELETE(m_pkGuildstorage);
	m_pkGuildstorage = nullptr;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseGuildstorage");

	SetGuildstorageLoadTime();
	m_bOpeningGuildstorage = false;
}
#endif

bool CHARACTER::BuildUpdatePartyPacket(TPacketGCPartyUpdate& out)
{
	if (!GetParty())
		return false;

	memset(&out, 0, sizeof(out));

	out.header = HEADER_GC_PARTY_UPDATE;
	out.pid = GetPlayerID();
	out.percent_hp = (GetMaxHP() <= 0) ? 0 : MINMAX(0, GetHP() * 100 / GetMaxHP(), 100);
	out.role = GetParty()->GetRole(GetPlayerID());

	sys_log(1, "PARTY %s role is %d", GetName(), out.role);

	LPCHARACTER l = GetParty()->GetLeaderCharacter();

	if (l && DISTANCE_APPROX(GetX() - l->GetX(), GetY() - l->GetY()) < PARTY_DEFAULT_RANGE)
	{
		if (g_iUseLocale)
			out.affects[0] = GetParty()->GetPartyBonusExpPercent();
		else
			out.affects[0] = GetParty()->GetExpBonusPercent();
		out.affects[1] = GetPoint(POINT_PARTY_ATTACKER_BONUS);
		out.affects[2] = GetPoint(POINT_PARTY_TANKER_BONUS);
		out.affects[3] = GetPoint(POINT_PARTY_BUFFER_BONUS);
		out.affects[4] = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
		out.affects[5] = GetPoint(POINT_PARTY_HASTE_BONUS);
		out.affects[6] = GetPoint(POINT_PARTY_DEFENDER_BONUS);
	}

	return true;
}

int CHARACTER::GetLeadershipSkillLevel() const
{
	return GetSkillLevel(SKILL_LEADERSHIP);
}

int CHARACTER::GetRoleProficiencySkillLevel() const
{
	return GetSkillLevel(SKILL_ROLE_PROFICIENCY);
}

int CHARACTER::GetInSightSkillLevel() const
{
	return GetSkillLevel(SKILL_INSIGHT);
}

void CHARACTER::QuerySafeboxSize()
{
	if (m_iSafeboxSize == -1)
	{
		DBManager::instance().ReturnQuery(QID_SAFEBOX_SIZE,
			GetPlayerID(),
			NULL,
			"SELECT size FROM safebox%s WHERE account_id = %u",
			get_table_postfix(),
			GetDesc()->GetAccountTable().id);
	}
}

void CHARACTER::SetSafeboxSize(int iSize)
{
	sys_log(1, "SetSafeboxSize: %s %d", GetName(), iSize);
	m_iSafeboxSize = iSize;
	DBManager::instance().Query("UPDATE safebox%s SET size = %d WHERE account_id = %u", get_table_postfix(), iSize / SAFEBOX_PAGE_SIZE, GetDesc()->GetAccountTable().id);
}

int CHARACTER::GetSafeboxSize() const
{
	return m_iSafeboxSize;
}

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
void CHARACTER::QueryGuildstorageSize()
{
	if (m_iGuildstorageSize == -1)
	{
		DBManager::Instance().ReturnQuery(QID_GUILDSTORAGE_SIZE,
			GetPlayerID(),
			nullptr,
			"SELECT guildstorage FROM guild%s WHERE id = %u",
			get_table_postfix(),
			GetGuild()->GetID());
	}
}

void CHARACTER::SetGuildstorageSize(int iSize)
{
	sys_log(1, "SetGuildstorageSize: %s %d", GetName(), iSize);
	m_iGuildstorageSize = iSize;
	GetGuild()->SetGuildstorage(iSize / SAFEBOX_PAGE_SIZE);
	//DBManager::Instance().Query("UPDATE guild%s SET guildstorage = %d WHERE id = %u", get_table_postfix(), iSize / SAFEBOX_PAGE_SIZE, GetGuild()->GetID());
}

int CHARACTER::GetGuildstorageSize() const
{
	return m_iGuildstorageSize;
}
#endif

void CHARACTER::SetNowWalking(bool bWalkFlag)
{
	//if (m_bNowWalking != bWalkFlag || IsNPC())
	if (m_bNowWalking != bWalkFlag)
	{
		if (bWalkFlag)
		{
			m_bNowWalking = true;
			m_dwWalkStartTime = get_dword_time();
		}
		else
		{
			m_bNowWalking = false;
		}

		//if (m_bNowWalking)
		{
			TPacketGCWalkMode p;
			p.vid = GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

			PacketView(&p, sizeof(p));
		}

		if (IsNPC())
		{
			if (m_bNowWalking)
				MonsterLog("m_bNowWalking");
			else
				MonsterLog("else_m_bNowWalking");
		}

		//sys_log(0, "%s is now %s", GetName(), m_bNowWalking ? "walking." : "running.");
	}
}

void CHARACTER::StartStaminaConsume()
{
	if (m_bStaminaConsume)
		return;

	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = true;
	//ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
	if (IsStaminaHalfConsume())
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec / 2, GetStamina());
	else
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
}

void CHARACTER::StopStaminaConsume()
{
	if (!m_bStaminaConsume)
		return;

	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = false;
	ChatPacket(CHAT_TYPE_COMMAND, "StopStaminaConsume %d", GetStamina());
}

bool CHARACTER::IsStaminaConsume() const
{
	return m_bStaminaConsume;
}

bool CHARACTER::IsStaminaHalfConsume() const
{
	return IsEquipUniqueItem(UNIQUE_ITEM_HALF_STAMINA);
}

void CHARACTER::ResetStopTime()
{
	m_dwStopTime = get_dword_time();
}

DWORD CHARACTER::GetStopTime() const
{
	return m_dwStopTime;
}

void CHARACTER::ResetPoint(int iLv)
{
	BYTE bJob = GetJob();

	PointChange(POINT_LEVEL, iLv - GetLevel());

	SetRealPoint(POINT_ST, JobInitialPoints[bJob].st);
	SetPoint(POINT_ST, GetRealPoint(POINT_ST));

	SetRealPoint(POINT_HT, JobInitialPoints[bJob].ht);
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));

	SetRealPoint(POINT_DX, JobInitialPoints[bJob].dx);
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));

	SetRealPoint(POINT_IQ, JobInitialPoints[bJob].iq);
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

	SetRandomHP((iLv - 1) * number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end));
	SetRandomSP((iLv - 1) * number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end));

	PointChange(POINT_STAT, (MINMAX(1, iLv, gPlayerMaxLevelStats) * 3) + GetPoint(POINT_LEVEL_STEP) - GetPoint(POINT_STAT));

	ComputePoints();

	// È¸ï¿½ï¿½
	PointChange(POINT_HP, GetMaxHP() - GetHP());
	PointChange(POINT_SP, GetMaxSP() - GetSP());

	PointsPacket();

	LogManager::instance().CharLog(this, 0, "RESET_POINT", "");
}

void CHARACTER::ResetExp()
{
	SetExp(0);

	PointsPacket();

	UpdatePointsPacket(POINT_EXP, GetExp());

	LogManager::instance().CharLog(this, 0, "RESET_EXP", "");
}

bool CHARACTER::IsChangeAttackPosition(LPCHARACTER target) const
{
	if (!IsNPC())
		return true;

	DWORD dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_NEAR;

	if (DISTANCE_APPROX(GetX() - target->GetX(), GetY() - target->GetY()) >
		AI_CHANGE_ATTACK_POISITION_DISTANCE + GetMobAttackRange())
		dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_FAR;

	return get_dword_time() - m_dwLastChangeAttackPositionTime > dwChangeTime;
}

void CHARACTER::GiveRandomSkillBook()
{
	LPITEM item = AutoGiveItem(ITEM_SKILLBOOK_VNUM);

	if (NULL != item)
	{
		BYTE bJob = 0;

		if (!number(0, 1))
			bJob = GetJob() + 1;

		DWORD dwSkillVnum = 0;

		do
		{
#if defined(__WOLFMAN_CHARACTER__)
			dwSkillVnum = number(1, 175);
			if (dwSkillVnum > 111 && dwSkillVnum < 170)
				continue;
#else
			dwSkillVnum = number(1, 111);
#endif
			const CSkillProto* pkSk = CSkillManager::instance().Get(dwSkillVnum);

			if (NULL == pkSk)
				continue;

			if (bJob && bJob != pkSk->dwType)
				continue;

			break;
		} while (true);

		item->SetSocket(0, dwSkillVnum);
	}
}

void CHARACTER::ReviveInvisible(int iDur)
{
	AddAffect(AFFECT_REVIVE_INVISIBLE, POINT_NONE, 0, AFF_REVIVE_INVISIBLE, iDur, 0, true);
}

void CHARACTER::ToggleMonsterLog()
{
	m_bMonsterLog = !m_bMonsterLog;

	if (m_bMonsterLog)
	{
		ChatPacket(CHAT_TYPE_NOTICE, "Registered Monster Log");
		CHARACTER_MANAGER::instance().RegisterForMonsterLog(this);
	}
	else
	{
		ChatPacket(CHAT_TYPE_NOTICE, "Unregistered Monster Log");
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
	}
}

void CHARACTER::SetGuild(CGuild* pGuild)
{
	if (m_pGuild != pGuild)
	{
		m_pGuild = pGuild;
		UpdatePacket();
	}
}

void CHARACTER::SendGreetMessage()
{
	__typeof__(DBManager::instance().GetGreetMessage()) v = DBManager::instance().GetGreetMessage();

	for (itertype(v) it = v.begin(); it != v.end(); ++it)
	{
		ChatPacket(CHAT_TYPE_NOTICE, it->c_str());
	}
}

void CHARACTER::BeginStateEmpty()
{
	MonsterLog("!");
}

void CHARACTER::EffectPacket(int enumEffectType)
{
	TPacketGCSpecialEffect p;

	p.header = HEADER_GC_SEPCIAL_EFFECT;
	p.type = enumEffectType;
	p.vid = GetVID();

	PacketAround(&p, sizeof(TPacketGCSpecialEffect));
}

void CHARACTER::SpecificEffectPacket(const char filename[MAX_EFFECT_FILE_NAME])
{
	TPacketGCSpecificEffect p;

	p.header = HEADER_GC_SPECIFIC_EFFECT;
	p.vid = GetVID();
	memcpy(p.effect_file, filename, MAX_EFFECT_FILE_NAME);

	PacketAround(&p, sizeof(TPacketGCSpecificEffect));
}

void CHARACTER::MonsterChat(BYTE bMonsterChatType)
{
	if (IsPC())
		return;

	char sbuf[256 + 1];

	if (IsMonster())
	{
		if (number(0, 60))
			return;

		snprintf(sbuf, sizeof(sbuf),
			"(locale.monster_chat[%i] and locale.monster_chat[%i][%d] or '')",
			GetRaceNum(), GetRaceNum(), bMonsterChatType * 3 + number(1, 3));
	}
	else
	{
		if (bMonsterChatType != MONSTER_CHAT_WAIT)
			return;

		if (IsGuardNPC())
		{
			if (number(0, 6))
				return;
		}
		else
		{
			if (number(0, 30))
				return;
		}

		snprintf(sbuf, sizeof(sbuf), "(locale.monster_chat[%i] and locale.monster_chat[%i][number(1, table.getn(locale.monster_chat[%i]))] or '')", GetRaceNum(), GetRaceNum(), GetRaceNum());
	}

	std::string text = quest::ScriptToString(sbuf);

	if (text.empty())
		return;

	struct packet_chat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(struct packet_chat) + text.size() + 1;
	pack_chat.type = CHAT_TYPE_TALKING;
	pack_chat.id = GetVID();
	pack_chat.bEmpire = 0;

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(text.c_str(), text.size() + 1);

	PacketAround(buf.read_peek(), buf.size());
}

void CHARACTER::SetQuestNPCID(DWORD vid)
{
	m_dwQuestNPCVID = vid;
}

LPCHARACTER CHARACTER::GetQuestNPC() const
{
	return CHARACTER_MANAGER::instance().Find(m_dwQuestNPCVID);
}

void CHARACTER::SetQuestItemPtr(LPITEM item)
{
	m_dwQuestItemVID = (item) ? item->GetVID() : 0;
}

void CHARACTER::ClearQuestItemPtr()
{
	m_dwQuestItemVID = 0;
}

LPITEM CHARACTER::GetQuestItemPtr() const
{
	if (!m_dwQuestItemVID)
		return nullptr;
	return ITEM_MANAGER::Instance().FindByVID(m_dwQuestItemVID);
}

LPDUNGEON CHARACTER::GetDungeonForce() const
{
	if (m_lWarpMapIndex > 10000)
		return CDungeonManager::instance().FindByMapIndex(m_lWarpMapIndex);

	return m_pkDungeon;
}

#ifdef ENABLE_12ZI
LPZODIAC CHARACTER::GetZodiacForce() const
{
	if (m_lWarpMapIndex > 10000)
		return CZodiacManager::Instance().FindByMapIndex(m_lWarpMapIndex);

	return m_pkZodiac;
}
#endif

void CHARACTER::SetBlockMode(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;

	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);

	SetQuestFlag("game_option.block_exchange", bFlag & BLOCK_EXCHANGE ? 1 : 0);
	SetQuestFlag("game_option.block_party_invite", bFlag & BLOCK_PARTY_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_guild_invite", bFlag & BLOCK_GUILD_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_whisper", bFlag & BLOCK_WHISPER ? 1 : 0);
	SetQuestFlag("game_option.block_messenger_invite", bFlag & BLOCK_MESSENGER_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_party_request", bFlag & BLOCK_PARTY_REQUEST ? 1 : 0);
}

void CHARACTER::SetBlockModeForce(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;
	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);
}

bool CHARACTER::IsGuardNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

int CHARACTER::GetPolymorphPower() const
{
	if (test_server)
	{
		int value = quest::CQuestManager::instance().GetEventFlag("poly");
		if (value)
			return value;
	}
	return aiPolymorphPowerByLevel[MINMAX(0, GetSkillLevel(SKILL_POLYMORPH), 40)];
}

void CHARACTER::SetPolymorph(DWORD dwRaceNum, bool bMaintainStat)
{

#ifdef ENABLE_12ZI
	if (GetZodiac() && IsDead())
		return;
#endif

	if (dwRaceNum < MAIN_RACE_MAX_NUM)
	{
		dwRaceNum = 0;
		bMaintainStat = false;
	}

	if (m_dwPolymorphRace == dwRaceNum)
		return;

	m_bPolyMaintainStat = bMaintainStat;
	m_dwPolymorphRace = dwRaceNum;

	sys_log(0, "POLYMORPH: %s race %u ", GetName(), dwRaceNum);

	if (dwRaceNum != 0)
		StopRiding();

#ifdef ENABLE_12ZI
	/*if (GetZodiac())
		CZodiacManager::Instance().PolyPC(this, dwRaceNum);*/
#endif

	SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
	m_afAffectFlag.Set(AFF_SPAWN);

	ViewReencode();

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	if (!bMaintainStat)
	{
		PointChange(POINT_ST, 0);
		PointChange(POINT_DX, 0);
		PointChange(POINT_IQ, 0);
		PointChange(POINT_HT, 0);
	}

	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Â¿ï¿½ï¿½ï¿½ ï¿½×´ï¿½ ï¿½ï¿½ï¿½, ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Ç®ï¿½ï¿½ï¿½ï¿½ ï¿½Ç´Âµï¿½
	// ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ä·ï¿½ valid combo intervalï¿½ï¿½ ï¿½Ù¸ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	// Combo ï¿½ï¿½ ï¿½Ç´ï¿½ Hackerï¿½ï¿½ ï¿½Î½ï¿½ï¿½Ï´ï¿½ ï¿½ï¿½ì°¡ ï¿½Ö´ï¿½.
	// ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Ç®ï¿½Å³ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ï°ï¿½ ï¿½Ç¸ï¿½,
	// valid combo intervalï¿½ï¿½ resetï¿½Ñ´ï¿½.
	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputeBattlePoints();
}

int CHARACTER::GetQuestFlag(const std::string& flag) const
{
	if (!IsPC())
	{
		sys_err("Trying to get qf %s from non player character", flag.c_str());
		return 0;
	}

	DWORD pid = GetPlayerID();

	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(pid);

	if (!pPC)
	{
		sys_err("Nullpointer when trying to access questflag %s for player with pid %lu", flag.c_str(), pid);
		return 0;
	}

	return pPC->GetFlag(flag);
}

void CHARACTER::SetQuestFlag(const std::string& flag, int value)
{
	DWORD pid = GetPlayerID();

	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(pid);

	if (!pPC)
	{
		sys_err("Nullpointer when trying to set questflag %s for player with pid %lu", flag.c_str(), pid);
		return;
	}

	pPC->SetFlag(flag, value);
}

void CHARACTER::DetermineDropMetinStone()
{
#if defined(__MAGIC_REDUCTION__)
	const int METIN_STONE_NUM = 17;
#else
	const int METIN_STONE_NUM = 15;
#endif

	static DWORD c_adwMetin[METIN_STONE_NUM] =
	{
		28030, // 60
		28031,
		28032,
		28033,
		28034,
		28035,
		28036,
		28037,
		28038,
		28039,
		28040,
		28041,
		28042,
		28043,
#if defined(__MAGIC_REDUCTION__)
		28044,
		28045,
#endif
		28012,
	};

	DWORD stone_num = GetRaceNum();
	int idx = std::lower_bound(aStoneDrop, aStoneDrop + STONE_INFO_MAX_NUM, stone_num) - aStoneDrop;
	if (idx >= STONE_INFO_MAX_NUM || aStoneDrop[idx].dwMobVnum != stone_num)
	{
		m_dwDropMetinStone = 0;
	}
	else
	{
		const SStoneDropInfo& info = aStoneDrop[idx];
		m_bDropMetinStonePct = info.iDropPct;
		{
			m_dwDropMetinStone = c_adwMetin[number(0, METIN_STONE_NUM - 1)];
			int iGradePct = number(1, 100);

			for (int iStoneLevel = 0; iStoneLevel < STONE_LEVEL_MAX_NUM; iStoneLevel++)
			{
				int iLevelGradePortion = info.iLevelPct[iStoneLevel];

				if (iGradePct <= iLevelGradePortion)
				{
					break;
				}
				else
				{
					iGradePct -= iLevelGradePortion;
					m_dwDropMetinStone += 100; // ï¿½ï¿½ +a -> +(a+1)ï¿½ï¿½ ï¿½É¶ï¿½ï¿½ï¿½ï¿½ï¿½ 100ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
				}
			}
		}
	}
}

void CHARACTER::SendEquipment(LPCHARACTER ch)
{
	TPacketViewEquip p;
	p.header = HEADER_GC_VIEW_EQUIP;
	p.vid = GetVID();
	for (int i = 0; i < WEAR_MAX_NUM; i++)
	{
		LPITEM item = GetWear(i);
		if (item)
		{
			p.equips[i].vnum = item->GetVnum();
			p.equips[i].count = (BYTE)item->GetCount();

			thecore_memcpy(p.equips[i].alSockets, item->GetSockets(), sizeof(p.equips[i].alSockets));
			thecore_memcpy(p.equips[i].aAttr, item->GetAttributes(), sizeof(p.equips[i].aAttr));
#if defined(__CHANGE_LOOK_SYSTEM__)
			p.equips[i].dwTransmutationVnum = item->GetTransmutationVnum();
#endif
#if defined(__SET_ITEM__)
			p.equips[i].bSetValue = item->GetItemSetValue();
#endif
		}
		else
		{
			p.equips[i].vnum = 0;
#if defined(__CHANGE_LOOK_SYSTEM__)
			p.equips[i].dwTransmutationVnum = 0;
#endif
#if defined(__SET_ITEM__)
			p.equips[i].bSetValue = 0;
#endif
		}
	}
	ch->GetDesc()->Packet(&p, sizeof(p));
}

bool CHARACTER::CanSummon(int iLeaderShip)
{
	return (iLeaderShip >= 30 && m_dwLastDeadTime + 300 > get_dword_time());
}

void CHARACTER::UnMount(bool bUnequipItem)
{
	if (bUnequipItem)
		UnEquipSpecialRideUniqueItem();

	RemoveAffect(AFFECT_MOUNT);
	RemoveAffect(AFFECT_MOUNT_BONUS);

	if (IsHorseRiding())
	{
		StopRiding();
	}
	else if (GetMountVnum())
	{
		MountVnum(0);
	}
}

void CHARACTER::MountVnum(DWORD vnum)
{
	if (m_dwMountVnum == vnum)
		return;

	if ((m_dwMountVnum != 0) && (vnum != 0))
		MountVnum(0);

	m_dwMountVnum = vnum;
	m_dwMountTime = get_dword_time();

	if (m_bIsObserver)
		return;

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();

	// NOTE : Mountï¿½Ñ´Ù°ï¿½ ï¿½Ø¼ï¿½ Client Sideï¿½ï¿½ ï¿½ï¿½Ã¼ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê´Â´ï¿½.
	// ï¿½×¸ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Sideï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Ä¡ ï¿½Ìµï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ê´Â´ï¿½. ï¿½Ö³ï¿½ï¿½Ï¸ï¿½ Client Sideï¿½ï¿½ï¿½ï¿½ Coliision Adjustï¿½ï¿½ ï¿½Ò¼ï¿½ ï¿½Ö´Âµï¿½
	// ï¿½ï¿½Ã¼ï¿½ï¿½ ï¿½Ò¸ï¿½ï¿½ï¿½×´Ù°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ä¡ï¿½ï¿½ ï¿½Ìµï¿½ï¿½ï¿½Å°ï¿½ï¿½ ï¿½Ì¶ï¿½ collision checkï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ç·ï¿½ ï¿½ï¿½æ¿¡ ï¿½ï¿½ï¿½Å³ï¿½ ï¿½Õ°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ñ´ï¿½.
	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
#ifdef __MOUNT_ENTITY_REFRESH__
	EncodeRemovePacket(this);
#endif
	EncodeInsertPacket(this);

	auto it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

		// Mountï¿½Ñ´Ù°ï¿½ ï¿½Ø¼ï¿½ Client Sideï¿½ï¿½ ï¿½ï¿½Ã¼ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê´Â´ï¿½.
#ifdef __MOUNT_ENTITY_REFRESH__
		EncodeRemovePacket(entity);
		if (!m_bIsObserver)
			EncodeInsertPacket(entity);
#else
		EncodeInsertPacket(entity);
#endif

#ifdef __MOUNT_ENTITY_REFRESH__
		if (!entity->IsObserverMode())
			entity->EncodeInsertPacket(this);
#endif
	}

	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputePoints();
}

namespace
{
	class FuncCheckWarp
	{
	public:
		FuncCheckWarp(LPCHARACTER pkWarp)
		{
			m_lTargetY = 0;
			m_lTargetX = 0;

			m_lX = pkWarp->GetX();
			m_lY = pkWarp->GetY();

			m_bInvalid = false;
			m_bEmpire = pkWarp->GetEmpire();

			char szTmp[64];

			if (3 != sscanf(pkWarp->GetName(), " %s %ld %ld ", szTmp, &m_lTargetX, &m_lTargetY))
			{
				if (number(1, 100) < 5)
					sys_err("Warp NPC name wrong : vnum(%d) name(%s)", pkWarp->GetRaceNum(), pkWarp->GetName());

				m_bInvalid = true;

				return;
			}

			m_lTargetX *= 100;
			m_lTargetY *= 100;

			m_bUseWarp = true;

			if (pkWarp->IsGoto())
			{
				LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(pkWarp->GetMapIndex());
				m_lTargetX += pkSectreeMap->m_setting.iBaseX;
				m_lTargetY += pkSectreeMap->m_setting.iBaseY;
				m_bUseWarp = false;
			}
		}

		bool Valid()
		{
			return !m_bInvalid;
		}

		void operator () (LPENTITY ent)
		{
			if (!Valid())
				return;

			if (!ent->IsType(ENTITY_CHARACTER))
				return;

			LPCHARACTER pkChr = (LPCHARACTER)ent;

			if (!pkChr->IsPC())
				return;

			int iDist = DISTANCE_APPROX(pkChr->GetX() - m_lX, pkChr->GetY() - m_lY);

			if (iDist > 300)
				return;

			if (m_bEmpire && pkChr->GetEmpire() && m_bEmpire != pkChr->GetEmpire())
				return;

			if (pkChr->IsHack())
				return;

			if (!pkChr->CanHandleItem(false, true))
				return;

			if (m_bUseWarp)
				pkChr->WarpSet(m_lTargetX, m_lTargetY);
			else
			{
				pkChr->Show(pkChr->GetMapIndex(), m_lTargetX, m_lTargetY);
				pkChr->Stop();
			}
		}

		bool m_bInvalid;
		bool m_bUseWarp;

		long m_lX;
		long m_lY;
		long m_lTargetX;
		long m_lTargetY;

		BYTE m_bEmpire;
	};
}

EVENTFUNC(warp_npc_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("warp_npc_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) // <Factor>
		return 0;

	if (!ch->GetSectree())
	{
		ch->m_pkWarpNPCEvent = NULL;
		return 0;
	}

	FuncCheckWarp f(ch);
	if (f.Valid())
		ch->GetSectree()->ForEachAround(f);

	return passes_per_sec / 2;
}

void CHARACTER::StartWarpNPCEvent()
{
	if (m_pkWarpNPCEvent)
		return;

	if (!IsWarp() && !IsGoto())
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkWarpNPCEvent = event_create(warp_npc_event, info, passes_per_sec / 2);
}

void CHARACTER::SyncPacket()
{
	TEMP_BUFFER buf;

	TPacketCGSyncPositionElement elem;

	elem.dwVID = GetVID();
	elem.lX = GetX();
	elem.lY = GetY();

	TPacketGCSyncPosition pack;

	pack.bHeader = HEADER_GC_SYNC_POSITION;
	pack.wSize = sizeof(TPacketGCSyncPosition) + sizeof(elem);

	buf.write(&pack, sizeof(pack));
	buf.write(&elem, sizeof(elem));

	PacketAround(buf.read_peek(), buf.size());
}

LPCHARACTER CHARACTER::GetMarryPartner() const
{
	return m_pkChrMarried;
}

void CHARACTER::SetMarryPartner(LPCHARACTER ch)
{
	m_pkChrMarried = ch;
}

int CHARACTER::GetMarriageBonus(DWORD dwItemVnum, bool bSum)
{
	if (IsNPC())
		return 0;

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());

	if (!pMarriage)
		return 0;

	return pMarriage->GetBonus(dwItemVnum, bSum, this);
}

void CHARACTER::ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID)
{
	if (!IsPC())
		return;

	TPacketGCQuestConfirm p;

	p.header = HEADER_GC_QUEST_CONFIRM;
	p.requestPID = dwRequestPID;
	p.timeout = iTimeout;
	strlcpy(p.msg, szMsg, sizeof(p.msg));

	GetDesc()->Packet(&p, sizeof(p));
}

bool CHARACTER::IsWarping() const
{
	return m_bWarping || (GetDesc() ? GetDesc()->IsWarpEvent() : false);
}

int CHARACTER::GetPremiumRemainSeconds(BYTE bType) const
{
	if (bType >= PREMIUM_MAX_NUM)
		return 0;

	return m_aiPremiumTimes[bType] - get_global_time();
}

bool CHARACTER::WarpToPID(DWORD dwPID, bool bWarpForce)
{
	LPCHARACTER victim;
	if ((victim = (CHARACTER_MANAGER::instance().FindByPID(dwPID))))
	{
		int mapIdx = victim->GetMapIndex();
		if (IS_SUMMONABLE_ZONE(mapIdx))
		{
			if (CAN_ENTER_ZONE(this, mapIdx))
			{
				WarpSet(victim->GetX(), victim->GetY());
			}
			else
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot teleport to the player."));
				return false;
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot teleport to the player."));
			return false;
		}
	}
	else
	{
		// ï¿½Ù¸ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Î±ï¿½ï¿½Îµï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ -> ï¿½Þ½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Ç¥ï¿½ï¿½ ï¿½Þ¾Æ¿ï¿½ï¿½ï¿½
		// 1. A.pid, B.pid ï¿½ï¿½ ï¿½Ñ¸ï¿½
		// 2. B.pidï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ñ¸ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ A.pid, ï¿½ï¿½Ç¥ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
		// 3. ï¿½ï¿½ï¿½ï¿½
		CCI* pcci = P2P_MANAGER::instance().FindByPID(dwPID);

		if (!pcci)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This player is not online."));
			return false;
		}

		if ((pcci->bChannel != g_bChannel) && !bWarpForce)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The player is playing on channel %d. (You are on channel %d.)"), pcci->bChannel, g_bChannel);
			return false;
		}
		else if (false == IS_SUMMONABLE_ZONE(pcci->lMapIndex))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot teleport to the player."));
			return false;
		}
		else
		{
			if (!CAN_ENTER_ZONE(this, pcci->lMapIndex))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot teleport to the player."));
				return false;
			}

			TPacketGGFindPosition p;
			p.header = HEADER_GG_FIND_POSITION;
			p.dwFromPID = GetPlayerID();
			p.dwTargetPID = dwPID;
			pcci->pkDesc->Packet(&p, sizeof(TPacketGGFindPosition));

			if (test_server)
				ChatPacket(CHAT_TYPE_PARTY, "sent find position packet for teleport");
		}
	}
	return true;
}

// ADD_REFINE_BUILDING
CGuild* CHARACTER::GetRefineGuild() const
{
	LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);

	return (chRefineNPC ? chRefineNPC->GetGuild() : NULL);
}

bool CHARACTER::IsRefineThroughGuild() const
{
	return GetRefineGuild() != NULL;
}

int CHARACTER::ComputeRefineFee(int iCost, int iMultiply) const
{
	CGuild* pGuild = GetRefineGuild();
	if (pGuild)
	{
		if (pGuild == GetGuild())
			return iCost * iMultiply * 9 / 10;

		// ï¿½Ù¸ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ãµï¿½ï¿½Ï´ï¿½ ï¿½ï¿½ï¿½ ï¿½ß°ï¿½ï¿½ï¿½ 3ï¿½ï¿½ ï¿½ï¿½
		LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);
		if (chRefineNPC && chRefineNPC->GetEmpire() != GetEmpire())
			return iCost * iMultiply * 3;

		return iCost * iMultiply;
	}
	else
		return iCost;
}

void CHARACTER::PayRefineFee(int iTotalMoney)
{
	int iFee = iTotalMoney / 10;
	CGuild* pGuild = GetRefineGuild();

	int iRemain = iTotalMoney;

	if (pGuild)
	{
		// ï¿½Ú±ï¿½ ï¿½ï¿½ï¿½ï¿½Ì¸ï¿½ iTotalMoneyï¿½ï¿½ ï¿½Ì¹ï¿½ 10%ï¿½ï¿½ ï¿½ï¿½ï¿½ÜµÇ¾ï¿½ï¿½Ö´ï¿½
		if (pGuild != GetGuild())
		{
			pGuild->RequestDepositMoney(this, iFee);
			iRemain -= iFee;
		}
	}

	PointChange(POINT_GOLD, -iRemain);
}
// END_OF_ADD_REFINE_BUILDING

// PREVENT_TRADE_WINDOW
bool CHARACTER::PreventTradeWindow(int flags, bool except) const
{
	if (WND_ALL & flags)
	{
		flags = 0x0;
		except = true;
	}

	if (except && !(WND_EXCHANGE & flags) || !except && (WND_EXCHANGE & flags))
		if (GetExchange())
			return true;

	if (except && !(WND_MYSHOP & flags) || !except && (WND_MYSHOP & flags))
		if (GetMyShop())
			return true;

	if (except && !(WND_SHOPOWNER & flags) || !except && (WND_SHOPOWNER & flags))
		if (GetShopOwner())
			return true;

	if (except && !(WND_SAFEBOX & flags) || !except && (WND_SAFEBOX & flags))
		if (IsOpenSafebox())
			return true;

	if (except && !(WND_REFINE & flags) || !except && (WND_REFINE & flags))
		if (IsUnderRefine())
			return true;

	if (except && !(WND_CUBE & flags) || !except && (WND_CUBE & flags))
		if (IsCubeOpen())
			return true;

#if defined(__ACCE_COSTUME_SYSTEM__)
	if (except && !(WND_ACCE_COMBINE & flags) || !except && (WND_ACCE_COMBINE & flags))
		if (isAcceOpened(true /* combination */))
			return true;

	if (except && !(WND_ACCE_ABSORB & flags) || !except && (WND_ACCE_ABSORB & flags))
		if (isAcceOpened(false /* absorption */))
			return true;
#endif

#if defined(__CHANGE_LOOK_SYSTEM__)
	if (except && !(WND_CHANGELOOK & flags) || !except && (WND_CHANGELOOK & flags))
		if (GetChangeLook())
			return true;
#endif

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
	if (except && !(WND_SHOPSEARCH & flags) || !except && (WND_SHOPSEARCH & flags))
		if (GetPrivateShopSearchState() != SHOP_SEARCH_OFF)
			return true;
#endif

#if defined(__MAILBOX__)
	if (except && !(WND_MAILBOX & flags) || !except && (WND_MAILBOX & flags))
		if (GetMailBox())
			return true;
#endif

#if defined(__SOUL_ROULETTE__)
	if (except && !(WND_ROULETTE & flags) || !except && (WND_ROULETTE & flags))
		if (GetSoulRoulette())
			return true;
#endif

#if defined(__BL_67_ATTR__)
	if (except && !(WND_ATTR67ADD & flags) || !except && (WND_ATTR67ADD & flags))
		if (Is67AttrOpen())
			return true;
#endif
#ifdef __AURA_SYSTEM__
	if (except && !(WND_AURA & flags) || !except && (WND_AURA & flags))
		if (IsAuraRefineWindowOpen())
			return true;
#endif

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
	if (except && !(WND_GUILDBANK & flags) || !except && (WND_GUILDBANK & flags))
		if (IsOpenGuildstorage())
			return true;
#endif

	return false;
}
// END_PREVENT_TRADE_WINDOW

// Hack ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ Ã¼Å©.
bool CHARACTER::IsHack(bool bSendMsg, bool bCheckShopOwner, int limittime)
{
	const int iPulse = thecore_pulse();

	if (test_server)
		bSendMsg = true;

	// Ã¢ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ Ã¼Å©
	if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After opening the Storeroom, you cannot go anywhere else for %d seconds."), limittime);

		if (test_server)
			ChatPacket(CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetSafeboxLoadTime(), PASSES_PER_SEC(limittime));
		return true;
	}

	// ï¿½Å·ï¿½ï¿½ï¿½ï¿½ï¿½ Ã¢ Ã¼Å©
	if (bCheckShopOwner)
	{
		if (PreventTradeWindow(WND_ALL))
		{
			if (bSendMsg)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot go elsewhere for a period of time after the trade."));

			return true;
		}
	}
	else
	{
		if (PreventTradeWindow(WND_SHOPOWNER, true/*except*/))
		{
			if (bSendMsg)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot go elsewhere for a period of time after the trade."));

			return true;
		}
	}

	// PREVENT_PORTAL_AFTER_EXCHANGE
	// ï¿½ï¿½È¯ ï¿½ï¿½ ï¿½Ã°ï¿½Ã¼Å©
	if (iPulse - GetExchangeTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After trading you cannot go elsewhere for %d seconds."), limittime);
		return true;
	}
	// END_PREVENT_PORTAL_AFTER_EXCHANGE

	// PREVENT_ITEM_COPY
	if (iPulse - GetMyShopTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After trading you cannot go elsewhere for %d seconds."), limittime);
		return true;
	}

	if (iPulse - GetRefineTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After a trade, you cannot use a scroll for another %d seconds."), limittime);
		return true;
	}

#if defined(__MAILBOX__)
	if (iPulse - GetMyMailBoxTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait %d sec. after opening the Mailbox before you can switch channels or teleport."), limittime);
		return true;
	}
#endif
	// END_PREVENT_ITEM_COPY

	return false;
}

BOOL CHARACTER::IsMonarch() const
{
	// MONARCH_LIMIT
	if (CMonarch::instance().IsMonarch(GetPlayerID(), GetEmpire()))
		return true;

	return false;

	// END_MONARCH_LIMIT
}

void CHARACTER::Say(const std::string& s)
{
	struct ::packet_script packet_script;

	packet_script.header = HEADER_GC_SCRIPT;
	packet_script.skin = 1;
	packet_script.src_size = s.size();
	packet_script.size = packet_script.src_size + sizeof(struct packet_script);

	TEMP_BUFFER buf;

	buf.write(&packet_script, sizeof(struct packet_script));
	buf.write(&s[0], s.size());

	if (IsPC())
	{
		GetDesc()->Packet(buf.read_peek(), buf.size());
	}
}

//
// Monarch
//
void CHARACTER::InitMC()
{
	for (int n = 0; n < MI_MAX; ++n)
	{
		m_dwMonarchCooltime[n] = thecore_pulse();
	}

	m_dwMonarchCooltimelimit[MI_HEAL] = PASSES_PER_SEC(MC_HEAL);
	m_dwMonarchCooltimelimit[MI_WARP] = PASSES_PER_SEC(MC_WARP);
	m_dwMonarchCooltimelimit[MI_TRANSFER] = PASSES_PER_SEC(MC_TRANSFER);
	m_dwMonarchCooltimelimit[MI_TAX] = PASSES_PER_SEC(MC_TAX);
	m_dwMonarchCooltimelimit[MI_SUMMON] = PASSES_PER_SEC(MC_SUMMON);

	m_dwMonarchCooltime[MI_HEAL] -= PASSES_PER_SEC(GetMCL(MI_HEAL));
	m_dwMonarchCooltime[MI_WARP] -= PASSES_PER_SEC(GetMCL(MI_WARP));
	m_dwMonarchCooltime[MI_TRANSFER] -= PASSES_PER_SEC(GetMCL(MI_TRANSFER));
	m_dwMonarchCooltime[MI_TAX] -= PASSES_PER_SEC(GetMCL(MI_TAX));
	m_dwMonarchCooltime[MI_SUMMON] -= PASSES_PER_SEC(GetMCL(MI_SUMMON));
}

DWORD CHARACTER::GetMC(enum MONARCH_INDEX e) const
{
	return m_dwMonarchCooltime[e];
}

void CHARACTER::SetMC(enum MONARCH_INDEX e)
{
	m_dwMonarchCooltime[e] = thecore_pulse();
}

bool CHARACTER::IsMCOK(enum MONARCH_INDEX e) const
{
	int iPulse = thecore_pulse();

	if ((iPulse - GetMC(e)) < GetMCL(e))
	{
		if (test_server)
			sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

		return false;
	}

	if (test_server)
		sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

	return true;
}

DWORD CHARACTER::GetMCL(enum MONARCH_INDEX e) const
{
	return m_dwMonarchCooltimelimit[e];
}

DWORD CHARACTER::GetMCLTime(enum MONARCH_INDEX e) const
{
	int iPulse = thecore_pulse();

	if (test_server)
		sys_log(0, " Pulse %d cooltime %d, limit %d", iPulse, GetMC(e), GetMCL(e));

	return (GetMCL(e)) / passes_per_sec - (iPulse - GetMC(e)) / passes_per_sec;
}

bool CHARACTER::IsSiegeNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

//------------------------------------------------
void CHARACTER::UpdateDepositPulse()
{
#ifdef ENABLE_GUILDRENEWAL_SYSTEM
	m_deposit_pulse = thecore_pulse() + PASSES_PER_SEC(10);	//10sec
#else
	m_deposit_pulse = thecore_pulse() + PASSES_PER_SEC(60 * 5);	//5min
#endif
}

bool CHARACTER::CanDeposit() const
{
	return (m_deposit_pulse == 0 || (m_deposit_pulse < thecore_pulse()));
}
//------------------------------------------------

ESex GET_SEX(LPCHARACTER ch)
{
	switch (ch->GetRaceNum())
	{
	case MAIN_RACE_WARRIOR_M:
	case MAIN_RACE_SURA_M:
	case MAIN_RACE_ASSASSIN_M:
	case MAIN_RACE_SHAMAN_M:
	case MAIN_RACE_WOLFMAN_M:
		return SEX_MALE;

	case MAIN_RACE_ASSASSIN_W:
	case MAIN_RACE_SHAMAN_W:
	case MAIN_RACE_WARRIOR_W:
	case MAIN_RACE_SURA_W:
		return SEX_FEMALE;

	}

	/* default sex = male */
	return SEX_MALE;
}

#if defined(__CONQUEROR_LEVEL__)
int64_t CHARACTER::GetMaxHP() const
{
	int64_t iMaxHP = m_pointsInstant.iMaxHP;
	if (GetSungMaWill(POINT_SUNGMA_HP) > GetPoint(POINT_SUNGMA_HP))
		if (iMaxHP > 0) iMaxHP /= 2;
	return iMaxHP;
}
#endif

int CHARACTER::GetHPPct() const
{
	if (GetMaxHP() <= 0)
		return 0;

	return (GetHP() * 100) / GetMaxHP();
}

bool CHARACTER::IsBerserk() const
{
	if (m_pkMobInst != NULL)
		return m_pkMobInst->m_IsBerserk;
	else
		return false;
}

void CHARACTER::SetBerserk(bool mode)
{
	if (m_pkMobInst != NULL)
		m_pkMobInst->m_IsBerserk = mode;
}

bool CHARACTER::IsGodSpeed() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsGodSpeed;
	}
	else
	{
		return false;
	}
}

void CHARACTER::SetGodSpeed(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsGodSpeed = mode;

		if (mode == true)
		{
			SetPoint(POINT_ATT_SPEED, 250);
		}
		else
		{
			SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		}
	}
}

bool CHARACTER::IsDeathBlow() const
{
	if (number(1, 100) <= m_pkMobData->m_table.bDeathBlowPoint)
	{
		return true;
	}
	else
	{
		return false;
	}
}

struct FFindReviver
{
	FFindReviver()
	{
		pChar = NULL;
		HasReviver = false;
	}

	void operator() (LPCHARACTER ch)
	{
		if (ch->IsMonster() != true)
		{
			return;
		}

		if (ch->IsReviver() == true && pChar != ch && ch->IsDead() != true)
		{
			if (number(1, 100) <= ch->GetMobTable().bRevivePoint)
			{
				HasReviver = true;
				pChar = ch;
			}
		}
	}

	LPCHARACTER pChar;
	bool HasReviver;
};

bool CHARACTER::HasReviverInParty() const
{
	LPPARTY party = GetParty();

	if (party != NULL)
	{
		if (party->GetMemberCount() == 1) return false;

		FFindReviver f;
		party->ForEachMemberPtr(f);
		return f.HasReviver;
	}

	return false;
}

bool CHARACTER::IsRevive() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsRevive;
	}

	return false;
}

void CHARACTER::SetRevive(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsRevive = mode;
	}
}

#define IS_SPEED_HACK_PLAYER(ch) (ch->m_speed_hack_count > SPEEDHACK_LIMIT_COUNT)

EVENTFUNC(check_speedhack_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("check_speedhack_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (NULL == ch || ch->IsNPC())
		return 0;

	if (IS_SPEED_HACK_PLAYER(ch))
	{
		// Write hack log
		LogManager::instance().SpeedHackLog(ch->GetPlayerID(), ch->GetX(), ch->GetY(), ch->m_speed_hack_count);

		//if (false == LC_IsEurope())
		{
			// Close connection
			LPDESC desc = ch->GetDesc();

			if (desc)
			{
				DESC_MANAGER::instance().DestroyDesc(desc);
				return 0;
			}
		}
	}

	ch->m_speed_hack_count = 0;

	ch->ResetComboHackCount();
	return PASSES_PER_SEC(60);
}

void CHARACTER::StartCheckSpeedHackEvent()
{
	if (m_pkCheckSpeedHackEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkCheckSpeedHackEvent = event_create(check_speedhack_event, info, PASSES_PER_SEC(60)); // 1ï¿½ï¿½
}

void CHARACTER::GoHome()
{
	WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));
}

void CHARACTER::SendGuildName(CGuild* pGuild)
{
	if (NULL == pGuild) return;

	DESC* desc = GetDesc();

	if (NULL == desc) return;
	if (m_known_guild.find(pGuild->GetID()) != m_known_guild.end()) return;

	m_known_guild.insert(pGuild->GetID());

	TPacketGCGuildName pack;
	memset(&pack, 0x00, sizeof(pack));

	pack.header = HEADER_GC_GUILD;
	pack.subheader = GUILD_SUBHEADER_GC_GUILD_NAME;
	pack.size = sizeof(TPacketGCGuildName);
	pack.guildID = pGuild->GetID();
	memcpy(pack.guildName, pGuild->GetName(), GUILD_NAME_MAX_LEN);

	desc->Packet(&pack, sizeof(pack));
}

void CHARACTER::SendGuildName(DWORD dwGuildID)
{
	SendGuildName(CGuildManager::instance().FindGuild(dwGuildID));
}

EVENTFUNC(destroy_when_idle_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("destroy_when_idle_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL) // <Factor>
		return 0;

	if (ch->GetVictim())
		return PASSES_PER_SEC(300);

	sys_log(1, "DESTROY_WHEN_IDLE: %s", ch->GetName());

	ch->m_pkDestroyWhenIdleEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::StartDestroyWhenIdleEvent()
{
	if (m_pkDestroyWhenIdleEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkDestroyWhenIdleEvent = event_create(destroy_when_idle_event, info, PASSES_PER_SEC(300));
}

void CHARACTER::SetComboSequence(BYTE seq)
{
	m_bComboSequence = seq;
}

BYTE CHARACTER::GetComboSequence() const
{
	return m_bComboSequence;
}

void CHARACTER::SetLastComboTime(DWORD time)
{
	m_dwLastComboTime = time;
}

DWORD CHARACTER::GetLastComboTime() const
{
	return m_dwLastComboTime;
}

void CHARACTER::SetValidComboInterval(int interval)
{
	m_iValidComboInterval = interval;
}

int CHARACTER::GetValidComboInterval() const
{
	return m_iValidComboInterval;
}

BYTE CHARACTER::GetComboIndex() const
{
	return m_bComboIndex;
}

void CHARACTER::IncreaseComboHackCount(int k)
{
	m_iComboHackCount += k;

	if (m_iComboHackCount >= 10)
	{
		if (GetDesc())
			if (GetDesc()->DelayedDisconnect(number(2, 7)))
			{
				sys_log(0, "COMBO_HACK_DISCONNECT: %s count: %d", GetName(), m_iComboHackCount);
				LogManager::instance().HackLog("Combo", this);
			}
	}
}

void CHARACTER::ResetComboHackCount()
{
	m_iComboHackCount = 0;
}

void CHARACTER::SkipComboAttackByTime(int interval)
{
	m_dwSkipComboAttackByTime = get_dword_time() + interval;
}

DWORD CHARACTER::GetSkipComboAttackByTime() const
{
	return m_dwSkipComboAttackByTime;
}

// ï¿½ï¿½ï¿½Ì³ï¿½ ï¿½Ù¸ï¿½ï¿½ï¿½ï¿½ï¿½ Å¸ï¿½ï¿½ ï¿½Ö³ï¿½?
bool CHARACTER::IsRiding() const
{
	return IsHorseRiding() || GetMountVnum();
}

bool CHARACTER::CanWarp() const
{
	const int iPulse = thecore_pulse();
	const int limit_time = PASSES_PER_SEC(g_nPortalLimitTime);

	if ((iPulse - GetSafeboxLoadTime()) < limit_time)
		return false;

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
	if ((iPulse - GetGuildstorageLoadTime()) < limit_time)
		return false;
#endif

	if ((iPulse - GetExchangeTime()) < limit_time)
		return false;

	if ((iPulse - GetMyShopTime()) < limit_time)
		return false;

	if ((iPulse - GetRefineTime()) < limit_time)
		return false;

#ifdef __ENABLE_NEW_OFFLINESHOP__
	if (iPulse - GetOfflineShopUseTime() < limit_time)
		return false;

	//if (GetOfflineShopGuest() || GetAuctionGuest())
		//return false;
#endif

#if defined(__MAILBOX__)
	if ((iPulse - GetMyMailBoxTime()) < limit_time)
		return false;
#endif

	if (PreventTradeWindow(WND_ALL))
		return false;

	return true;
}

DWORD CHARACTER::GetNextExp() const
{
	if (PLAYER_EXP_TABLE_MAX < GetLevel())
		return 2500000000;
	else
		return exp_table[GetLevel()];
}

#if defined(__CONQUEROR_LEVEL__)
DWORD CHARACTER::GetConquerorNextExp() const
{
	if (PLAYER_CONQUEROR_EXP_TABLE_MAX < GetConquerorLevel())
		return 2500000000;
	else
		return conqueror_exp_table[GetConquerorLevel()];
}
#endif

int CHARACTER::GetSkillPowerByLevel(int level, bool bMob) const
{
	return CTableBySkill::instance().GetSkillPowerByLevelFromType(GetJob(), GetSkillGroup(), MINMAX(0, level, SKILL_MAX_LEVEL), bMob);
}

#if defined(__ACCE_COSTUME_SYSTEM__)
bool CHARACTER::IsOpenAcce()
{
	if (m_bAcceCombination || m_bAcceAbsorption)
		return true;

	return false;
}

void CHARACTER::OpenAcce(bool bCombination)
{
	if (isAcceOpened(bCombination))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Drag the items into the window."));
		return;
	}

	if (bCombination)
	{
		if (m_bAcceAbsorption)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item while the windows for absorption and combination are open."));
			return;
		}

		m_bAcceCombination = true;
	}
	else
	{
		if (m_bAcceCombination)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item while the windows for absorption and combination are open."));
			return;
		}

		m_bAcceAbsorption = true;
	}

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_OPEN;
	sPacket.bWindow = bCombination;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));

	ClearAcceMaterials();
}

void CHARACTER::CloseAcce()
{
	if ((!m_bAcceCombination) && (!m_bAcceAbsorption))
		return;

	bool bWindow = (m_bAcceCombination == true ? true : false);

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_CLOSE;
	sPacket.bWindow = bWindow;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));

	if (bWindow)
		m_bAcceCombination = false;
	else
		m_bAcceAbsorption = false;

	ClearAcceMaterials();
}

void CHARACTER::ClearAcceMaterials()
{
	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();
	for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			continue;

		pkItemMaterial[i]->Lock(false);
		pkItemMaterial[i] = NULL;
	}
}

bool CHARACTER::AcceIsSameGrade(long lGrade)
{
	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();
	if (!pkItemMaterial[0])
		return false;

	bool bReturn = (pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD) == lGrade ? true : false);
	return bReturn;
}

DWORD CHARACTER::GetAcceCombinePrice(long lGrade)
{
	DWORD dwPrice = 0;
	switch (lGrade)
	{
	case 2:
		dwPrice = ACCE_GRADE_2_PRICE;
		break;

	case 3:
		dwPrice = ACCE_GRADE_3_PRICE;
		break;

	case 4:
		dwPrice = ACCE_GRADE_4_PRICE;
		break;

	default:
		dwPrice = ACCE_GRADE_1_PRICE;
		break;
	}

	return dwPrice;
}

BYTE CHARACTER::CheckEmptyMaterialSlot()
{
	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();
	for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			return i;
	}

	return 255;
}

void CHARACTER::GetAcceCombineResult(DWORD& dwItemVnum, DWORD& dwMinAbs, DWORD& dwMaxAbs)
{
	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();

	if (m_bAcceCombination)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			long lVal = pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD);
			if (lVal == 4)
			{
				dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
				dwMinAbs = pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET);
				DWORD dwMaxAbsCalc = (dwMinAbs + ACCE_GRADE_4_ABS_RANGE > ACCE_GRADE_4_ABS_MAX ? ACCE_GRADE_4_ABS_MAX : (dwMinAbs + ACCE_GRADE_4_ABS_RANGE));
				dwMaxAbs = dwMaxAbsCalc;
			}
			else
			{
				DWORD dwMaskVnum = pkItemMaterial[0]->GetOriginalVnum();
				TItemTable* pTable = ITEM_MANAGER::instance().GetTable(dwMaskVnum + 1);
				if (pTable)
					dwMaskVnum += 1;

				dwItemVnum = dwMaskVnum;
				switch (lVal)
				{
				case 2:
				{
					dwMinAbs = ACCE_GRADE_3_ABS;
					dwMaxAbs = ACCE_GRADE_3_ABS;
				}
				break;
				case 3:
				{
					dwMinAbs = ACCE_GRADE_4_ABS_MIN;
					dwMaxAbs = ACCE_GRADE_4_ABS_MAX_COMB;
				}
				break;
				default:
				{
					dwMinAbs = ACCE_GRADE_2_ABS;
					dwMaxAbs = ACCE_GRADE_2_ABS;
				}
				break;
				}
			}
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
	else
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
			dwMinAbs = pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET);
			dwMaxAbs = dwMinAbs;
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
}

void CHARACTER::AddAcceMaterial(TItemPos tPos, BYTE bPos)
{
	if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
	{
		if (bPos == 255)
		{
			bPos = CheckEmptyMaterialSlot();
			if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
				return;
		}
		else
			return;
	}

	LPITEM pkItem = GetItem(tPos);
	if (!pkItem)
		return;

	if ((pkItem->GetCell() >= INVENTORY_MAX_NUM) || (pkItem->IsEquipped()) || (tPos.IsBeltInventoryPosition()) || (pkItem->IsDragonSoul()))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME) && (m_bAcceCombination))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME) && (bPos == 0) && (m_bAcceAbsorption))
		return;

	if (pkItem->isLocked())
		return;

#if defined(__SOUL_BIND_SYSTEM__)
	if (pkItem->IsSealed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use a soulbound item."));
		return;
	}
#endif

	if ((m_bAcceCombination) && (bPos == 1) && (!AcceIsSameGrade(pkItem->GetValue(ACCE_GRADE_VALUE_FIELD))))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("These sashes cannot be combined."));
		return;
	}

	if ((m_bAcceCombination) && (pkItem->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_GRADE_4_ABS_MAX))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The sash cannot take on any further bonuses."));
		return;
	}

	if ((bPos == 1) && (m_bAcceAbsorption))
	{
		if ((pkItem->GetType() != ITEM_WEAPON) && (pkItem->GetType() != ITEM_ARMOR))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only use Shoulder Sashes as well as weapons and armour pieces."));
			return;
		}
		else if ((pkItem->GetType() == ITEM_ARMOR) && (pkItem->GetSubType() != ARMOR_BODY))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only use Shoulder Sashes as well as weapons and armour pieces."));
			return;
		}
	}
	else if ((pkItem->GetSubType() != COSTUME_ACCE) && (m_bAcceCombination))
		return;
	else if ((pkItem->GetSubType() != COSTUME_ACCE) && (bPos == 0) && (m_bAcceAbsorption))
		return;
	else if ((pkItem->GetSocket(ACCE_ABSORBED_SOCKET) > 0) && (bPos == 0) && (m_bAcceAbsorption))
		return;

	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();
	if ((bPos == 1) && (!pkItemMaterial[0]))
		return;

	if (pkItemMaterial[bPos])
		return;

	pkItemMaterial[bPos] = pkItem;
	pkItemMaterial[bPos]->Lock(true);

	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetAcceCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_ADDED;
	sPacket.bWindow = m_bAcceCombination == true ? true : false;
	sPacket.dwPrice = GetAcceCombinePrice(pkItem->GetValue(ACCE_GRADE_VALUE_FIELD));
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = dwItemVnum;
	sPacket.dwMinAbs = dwMinAbs;
	sPacket.dwMaxAbs = dwMaxAbs;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
}

void CHARACTER::RemoveAcceMaterial(BYTE bPos)
{
	if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
		return;

	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();

	DWORD dwPrice = 0;

	if (bPos == 1)
	{
		if (pkItemMaterial[bPos])
		{
			pkItemMaterial[bPos]->Lock(false);
			pkItemMaterial[bPos] = NULL;
		}

		if (pkItemMaterial[0])
			dwPrice = GetAcceCombinePrice(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));
	}
	else
		ClearAcceMaterials();

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_REMOVED;
	sPacket.bWindow = m_bAcceCombination == true ? true : false;
	sPacket.dwPrice = dwPrice;
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
}

BYTE CHARACTER::CanRefineAcceMaterials()
{

	if (GetOfflineShopGuest() || GetAuctionGuest())
		return 0;

	if (GetExchange() || IsOpenSafebox() || GetShopOwner() || IsCubeOpen())
		return 0;

	BYTE bReturn = 0;
	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();
	if (m_bAcceCombination)
	{
		for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
		{
			if (pkItemMaterial[i])
			{
				if ((pkItemMaterial[i]->GetType() == ITEM_COSTUME) && (pkItemMaterial[i]->GetSubType() == COSTUME_ACCE))
					bReturn = 1;
				else
				{
					bReturn = 0;
					break;
				}
			}
			else
			{
				bReturn = 0;
				break;
			}
		}
	}
	else if (m_bAcceAbsorption)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			if ((pkItemMaterial[0]->GetType() == ITEM_COSTUME) && (pkItemMaterial[0]->GetSubType() == COSTUME_ACCE))
				bReturn = 2;
			else
				bReturn = 0;

			if ((pkItemMaterial[1]->GetType() == ITEM_WEAPON) || ((pkItemMaterial[1]->GetType() == ITEM_ARMOR) && (pkItemMaterial[1]->GetSubType() == ARMOR_BODY)))
				bReturn = 2;
			else
				bReturn = 0;

			if (pkItemMaterial[0]->GetSocket(ACCE_ABSORBED_SOCKET) > 0)
				bReturn = 0;
		}
		else
			bReturn = 0;
	}

	return bReturn;
}

void CHARACTER::RefineAcceMaterials()
{
	if (PreventTradeWindow(WND_ACCE_COMBINE | WND_ACCE_ABSORB, true /*except*/))
	{
		CloseAcce();
		return;
	}

	BYTE bCan = CanRefineAcceMaterials();
	if (bCan == 0)
		return;

	LPITEM* pkItemMaterial;
	pkItemMaterial = GetAcceMaterials();

	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetAcceCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);
	unsigned long long llPrice = GetAcceCombinePrice(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));

	if (bCan == 1)
	{
		int iSuccessChance = 0;
		long lVal = pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD);
		switch (lVal)
		{
		case 2:
			iSuccessChance = ACCE_COMBINE_GRADE_2;
			break;

		case 3:
			iSuccessChance = ACCE_COMBINE_GRADE_3;
			break;

		case 4:
			iSuccessChance = ACCE_COMBINE_GRADE_4;
			break;

		default:
			iSuccessChance = ACCE_COMBINE_GRADE_1;
			break;
		}

		if (GetGold() < llPrice)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have enough Yang."));
			return;
		}

		int iChance = number(1, 100);
		bool bSucces = (iChance <= iSuccessChance ? true : false);

		if (bSucces)
		{
			LPITEM pkItem = ITEM_MANAGER::instance().CreateItem(dwItemVnum, 1, 0, false);
			if (!pkItem)
			{
				sys_err("%d can't be created.", dwItemVnum);
				return;
			}

			ITEM_MANAGER::CopyAllAttrTo(pkItemMaterial[0], pkItem);
			LogManager::instance().ItemLog(this, pkItem, "COMBINE SUCCESS", pkItem->GetName());
			DWORD dwAbs = (dwMinAbs == dwMaxAbs ? dwMinAbs : number(dwMinAbs + 1, dwMaxAbs));

			if (g_bAcceCombineIncAbsOnLastGrade)
			{
				if (lVal == 4)
					dwAbs = pkItem->GetSocket(ACCE_ABSORPTION_SOCKET) + 1;
			}

			pkItem->SetSocket(ACCE_ABSORPTION_SOCKET, dwAbs);
			pkItem->SetSocket(ACCE_ABSORBED_SOCKET, pkItemMaterial[0]->GetSocket(ACCE_ABSORBED_SOCKET));

			PointChange(POINT_GOLD, -llPrice);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -llPrice);

			WORD wCell = pkItemMaterial[0]->GetCell();
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[0], "COMBINE (REFINE SUCCESS)");
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "COMBINE (REFINE SUCCESS)");

			pkItem->AddToCharacter(this, TItemPos(INVENTORY, wCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
			pkItem->AttrLog();

			if (lVal == 4)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("New absorption rate: %d"), dwAbs);
			else
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Success!"));

			EffectPacket(SE_ACCE_SUCESS_ABSORB);
			LogManager::instance().AcceLog(GetPlayerID(), GetX(), GetY(), dwItemVnum, pkItem->GetID(), 1, dwAbs, 1);

			ClearAcceMaterials();
		}
		else
		{
			PointChange(POINT_GOLD, -llPrice);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -llPrice);

			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "COMBINE (REFINE FAIL)");

			if (lVal == 4)
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("New absorption rate: %d"), pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET));
			else
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Failed!"));

			LogManager::instance().AcceLog(GetPlayerID(), GetX(), GetY(), dwItemVnum, 0, 0, 0, 0);

			pkItemMaterial[1] = NULL;
		}

		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;

		TPacketAcce sPacket;
		sPacket.header = HEADER_GC_ACCE;
		sPacket.subheader = ACCE_SUBHEADER_CG_REFINED;
		sPacket.bWindow = m_bAcceCombination == true ? true : false;
		sPacket.dwPrice = llPrice;
		sPacket.bPos = 0;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		if (bSucces)
			sPacket.dwMaxAbs = 100;
		else
			sPacket.dwMaxAbs = 0;

		GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
	}
	else
	{
		pkItemMaterial[1]->CopyAttributeTo(pkItemMaterial[0]);
		LogManager::instance().ItemLog(this, pkItemMaterial[0], "ABSORB (REFINE SUCCESS)", pkItemMaterial[0]->GetName());
		pkItemMaterial[0]->SetSocket(ACCE_ABSORBED_SOCKET, pkItemMaterial[1]->GetOriginalVnum());
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (pkItemMaterial[0]->GetAttributeValue(i) < 0)
				pkItemMaterial[0]->SetForceAttribute(i, pkItemMaterial[0]->GetAttributeType(i), 0);
		}

#if defined(__ITEM_APPLY_RANDOM__)
		if (pkItemMaterial[0] && pkItemMaterial[1])
		{
			for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
			{
				TItemTable* pkItemAbsorbed = ITEM_MANAGER::Instance().GetTable(pkItemMaterial[1]->GetVnum());
				if (pkItemAbsorbed->aApplies[i].bType == APPLY_RANDOM)
				{
					BYTE byRandomType = pkItemMaterial[1]->GetRandomApply(i).bType;
					short sRandomValue = pkItemMaterial[1]->GetRandomApply(i).sValue;
					pkItemMaterial[0]->SetForceRandomApply(i, byRandomType, sRandomValue, 0);
				}
			}
		}
#endif

#ifdef ENABLE_SET_ITEM
		pkItemMaterial[0]->SetItemSetValue(pkItemMaterial[0]->GetItemSetValue());
#endif

		//ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GetElementGrade: %s GetElementAttacks: %s GetElementsType: %s GetElementsValues: %s!"), pkItemMaterial[1]->GetElementGrade(), pkItemMaterial[1]->GetElementAttacks(), pkItemMaterial[1]->GetElementsType(), pkItemMaterial[1]->GetElementsValues());

#ifdef ENABLE_REFINE_ELEMENT
		pkItemMaterial[0]->SetElement(pkItemMaterial[1]->GetElementGrade(),
			pkItemMaterial[1]->GetElementAttacks(),
			pkItemMaterial[1]->GetElementsType(),
			pkItemMaterial[1]->GetElementsValues());
#endif

		ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "ABSORBED (REFINE SUCCESS)");

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemMaterial[0]);
		pkItemMaterial[0]->AttrLog();

		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Success!"));

		ClearAcceMaterials();

		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;

		TPacketAcce sPacket;
		sPacket.header = HEADER_GC_ACCE;
		sPacket.subheader = ACCE_SUBHEADER_CG_REFINED;
		sPacket.bWindow = m_bAcceCombination == true ? true : false;
		sPacket.dwPrice = llPrice;
		sPacket.bPos = 255;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		sPacket.dwMaxAbs = 1;
		GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
	}
}

bool CHARACTER::CleanAcceAttr(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
		return false;

	if ((!pkItem) || (!pkTarget))
		return false;

	if (!pkTarget->IsCostumeAcce())
		return false;

#if defined(__SOUL_BIND_SYSTEM__)
	if ((pkTarget->IsSealed()))
		return false;
#endif

	if (pkTarget->isLocked())
		return false;

	if (pkTarget->GetSocket(ACCE_ABSORBED_SOCKET) <= 0)
		return false;

	pkTarget->SetSocket(ACCE_ABSORBED_SOCKET, 0);
	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		pkTarget->SetForceAttribute(i, 0, 0);

	pkItem->SetCount(pkItem->GetCount() - 1);
	LogManager::instance().ItemLog(this, pkTarget, "USE_ITEM (ACCE REVERSE ATTR)", pkTarget->GetName());
	return true;
}
#endif

#if defined(__SORT_INVENTORY_ITEMS__)
static bool SortItems(const LPITEM& s1, const LPITEM& s2)
{
	std::string name(s1->GetLocaleName());
	std::string name2(s2->GetLocaleName());

	return name < name2;
}

void CHARACTER::SortInventoryItems()
{
	if (IsDead() || PreventTradeWindow(WND_ALL))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please close other windows before sorting your inventory."));
		return;
	}

	if (GetSortInventoryPulse() > get_global_time())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait %d seconds to sort your inventory again."), GetSortInventoryPulse() - get_global_time());
		return;
	}

	//< 2020.08.08.Owsap - Prevent sorting items while running quest.
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return;

	if (m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been cancelled."));
		event_cancel(&m_pkTimedEvent);
		// Cancel timer event and continue with sorting...
	}
	//>

	std::vector<LPITEM> vec_pkItems;
	LPITEM pkItem;
	const auto size = static_cast<WORD>(INVENTORY_MAX_NUM);

	for (WORD i = 0; i < size; ++i)
	{
		if (pkItem = GetInventoryItem(i))
		{
			vec_pkItems.emplace_back(pkItem);
			pkItem->RemoveFromCharacter();
			SyncQuickslot(QUICKSLOT_TYPE_ITEM, static_cast<BYTE>(i), WORD_MAX);
		}
	}

	std::sort(vec_pkItems.begin(), vec_pkItems.end(), SortItems);

	for (const auto& item : vec_pkItems)
	{
		const auto itemTable = ITEM_MANAGER::instance().GetTable(item->GetVnum());
		if (!itemTable)
			continue;

		static const std::initializer_list<DWORD> excepList = {
			ITEM_AUTO_HP_RECOVERY_S,
			ITEM_AUTO_HP_RECOVERY_M,
			ITEM_AUTO_HP_RECOVERY_L,
			ITEM_AUTO_HP_RECOVERY_X,
			ITEM_AUTO_SP_RECOVERY_S,
			ITEM_AUTO_SP_RECOVERY_M,
			ITEM_AUTO_SP_RECOVERY_L,
			ITEM_AUTO_SP_RECOVERY_X,
			REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S,
			REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS,
			REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S,
			REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S,
			REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS,
			FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S,
			FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S
		};

		if (itemTable->dwFlags & ITEM_FLAG_STACKABLE && itemTable->bType != ITEM_BLEND &&
			(std::find(excepList.begin(), excepList.end(), item->GetVnum()) == excepList.end() && IS_SUMMON_ITEM(item->GetVnum()) == false))
		{
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
			AutoGiveItem(item->GetVnum(), item->GetCount(), -1, false, false);
#else
			AutoGiveItem(item->GetVnum(), item->GetCount(), -1, false);
#endif
			M2_DESTROY_ITEM(item);
		}
		else
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
			AutoGiveItem(item, false, false);
#else
			AutoGiveItem(item, false);
#endif
	};

	SetSortInventoryPulse(get_global_time() + 15);
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your inventory items have been sorted."));
}

#if defined(__SPECIAL_INVENTORY_SYSTEM__)
void CHARACTER::SortSpecialInventoryItems(BYTE type)
{
	if (IsDead() || PreventTradeWindow(WND_ALL))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please close other windows before sorting your inventory."));
		return;
	}

	if (GetSortSpecialInventoryPulse() > get_global_time())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please wait %d seconds to sort your inventory again."), GetSortSpecialInventoryPulse() - get_global_time());
		return;
	}

	//< 2020.08.08.Owsap - Prevent sorting items while running quest and m_pkTimedEvent
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return;

	if (m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been cancelled."));
		event_cancel(&m_pkTimedEvent);
		// Cancel timer event and continue with sorting...
	}
	//>

	std::vector<LPITEM> vec_pkItems;
	LPITEM pkItem;
	const auto size = static_cast<WORD>(INVENTORY_MAX_NUM);

	switch (type)
	{
	case 0:
		for (WORD i = SKILL_BOOK_INVENTORY_SLOT_START; i < SKILL_BOOK_INVENTORY_SLOT_END; ++i)
		{
			if (pkItem = GetSkillBookInventoryItem(i))
			{
				vec_pkItems.emplace_back(pkItem);
				pkItem->RemoveFromCharacter();
			}
		}
		break;

	case 1:
		for (WORD i = UPGRADE_ITEMS_INVENTORY_SLOT_START; i < UPGRADE_ITEMS_INVENTORY_SLOT_END; ++i)
		{
			if (pkItem = GetUpgradeItemsInventoryItem(i))
			{
				vec_pkItems.emplace_back(pkItem);
				pkItem->RemoveFromCharacter();
			}
		}
		break;

	case 2:
		for (WORD i = STONE_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
		{
			if (pkItem = GetStoneInventoryItem(i))
			{
				vec_pkItems.emplace_back(pkItem);
				pkItem->RemoveFromCharacter();
			}
		}
		break;

	case 3:
		for (WORD i = GIFT_BOX_INVENTORY_SLOT_START; i < GIFT_BOX_INVENTORY_SLOT_END; ++i)
		{
			if (pkItem = GetGiftBoxInventoryItem(i))
			{
				vec_pkItems.emplace_back(pkItem);
				pkItem->RemoveFromCharacter();
			}
		}
		break;

	case 4:
		for (WORD i = ATTRIBUTE_INVENTORY_SLOT_START; i < ATTRIBUTE_INVENTORY_SLOT_END; ++i)
		{
			if (pkItem = GetAttributeInventoryItem(i))
			{
				vec_pkItems.emplace_back(pkItem);
				pkItem->RemoveFromCharacter();
			}
		}
		break;

	default:
		for (WORD i = SKILL_BOOK_INVENTORY_SLOT_START; i < SKILL_BOOK_INVENTORY_SLOT_END; ++i)
		{
			if (pkItem = GetSkillBookInventoryItem(i))
			{
				vec_pkItems.emplace_back(pkItem);
				pkItem->RemoveFromCharacter();
			}
		}
		break;
	}

	std::sort(vec_pkItems.begin(), vec_pkItems.end(), SortItems);

	for (const auto& item : vec_pkItems)
	{
		const auto itemTable = ITEM_MANAGER::instance().GetTable(item->GetVnum());
		if (!itemTable)
			continue;

		if (itemTable->dwFlags & ITEM_FLAG_STACKABLE && itemTable->bType != ITEM_BLEND)
		{
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
			//< 2020.08.08.Owsap - Fix book stacking while sorting.
			if (item->GetType() == ITEM_SKILLBOOK || item->GetType() == ITEM_SKILLFORGET)
			{
				GiveSkillBook(item->GetSocket(0), item->GetCount());
			}
			//>
			else
				AutoGiveItem(item->GetVnum(), item->GetCount(), -1, false, false);
#else
			AutoGiveItem(item->GetVnum(), item->GetCount(), -1, false);
#endif
			M2_DESTROY_ITEM(item);
		}
		else
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
			AutoGiveItem(item, false, false);
#else
			AutoGiveItem(item, false);
#endif
	};

	SetSortSpecialInventoryPulse(get_global_time() + 15);
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your special inventory items have been sorted."));
}
#endif
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
#ifdef ENABLE_PET_SUMMON_AFTER_REWARP
//EVENTFUNC(summon_growth_pet_event);
// void CHARACTER::SetSummonGrowthPetWithDelay(LPITEM pPetItem)
// {
// 	if (!pPetItem)
// 		return;

// 	SummonGrowthPetEventInfo* info = AllocEventInfo<SummonGrowthPetEventInfo>(); // Corectat
// 	info->ch = this;
// 	info->pPetItem = pPetItem;

// 	event_create(summon_growth_pet_event, info, PASSES_PER_SEC(5)); // 1 secundă delay
// }

EVENTFUNC(summon_growth_pet_event)
{
	GiveBuffEventInfo* info = static_cast<GiveBuffEventInfo*>(event->info);
	if (!info)
		return 0;

	LPCHARACTER ch = info->ch;
	CGrowthPetSystemActor* petActor = info->petActor;

	if (ch && petActor)
	{
		petActor->GiveBuff();
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Buff-urile au fost aplicate pet-ului."));
	}

	return 0; // Evenimentul se termină
}
// {

// 	SummonGrowthPetEventInfo* info = static_cast<SummonGrowthPetEventInfo*>(event->info);
// 	if (!info)
// 		return 0;

// 	LPCHARACTER ch = info->ch;
// 	LPITEM pPetItem = info->pPetItem;

// 	if (ch && pPetItem)
// 	{
// 		ch->SetSummonGrowthPet(pPetItem);
// 		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("AM INVIAT PETUL"));
// 	}

// 	return 0; // Evenimentul se termină după execuție
// }
//void CHARACTER::SetSummonGrowthPet(LPITEM pPetItem)
//{
//	if (!pPetItem)
//		return;
//
//	if (pPetItem->GetType() != ITEM_PET || pPetItem->GetSubType() != PET_UPBRINGING)
//		return;
//
//	SetQuestFlag("growthpet_system.growthpet_summoned", pPetItem->GetGrowthPetItemInfo().pet_vnum);
//	SetQuestFlag("growthpet_system.growthpet_id", pPetItem->GetID());
//	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Summoning Growth Pet: VNUM=%d, ItemID=%d"), pPetItem->GetGrowthPetItemInfo().pet_vnum, pPetItem->GetID());
//
//
//	CGrowthPetSystem* petSystem = GetGrowthPetSystem();
//	if (petSystem && !petSystem->IsActivePet())
//	{
//		petSystem->Summon(pPetItem->GetGrowthPetItemInfo().pet_vnum, pPetItem, "", false);
//	}
//
//	pPetItem->SetSocket(3, TRUE);
//}
void CHARACTER::SetSummonGrowthPet(LPITEM pPetItem)
{
	if (!pPetItem)
		return;
	if (pPetItem->GetType() != ITEM_PET || pPetItem->GetSubType() != PET_UPBRINGING)
		return;
	SetQuestFlag("growthpet_system.growthpet_summoned", pPetItem->GetValue(0));
	SetQuestFlag("growthpet_system.growthpet_id", pPetItem->GetID());
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Summoning Growth Pet: VNUM=%d, ItemID=%d"), pPetItem->GetValue(0), pPetItem->GetID());
	CGrowthPetSystem* petSystem = GetGrowthPetSystem();
	if (petSystem)
	{
		if (petSystem->IsActivePet())
		{
			// Dac? pet-ul este deja activ, ne asigur?m c? îl elimin?m corect înainte de a-l chema din nou
			petSystem->Dismiss();
		}
		// Re-sumon?m pet-ul dup? teleport
		CGrowthPetSystemActor* petActor = petSystem->Summon(pPetItem->GetValue(0), pPetItem, "", false);
		// Reaplic?m buff-urile ?i recalcul?m statisticile
		ComputePoints();
		// if (petActor)
		// {
		// 	petActor->GiveBuff();  // Apel?m GiveBuff() pe pet-ul respectiv
		// }
		if (petActor)
		{
			// Creăm evenimentul pentru a aplica buff-ul după 2 secunde
			GiveBuffEventInfo* info = AllocEventInfo<GiveBuffEventInfo>();
			info->ch = this;
			info->petActor = petActor;

			event_create(summon_growth_pet_event, info, PASSES_PER_SEC(2)); // Rulează după 2 secunde
		}
	}
	// Marc?m item-ul ca având pet-ul activ
	pPetItem->SetSocket(3, TRUE);
}
// void CHARACTER::SetSummonGrowthPet(LPITEM pPetItem)
// {
// 	if (!pPetItem)
// 		return;

// 	if (pPetItem->GetType() != ITEM_PET || pPetItem->GetSubType() != PET_UPBRINGING)
// 		return;

// 	SetQuestFlag("growthpet_system.growthpet_summoned", pPetItem->GetGrowthPetItemInfo().pet_vnum);
// 	SetQuestFlag("growthpet_system.growthpet_id", pPetItem->GetID());

// 	CGrowthPetSystem* petSystem = GetGrowthPetSystem();
// 	if (petSystem)
// 	{
// 		if (petSystem->IsActivePet())
// 		{
// 			// Dac? pet-ul este deja activ, ne asigur?m c? îl elimin?m corect înainte de a-l chema din nou
// 			petSystem->Dismiss();
// 		}

// 		// Re-sumon?m pet-ul dup? teleport
// 		petSystem->Summon(pPetItem->GetGrowthPetItemInfo().pet_vnum, pPetItem, "", false);

// 		// Reaplic?m buff-urile ?i recalcul?m statisticile
// 		ComputePoints();
// 		//GiveBuff();
// 	}

// 	// Marc?m item-ul ca având pet-ul activ
// 	pPetItem->SetSocket(3, TRUE);
// }
LPITEM CHARACTER::GetSummonGrowthPet() const
{
	return m_pkSummonGrowthPet;
}
#endif
void CHARACTER::SetGrowthPetInfo(TGrowthPetInfo petInfo)
{
	const auto it = m_GrowthPetInfo.find(petInfo.pet_id);
	if (it != m_GrowthPetInfo.end())
	{
		(*it).second = petInfo;
		return;
	}

	m_GrowthPetInfo.insert(std::make_pair(petInfo.pet_id, petInfo));
}

std::vector<TGrowthPetInfo> CHARACTER::GetPetList() const
{
	std::vector<TGrowthPetInfo> pets{};
	pets.reserve(m_GrowthPetInfo.size());
	for (auto& iter : m_GrowthPetInfo)
	{
		pets.emplace_back(iter.second);
	}
	return pets;
}

void CHARACTER::SendGrowthPetInfoPacket()
{
	if (!GetDesc())
		return;

#ifdef _DEBUG
	if (m_GrowthPetInfo.empty())
		return;
#endif

	TPacketGCGrowthPetInfo packet{};
	packet.bHeader = HEADER_GC_GROWTH_PET_INFO;
	packet.wSize = static_cast<uint16_t>(sizeof(packet) + sizeof(TGrowthPetInfo) * m_GrowthPetInfo.size());

	GetDesc()->BufferedPacket(&packet, sizeof(packet));
	GetDesc()->Packet(&GetPetList()[0], sizeof(TGrowthPetInfo) * m_GrowthPetInfo.size());
}

void CHARACTER::SetPetWindowType(uint8_t pet_window_type)
{
#ifdef ENABLE_PET_ATTR_DETERMINE
	if (pet_window_type == 1)
	{
		const int iPulse = thecore_pulse();
		if (iPulse - GetGrowthPetDetermineLoadTime() < PASSES_PER_SEC(3))
		{
			//LocaleChatPacket(CHAT_TYPE_INFO, ELocaleStrings::DU_KANNST_ES_3_SEKUNDEN_NACH_SCHLIESEN_DES_WERTE_WANDLUNGSFENSTERS_FUER_PETS_OEFFNEN);
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can open it 3 seconds after closing the menu for modifying pet stats."));
			return;
		}

		if (GetExchange()
#	ifdef ENABLE_PREMIUM_PRIVATE_SHOP
			|| GetMyShop()
			|| GetViewingShopOwner()
#	else
			|| GetMyShop()
			|| GetShopOwner()
#	endif
			|| IsOpenSafebox() || IsCubeOpen()
# ifdef ENABLE_GUILDSTORAGE_SYSTEM
			|| IsOpenGuildstorage()
# endif
			)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot do this whilst carrying out another action."));
			return;
		}
	}
#endif

	TEMP_BUFFER buf;
	TPacketGCGrowthPet pack;
	TSubPacketCGGrowthPetWindowType sub{};
	pack.wSize = sizeof(TPacketGCGrowthPet) + sizeof(TSubPacketCGGrowthPetWindowType);
	pack.bSubHeader = GROWTHPET_SUBHEADER_GC_WINDOW;
	sub.bPetWindowType = pet_window_type;

	LPDESC desc = GetDesc();
	if (!desc)
	{
		sys_err("User(%s)'s DESC is nullptr POINT.", GetName());
		return;
	}

	buf.write(&pack, sizeof(TPacketGCGrowthPet));
	buf.write(&sub, sizeof(TSubPacketCGGrowthPetWindowType));

	m_GrowthPetWindowType = pet_window_type;
#ifdef ENABLE_PET_ATTR_DETERMINE
	if (pet_window_type == 1)
		SetGrowthPetDetermineLoadTime();
#endif

	desc->Packet(buf.read_peek(), buf.size());
}
#endif

#ifdef ENABLE_ATTR_6TH_7TH_EXTEND
uint8_t CHARACTER::GetAlignRank()
{
	uint8_t bAlignIndex = 0;

	if (GetRealAlignment() >= 120000)
		bAlignIndex = PVP_LEVEL_CHIVALRIC;
	else if (GetRealAlignment() >= 80000)
		bAlignIndex = PVP_LEVEL_NOBLE;
	else if (GetRealAlignment() >= 40000)
		bAlignIndex = PVP_LEVEL_GOOD;
	else if (GetRealAlignment() >= 10000)
		bAlignIndex = PVP_LEVEL_FRIENDLY;
	else if (GetRealAlignment() >= 0)
		bAlignIndex = PVP_LEVEL_NEUTRAL;
	else if (GetRealAlignment() > -40000)
		bAlignIndex = PVP_LEVEL_AGGRESSIVE;
	else if (GetRealAlignment() > -80000)
		bAlignIndex = PVP_LEVEL_FRAUDULENT;
	else if (GetRealAlignment() > -120000)
		bAlignIndex = PVP_LEVEL_MALICIOUS;
	else
		bAlignIndex = PVP_LEVEL_CRUEL;

	return bAlignIndex;
}
#endif

#if defined(__HIDE_COSTUME_SYSTEM__)
void CHARACTER::SetBodyCostumeHidden(bool hidden)
{
	m_bHideBodyCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetBodyCostumeHidden %d", m_bHideBodyCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_body", m_bHideBodyCostume ? 1 : 0);
}

void CHARACTER::SetHairCostumeHidden(bool hidden)
{
	m_bHideHairCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetHairCostumeHidden %d", m_bHideHairCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_hair", m_bHideHairCostume ? 1 : 0);
}

#if defined(__ACCE_COSTUME_SYSTEM__)
void CHARACTER::SetAcceCostumeHidden(bool hidden)
{
	m_bHideAcceCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetAcceCostumeHidden %d", m_bHideAcceCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_acce", m_bHideAcceCostume ? 1 : 0);
}
#endif

#if defined(__WEAPON_COSTUME_SYSTEM__)
void CHARACTER::SetWeaponCostumeHidden(bool hidden)
{
	m_bHideWeaponCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetWeaponCostumeHidden %d", m_bHideWeaponCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_weapon", m_bHideWeaponCostume ? 1 : 0);
}
#endif

void CHARACTER::SetAuraCostumeHidden(bool hidden)
{
	m_bHideAuraCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetAuraCostumeHidden %d", m_bHideAuraCostume ? 1 : 0);
	SetQuestFlag("costume_option.hide_aura", m_bHideAuraCostume ? 1 : 0);
}

#endif

#if defined(__BL_67_ATTR__)
void CHARACTER::Open67Attr()
{
	LPDESC d = GetDesc();
	if (!d)
		return;

	if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || Is67AttrOpen())
	{
		ChatPacket(CHAT_TYPE_INFO, "You have to close other windows.");
		return;
	}

	Set67Attr(true);

	TPacket67AttrOpenClose p;
	p.bHeader = HEADER_GC_OPEN_67_ATTR;
	d->Packet(&p, sizeof(p));
}
#endif

#if defined(__ANTI_EXP_RING__)
bool CHARACTER::HasFrozenEXP()
{
	if (g_bAntiExpRing)
	{
		int iAntiExp = GetQuestFlag("anti_exp.freeze");
		if (iAntiExp != 0)
			return true;
		else
			return false;
	}
	else
		return false;
}

void CHARACTER::FreezeEXP()
{
	return SetQuestFlag("anti_exp.freeze", 1);
}

void CHARACTER::UnfreezeEXP()
{
	return SetQuestFlag("anti_exp.freeze", 0);
}
#endif

bool CHARACTER::IsInSafezone() const
{
	LPSECTREE sectree = GetSectree();
	return (sectree && sectree->IsAttr(GetX(), GetY(), ATTR_BANPK));
}

bool CHARACTER::IsInBlockedArea(long x, long y) const
{
	LPSECTREE sectree = GetSectree();
	return (sectree && sectree->IsAttr(x > 0 ? x : GetX(), x > 0 ? y : GetY(), ATTR_BLOCK | ATTR_OBJECT));
}

#if defined(__MOVE_CHANNEL__)
EVENTINFO(move_channel_info)
{
	DynamicCharacterPtr ch;
	int iSec;
	long lNewAddr;
	WORD wNewPort;

	move_channel_info()
		: iSec(0),
		ch(),
		lNewAddr(0),
		wNewPort(0)
	{
	}
};

EVENTFUNC(move_channel)
{
	move_channel_info* info = dynamic_cast<move_channel_info*>(event->info);

	if (!info) // no info
		return 0;

	LPCHARACTER ch = info->ch;

	if (!ch) // no char
		return 0;

	if (!ch->GetDesc()) // no desc
		return 0;

	if (info->iSec > 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Moving channel in %d seconds."), info->iSec);
		--info->iSec;
		return PASSES_PER_SEC(1);
	}

	ch->MoveChannel(info->lNewAddr, info->wNewPort);
	ch->m_pkTimedEvent = NULL;

	return 0;
}

bool CHARACTER::MoveChannel(long lNewAddr, WORD wNewPort)
{
	if (!IsPC())
		return false;

	if (!CanWarp())
		return false;

	if (!GetDesc())
		return false;

	if (IsWarping())
		return false;

	long x = GetX();
	long y = GetY();

	long lAddr = lNewAddr;
	long lMapIndex = GetMapIndex();
	WORD wPort = wNewPort;

	if (lMapIndex >= 10000)
	{
		sys_err("Invalid change channel request from dungeon %d!", lMapIndex);
		return false;
	}

	if (g_bChannel == 99)
	{
		sys_err("%s attempted to change channel from CH99, ignoring request.", GetName());
		return false;
	}

	m_bWarping = true;

	Stop();
	Save();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
		ViewCleanup();

		EncodeRemovePacket(this);
	}

	m_lWarpMapIndex = lMapIndex;
	m_posWarp.x = x;
	m_posWarp.y = y;

	sys_log(0, "ChangeChannel %s, %ld %ld map %ld to port %d", GetName(), x, y, GetMapIndex(), wPort);

	TPacketGCWarp p;

	p.bHeader = HEADER_GC_WARP;
	p.lX = x;
	p.lY = y;
	p.lAddr = lAddr;
	p.wPort = wPort;

	//GetDesc()->Packet(&p, sizeof(p));

	GetDesc()->WarpPacket(p);

	char buf[256];
	snprintf(buf, sizeof(buf), "%s Port%d Map%ld x%ld y%ld", GetName(), wPort, GetMapIndex(), x, y);
	LogManager::instance().CharLog(this, 0, "CHANGE_CH", buf);

	return true;
}

bool CHARACTER::StartMoveChannel(long lNewAddr, WORD wNewPort)
{
	if (IsHack(false, true, 10))
		return false;

	move_channel_info* info = AllocEventInfo<move_channel_info>();
	info->ch = this;
	info->iSec = CanWarp() && !IsPosition(POS_FIGHTING) ? 3 : 10;
	info->lNewAddr = lNewAddr;
	info->wNewPort = wNewPort;

	m_pkTimedEvent = event_create(move_channel, info, 1);

	return true;
}
#endif

#ifdef __ENABLE_NEW_OFFLINESHOP__
void CHARACTER::SetShopSafebox(offlineshop::CShopSafebox* pk)
{
	if (m_pkShopSafebox && pk == NULL)
	{
		m_pkShopSafebox->SetOwner(NULL);
	}

	else if (m_pkShopSafebox == NULL && pk)
	{
		pk->SetOwner(this);
	}

	m_pkShopSafebox = pk;
}
#endif

#if defined(__SKILL_COLOR_SYSTEM__)
void CHARACTER::SetSkillColor(DWORD* dwSkillColor)
{
	memcpy(m_dwSkillColor, dwSkillColor, sizeof(m_dwSkillColor));
	UpdatePacket();
}
#endif

BYTE CHARACTER::GetLanguage() const
{
	if (IsPC() && (!GetDesc() || !GetDesc()->GetCharacter()))
		return LOCALE_DEFAULT;

	if (GetDesc())
		return GetDesc()->GetLanguage();

	return LOCALE_DEFAULT;
}

bool CHARACTER::ChangeLanguage(BYTE bLanguage)
{
	if (!IsPC())
		return false;

	if (!CanWarp())
		return false;

	TPacketChangeLanguage packet;
	packet.bHeader = HEADER_GC_REQUEST_CHANGE_LANGUAGE;
	packet.bLanguage = bLanguage;
	GetDesc()->Packet(&packet, sizeof(packet));

	GetDesc()->SetLanguage(bLanguage);
	UpdatePacket();

	char buf[256];
	snprintf(buf, sizeof(buf), "%s Language %d", GetName(), bLanguage);
	LogManager::instance().CharLog(this, 0, "CHANGE_LANGUAGE", buf);

	return true;
}

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
void CHARACTER::OpenPrivateShopSearch(DWORD dwVnum)
{
	if (GetDesc() == NULL)
		return;

	if (PreventTradeWindow(WND_SHOPSEARCH, true/*except*/))
	{
		return;
	}

	TPacketGCPrivateShopSearchOpen p;
	p.header = HEADER_GC_PRIVATESHOP_SEARCH_OPEN;
	GetDesc()->Packet(&p, sizeof(p));

	bPrivateShopSearchState = (dwVnum == PRIVATESHOP_SEARCH_LOOKING_GLASS) ? SHOP_SEARCH_LOOKING : SHOP_SEARCH_TRADING;
}
#endif

#if defined(__CHANGE_LOOK_SYSTEM__)
void CHARACTER::SetChangeLook(CChangeLook* pChangeLook)
{
	if (m_pkChangeLook != nullptr)
		delete m_pkChangeLook;

	m_pkChangeLook = pChangeLook;
}

CChangeLook* CHARACTER::GetChangeLook() const
{
	return m_pkChangeLook;
}
#endif

#if defined(__MAILBOX__)
void CHARACTER::SetMailBox(CMailBox* m)
{
	if (m_pkMailBox)
		delete m_pkMailBox;

	m_pkMailBox = m;
}
#endif

#if defined(__CONQUEROR_LEVEL__)
bool CHARACTER::IsNewWorldMap(long lMapIndex)
{
	TSungMaWillMap::iterator it = SungMaWillMap.find(lMapIndex);
	return (it != SungMaWillMap.end() ? true : false);
}

void CHARACTER::SetConqueror(bool bSet)
{
	if (bSet)
	{
		if (GetConquerorLevel() > 0)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have already reached the Champion Level."));
			return;
		}

		DWORD dwNextExp = GetNextExp() / 4;
		if ((GetLevel() < gPlayerMaxLevel) || (GetLevel() >= gPlayerMaxLevel && GetExp() < dwNextExp))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You must fill up at least 1 EXP Marble at level 120 to reach Champion Level."));
			return;
		}
	}

	PointChange(POINT_CONQUEROR_LEVEL, MINMAX(0, bSet ? 1 : 0, gPlayerMaxLevel) - GetConquerorLevel());

	SetRealPoint(POINT_CONQUEROR_LEVEL_STEP, 0);
	SetPoint(POINT_CONQUEROR_LEVEL_STEP, GetRealPoint(POINT_CONQUEROR_LEVEL_STEP));

	SetRealPoint(POINT_CONQUEROR_POINT, 0);
	SetPoint(POINT_CONQUEROR_POINT, GetRealPoint(POINT_CONQUEROR_POINT));

	SetRealPoint(POINT_SUNGMA_STR, 0);
	SetPoint(POINT_SUNGMA_STR, GetRealPoint(POINT_SUNGMA_STR));

	SetRealPoint(POINT_SUNGMA_HP, 0);
	SetPoint(POINT_SUNGMA_HP, GetRealPoint(POINT_SUNGMA_HP));

	SetRealPoint(POINT_SUNGMA_MOVE, 0);
	SetPoint(POINT_SUNGMA_MOVE, GetRealPoint(POINT_SUNGMA_MOVE));

	SetRealPoint(POINT_SUNGMA_IMMUNE, 0);
	SetPoint(POINT_SUNGMA_IMMUNE, GetRealPoint(POINT_SUNGMA_IMMUNE));

	SetConquerorExp(0);

	ComputePoints();
	PointsPacket();
	UpdatePointsPacket(POINT_CONQUEROR_EXP, GetConquerorExp());
}

void CHARACTER::SetSungMaWill()
{
	if (IsPC() == false)
		return;

	if (GetConquerorLevel() <= 0)
		return;

#ifdef ENABLE_QUEEN_NETHIS
	if (SnakeLair::CSnk::instance().IsSnakeMap(GetMapIndex()))
		ChatPacket(CHAT_TYPE_COMMAND, "SungMaAttr 35 25 35 40");
#endif

#ifdef ENABLE_WHITE_DRAGON
	if (WhiteDragon::CWhDr::instance().IsWhiteMap(GetMapIndex()))
		ChatPacket(CHAT_TYPE_COMMAND, "SungMaAttr 45 35 45 50");
#endif

	TSungMaWillMap::iterator it = SungMaWillMap.find(GetMapIndex());
	if (it != SungMaWillMap.end())
		ChatPacket(CHAT_TYPE_COMMAND, "SungMaAttr %d %d %d %d", it->second.str, it->second.hp, it->second.move, it->second.immune);
}

uint8_t CHARACTER::GetSungMaWill(uint8_t type) const
{
	if (IsPC() == false)
		return 0;

	if (GetConquerorLevel() <= 0)
		return 0;

	TSungMaWillMap::iterator it = SungMaWillMap.find(GetMapIndex());
	if (it != SungMaWillMap.end())
	{
		switch (type)
		{
		case POINT_SUNGMA_STR:
			return it->second.str;
		case POINT_SUNGMA_HP:
			return it->second.hp;
		case POINT_SUNGMA_MOVE:
			return it->second.move;
		case POINT_SUNGMA_IMMUNE:
			return it->second.immune;
		default:
			return 0;
		}
	}
	return 0;
}
#endif

/*** Ikarus function created for offlineshop & special inventory ***/
bool CHARACTER::CanTakeInventoryItem(LPITEM item, TItemPos* cell)
{
	int iEmpty = -1;
	if (item->IsDragonSoul())
	{
		cell->window_type = DRAGON_SOUL_INVENTORY;
		cell->cell = iEmpty = GetEmptyDragonSoulInventory(item);
	}

#ifdef __SPECIAL_INVENTORY_SYSTEM__
	else if (item->IsSkillBook())
	{
		cell->window_type = INVENTORY;
		cell->cell = iEmpty = GetEmptySkillBookInventory(item->GetSize());
	}

	else if (item->IsUpgradeItem())
	{
		cell->window_type = INVENTORY;
		cell->cell = iEmpty = GetEmptyUpgradeItemsInventory(item->GetSize());
	}

	else if (item->IsStone())
	{
		cell->window_type = INVENTORY;
		cell->cell = iEmpty = GetEmptyStoneInventory(item->GetSize());
	}

	else if (item->IsGiftBox())
	{
		cell->window_type = INVENTORY;
		cell->cell = iEmpty = GetEmptyGiftBoxInventory(item->GetSize());
	}

	else if (item->IsAttribute())
	{
		cell->window_type = INVENTORY;
		cell->cell = iEmpty = GetEmptyAttributeInventory(item->GetSize());
	}
#endif

	else
	{
		cell->window_type = INVENTORY;
		cell->cell = iEmpty = GetEmptyInventory(item->GetSize());
	}

	return iEmpty != -1;
}
/*** End ***/

#ifdef ENABLE_12ZI
void CHARACTER::BeadTime()
{
	int animaSphere = GetAnimaSphere();
	int lastTime = GetQuestFlag("12zi_temple.beadtime");
	int remainTime = 3600 - (get_global_time() - lastTime);

	if (animaSphere >= 36)
	{
		ChatPacket(CHAT_TYPE_COMMAND, "Bead_count %d", GetAnimaSphere());
		ChatPacket(CHAT_TYPE_COMMAND, "Bead_time %d", 0);
		return;
	}

	if ((animaSphere == 0) && (lastTime == 0))
	{
		SetAnimaSphere(36);
		SetQuestFlag("12zi_temple.beadtime", get_global_time());
		ChatPacket(CHAT_TYPE_COMMAND, "Bead_count %d", GetAnimaSphere());
		ChatPacket(CHAT_TYPE_COMMAND, "Bead_time %d", 0);
		return;
	}

	if (animaSphere < 36 && ((get_global_time() - lastTime) > 3600))
	{
		int iTime = get_global_time() - lastTime;
		int iCount = iTime / 3600;

		if ((animaSphere + iCount) <= 36)
		{
			SetAnimaSphere(iCount);
		}
		else if ((animaSphere + iCount) > 36)
		{
			int jCount = 36 - animaSphere;
			if (jCount <= 36)
				SetAnimaSphere(jCount);

			ChatPacket(CHAT_TYPE_COMMAND, "Bead_count %d", GetAnimaSphere());
			ChatPacket(CHAT_TYPE_COMMAND, "Bead_time %d", 0);
		}

		SetQuestFlag("12zi_temple.beadtime", get_global_time());
		ChatPacket(CHAT_TYPE_COMMAND, "Bead_count %d", GetAnimaSphere());
		ChatPacket(CHAT_TYPE_COMMAND, "Bead_time %d", remainTime);
		return;
	}

	ChatPacket(CHAT_TYPE_COMMAND, "Bead_count %d", animaSphere);
	ChatPacket(CHAT_TYPE_COMMAND, "Bead_time %d", remainTime);
}

int CHARACTER::GetAnimaSphere()
{
	auto pMsg(DBManager::Instance().DirectQuery("SELECT bead FROM player.player WHERE id = '%d';", GetPlayerID()));
	if (pMsg->Get()->uiNumRows == 0)
		return 0;

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	int bBead = 0;
	str_to_number(bBead, row[0]);

	return bBead;
}

void CHARACTER::CheckFloor()
{
	if (GetMapIndex() == MAP_12ZI_STAGE)
	{
		if (DISTANCE_APPROX(GetX() - 1229300, GetY() - 743200) > 7000)
			WarpSet(1229200, 739800, MAP_12ZI_STAGE);
	}

	if (GetMapIndex() >= 3550000 && GetMapIndex() < 3560000)
	{
		if (GetQuestFlag("12zi_temple.IsDead") == 1)
			Dead();

		LPZODIAC pZodiac = CZodiacManager::Instance().FindByMapIndex(GetMapIndex());
		if (pZodiac)
		{
			uint8_t Floor = pZodiac->GetFloor();
			if (Floor != 0)
			{
				uint32_t time = 0;

				if (Floor == 0)
				{
					SetQuestFlag("12zi_temple.PrismNeed", 0);
					SetQuestFlag("12zi_temple.IsDead", 0);
				}

				if (Floor == 7 || Floor == 14 || Floor == 21 || Floor == 28 || Floor == 35 || Floor == 36 || Floor == 37 || Floor == 38 || Floor == 39 || Floor == 40) //Bonus floor
					//if (Floor == 40) //Bonus floor
					time = 5 * 60;
				else //Normal floor
					time = 10 * 60;

				ZodiacFloorMessage(Floor);

				uint32_t Time_1 = (get_global_time() + 1) - pZodiac->GetTime();

				if (Floor == 40)
					ChatPacket(CHAT_TYPE_COMMAND, "ZodiacTime %d %d %d %d", Floor, 0, time, Time_1);
				else
					ChatPacket(CHAT_TYPE_COMMAND, "ZodiacTime %d %d %d %d", Floor, 1, time, Time_1);
			}
			else
			{
				ChatPacket(CHAT_TYPE_COMMAND, "ZodiacTime %d %d %d %d", 0, 1, 10, 0);
			}
		}
		else
		{
			WarpSet(1229200, 739800, MAP_12ZI_STAGE);
		}
	}
}

void CHARACTER::SetAnimaSphere(int amount)
{
	int value = abs(amount);

	if (amount > 0)
	{
		DBManager::Instance().DirectQuery("UPDATE player.player SET bead = bead + '%d' WHERE id = '%d'", value, GetPlayerID());
	}
	else
	{
		SetQuestFlag("12zi_temple.beadtime", get_global_time());
		DBManager::Instance().DirectQuery("UPDATE player.player SET bead = bead - '%d' WHERE id = '%d'", value, GetPlayerID());
	}

	ChatPacket(CHAT_TYPE_COMMAND, "Bead_count %d", GetAnimaSphere());
}

void CHARACTER::IsZodiacEffectMob()
{
	if (!this)
		return;

	if (!IsMonster())
		return;

	if (IsDead())
		return;

	uint32_t Monster = GetRaceNum();

	if (!Monster || Monster == 0)
		return;

	if (Monster == 2750 || Monster == 2860) //Officer (Zi or Hai)
	{
		if (number(1, 2) == 1)
		{
			EffectPacket(SE_SKILL_DAMAGE_ZONE);
		}
		else
		{
			EffectPacket(SE_SKILL_SAFE_ZONE);
		}
	}
}

void CHARACTER::IsZodiacEffectPC(uint32_t Monster)
{
	if (!this)
		return;

	if (!IsPC())
		return;

	if (IsDead())
		return;

	if (!Monster || Monster == 0)
		return;

	if (!GetDesc() || !GetDesc()->GetCharacter())
	{
		sys_err("Character::IsZodiacEffectPC : cannot get desc or character");
		return;
	}

	if (Monster == 20464) //Canon
		EffectPacket(SE_DEAPO_BOOM);
	else if (Monster == 2770 || Monster == 2771 || Monster == 2772) //Yin
		EffectPacket(SE_METEOR);
	else if (Monster == 2790 || Monster == 2791 || Monster == 2792) //Chen
		EffectPacket(SE_BEAD_RAIN);
	else if (Monster == 2830 || Monster == 2831 || Monster == 2832) //Shen
		EffectPacket(SE_FALL_ROCK);
	else if (Monster == 2800 || Monster == 2801 || Monster == 2802) //Si
		EffectPacket(SE_ARROW_RAIN);
	else if (Monster == 2810 || Monster == 2811 || Monster == 2812) //Wu
		EffectPacket(SE_HORSE_DROP);
	else if (Monster == 2840 || Monster == 2841 || Monster == 2842) //Yu
		EffectPacket(SE_EGG_DROP);
}

void CHARACTER::ZodiacFloorMessage(uint8_t Floor)
{
	if (!IsPC())
		return;

#ifdef ENABLE_CHAT_MISSION_ALTERNATIVE
	if ((Floor >= 1 && Floor <= 5) || (Floor == 9) || (Floor == 10) || (Floor == 16) || (Floor == 20) || (Floor == 23) || (Floor == 25) || (Floor == 26) || (Floor >= 31 && Floor <= 33))
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("Defeat all monsters."));
	else if (Floor == 6)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("Defeat the Zodiac boss."));
	else if (Floor == 7 || Floor == 21)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("Destroy the right Metin stone!"));
	else if (Floor == 8 || Floor == 27 || Floor == 30)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("Destroy all Metin stones."));
	else if (Floor == 11 || Floor == 17)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("Defeat the Zodiac boss."));
	else if (Floor == 12 || Floor == 19 || Floor == 24)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("Defeat all monsters."));
	else if (Floor == 13 || Floor == 18 || Floor == 29)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("Destroy all Metin stones."));
	else if (Floor == 14 || Floor == 28)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("Bonus level: Destroy Metin stones."));
	else if (Floor == 15 || Floor == 34)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("Defeat monsters."));
	else if (Floor == 22)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("Defeat all Zodiac bosses."));
	else if (Floor >= 35 && Floor <= 39)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("Bonus level: Destroy a Metin stone."));
	else if (Floor == 40)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("Bonus level: You have 5 minutes to trade with the merchant and stock up on supplies."));
	else if (Floor == 41)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("The mission was unsuccessful."));
	else if (Floor == 42)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("You just missed the bonus level."));
	else if (Floor == 43)
		ChatPacket(CHAT_TYPE_MISSION, LC_TEXT("Time's up."));
#endif
}

void CHARACTER::EffectZodiacPacket(long X, long Y, int enumEffectType, int enumEffectType2)
{
	TPacketGCSpecialZodiacEffect p{};

	p.header = HEADER_GC_SEPCIAL_ZODIAC_EFFECT;
	p.type = enumEffectType;
	p.type2 = enumEffectType2;
	p.vid = GetVID();
	p.x = X;
	p.y = Y;

	PacketAround(&p, sizeof(p));
}

uint32_t CHARACTER::CountZodiacItems(uint32_t Vnum)
{
	auto pMsg(DBManager::Instance().DirectQuery("SELECT count FROM player.zodiac_npc WHERE item_vnum = '%u' and owner_id = '%d'", Vnum, GetPlayerID()));
	if (pMsg->Get()->uiNumRows == 0)
		return 0;

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	uint32_t dwCount = 0;
	str_to_number(dwCount, row[0]);
	return dwCount;
}

void CHARACTER::SetZodiacItems(uint32_t Vnum, int Count)
{
	auto pMsg(DBManager::Instance().DirectQuery("SELECT owner_id FROM player.zodiac_npc WHERE item_vnum = '%u' and owner_id = '%u'", Vnum, GetPlayerID()));
	if (pMsg->Get()->uiNumRows == 0)
	{
		char szQuery[512];
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO player.zodiac_npc(owner_id, item_vnum, count) VALUES(%u, %u, %d)", GetPlayerID(), Vnum, Count);
		DBManager::Instance().DirectQuery(szQuery);
		return;
	}
	else
	{
		char szQuery2[512];
		snprintf(szQuery2, sizeof(szQuery2), "UPDATE player.zodiac_npc SET count = '%d' WHERE item_vnum = %u and owner_id = '%u'", Count, Vnum, GetPlayerID());
		DBManager::Instance().DirectQuery(szQuery2);
	}
}

uint32_t CHARACTER::PurchaseCountZodiacItems(uint32_t Vnum)
{
	auto pMsg(DBManager::Instance().DirectQuery("SELECT count FROM player.zodiac_npc_sold WHERE item_vnum = '%u' and owner_id = '%d'", Vnum, GetPlayerID()));
	if (pMsg->Get()->uiNumRows == 0)
		return 0;

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	uint32_t dwCount = 0;
	str_to_number(dwCount, row[0]);
	return dwCount;
}

void CHARACTER::SetPurchaseZodiacItems(uint32_t Vnum, int Count)
{
	auto pMsg(DBManager::Instance().DirectQuery("SELECT owner_id FROM player.zodiac_npc_sold WHERE item_vnum = '%u' and owner_id = '%u'", Vnum, GetPlayerID()));
	if (pMsg->Get()->uiNumRows == 0)
	{
		char szQuery[512];
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO player.zodiac_npc_sold(owner_id, item_vnum, count) VALUES(%u, %u, %d)", GetPlayerID(), Vnum, Count);
		DBManager::Instance().DirectQuery(szQuery);
		return;
	}
	else
	{
		char szQuery2[512];
		snprintf(szQuery2, sizeof(szQuery2), "UPDATE player.zodiac_npc_sold SET count = '%d' WHERE item_vnum = '%u' and owner_id = '%u'", Count, Vnum, GetPlayerID());
		DBManager::Instance().DirectQuery(szQuery2);
	}
}
#endif

#ifdef ENABLE_MEDAL_OF_HONOR
void CHARACTER::CheckMedals()
{
	// if ((get_global_time() - GetShopExDailyTimePoint(SHOP_COIN_TYPE_MEDAL_OF_HONOR)) > 86400)
	// {
	// 	SetShopExUsablePoint(SHOP_COIN_TYPE_MEDAL_OF_HONOR, LIMIT_DAILY_MAX_MEDALS);
	// 	SetShopExDailyTimePoint(SHOP_COIN_TYPE_MEDAL_OF_HONOR, get_global_time());
	// }
	if ((get_global_time() - GetShopExDailyTimePoint(SHOP_COIN_TYPE_MEDAL_OF_HONOR)) > 86400)
	{
		SetShopExUsablePoint(SHOP_COIN_TYPE_MEDAL_OF_HONOR, LIMIT_DAILY_MAX_MEDALS);
		SetShopExDailyTimePoint(SHOP_COIN_TYPE_MEDAL_OF_HONOR, get_global_time());
	}
}
#endif

#ifdef ENABLE_WHITE_DRAGON
bool CHARACTER::IsWhiteMap()
{
	return WhiteDragon::CWhDr::instance().IsWhiteMap(GetMapIndex());
}
#endif

#ifdef ENABLE_QUEEN_NETHIS
bool CHARACTER::IsSnakeMap()
{
	return SnakeLair::CSnk::instance().IsSnakeMap(GetMapIndex());
}
#endif

#ifdef ENABLE_GUILD_TOKEN_AUTH
bool CHARACTER::IsGuildMaster() const
{
	const auto guild = GetGuild();
	return (guild && GetPlayerID() == guild->GetMasterPID());
}

uint64_t CHARACTER::GetGuildToken() const
{
	const auto guild = GetGuild();
	if (guild && IsGuildMaster())
		return guild->GetToken();
	return 0;
}

void CHARACTER::SendGuildToken()
{
	CGuildManager::instance().SendGuildToken(this, GetGuildToken());
}
#endif

#if defined(__SET_ITEM__)
void CHARACTER::RefreshItemSetBonus()
{
	RemoveAffect(AFFECT_SET_ITEM);

	bool bSetBonus = false;
	for (const auto& [bSetValue, vItems] : ITEM_MANAGER::Instance().GetItemSetItemMap())
	{
		BYTE bWearCount = 0;

		const auto& rkItemSetValueMap = ITEM_MANAGER::Instance().GetItemSetValueMap();
		if (rkItemSetValueMap.empty())
			break;

		const auto& ItemSetValueMap = rkItemSetValueMap.find(bSetValue);
		if (ItemSetValueMap == rkItemSetValueMap.end())
			continue;

		for (const auto& [bSetType, kItemTuple] : vItems)
		{
			LPITEM pItem = nullptr;
			LPITEM pUnique1 = nullptr;
			LPITEM pUnique2 = nullptr;
			CPetSystem* pPetSystem = nullptr;

			switch (bSetType)
			{
			case SET_ITEM_COSTUME_BODY:
				pItem = GetWear(WEAR_COSTUME_BODY);
				break;

			case SET_ITEM_COSTUME_HAIR:
				pItem = GetWear(WEAR_COSTUME_HAIR);
				break;

#if defined(__MOUNT_COSTUME_SYSTEM__)
			case SET_ITEM_COSTUME_MOUNT:
				pItem = GetWear(WEAR_COSTUME_MOUNT);
				break;
#endif

#if defined(__ACCE_COSTUME_SYSTEM__)
			case SET_ITEM_COSTUME_ACCE:
				pItem = GetWear(WEAR_COSTUME_ACCE);
				break;
#endif

#if defined(__WEAPON_COSTUME_SYSTEM__)
			case SET_ITEM_COSTUME_WEAPON:
				pItem = GetWear(WEAR_COSTUME_WEAPON);
				break;
#endif

			case SET_ITEM_UNIQUE:
				pUnique1 = GetWear(WEAR_UNIQUE1);
				pUnique2 = GetWear(WEAR_UNIQUE2);
				break;

			case SET_ITEM_PET:
				pPetSystem = GetPetSystem();
				break;
			}

			const auto& [dwMinVnum, dwMaxVnum, bRange] = kItemTuple;

			if (pPetSystem && CHECK_VNUM_RANGE(pPetSystem->GetSummonItemVnum(), dwMinVnum, dwMaxVnum, bRange))
				++bWearCount;

			if (pUnique1 && CHECK_VNUM_RANGE(pUnique1->GetVnum(), dwMinVnum, dwMaxVnum, bRange))
				++bWearCount;

			if (pUnique2 && CHECK_VNUM_RANGE(pUnique2->GetVnum(), dwMinVnum, dwMaxVnum, bRange))
				++bWearCount;

			if (pItem && CHECK_VNUM_RANGE(pItem->GetVnum(), dwMinVnum, dwMaxVnum, bRange))
				++bWearCount;

			for (const auto& [bCount, vSetBonus] : ItemSetValueMap->second)
			{
				if (bWearCount != bCount)
				{
					bSetBonus = false;
					continue;
				}

				for (const auto& [wApplyType, lApplyValue] : vSetBonus)
				{
					AddAffect(AFFECT_SET_ITEM, aApplyInfo[wApplyType].bPointType, lApplyValue, 0, INFINITE_AFFECT_DURATION, 0, true, true);
					bSetBonus = true;
				}
			}
		}

		if (bSetBonus)
			break;
	}
}

CHARACTER::SetItemCountMap CHARACTER::GetItemSetCountMap() const
{
	std::vector<LPITEM> vItems = { GetWear(WEAR_BODY), GetWear(WEAR_HEAD), GetWear(WEAR_WEAPON) };
	std::map<BYTE, BYTE> mSetCount = {
		{ SET_ITEM_SET_VALUE_1, 0 },
		{ SET_ITEM_SET_VALUE_2, 0 },
		{ SET_ITEM_SET_VALUE_3, 0 },
		{ SET_ITEM_SET_VALUE_4, 0 },
		{ SET_ITEM_SET_VALUE_5, 0 },
	};

	for (const LPITEM& pkItem : vItems)
	{
		if (pkItem == nullptr)
			continue;

		const BYTE bSetValue = pkItem->GetItemSetValue();
		if (bSetValue != SET_ITEM_SET_VALUE_NONE)
			++mSetCount[bSetValue];
	}

	return mSetCount;
}

void CHARACTER::RefreshItemSetBonusByValue()
{
	for (DWORD dwType = AFFECT_SET_ITEM_SET_VALUE_1; dwType <= AFFECT_SET_ITEM_SET_VALUE_5; ++dwType)
		RemoveAffect(dwType);

	const auto& rkItemSetValueMap = ITEM_MANAGER::Instance().GetItemSetValueMap();
	if (rkItemSetValueMap.empty())
		return;

	for (const auto& rkItemSetPair : GetItemSetCountMap())
	{
		BYTE bSetValue = rkItemSetPair.first;
		BYTE bWearCount = rkItemSetPair.second;

		const auto& rkSetItemMap = rkItemSetValueMap.find(bSetValue);
		if (rkSetItemMap == rkItemSetValueMap.end())
			continue;

		for (const auto& kSetItem : rkSetItemMap->second)
		{
			const auto& vSetBonus = kSetItem.second;
			if (bWearCount != kSetItem.first)
				continue;

			for (const auto& kSetBonus : vSetBonus)
				AddAffect((AFFECT_SET_ITEM_SET_VALUE_1 - 1) + bSetValue, aApplyInfo[kSetBonus.first].bPointType, kSetBonus.second, 0, INFINITE_AFFECT_DURATION, 0, true, true);
		}
	}
}
#endif

#ifdef ENABLE_REFINE_ELEMENT	
uint8_t CHARACTER::GetElementsEffect()
{
	LPITEM w_weapon = GetWear(WEAR_WEAPON);
	if (w_weapon)
	{
		if (w_weapon->GetElementGrade() == 3)
		{
			return w_weapon->GetElementsType();
		}
	}
	return 0;
}
#endif

#ifdef ENABLE_PET_SYSTEM
void CHARACTER::PetSummon(LPITEM petItem)
{
	CPetSystem* petSystem = GetPetSystem();
	uint32_t mobVnum = 0;

	if (!petSystem || !petItem)
		return;

	if (!(petItem->GetType() == ITEM_PET && petItem->GetSubType() == PET_PAY))
		return;

	if (petItem->GetValue(0) != 0)
		mobVnum = petItem->GetValue(0);

	petSystem->Summon(mobVnum, petItem, false);
}

void CHARACTER::PetUnsummon(LPITEM petItem)
{
	CPetSystem* petSystem = GetPetSystem();
	uint32_t mobVnum = 0;

	if (!petSystem || !petItem)
		return;

	if (!(petItem->GetType() == ITEM_PET && petItem->GetSubType() == PET_PAY))
		return;

	if (petItem->GetValue(0) != 0)
		mobVnum = petItem->GetValue(0);

	petSystem->Unsummon(mobVnum);
}

void CHARACTER::CheckPet()
{
	CPetSystem* petSystem = GetPetSystem();
	LPITEM petItem = GetWear(WEAR_PET);
	uint32_t mobVnum = 0;

	if (!petSystem || !petItem)
		return;

#ifdef PET_AUTO_PICKUP
	petSystem->ClearState();
#endif

	if (!(petItem->GetType() == ITEM_PET && petItem->GetSubType() == PET_PAY))
		return;

	if (petItem->GetValue(0) != 0)
		mobVnum = petItem->GetValue(0);

	if (petSystem->CountSummoned() == 0)
		petSystem->Summon(mobVnum, petItem, false);
}

bool CHARACTER::IsPetSummon()
{
	return (GetWear(WEAR_PET));
}
#endif

#ifdef ENABLE_GUILDWAR_BUTTON
void CHARACTER::SendWarTeleportButton(bool show)
{
	if (CWarMapManager::Instance().IsWarMap(GetMapIndex()))
		show = false;

	CGuild* pGuild = this->GetGuild();
	if (pGuild && pGuild->UnderAnyWar(GUILD_WAR_TYPE_FIELD))
		show = false;

	ChatPacket(CHAT_TYPE_COMMAND, "guild_war %d", show);
}
#endif

// #ifdef ENABLE_SET_ITEM
// void CHARACTER::RefreshSetBonus() {
//     int setID, setCount;
//     GetSetCount(setID, setCount);

//     	if (setID < 0 && setID > 5){
// 		return;
// 	}
// 	struct BonusInfo {
// 		bool piece;	// true == 2,3 - false == 3
// 		std::map<int, std::vector<int>> bonusInfo = {};
// 	};
// 	std::map<int, BonusInfo> setInfo = {
// 		// true == 2,3 pieces || false == only 3 pieces
// 		{1, BonusInfo{true, {{2, {APPLY_MAX_HP, 500, APPLY_MALL_EXPBONUS, 5}}, {3, {APPLY_MAX_HP, 1500, APPLY_MALL_EXPBONUS, 15}}}}},
// 		{2, BonusInfo{true, {{2, {APPLY_MAX_HP, 500, APPLY_MOV_SPEED, 3}}, {3, {APPLY_MAX_HP, 1500, APPLY_MOV_SPEED, 7}}}}},
// 		{3, BonusInfo{true, {{2, {APPLY_MAX_HP, 500, APPLY_ATT_BONUS_TO_MONSTER, 1}}, {3, {APPLY_MAX_HP, 500, APPLY_ATT_BONUS_TO_MONSTER, 5}}}}},
// 		{4, BonusInfo{false, {{3, {APPLY_MAX_HP, 2500, APPLY_MELEE_MAGIC_ATTBONUS_PER, 10, APPLY_MOV_SPEED, 5}}}}},
// 		{5, BonusInfo{false, {{3, {APPLY_MAX_HP, 3500, APPLY_MELEE_MAGIC_ATTBONUS_PER, 15, APPLY_MOV_SPEED, 7}}}}},
// 	};

// 	auto tVec = setInfo[setID].bonusInfo[setCount];
// 	if (tVec.size() % 2 != 0) {
// 		//ChatPacket(7, "Error at bonus vector");
// 	}
// 	else
// 	{
// 		if (!setInfo[setID].piece && setCount == 3)
// 		{
// 			int idx = 0;
// 			for (int i = 0; i < tVec.size() / 2; ++i)
// 			{
// 				const int bID = tVec[i * 2];
// 				const int bVal = tVec[(i * 2) + 1];
// 				AddAffect(AFFECT_SET_ITEM, POINT_NONE, 0, 0, INFINITE_AFFECT_DURATION, 0, true);
// 				AddAffect(AFFECT_SET_ITEM_SET_VALUE_1 + idx, aApplyInfo[bID].bPointType, bVal, 0, 60 * 60 * 24, 0, true, true);
// 				++idx;
// 			}
// 		}
// 		else if (setInfo[setID].piece)
// 		{
// 			int idx = 0;
// 			for (int i = 0; i < tVec.size() / 2; ++i)
// 			{
// 				const int bID = tVec[i * 2];
// 				const int bVal = tVec[(i * 2) + 1];
// 				AddAffect(AFFECT_SET_ITEM, POINT_NONE, 0, 0, INFINITE_AFFECT_DURATION, 0, true);
// 				AddAffect(AFFECT_SET_ITEM_SET_VALUE_1 + idx, aApplyInfo[bID].bPointType, bVal, 0, 60 * 60 * 24, 0, true, true);
// 				++idx;
// 			}
// 		}
// 	}
// }

// void CHARACTER::ClearAffectSetBonus() {
// 	CAffect* pSetItem = FindAffect(AFFECT_SET_ITEM);
//     if (pSetItem) {
//         RemoveAffect(pSetItem);
//     }

//     for (int idx = AFFECT_SET_ITEM_SET_VALUE_1; idx <= AFFECT_SET_ITEM_SET_VALUE_3; ++idx) {
//         CAffect* pAffect = FindAffect(idx);
//         if (pAffect) {
//             RemoveAffect(pAffect);
//         }
//     }
// }
// #endif
