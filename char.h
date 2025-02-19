#ifndef __INC_CHAR_H__
#define __INC_CHAR_H__

#include <boost/unordered_map.hpp>
#include "../../common/service.h"

#include "../../common/stl.h"
#include "entity.h"
#include "FSM.h"
#include "horse_rider.h"
#include "vid.h"
#include "constants.h"
#include "affect.h"
#include "affect_flag.h"
#ifndef ENABLE_CUBE_RENEWAL
#include "cube.h"
#endif
#include "mining.h"
#if defined(__MINI_GAME_CATCH_KING__)
#	include "packet.h"
#endif
#if defined(__CHANGE_LOOK_SYSTEM__)
#	include "changelook.h"
#endif
#if defined(__MAILBOX__)
#	include "MailBox.h"
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
#include "BiologSystemManager.h"
#endif

class CBuffOnAttributes;
class CPetSystem;

#ifdef ENABLE_GROWTH_PET_SYSTEM
class CGrowthPetSystem;
#endif

#define INSTANT_FLAG_DEATH_PENALTY (1 << 0)
#define INSTANT_FLAG_SHOP (1 << 1)
#define INSTANT_FLAG_EXCHANGE (1 << 2)
#define INSTANT_FLAG_STUN (1 << 3)
#define INSTANT_FLAG_NO_REWARD (1 << 4)

#define AI_FLAG_NPC (1 << 0)
#define AI_FLAG_AGGRESSIVE (1 << 1)
#define AI_FLAG_HELPER (1 << 2)
#define AI_FLAG_STAYZONE (1 << 3)

#if defined(__MINI_GAME_OKEY__)
#	define MAX_CARDS_IN_HAND 5
#	define MAX_CARDS_IN_FIELD 3
#endif

#define SET_OVER_TIME(ch, time) (ch)->SetOverTime(time)

extern int g_nPortalLimitTime;

extern bool IS_SUMMON_ITEM(int vnum);
extern bool IS_SUMMONABLE_ZONE(int map_index);

extern bool IS_MONKEY_DUNGEON(int map_index);
extern bool IS_MAZE_DUNGEON(int map_index);

extern bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index);
extern bool IS_BLOCKED_PET_SUMMON_MAP(int map_index);
extern bool IS_BLOCKED_PET_DUNGEON_MAP(int map_index);
extern bool IS_BLOCKED_MOUNT_SUMMON_MAP(int map_index);
extern bool IS_BLOCKED_MOUNT_DUNGEON_MAP(int map_index);

enum
{
	MAIN_RACE_WARRIOR_M,
	MAIN_RACE_ASSASSIN_W,
	MAIN_RACE_SURA_M,
	MAIN_RACE_SHAMAN_W,
	MAIN_RACE_WARRIOR_W,
	MAIN_RACE_ASSASSIN_M,
	MAIN_RACE_SURA_W,
	MAIN_RACE_SHAMAN_M,
	MAIN_RACE_WOLFMAN_M,
	MAIN_RACE_MAX_NUM,
};

enum
{
	POISON_LENGTH = 30,
	BLEEDING_LENGTH = 30,
	STAMINA_PER_STEP = 1,
	SAFEBOX_PAGE_SIZE = 9,
	AI_CHANGE_ATTACK_POISITION_TIME_NEAR = 10000,
	AI_CHANGE_ATTACK_POISITION_TIME_FAR = 1000,
	AI_CHANGE_ATTACK_POISITION_DISTANCE = 100,
	SUMMON_MONSTER_COUNT = 3,
#if defined(__EREBUS_DUNGEON__)
	SUMMON_HEALER_COUNT = 4,
#endif
};

enum
{
	FLY_NONE,
	FLY_EXP,
	FLY_HP_MEDIUM,
	FLY_HP_BIG,
	FLY_SP_SMALL,
	FLY_SP_MEDIUM,
	FLY_SP_BIG,
	FLY_FIREWORK1,
	FLY_FIREWORK2,
	FLY_FIREWORK3,
	FLY_FIREWORK4,
	FLY_FIREWORK5,
	FLY_FIREWORK6,
	FLY_FIREWORK_CHRISTMAS,
	FLY_CHAIN_LIGHTNING,
	FLY_HP_SMALL,
	FLY_SKILL_MUYEONG,
#if defined(__QUIVER_SYSTEM__)
	FLY_QUIVER_ATTACK_NORMAL,
#endif
#if defined(__CONQUEROR_LEVEL__)
	FLY_CONQUEROR_EXP,
#endif
};

enum EDamageType
{
	DAMAGE_TYPE_NONE,
	DAMAGE_TYPE_NORMAL,
	DAMAGE_TYPE_NORMAL_RANGE,
	// ��ų
	DAMAGE_TYPE_MELEE,
	DAMAGE_TYPE_RANGE,
	DAMAGE_TYPE_FIRE,
	DAMAGE_TYPE_ICE,
	DAMAGE_TYPE_ELEC,
	DAMAGE_TYPE_MAGIC,
	DAMAGE_TYPE_POISON,
	DAMAGE_TYPE_SPECIAL,
	DAMAGE_TYPE_BLEEDING,
};

enum DamageFlag
{
	DAMAGE_NORMAL = (1 << 0),
	DAMAGE_POISON = (1 << 1),
	DAMAGE_DODGE = (1 << 2),
	DAMAGE_BLOCK = (1 << 3),
	DAMAGE_PENETRATE = (1 << 4),
	DAMAGE_CRITICAL = (1 << 5),
	DAMAGE_BLEEDING = (1 << 6),
};

enum EPointTypes
{
	POINT_NONE,								// 0
	POINT_LEVEL,							// 1
	POINT_VOICE,							// 2
	POINT_EXP,								// 3
	POINT_NEXT_EXP,							// 4
	POINT_HP,								// 5
	POINT_MAX_HP,							// 6
	POINT_SP,								// 7
	POINT_MAX_SP,							// 8
	POINT_STAMINA,							// 9 ���׹̳�
	POINT_MAX_STAMINA,						// 10 �ִ� ���׹̳�

	POINT_GOLD,								// 11
	POINT_ST,								// 12 �ٷ�
	POINT_HT,								// 13 ü��
	POINT_DX,								// 14 ��ø��
	POINT_IQ,								// 15 ���ŷ�
	POINT_DEF_GRADE,						// 16 ...
	POINT_ATT_SPEED,						// 17 ���ݼӵ�
	POINT_ATT_GRADE,						// 18 ���ݷ� MAX
	POINT_MOV_SPEED,						// 19 �̵��ӵ�
	POINT_CLIENT_DEF_GRADE,					// 20 �����
	POINT_CASTING_SPEED,					// 21 �ֹ��ӵ� (��ٿ�Ÿ��*100) / (100 + �̰�) = ���� ��ٿ� Ÿ��
	POINT_MAGIC_ATT_GRADE,					// 22 �������ݷ�
	POINT_MAGIC_DEF_GRADE,					// 23 ��������
	POINT_EMPIRE_POINT,						// 24 ��������
	POINT_LEVEL_STEP,						// 25 �� ���������� �ܰ�.. (1 2 3 �� �� ����, 4 �Ǹ� ���� ��)
	POINT_STAT,								// 26 �ɷ�ġ �ø� �� �ִ� ����
	POINT_SUB_SKILL,						// 27 ���� ��ų ����Ʈ
	POINT_SKILL,							// 28 ��Ƽ�� ��ų ����Ʈ
	POINT_WEAPON_MIN,						// 29 ���� �ּ� ������
	POINT_WEAPON_MAX,						// 30 ���� �ִ� ������
	POINT_PLAYTIME,							// 31 �÷��̽ð�
	POINT_HP_REGEN,							// 32 HP ȸ����
	POINT_SP_REGEN,							// 33 SP ȸ����

	POINT_BOW_DISTANCE,						// 34 Ȱ �����Ÿ� ����ġ (meter)

	POINT_HP_RECOVERY,						// 35 ü�� ȸ�� ������
	POINT_SP_RECOVERY,						// 36 ���ŷ� ȸ�� ������

	POINT_POISON_PCT,						// 37 �� Ȯ��
	POINT_STUN_PCT,							// 38 ���� Ȯ��
	POINT_SLOW_PCT,							// 39 ���ο� Ȯ��
	POINT_CRITICAL_PCT,						// 40 ũ��Ƽ�� Ȯ��
	POINT_PENETRATE_PCT,					// 41 ����Ÿ�� Ȯ��
	POINT_CURSE_PCT,						// 42 ���� Ȯ��

	POINT_ATTBONUS_HUMAN,					// 43 �ΰ����� ����
	POINT_ATTBONUS_ANIMAL,					// 44 �������� ������ % ����
	POINT_ATTBONUS_ORC,						// 45 ���Ϳ��� ������ % ����
	POINT_ATTBONUS_MILGYO,					// 46 �б����� ������ % ����
	POINT_ATTBONUS_UNDEAD,					// 47 ��ü���� ������ % ����
	POINT_ATTBONUS_DEVIL,					// 48 ����(�Ǹ�)���� ������ % ����
	POINT_ATTBONUS_INSECT,					// 49 ������
	POINT_ATTBONUS_FIRE,					// 50 ȭ����
	POINT_ATTBONUS_ICE,						// 51 ������
	POINT_ATTBONUS_DESERT,					// 52 �縷��
	POINT_ATTBONUS_MONSTER,					// 53 ��� ���Ϳ��� ����
	POINT_ATTBONUS_WARRIOR,					// 54 ���翡�� ����
	POINT_ATTBONUS_ASSASSIN,				// 55 �ڰ����� ����
	POINT_ATTBONUS_SURA,					// 56 ���󿡰� ����
	POINT_ATTBONUS_SHAMAN,					// 57 ���翡�� ����
	POINT_ATTBONUS_TREE,					// 58 �������� ���� 20050729.myevan UNUSED5

	POINT_RESIST_WARRIOR,					// 59 ���翡�� ����
	POINT_RESIST_ASSASSIN,					// 60 �ڰ����� ����
	POINT_RESIST_SURA,						// 61 ���󿡰� ����
	POINT_RESIST_SHAMAN,					// 62 ���翡�� ����

	POINT_STEAL_HP,							// 63 ������ ����
	POINT_STEAL_SP,							// 64 ���ŷ� ����

	POINT_MANA_BURN_PCT,					// 65 ���� ��

	/// ���ؽ� ���ʽ� ///

	POINT_DAMAGE_SP_RECOVER,				// 66 ���ݴ��� �� ���ŷ� ȸ�� Ȯ��

	POINT_BLOCK,							// 67 ������
	POINT_DODGE,							// 68 ȸ����

	POINT_RESIST_SWORD,						// 69
	POINT_RESIST_TWOHAND,					// 70
	POINT_RESIST_DAGGER,					// 71
	POINT_RESIST_BELL,						// 72
	POINT_RESIST_FAN,						// 73
	POINT_RESIST_BOW,						// 74 ȭ�� ���� : ����� ����
	POINT_RESIST_FIRE,						// 75 ȭ�� ���� : ȭ�����ݿ� ���� ����� ����
	POINT_RESIST_ELEC,						// 76 ���� ���� : ������ݿ� ���� ����� ����
	POINT_RESIST_MAGIC,						// 77 ���� ���� : �������� ���� ����� ����
	POINT_RESIST_WIND,						// 78 �ٶ� ���� : �ٶ����ݿ� ���� ����� ����

	POINT_REFLECT_MELEE,					// 79 ���� �ݻ�

	/// Ư�� ���ؽ� ///
	POINT_REFLECT_CURSE,					// 80 ���� �ݻ�
	POINT_POISON_REDUCE,					// 81 �������� ����

	/// �� �Ҹ�� ///
	POINT_KILL_SP_RECOVER,					// 82 �� �Ҹ�� MP ȸ��
	POINT_EXP_DOUBLE_BONUS,					// 83
	POINT_GOLD_DOUBLE_BONUS,				// 84
	POINT_ITEM_DROP_BONUS,					// 85

	/// ȸ�� ���� ///
	POINT_POTION_BONUS,						// 86
	POINT_KILL_HP_RECOVERY,					// 87

	POINT_IMMUNE_STUN,						// 88
	POINT_IMMUNE_SLOW,						// 89
	POINT_IMMUNE_FALL,						// 90
	//////////////////

	POINT_PARTY_ATTACKER_BONUS,				// 91
	POINT_PARTY_TANKER_BONUS,				// 92

	POINT_ATT_BONUS,						// 93
	POINT_DEF_BONUS,						// 94

	POINT_ATT_GRADE_BONUS,					// 95
	POINT_DEF_GRADE_BONUS,					// 96
	POINT_MAGIC_ATT_GRADE_BONUS,			// 97
	POINT_MAGIC_DEF_GRADE_BONUS,			// 98

	POINT_RESIST_NORMAL_DAMAGE,				// 99

	POINT_HIT_HP_RECOVERY,					// 100
	POINT_HIT_SP_RECOVERY,					// 101
	POINT_MANASHIELD,						// 102 ��ż�ȣ ��ų�� ���� �������� ȿ�� ����

	POINT_PARTY_BUFFER_BONUS,				// 103
	POINT_PARTY_SKILL_MASTER_BONUS,			// 104

	POINT_HP_RECOVER_CONTINUE,				// 105
	POINT_SP_RECOVER_CONTINUE,				// 106

	POINT_STEAL_GOLD,						// 107
	POINT_POLYMORPH,						// 108 ������ ���� ��ȣ
	POINT_MOUNT,							// 109 Ÿ���ִ� ���� ��ȣ

	POINT_PARTY_HASTE_BONUS,				// 110
	POINT_PARTY_DEFENDER_BONUS,				// 111
	POINT_STAT_RESET_COUNT,					// 112 ���� �ܾ� ����� ���� ���� ���� ����Ʈ (1�� 1����Ʈ ���°���)

	POINT_HORSE_SKILL,						// 113

	POINT_MALL_ATTBONUS,					// 114 ���ݷ� +x%
	POINT_MALL_DEFBONUS,					// 115 ���� +x%
	POINT_MALL_EXPBONUS,					// 116 ����ġ +x%
	POINT_MALL_ITEMBONUS,					// 117 ������ ����� x/10��
	POINT_MALL_GOLDBONUS,					// 118 �� ����� x/10��

	POINT_MAX_HP_PCT,						// 119 �ִ������ +x%
	POINT_MAX_SP_PCT,						// 120 �ִ����ŷ� +x%

	POINT_SKILL_DAMAGE_BONUS,				// 121 ��ų ������ *(100+x)%
	POINT_NORMAL_HIT_DAMAGE_BONUS,			// 122 ��Ÿ ������ *(100+x)%

	// DEFEND_BONUS_ATTRIBUTES
	POINT_SKILL_DEFEND_BONUS,				// 123 ��ų ��� ������
	POINT_NORMAL_HIT_DEFEND_BONUS,			// 124 ��Ÿ ��� ������
	// END_OF_DEFEND_BONUS_ATTRIBUTES

	// PC_BANG_ITEM_ADD 
	POINT_PC_BANG_EXP_BONUS,				// 125 PC�� ���� ����ġ ���ʽ�
	POINT_PC_BANG_DROP_BONUS,				// 126 PC�� ���� ��ӷ� ���ʽ�
	// END_PC_BANG_ITEM_ADD
	POINT_RAMADAN_CANDY_BONUS_EXP,			// 127 �󸶴� ���� ����ġ ������

	POINT_ENERGY = 128,						// 128 ���

	// ��� ui ��.
	// �������� ���� �ʱ⸸, Ŭ���̾�Ʈ���� ����� �� �ð��� POINT�� �����ϱ� ������ �̷��� �Ѵ�.
	// �� �β�����
	POINT_ENERGY_END_TIME = 129,			// 129 ��� ���� �ð�

	POINT_COSTUME_ATTR_BONUS = 130,			// 130
	POINT_MAGIC_ATT_BONUS_PER = 131,		// 131
	POINT_MELEE_MAGIC_ATT_BONUS_PER = 132,	// 132

	// �߰� �Ӽ� ����
	POINT_RESIST_ICE = 133,					// 133 �ñ� ���� : �������ݿ� ���� ����� ����
	POINT_RESIST_EARTH = 134,				// 134 ���� ���� : �������ݿ� ���� ����� ����
	POINT_RESIST_DARK = 135,				// 135 ��� ���� : �������ݿ� ���� ����� ����

	POINT_RESIST_CRITICAL = 136,			// 136 ũ��Ƽ�� ���� : ����� ũ��Ƽ�� Ȯ���� ����
	POINT_RESIST_PENETRATE = 137,			// 137 ����Ÿ�� ���� : ����� ����Ÿ�� Ȯ���� ����

	POINT_BLEEDING_REDUCE = 138,			// 138
	POINT_BLEEDING_PCT = 139,				// 139
	POINT_ATTBONUS_WOLFMAN = 140,			// 140
	POINT_RESIST_WOLFMAN = 141,				// 141
	POINT_RESIST_CLAW = 142,				// 142

#if defined(__ACCE_COSTUME_SYSTEM__)
	POINT_ACCEDRAIN_RATE,					// 143
#endif

#if defined(__MAGIC_REDUCTION__)
	POINT_RESIST_MAGIC_REDUCTION,			// 144
#endif

#if defined(__ELEMENT_SYSTEM__)
	POINT_ENCHANT_ELECT,					// 145
	POINT_ENCHANT_FIRE,						// 146
	POINT_ENCHANT_ICE,						// 147
	POINT_ENCHANT_WIND,						// 148
	POINT_ENCHANT_EARTH,					// 149
	POINT_ENCHANT_DARK,						// 150
	POINT_ATTBONUS_CZ,						// 151
	POINT_ATTBONUS_SWORD,					// 152
	POINT_ATTBONUS_TWOHAND,					// 153
	POINT_ATTBONUS_DAGGER,					// 154
	POINT_ATTBONUS_BELL,					// 155
	POINT_ATTBONUS_FAN,						// 156
	POINT_ATTBONUS_BOW,						// 157
	POINT_ATTBONUS_CLAW,					// 158
	POINT_RESIST_HUMAN,						// 159
#endif
	POINT_ATTBONUS_STONE,					// 160

	// 20220314.owsap ������ ������ ����Ʈ.
	POINT_RESERVED_200 = 200,
	POINT_RESERVED_201,
	POINT_RESERVED_202,
	POINT_RESERVED_203,
	POINT_RESERVED_204,

#if defined(__CONQUEROR_LEVEL__)
	POINT_SUNGMA_STR = 219,
	POINT_SUNGMA_HP = 220,
	POINT_SUNGMA_MOVE = 221,
	POINT_SUNGMA_IMMUNE = 222,

	POINT_CONQUEROR_LEVEL = 223,
	POINT_CONQUEROR_LEVEL_STEP = 224,
	POINT_CONQUEROR_EXP = 225,
	POINT_CONQUEROR_NEXT_EXP = 226,
	POINT_CONQUEROR_POINT = 227,
#endif

	// 20220314.owsap Ư���� ����Ʈ�� ���� ����� �ֹ�.
	POINT_RESERVED_240 = 240,
#if defined(__CHEQUE_SYSTEM__)
	POINT_CHEQUE = 241,
#endif
#if defined(__GEM_SYSTEM__)
	POINT_GEM = 242,
#endif
#ifdef ENABLE_REFINE_ELEMENT
	POINT_ATT_ELEMENTS = 243,
#endif
#ifdef ENABLE_MEDAL_OF_HONOR
	POINT_MEDAL_OF_HONOR = 244,
#endif
#ifdef ENABLE_ATTR_6TH_7TH_EXTEND
	POINT_RESIST_FIST,
	POINT_SKILL_DAMAGE_SAMYEON,
	POINT_SKILL_DAMAGE_TANHWAN,
	POINT_SKILL_DAMAGE_PALBANG,
	POINT_SKILL_DAMAGE_GIGONGCHAM,
	POINT_SKILL_DAMAGE_GYOKSAN,
	POINT_SKILL_DAMAGE_GEOMPUNG,
	POINT_SKILL_DAMAGE_AMSEOP,
	POINT_SKILL_DAMAGE_GUNGSIN,
	POINT_SKILL_DAMAGE_CHARYUN,
	POINT_SKILL_DAMAGE_SANGONG,
	POINT_SKILL_DAMAGE_YEONSA,
	POINT_SKILL_DAMAGE_KWANKYEOK,
	POINT_SKILL_DAMAGE_GIGUNG,
	POINT_SKILL_DAMAGE_HWAJO,
	POINT_SKILL_DAMAGE_SWAERYUNG,
	POINT_SKILL_DAMAGE_YONGKWON,
	POINT_SKILL_DAMAGE_PABEOB,
	POINT_SKILL_DAMAGE_MARYUNG ,
	POINT_SKILL_DAMAGE_HWAYEOMPOK,
	POINT_SKILL_DAMAGE_MAHWAN,
	POINT_SKILL_DAMAGE_BIPABU,
	POINT_SKILL_DAMAGE_YONGBI,
	POINT_SKILL_DAMAGE_PAERYONG,
	POINT_SKILL_DAMAGE_NOEJEON,
	POINT_SKILL_DAMAGE_BYEURAK,
	POINT_SKILL_DAMAGE_CHAIN,
	POINT_SKILL_DAMAGE_CHAYEOL,
	POINT_SKILL_DAMAGE_SALPOONG,
	POINT_SKILL_DAMAGE_GONGDAB,
	POINT_SKILL_DAMAGE_PASWAE,
	POINT_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE,
	POINT_SKILL_DEFEND_BONUS_BOSS_OR_MORE,
	POINT_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE,
	POINT_SKILL_DAMAGE_BONUS_BOSS_OR_MORE,
	POINT_HIT_BUFF_ENCHANT_FIRE,
	POINT_HIT_BUFF_ENCHANT_ICE,
	POINT_HIT_BUFF_ENCHANT_ELEC,
	POINT_HIT_BUFF_ENCHANT_WIND,
	POINT_HIT_BUFF_ENCHANT_DARK,
	POINT_HIT_BUFF_ENCHANT_EARTH,
	POINT_HIT_BUFF_RESIST_FIRE,
	POINT_HIT_BUFF_RESIST_ICE,
	POINT_HIT_BUFF_RESIST_ELEC,
	POINT_HIT_BUFF_RESIST_WIND,
	POINT_HIT_BUFF_RESIST_DARK,
	POINT_HIT_BUFF_RESIST_EARTH,
	POINT_USE_SKILL_CHEONGRANG_MOV_SPEED,
	POINT_USE_SKILL_CHEONGRANG_CASTING_SPEED,
	POINT_USE_SKILL_CHAYEOL_CRITICAL_PCT,
	POINT_USE_SKILL_SANGONG_ATT_GRADE_BONUS,
	POINT_USE_SKILL_GIGUNG_ATT_GRADE_BONUS,
	POINT_USE_SKILL_JEOKRANG_DEF_BONUS,
	POINT_USE_SKILL_GWIGEOM_DEF_BONUS,
	POINT_USE_SKILL_TERROR_ATT_GRADE_BONUS,
	POINT_USE_SKILL_MUYEONG_ATT_GRADE_BONUS,
	POINT_USE_SKILL_MANASHILED_CASTING_SPEED,
	POINT_USE_SKILL_HOSIN_DEF_BONUS,
	POINT_USE_SKILL_GICHEON_ATT_GRADE_BONUS,
	POINT_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS,
	POINT_USE_SKILL_JEUNGRYEOK_DEF_BONUS,
	POINT_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS,
	POINT_USE_SKILL_CHUNKEON_CASTING_SPEED,
	POINT_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS,
	POINT_SKILL_DURATION_INCREASE_EUNHYUNG,
	POINT_SKILL_DURATION_INCREASE_GYEONGGONG,
	POINT_SKILL_DURATION_INCREASE_GEOMKYUNG,
	POINT_SKILL_DURATION_INCREASE_JEOKRANG,
	POINT_USE_SKILL_PALBANG_HP_ABSORB,
	POINT_USE_SKILL_AMSEOP_HP_ABSORB,
	POINT_USE_SKILL_YEONSA_HP_ABSORB,
	POINT_USE_SKILL_YONGBI_HP_ABSORB,
	POINT_USE_SKILL_CHAIN_HP_ABSORB,
	POINT_USE_SKILL_PASWAE_SP_ABSORB,
	POINT_USE_SKILL_GIGONGCHAM_STUN,
	POINT_USE_SKILL_CHARYUN_STUN,
	POINT_USE_SKILL_PABEOB_STUN,
	POINT_USE_SKILL_MAHWAN_STUN,
	POINT_USE_SKILL_GONGDAB_STUN,
	POINT_USE_SKILL_SAMYEON_STUN,
	POINT_USE_SKILL_GYOKSAN_KNOCKBACK,
	POINT_USE_SKILL_SEOMJEON_KNOCKBACK,
	POINT_USE_SKILL_SWAERYUNG_KNOCKBACK,
	POINT_USE_SKILL_HWAYEOMPOK_KNOCKBACK,
	POINT_USE_SKILL_GONGDAB_KNOCKBACK,
	POINT_USE_SKILL_KWANKYEOK_KNOCKBACK,
	POINT_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER,
	POINT_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER,
	POINT_DAMAGE_HP_RECOVERY,
	POINT_DAMAGE_SP_RECOVERY,
	POINT_ALIGNMENT_DAMAGE_BONUS,
	POINT_NORMAL_DAMAGE_GUARD,
	POINT_MORE_THEN_HP90_DAMAGE_REDUCE,
	POINT_USE_SKILL_TUSOK_HP_ABSORB,
	POINT_USE_SKILL_PAERYONG_HP_ABSORB,
	POINT_USE_SKILL_BYEURAK_HP_ABSORB,
	POINT_FIRST_ATTRIBUTE_BONUS,
	POINT_SECOND_ATTRIBUTE_BONUS,
	POINT_THIRD_ATTRIBUTE_BONUS,
	POINT_FOURTH_ATTRIBUTE_BONUS,
	POINT_FIFTH_ATTRIBUTE_BONUS,
	POINT_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER ,
	POINT_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER,
	POINT_USE_SKILL_CHAYEOL_HP_ABSORB,
	POINT_HIT_PCT,
	POINT_ATTBONUS_PER_HUMAN,
	POINT_ATTBONUS_PER_ANIMAL,
	POINT_ATTBONUS_PER_ORC,
	POINT_ATTBONUS_PER_MILGYO,
	POINT_ATTBONUS_PER_UNDEAD,
	POINT_ATTBONUS_PER_DEVIL,
	POINT_ENCHANT_PER_ELECT,
	POINT_ENCHANT_PER_FIRE,
	POINT_ENCHANT_PER_ICE,
	POINT_ENCHANT_PER_WIND,
	POINT_ENCHANT_PER_EARTH,
	POINT_ENCHANT_PER_DARK,
	POINT_ATTBONUS_PER_CZ,
	POINT_ATTBONUS_PER_INSECT,
	POINT_ATTBONUS_PER_DESERT,
	POINT_ATTBONUS_PER_STONE,
	POINT_ATTBONUS_PER_MONSTER,
	POINT_RESIST_PER_HUMAN,
	POINT_RESIST_PER_ICE,
	POINT_RESIST_PER_DARK,
	POINT_RESIST_PER_EARTH,
	POINT_RESIST_PER_FIRE,
	POINT_RESIST_PER_ELEC,
	POINT_RESIST_PER_MAGIC,
	POINT_RESIST_PER_WIND,
	POINT_HIT_BUFF_SUNGMA_STR,
	POINT_HIT_BUFF_SUNGMA_MOVE,
	POINT_HIT_BUFF_SUNGMA_HP,
	POINT_HIT_BUFF_SUNGMA_IMMUNE,
	POINT_MOUNT_MELEE_MAGIC_ATTBONUS_PER,
	POINT_DISMOUNT_MOVE_SPEED_BONUS_PER,
	POINT_HIT_AUTO_HP_RECOVERY,
	POINT_HIT_AUTO_SP_RECOVERY,
	POINT_USE_SKILL_COOLTIME_DECREASE_ALL,
	POINT_HIT_STONE_ATTBONUS_STONE,
	POINT_HIT_STONE_DEF_GRADE_BONUS,
	POINT_KILL_BOSS_ITEM_BONUS,
	POINT_MOB_HIT_MOB_AGGRESSIVE,
	POINT_NO_DEATH_AND_HP_RECOVERY30,
	POINT_AUTO_PICKUP,
	POINT_MOUNT_NO_KNOCKBACK,
	POINT_SUNGMA_PER_STR,
	POINT_SUNGMA_PER_HP,
	POINT_SUNGMA_PER_MOVE,
	POINT_SUNGMA_PER_IMMUNE,
	POINT_IMMUNE_POISON100,
	POINT_IMMUNE_BLEEDING100,
	POINT_MONSTER_DEFEND_BONUS,
#endif
	//POINT_MAX_NUM = 255, // common/length.h
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM
};

enum EPositions
{
	POS_DEAD,
	POS_SLEEPING,
	POS_RESTING,
	POS_SITTING,
	POS_FISHING,
	POS_FIGHTING,
	POS_MOUNTING,
	POS_STANDING
};

enum EBlockAction
{
	BLOCK_EXCHANGE = (1 << 0),
	BLOCK_PARTY_INVITE = (1 << 1),
	BLOCK_GUILD_INVITE = (1 << 2),
	BLOCK_WHISPER = (1 << 3),
	BLOCK_MESSENGER_INVITE = (1 << 4),
	BLOCK_PARTY_REQUEST = (1 << 5),
};

// <Factor> Dynamically evaluated CHARACTER* equivalent.
// Referring to SCharDeadEventInfo.
struct DynamicCharacterPtr
{
	DynamicCharacterPtr() : is_pc(false), id(0) {}
	DynamicCharacterPtr(const DynamicCharacterPtr& o)
		: is_pc(o.is_pc), id(o.id) {}

	// Returns the LPCHARACTER found in CHARACTER_MANAGER.
	LPCHARACTER Get() const;
	// Clears the current settings.
	void Reset()
	{
		is_pc = false;
		id = 0;
	}

	// Basic assignment operator.
	DynamicCharacterPtr& operator=(const DynamicCharacterPtr& rhs)
	{
		is_pc = rhs.is_pc;
		id = rhs.id;
		return *this;
	}
	// Supports assignment with LPCHARACTER type.
	DynamicCharacterPtr& operator=(LPCHARACTER character);
	// Supports type casting to LPCHARACTER.
	operator LPCHARACTER() const
	{
		return Get();
	}

	bool is_pc;
	uint32_t id;
};

/* �����ϴ� ������ */
typedef struct character_point
{
	int64_t points[POINT_MAX_NUM];

	BYTE job;
	BYTE voice;

	BYTE level;
	DWORD exp;

#if defined(__CONQUEROR_LEVEL__)
	BYTE conqueror_level;
	DWORD conqueror_exp;
#endif

	long long gold;
#if defined(__CHEQUE_SYSTEM__)
	int cheque;
#endif
#if defined(__GEM_SYSTEM__)
	int gem;
#endif

#ifdef ENABLE_MEDAL_OF_HONOR
	int medal_honor;
#endif
#if defined(ENABLE_BATTLE_FIELD) || defined(ENABLE_MEDAL_OF_HONOR)
	int aiShopExUsablePoint[SHOP_COIN_TYPE_MAX];
	int aiShopExDailyUse[SHOP_COIN_TYPE_MAX];
#endif

	int64_t hp;
	int64_t sp;

	int iRandomHP;
	int iRandomSP;

	int stamina;

	BYTE skill_group;
} CHARACTER_POINT;

/* ������� �ʴ� ĳ���� ������ */
typedef struct character_point_instant
{
	long long points[POINT_MAX_NUM];

	float fRot;

	int64_t iMaxHP;
	int64_t iMaxSP;

	long position;

	long instant_flag;
	DWORD dwAIFlag;
	DWORD dwImmuneFlag;
	DWORD dwLastShoutPulse;

	WORD parts[PART_MAX_NUM];

	LPITEM pItems[INVENTORY_AND_EQUIP_SLOT_MAX];
	WORD bItemGrid[INVENTORY_AND_EQUIP_SLOT_MAX];

	// ��ȥ�� �κ��丮.
	LPITEM pDSItems[DRAGON_SOUL_INVENTORY_MAX_NUM];
	WORD wDSItemGrid[DRAGON_SOUL_INVENTORY_MAX_NUM];

#ifdef ENABLE_SWITCHBOT
	LPITEM pSwitchbotItems[SWITCHBOT_SLOT_COUNT];
#endif

	// by mhh
#ifndef ENABLE_CUBE_RENEWAL
	LPITEM pCubeItems[CUBE_MAX_NUM];
#endif
	LPCHARACTER pCubeNpc;
#if defined(__ACCE_COSTUME_SYSTEM__)
	LPITEM pAcceMaterials[ACCE_WINDOW_MAX_MATERIALS];
#endif

#ifdef __AURA_SYSTEM__
	LPENTITY m_pAuraRefineWindowOpener;
#endif

	LPCHARACTER battle_victim;

	BYTE gm_level;

	BYTE bBasePart; // ��� ��ȣ

	int iMaxStamina;

	BYTE bBlockMode;

	int iDragonSoulActiveDeck;
	LPENTITY m_pDragonSoulRefineWindowOpener;
} CHARACTER_POINT_INSTANT;

#define TRIGGERPARAM LPCHARACTER ch, LPCHARACTER causer

typedef struct trigger
{
	BYTE type;
	int (*func) (TRIGGERPARAM);
	long value;
} TRIGGER;

class CTrigger
{
public:
	CTrigger() : bType(0), pFunc(NULL)
	{
	}

	BYTE bType;
	int (*pFunc) (TRIGGERPARAM);
};

EVENTINFO(char_event_info)
{
	DynamicCharacterPtr ch;
};

#ifdef ENABLE_ATTR_6TH_7TH_EXTEND
EVENTINFO(hitbuff_event_info)
{
	DynamicCharacterPtr ch;
};
#endif

#ifdef ENABLE_ELEMENTAL_WORLD
EVENTINFO(elemental_world_event_info)
{
	DynamicCharacterPtr ch;
};
#endif

struct TSkillUseInfo
{
	int iHitCount;
	int iMaxHitCount;
	int iSplashCount;
	DWORD dwNextSkillUsableTime;
	int iRange;
	bool bUsed;
	DWORD dwVID;
	bool isGrandMaster;

	boost::unordered_map<DWORD, size_t> TargetVIDMap;

	TSkillUseInfo()
		: iHitCount(0), iMaxHitCount(0), iSplashCount(0), dwNextSkillUsableTime(0), iRange(0), bUsed(false),
		dwVID(0), isGrandMaster(false)
	{}

	bool HitOnce(DWORD dwVnum = 0);

	bool UseSkill(bool isGrandMaster, DWORD vid, DWORD dwCooltime, int splashcount = 1, int hitcount = -1, int range = -1);
	DWORD GetMainTargetVID() const { return dwVID; }
	void SetMainTargetVID(DWORD vid) { dwVID = vid; }
	void ResetHitCount() { if (iSplashCount) { iHitCount = iMaxHitCount; iSplashCount--; } }
};

typedef struct packet_party_update TPacketGCPartyUpdate;
class CExchange;
class CSkillProto;
class CParty;
class CDungeon;
class CWarMap;
class CAffect;
class CGuild;
class CSafebox;
class CArena;
#ifdef __ENABLE_NEW_OFFLINESHOP__
namespace offlineshop
{
	class CShop;
	class CShopSafebox;
	class CAuction;
}
#endif
class CShop;
typedef class CShop* LPSHOP;

class CMob;
class CMobInstance;
typedef struct SMobSkillInfo TMobSkillInfo;

// SKILL_POWER_BY_LEVEL
extern int GetSkillPowerByLevelFromType(int job, int skillgroup, int skilllevel);
// END_SKILL_POWER_BY_LEVEL

namespace marriage
{
	class WeddingMap;
}
enum e_overtime
{
	OT_NONE,
	OT_3HOUR,
	OT_5HOUR,
};

typedef std::list<CAffect*> AffectContainerList;
typedef TR1_NS::unordered_map<DWORD, BYTE> AffectStackMap;
typedef std::map<int, LPEVENT> MobSkillEventMap;

class CHARACTER : public CEntity, public CFSM, public CHorseRider
{
#ifdef ENABLE_REFINE_ELEMENT
public:
	bool ElementsSpellItem(LPITEM pkItem, LPITEM pkTarget);
	bool IsOpenElementsSpell() const noexcept { return m_OpenElementsSpell ? true : false; }
	void SetOpenElementsSpell(bool b, int iAdditionalCell = -1);
	void ElementsSpellItemFunc(int pos, uint8_t type_select = -1);
	uint8_t GetElementsEffect();

private:
	bool m_OpenElementsSpell;
	int m_iElementsAdditionalCell;
#endif

public:
	void WonExchange(BYTE bOption, WORD wValue);

#ifdef __AURA_SYSTEM__
private:
	BYTE		m_bAuraRefineWindowType;
	bool		m_bAuraRefineWindowOpen;
	TItemPos	m_pAuraRefineWindowItemSlot[AURA_SLOT_MAX];
	TAuraRefineInfo m_bAuraRefineInfo[AURA_REFINE_INFO_SLOT_MAX];

protected:
	BYTE		__GetAuraAbsorptionRate(BYTE bLevel, BYTE bBoostIndex) const;
	TAuraRefineInfo __GetAuraRefineInfo(TItemPos Cell);
	TAuraRefineInfo __CalcAuraRefineInfo(TItemPos Cell, TItemPos MaterialCell);
	TAuraRefineInfo __GetAuraEvolvedRefineInfo(TItemPos Cell);

#if defined(__SET_ITEM__)
public:
	using SetItemCountMap = std::map<BYTE, BYTE>;
	SetItemCountMap GetItemSetCountMap() const;

	void RefreshItemSetBonus();
	void RefreshItemSetBonusByValue();
#endif

public:
	void		OpenAuraRefineWindow(LPENTITY pOpener, EAuraWindowType type);
	bool		IsAuraRefineWindowOpen() const { return  m_bAuraRefineWindowOpen; }
	BYTE		GetAuraRefineWindowType() const { return  m_bAuraRefineWindowType; }
	LPENTITY	GetAuraRefineWindowOpener() { return  m_pointsInstant.m_pAuraRefineWindowOpener; }

	bool		IsAuraRefineWindowCanRefine();

	void		AuraRefineWindowCheckIn(BYTE bAuraRefineWindowType, TItemPos AuraCell, TItemPos ItemCell);
	void		AuraRefineWindowCheckOut(BYTE bAuraRefineWindowType, TItemPos AuraCell);
	void		AuraRefineWindowAccept(BYTE bAuraRefineWindowType);
	void		AuraRefineWindowClose();
#endif

#ifdef ENABLE_12ZI
public:
	void BeadTime();
	void CheckFloor();

	void SetAnimaSphere(int amount);
	int GetAnimaSphere();

	void IsZodiacEffectMob();
	void IsZodiacEffectPC(uint32_t Monster);

	void ZodiacFloorMessage(uint8_t Floor);

	void SetLastZodiacAttackTime(uint32_t time) { m_dwLastZodiacAttackTime = time; }
	uint32_t GetLastZodiacAttackTime() const { return m_dwLastZodiacAttackTime; }

	void IncDeadCount() { m_dwDeadCount++; }
	void SetDeadCount(uint32_t dead) { m_dwDeadCount = dead; }
	uint32_t GetDeadCount() const { return m_dwDeadCount; }

	void EffectZodiacPacket(long X, long Y, int enumEffectType, int enumEffectType2 = 0);

	bool IsZodiacBoss();
	bool IsZodiacOfficer();
	bool IsZodiacStatue();
	uint16_t GetStatueVnum() const;

	LPEVENT m_pkZodiacSkill1;
	LPEVENT m_pkZodiacSkill2;
	LPEVENT m_pkZodiacSkill3;
	LPEVENT m_pkZodiacSkill4;
	LPEVENT m_pkZodiacSkill5;
	LPEVENT m_pkZodiacSkill6;
	LPEVENT m_pkZodiacSkill7;
	LPEVENT m_pkZodiacSkill8;
	LPEVENT m_pkZodiacSkill9;
	LPEVENT m_pkZodiacSkill10;
	LPEVENT m_pkZodiacSkill11;

	void ZodiacDamage(uint8_t Type, LPCHARACTER Victim = nullptr);

	int ZodiacSkillAttack(LPCHARACTER Victim, uint32_t dwVnum, uint8_t bSkillLevel);

	uint32_t CountZodiacItems(uint32_t Vnum);
	void SetZodiacItems(uint32_t Vnum, int Count);

	uint32_t PurchaseCountZodiacItems(uint32_t Vnum);
	void SetPurchaseZodiacItems(uint32_t Vnum, int Count);

	void CanonDamage();

	void SpawnZodiacGroup(LPZODIAC pZodiac);
	void SpawnZodiacStone(LPZODIAC pZodiac);

	void ZTT_CHECK_BOX(int color, int index);
	void ZTT_LOAD_INFO();
	int ZTT_CHECK_ITEM_ROW(int color, int index);
	int ZTT_CHECK_ITEM_COLUMN(int color, int index);
	void ZTT_CHECK_REWARD();
	void ZTT_REWARD(int type);

	uint16_t IsZodiacCannon() { return GetRaceNum() == 20464; }

private:
	uint32_t m_dwLastZodiacAttackTime;
	uint32_t m_dwDeadCount;
#endif

protected:
	//////////////////////////////////////////////////////////////////////////////////
	// Entity ����
	virtual void EncodeInsertPacket(LPENTITY entity);
	virtual void EncodeRemovePacket(LPENTITY entity);
	//////////////////////////////////////////////////////////////////////////////////

public:
	LPCHARACTER FindCharacterInView(const char* name, bool bFindPCOnly);
	void UpdatePacket();
#ifdef ENABLE_TELEPORT_WINDOW
	LPEVENT m_pkTeleportEvent;
#endif

	//////////////////////////////////////////////////////////////////////////////////
	// FSM (Finite State Machine) ����
protected:
	CStateTemplate<CHARACTER> m_stateMove;
	CStateTemplate<CHARACTER> m_stateBattle;
	CStateTemplate<CHARACTER> m_stateIdle;

public:
	virtual void StateMove();
	virtual void StateBattle();
	virtual void StateIdle();
	virtual void StateFlag();
	virtual void StateFlagBase();
	void StateHorse();

protected:
	// STATE_IDLE_REFACTORING
	void __StateIdle_Monster();
	void __StateIdle_Stone();
	void __StateIdle_NPC();
	// END_OF_STATE_IDLE_REFACTORING

public:
	DWORD GetAIFlag() const { return m_pointsInstant.dwAIFlag; }

	void SetAggressive();
	bool IsAggressive() const;

	void SetCoward();
	bool IsCoward() const;
	void CowardEscape();

	void SetNoAttackShinsu();
	bool IsNoAttackShinsu() const;

	void SetNoAttackChunjo();
	bool IsNoAttackChunjo() const;

	void SetNoAttackJinno();
	bool IsNoAttackJinno() const;

	void SetAttackMob();
	bool IsAttackMob() const;

	virtual void BeginStateEmpty();
	virtual void EndStateEmpty() {}

	void Restart(const uint8_t c_uiSubCmd);
	void RestartAtSamePos();

protected:
	DWORD m_dwStateDuration;
	//////////////////////////////////////////////////////////////////////////////////

public:
	CHARACTER();
	virtual ~CHARACTER();

	void Create(const char* c_pszName, DWORD vid, bool isPC);
	void Destroy();

	//void Disconnect(const char* c_pszReason);
	void Disconnect(const char * c_pszReason, int type = -1);

protected:
	void Initialize();

	//////////////////////////////////////////////////////////////////////////////////
	// Basic Points

#if defined(__SEND_TARGET_INFO__)
private:
	DWORD dwLastTargetInfoPulse;

public:
	DWORD GetLastTargetInfoPulse() const { return dwLastTargetInfoPulse; }
	void SetLastTargetInfoPulse(DWORD pulse) { dwLastTargetInfoPulse = pulse; }
#endif

public:
	DWORD GetPlayerID() const { return m_dwPlayerID; }

	void SetPlayerProto(const TPlayerTable* table);
	void CreatePlayerProto(TPlayerTable& tab); // ���� �� ���

	void SetProto(const CMob* c_pkMob);
	WORD GetRaceNum() const;

	void Save(); // DelayedSave
	void SaveReal(); // ���� ����
	void FlushDelayedSaveItem();

#if defined(__EXTENDED_BLEND_AFFECT__)
	bool UseExtendedBlendAffect(LPITEM item, int affect_type, int apply_type, int apply_value, int apply_duration);
#endif
#if defined(__BLEND_AFFECT__)
	bool SetBlendAffect(LPITEM item);
#endif

	const char* GetName() const;
	const VID& GetVID() const { return m_vid; }

	void SetName(const std::string& name) { m_stName = name; }

	void SetRace(BYTE race);
	bool ChangeSex();

	bool IsFemale() const;
	bool IsMale() const;

	DWORD GetAID() const;
	int GetChangeEmpireCount() const;
	void SetChangeEmpireCount();
	int ChangeEmpire(BYTE empire);

	BYTE GetJob() const;
	BYTE GetCharType() const;

	bool IsPC() const { return GetDesc() ? true : false; }
	bool IsNPC() const { return m_bCharType != CHAR_TYPE_PC; }

	bool IsMonster() const { return m_bCharType == CHAR_TYPE_MONSTER; }
	bool IsStone() const { return m_bCharType == CHAR_TYPE_STONE; }
	bool IsDoor() const { return m_bCharType == CHAR_TYPE_DOOR; }
	bool IsBuilding() const { return m_bCharType == CHAR_TYPE_BUILDING; }
	bool IsWarp() const { return m_bCharType == CHAR_TYPE_WARP; }
	bool IsGoto() const { return m_bCharType == CHAR_TYPE_GOTO; }
	bool IsHorse() const { return m_bCharType == CHAR_TYPE_HORSE; }

	DWORD GetLastShoutPulse() const { return m_pointsInstant.dwLastShoutPulse; }
	void SetLastShoutPulse(DWORD pulse) { m_pointsInstant.dwLastShoutPulse = pulse; }
	int GetLevel() const { return m_points.level; }
	void SetLevel(BYTE level);

#if defined(__CONQUEROR_LEVEL__)
	void SetConqueror(bool bSet = true);
	int GetConquerorLevel() const { return m_points.conqueror_level; }
	void SetConquerorLevel(BYTE level) { m_points.conqueror_level = level; }

	DWORD GetConquerorExp() const { return m_points.conqueror_exp; }
	void SetConquerorExp(DWORD exp) { m_points.conqueror_exp = exp; }
	DWORD GetConquerorNextExp() const;
#endif

	BYTE GetGMLevel() const;
	BOOL IsGM() const;
	void SetGMLevel();

	DWORD GetExp() const { return m_points.exp; }
	void SetExp(DWORD exp) { m_points.exp = exp; }
	DWORD GetNextExp() const;
	LPCHARACTER DistributeExp(); // ���� ���� ���� ����� �����Ѵ�.
	void DistributeHP(LPCHARACTER pkKiller);
	void DistributeSP(LPCHARACTER pkKiller, int iMethod = 0);

	void SetPosition(int pos);
	bool IsPosition(int pos) const { return m_pointsInstant.position == pos ? true : false; }
	int GetPosition() const { return m_pointsInstant.position; }

	void SetPart(BYTE bPartPos, WORD wVal);
	DWORD GetPart(BYTE bPartPos) const;
	DWORD GetOriginalPart(BYTE bPartPos) const;

	void SetHP(int64_t hp) { m_points.hp = hp; }
	int64_t GetHP() const { return m_points.hp; }

	void SetSP(int64_t sp) { m_points.sp = sp; }
	int64_t GetSP() const { return m_points.sp; }

	void SetMaxHP(int64_t iVal) { m_pointsInstant.iMaxHP = iVal; }
#if defined(__CONQUEROR_LEVEL__)
	int64_t GetMaxHP() const;
#else
	int64_t GetMaxHP() const { return m_pointsInstant.iMaxHP; }
#endif

	void SetMaxSP(int64_t iVal) { m_pointsInstant.iMaxSP = iVal; }
	int64_t GetMaxSP() const { return m_pointsInstant.iMaxSP; }

	void SetStamina(int64_t stamina) { m_points.stamina = stamina; }
	int64_t GetStamina() const { return m_points.stamina; }

	void SetMaxStamina(int64_t iVal) { m_pointsInstant.iMaxStamina = iVal; }
	int64_t GetMaxStamina() const { return m_pointsInstant.iMaxStamina; }

	void SetRandomHP(int v) { m_points.iRandomHP = v; }
	void SetRandomSP(int v) { m_points.iRandomSP = v; }

	int GetRandomHP() const { return m_points.iRandomHP; }
	int GetRandomSP() const { return m_points.iRandomSP; }

	int GetHPPct() const;

	void SetRealPoint(uint16_t idx, int64_t val); //@fixme532
	int64_t GetRealPoint(uint16_t idx) const; //@fixme532

	void SetPoint(uint16_t idx, int64_t val); //@fixme532
	int64_t GetPoint(uint16_t idx) const; //@fixme532
	int64_t GetLimitPoint(uint16_t idx) const; //@fixme532
	int64_t GetPolymorphPoint(uint16_t idx) const; //@fixme532

	const TMobTable& GetMobTable() const;
	BYTE GetMobRank() const;
	BYTE GetMobBattleType() const;
	BYTE GetMobSize() const;
	DWORD GetMobDamageMin() const;
	DWORD GetMobDamageMax() const;
	WORD GetMobAttackRange() const;
	DWORD GetMobDropItemVnum() const;
	float GetMobDamageMultiply() const;
#if defined(__ELEMENT_SYSTEM__)
	int GetMobElement(BYTE bElement) const;
#endif

	// NEWAI
	bool IsBerserker() const;
	bool IsBerserk() const;
	void SetBerserk(bool mode);

	bool IsStoneSkinner() const;

	bool IsGodSpeeder() const;
	bool IsGodSpeed() const;
	void SetGodSpeed(bool mode);

	bool IsDeathBlower() const;
	bool IsDeathBlow() const;

	bool IsHealer() const;

	bool IsReviver() const;
	bool HasReviverInParty() const;
	bool IsRevive() const;
	void SetRevive(bool mode);
	// NEWAI END

	bool IsRaceFlag(DWORD dwBit) const;
	bool IsSummonMonster() const;
	DWORD GetSummonVnum() const;

	int m_newSummonInterval;
	int m_lastSummonTime;
#if defined(__EREBUS_DUNGEON__)
	int m_iLastHealerSummonDelay;
#endif
	bool CanSummonMonster() const;
	void MarkSummonedMonster();

	DWORD GetPolymorphItemVnum() const;
	DWORD GetMonsterDrainSPPoint() const;

	void MainCharacterPacket(); // ���� ����ĳ���Ͷ�� �����ش�.

	void ComputePoints();
	void ComputeBattlePoints();
	void PointChange(uint16_t type, int64_t amount, bool bAmount = false, bool bBroadcast = false); //@fixme532
	void PointsPacket();
	void UpdatePointsPacket(BYTE type, long long val, long long amount = 0, bool bAmount = false, bool bBroadcast = false);
	void ApplyPoint(uint16_t bApplyType, int iVal); //@fixme532
	void CheckMaximumPoints(); // HP, SP ���� ���� ���� �ִ밪 ���� ������ �˻��ϰ� ���ٸ� �����.

#if defined(__WJ_SHOW_MOB_INFO__)
	bool Show(long lMapIndex, long x, long y, long z = LONG_MAX, bool bShowSpawnMotion = false, bool bAggressive = false);
#else
	bool Show(long lMapIndex, long x, long y, long z = LONG_MAX, bool bShowSpawnMotion = false);
#endif

	void Sitdown(int is_ground);
	void Standup();

	void SetRotation(float fRot);
	void SetRotationToXY(long x, long y);
	float GetRotation() const { return m_pointsInstant.fRot; }

	void MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion* packet);
	void Motion(BYTE motion, LPCHARACTER victim = NULL);

	void ChatPacket(BYTE type, const char* format, ...);
	void MonsterChat(BYTE bMonsterChatType);
	void SendGreetMessage();

	void ResetPoint(int iLv);
	void ResetExp();

	void SetBlockMode(BYTE bFlag);
	void SetBlockModeForce(BYTE bFlag);
	bool IsBlockMode(BYTE bFlag) const { return (m_pointsInstant.bBlockMode & bFlag) ? true : false; }

	bool IsPolymorphed() const { return m_dwPolymorphRace > 0; }
	bool IsPolyMaintainStat() const { return m_bPolyMaintainStat; } // ���� ������ �����ϴ� ��������.
	void SetPolymorph(DWORD dwRaceNum, bool bMaintainStat = false);
	DWORD GetPolymorphVnum() const { return m_dwPolymorphRace; }
	int GetPolymorphPower() const;

	// FISING
	void fishing();
	void fishing_take();
	// END_OF_FISHING

	// MINING
	void mining(LPCHARACTER chLoad);
	void mining_cancel();
	void mining_take();
	// END_OF_MINING

#if defined(__ANTI_EXP_RING__)
	bool HasFrozenEXP();
	void FreezeEXP();
	void UnfreezeEXP();
#endif

#if defined(__SOUL_SYSTEM__)
	void UseSoulAttack(BYTE bSoulType);
	bool CheckSoulItem();
#endif

	void ResetPlayTime(DWORD dwTimeRemain = 0);

	void CreateFly(BYTE bType, LPCHARACTER pkVictim);

	void ResetChatCounter() { m_bChatCounter = 0; }
	void IncreaseChatCounter() { ++m_bChatCounter; }
	BYTE GetChatCounter() const { return m_bChatCounter; }

#ifdef ENABLE_GROWTH_PET_SYSTEM
	void EvolvePetRace(uint32_t dwRaceNum);
	void SendPetLevelUpEffect(int vid, int value);
#endif

// #ifdef ENABLE_SET_ITEM
// 	void GetSetCount(int& setID, int& setCount);
// #endif

	void ResetWhisperCounter() { m_bWhisperCounter = 0; }
	bool IncreaseWhisperCounter() { ++m_bWhisperCounter; return m_bWhisperCounter; }
	BYTE GetWhisperCounter() const { return m_bWhisperCounter; }

protected:
	DWORD m_dwPolymorphRace;
	bool m_bPolyMaintainStat;
	DWORD m_dwLoginPlayTime;
	DWORD m_dwPlayerID;
	VID m_vid;
	std::string m_stName;
	BYTE m_bCharType;

	CHARACTER_POINT m_points;
	CHARACTER_POINT_INSTANT m_pointsInstant;

	int m_iMoveCount;
	DWORD m_dwPlayStartTime;
	BYTE m_bAddChrState;
	bool m_bSkipSave;
	BYTE m_bChatCounter;
	BYTE m_bWhisperCounter;

	// End of Basic Points

	//////////////////////////////////////////////////////////////////////////////////
	// Move & Synchronize Positions
	//////////////////////////////////////////////////////////////////////////////////
public:
	bool IsStateMove() const { return IsState((CState&)m_stateMove); }
	bool IsStateIdle() const { return IsState((CState&)m_stateIdle); }
	bool IsWalking() const { return m_bNowWalking || GetStamina() <= 0; }
	void SetWalking(bool bWalkFlag) { m_bWalking = bWalkFlag; }
	void SetNowWalking(bool bWalkFlag);
	void ResetWalking() { SetNowWalking(m_bWalking); }

	bool Goto(long x, long y); // �ٷ� �̵� ��Ű�� �ʰ� ��ǥ ��ġ�� BLENDING ��Ų��.
	void Stop();

	bool CanMove() const; // �̵��� �� �ִ°�?

	void SyncPacket();
	bool Sync(long x, long y); // ���� �� �޼ҵ�� �̵� �Ѵ� (�� �� ���ǿ� ���� �̵� �Ұ��� ����)
	bool Move(long x, long y); // ������ �˻��ϰ� Sync �޼ҵ带 ���� �̵� �Ѵ�.
	void OnMove(bool bIsAttack = false); // �����϶� �Ҹ���. Move() �޼ҵ� �̿ܿ����� �Ҹ� �� �ִ�.
	DWORD GetMotionMode() const;
	float GetMoveMotionSpeed() const;
	float GetMoveSpeed() const;
	void CalculateMoveDuration();
	void SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime = 0, int iRot = -1);

	DWORD GetCurrentMoveDuration() const { return m_dwMoveDuration; }
	DWORD GetWalkStartTime() const { return m_dwWalkStartTime; }
	DWORD GetLastMoveTime() const { return m_dwLastMoveTime; }
	DWORD GetLastAttackTime() const { return m_dwLastAttackTime; }

	void SetLastAttacked(DWORD time); // ���������� ���ݹ��� �ð� �� ��ġ�� ������

	bool SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList = true);
	bool IsSyncOwner(LPCHARACTER ch) const;

#ifdef ENABLE_12ZI
	void SetLastZodiacCzLastTime(int time) { m_dwZodiacCzLastTime = time; }
	int GetLastZodiacCzLastTime() const { return m_dwZodiacCzLastTime; }
#endif

	bool WarpSet(long x, long y, long lRealMapIndex = 0);
	void SetWarpLocation(long lMapIndex, long x, long y);
	void WarpEnd();
	const PIXEL_POSITION& GetWarpPosition() const { return m_posWarp; }
	bool WarpToPID(DWORD dwPID, bool bWarpForce = false);

	void SaveExitLocation();
	void ExitToSavedLocation();

	void StartStaminaConsume();
	void StopStaminaConsume();
	bool IsStaminaConsume() const;
	bool IsStaminaHalfConsume() const;

	void ResetStopTime();
	DWORD GetStopTime() const;
#ifdef FIX_GMVSHACKER
    void UpdateAttackTimeTo(DWORD dwVID, DWORD dwTime) { m_map_dwAttackTime[dwVID] = dwTime; }
    DWORD GetLastAttackTimeTo(DWORD dwVID) { return m_map_dwAttackTime[dwVID];  }

    void IncreaseSyncAttackSusCount() { m_dwSyncAttackSusCount++; }
    void ResetSyncAttackSusCount() { m_dwSyncAttackSusCount = 0; }
    DWORD GetSyncAttackSusCount() const { return m_dwSyncAttackSusCount; }

    private:
        std::map <DWORD, DWORD> m_map_dwAttackTime;
        DWORD m_dwSyncAttackSusCount{ 0 };
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
public:
	int GetOfflineShopUseTime() const {return m_iOfflineShopUseTime;}
	void SetOfflineShopUseTime(){m_iOfflineShopUseTime = thecore_pulse();}

private:
	int m_iOfflineShopUseTime = 0;

public:
#endif
#if defined(__MOVE_CHANNEL__)
	bool MoveChannel(long lNewAddr, WORD wNewPort);
	bool StartMoveChannel(long lNewAddr, WORD wNewPort);
#endif

	BYTE GetLanguage() const;
	bool ChangeLanguage(BYTE bLanguage);

protected:
	void ClearSync();

	float m_fSyncTime;
	LPCHARACTER m_pkChrSyncOwner;
	CHARACTER_LIST m_kLst_pkChrSyncOwned; // ���� SyncOwner�� �ڵ�

	PIXEL_POSITION m_posDest;
	PIXEL_POSITION m_posStart;
	PIXEL_POSITION m_posWarp;
	long m_lWarpMapIndex;

	PIXEL_POSITION m_posExit;
	long m_lExitMapIndex;

	DWORD m_dwMoveStartTime;
	DWORD m_dwMoveDuration;

	DWORD m_dwLastMoveTime;
	DWORD m_dwLastAttackTime;
	DWORD m_dwWalkStartTime;
	DWORD m_dwStopTime;

	bool m_bWalking;
	bool m_bNowWalking;
	bool m_bStaminaConsume;
#ifdef ENABLE_12ZI
	int m_dwZodiacCzLastTime;
#endif
	// End

	// Quickslot ����
public:
	void SyncQuickslot(BYTE bType, WORD wOldPos, WORD wNewPos);
#if defined(__SWAP_ITEM_SYSTEM__)
	int GetQuickslotPosition(BYTE bType, WORD bInventoryPos);
#endif
	bool GetQuickslot(BYTE pos, TQuickslot** ppSlot);
	bool SetQuickslot(BYTE pos, TQuickslot& rSlot);
	bool DelQuickslot(BYTE pos);
	bool SwapQuickslot(BYTE a, BYTE b);
	void ChainQuickslotItem(LPITEM pItem, BYTE bType, WORD wOldPos);

protected:
	TQuickslot m_quickslot[QUICKSLOT_MAX_NUM];
#if defined(__SWAP_ITEM_SYSTEM__)
	std::vector<TQuickslot> m_tmpQuickslots;
#endif

	////////////////////////////////////////////////////////////////////////////////////////
	// Affect
public:
	void StartAffectEvent();
	void ClearAffect(bool bSave = false);
	void ComputeAffect(CAffect* pkAff, bool bAdd);
	bool AddAffect(DWORD dwType, uint16_t bApplyOn, long lApplyValue, DWORD dwFlag, long lDuration, long lSPCost, bool bOverride, bool IsCube = false); //@fixme532
	void RefreshAffect();
	bool RemoveAffect(DWORD dwType);
	bool IsAffectFlag(DWORD dwAff) const;

	bool UpdateAffect(); // called from EVENT
	int ProcessAffect();

	void LoadAffect(DWORD dwCount, TPacketAffectElement* pElements);
	void SaveAffect();

	struct load_affect_login_event_info
	{
		DWORD pid;
		DWORD count;
		char* data;

		load_affect_login_event_info()
			: pid(0)
			, count(0)
			, data(0)
		{
		}
	};

	load_affect_login_event_info* affectInfo;

public:
	// Affect loading�� ���� �����ΰ�?
	bool IsLoadedAffect() const { return m_bIsLoadedAffect; }

	bool IsGoodAffect(BYTE bAffectType) const;

	void RemoveGoodAffect();
	void RemoveBadAffect();

	CAffect* FindAffect(DWORD dwType, uint16_t bApply = APPLY_NONE) const; //@fixme532
	const AffectContainerList& GetAffectContainer() const { return m_list_pkAffect; }
	bool RemoveAffect(CAffect* pkAff, bool single = true);

	void SetAffectStack(CAffect* pkAff, BYTE value);
	BYTE GetAffectStack(CAffect* pkAff);
	void ClearAffectStack(CAffect* pkAff);

	AffectStackMap m_map_affectStack;

protected:
	bool m_bIsLoadedAffect;
	TAffectFlag m_afAffectFlag;
	AffectContainerList m_list_pkAffect;

public:
	// PARTY_JOIN_BUG_FIX
	void SetParty(LPPARTY pkParty);
	LPPARTY GetParty() const { return m_pkParty; }

	bool RequestToParty(LPCHARACTER leader);
	void DenyToParty(LPCHARACTER member);
	void AcceptToParty(LPCHARACTER member);

	/// �ڽ��� ��Ƽ�� �ٸ� character �� �ʴ��Ѵ�.
	/**
	* @param pchInvitee �ʴ��� ��� character. ��Ƽ�� ���� ������ �����̾�� �Ѵ�.
	*
	* ���� character �� ���°� ��Ƽ�� �ʴ��ϰ� �ʴ���� �� �ִ� ���°� �ƴ϶�� �ʴ��ϴ� ĳ���Ϳ��� �ش��ϴ� ä�� �޼����� �����Ѵ�.
	**/
	void PartyInvite(LPCHARACTER pchInvitee);

	/// �ʴ��ߴ� character �� ������ ó���Ѵ�.
	/**
	* @param pchInvitee ��Ƽ�� ������ character. ��Ƽ�� ���������� �����̾�� �Ѵ�.
	*
	* pchInvitee �� ��Ƽ�� ������ �� �ִ� ��Ȳ�� �ƴ϶�� �ش��ϴ� ä�� �޼����� �����Ѵ�.
	**/
	void PartyInviteAccept(LPCHARACTER pchInvitee);

	/// �ʴ��ߴ� character �� �ʴ� �źθ� ó���Ѵ�.
	/**
	* @param [in] dwPID �ʴ� �ߴ� character �� PID
	**/
	void PartyInviteDeny(DWORD dwPID);

	bool BuildUpdatePartyPacket(TPacketGCPartyUpdate& out);
	int GetLeadershipSkillLevel() const;
	int GetRoleProficiencySkillLevel() const;
	int GetInSightSkillLevel() const;

	bool CanSummon(int iLeaderShip);

	void SetPartyRequestEvent(LPEVENT pkEvent) { m_pkPartyRequestEvent = pkEvent; }

protected:

	/// ��Ƽ�� �����Ѵ�.
	/**
	* @param pkLeader ������ ��Ƽ�� ����
	**/
	void PartyJoin(LPCHARACTER pkLeader);

	/**
	* ��Ƽ ������ �� �� ���� ����� �����ڵ�.
	* Error code �� �ð��� �������ΰ��� ���� ���氡����(mutable) type �� ����(static) type ���� ������.
	* Error code �� ���� PERR_SEPARATOR ���� ������ ���氡���� type �̰� ������ ���� type �̴�.
	**/
	enum PartyJoinErrCode
	{
		PERR_NONE = 0, ///< ó������
		PERR_SERVER, ///< ���������� ��Ƽ���� ó�� �Ұ�
		PERR_DUNGEON, ///< ĳ���Ͱ� ������ ����
		PERR_OBSERVER, ///< ���������
		PERR_LVBOUNDARY, ///< ��� ĳ���Ϳ� �������̰� ��
		PERR_LOWLEVEL, ///< �����Ƽ�� �ְ��������� 30���� ����
		PERR_HILEVEL, ///< �����Ƽ�� ������������ 30���� ����
		PERR_ALREADYJOIN, ///< ��Ƽ���� ��� ĳ���Ͱ� �̹� ��Ƽ��
		PERR_PARTYISFULL, ///< ��Ƽ�ο� ���� �ʰ�
		PERR_SEPARATOR, ///< Error type separator.
		PERR_DIFFEMPIRE, ///< ��� ĳ���Ϳ� �ٸ� ������
		PERR_MAX ///< Error code �ְ�ġ. �� �տ� Error code �� �߰��Ѵ�.
	};

	/// ��Ƽ �����̳� �Ἲ ������ ������ �˻��Ѵ�.
	/**
	* @param pchLeader ��Ƽ�� leader �̰ų� �ʴ��� character
	* @param pchGuest �ʴ�޴� character
	* @return ��� PartyJoinErrCode �� ��ȯ�� �� �ִ�.
	**/
	static PartyJoinErrCode IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

	/// ��Ƽ �����̳� �Ἲ ������ ������ ������ �˻��Ѵ�.
	/**
	* @param pchLeader ��Ƽ�� leader �̰ų� �ʴ��� character
	* @param pchGuest �ʴ�޴� character
	* @return mutable type �� code �� ��ȯ�Ѵ�.
	**/
	static PartyJoinErrCode IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

	LPPARTY m_pkParty;
	DWORD m_dwLastDeadTime;
	LPEVENT m_pkPartyRequestEvent;

	/**
	* ��Ƽ��û Event map.
	* key: �ʴ���� ĳ������ PID
	* value: event�� pointer
	*
	* �ʴ��� ĳ���͵鿡 ���� event map.
	**/
	typedef std::map<DWORD, LPEVENT> EventMap;
	EventMap m_PartyInviteEventMap;

	// END_OF_PARTY_JOIN_BUG_FIX

	////////////////////////////////////////////////////////////////////////////////////////
	// Dungeon
public:
	void SetDungeon(LPDUNGEON pkDungeon);
	LPDUNGEON GetDungeon() const { return m_pkDungeon; }
	LPDUNGEON GetDungeonForce() const;
protected:
	LPDUNGEON m_pkDungeon;
	int m_iEventAttr;

	////////////////////////////////////////////////////////////////////////////////////////
	// Zodiac
#ifdef ENABLE_12ZI
public:
	void SetZodiac(LPZODIAC pkZodiac);
	LPZODIAC GetZodiac() const { return m_pkZodiac; }
	LPZODIAC GetZodiacForce() const;

protected:
	LPZODIAC m_pkZodiac;
#endif

	////////////////////////////////////////////////////////////////////////////////////////
	// Guild
public:
	void SetGuild(CGuild* pGuild);
	CGuild* GetGuild() const { return m_pGuild; }

	void SetWarMap(CWarMap* pWarMap);
	CWarMap* GetWarMap() const { return m_pWarMap; }

protected:
	CGuild* m_pGuild;
	DWORD m_dwUnderGuildWarInfoMessageTime;
	CWarMap* m_pWarMap;

#ifdef ENABLE_GUILD_DONATE_ATTENDANCE
public:
	int GetLastOfferNewExpTime() const noexcept { return m_dwLastOfferNewExpTime; }
	void SetLastOfferNewExpTime() noexcept { m_dwLastOfferNewExpTime = thecore_pulse(); }

private:
	int m_dwLastOfferNewExpTime;
#endif

	////////////////////////////////////////////////////////////////////////////////////////
	// Item related
public:
	bool CanHandleItem(bool bSkipRefineCheck = false, bool bSkipObserver = false); // ������ ���� ������ �� �� �ִ°�?

	bool IsItemLoaded() const { return m_bItemLoaded; }
	void SetItemLoaded() { m_bItemLoaded = true; }

	void ClearItem();

#if defined(__SORT_INVENTORY_ITEMS__)
	void SortInventoryItems();
	void SetSortInventoryPulse(int pulse) { m_sortInventoryPulse = pulse; }
	int GetSortInventoryPulse() { return m_sortInventoryPulse; }
#if defined(__SPECIAL_INVENTORY_SYSTEM__)
	void SortSpecialInventoryItems(BYTE type);
	void SetSortSpecialInventoryPulse(int pulse) { m_sortSpecialInventoryPulse = pulse; }
	int GetSortSpecialInventoryPulse() { return m_sortSpecialInventoryPulse; }
#endif
#endif

#if defined(__WJ_PICKUP_ITEM_EFFECT__)
	void SetItem(TItemPos Cell, LPITEM item, bool isHighLight = false);
#else
	void SetItem(TItemPos Cell, LPITEM item);
#endif

	LPITEM GetItem(TItemPos Cell) const;
#if defined(__SWAP_ITEM_SYSTEM__)
	LPITEM GetItem_NEW(TItemPos Cell) const;
#endif
	LPITEM GetInventoryItem(WORD wCell) const;
	LPITEM GetDragonSoulItem(const WORD& wCell) const;

#if defined(__SPECIAL_INVENTORY_SYSTEM__)
	LPITEM GetSkillBookInventoryItem(WORD wCell) const;
	LPITEM GetUpgradeItemsInventoryItem(WORD wCell) const;
	LPITEM GetStoneInventoryItem(WORD wCell) const;
	LPITEM GetGiftBoxInventoryItem(WORD wCell) const;
	LPITEM GetAttributeInventoryItem(WORD wCell) const;
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	LPITEM GetInventoryFeedItem(uint16_t wCell) const;
#endif

	bool IsEmptyItemGrid(TItemPos Cell, BYTE size, int iExceptionCell = -1) const;
	bool IsEmptyItemGridSpecial(TItemPos Cell, BYTE bSize, int iExceptionCell, std::vector<WORD>& vec) const;

	void SetWear(UINT bCell, LPITEM item);
	LPITEM GetWear(UINT bCell) const;

	// MYSHOP_PRICE_LIST
	void UseSilkBotary(void); /// ��� ������ �������� ���
#if defined(__MYSHOP_DECO__)
	void UseDecoBundle(void);
#endif
	/// DB ĳ�÷� ���� �޾ƿ� �������� ����Ʈ�� �������� �����ϰ� ������ ������ ����� ó���Ѵ�.
	/**
	* @param [in] p �������� ����Ʈ ��Ŷ
	*
	* ������ �� ó�� ��� ������ ������ ��� �� UseSilkBotary ���� DB ĳ�÷� �������� ����Ʈ�� ��û�ϰ�
	* ������� ������ �� �Լ����� ���� ��ܺ����� ����� ó���Ѵ�.
	**/
	void UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p);
	// END_OF_MYSHOP_PRICE_LIST

	bool UseItemEx(LPITEM item, TItemPos DestCell);
	bool UseItem(TItemPos Cell, TItemPos DestCell = NPOS);

	// ADD_REFINE_BUILDING
	bool IsRefineThroughGuild() const;
	CGuild* GetRefineGuild() const;
	int ComputeRefineFee(int iCost, int iMultiply = 5) const;
	void PayRefineFee(int iTotalMoney);
	void SetRefineNPC(LPCHARACTER ch);
	// END_OF_ADD_REFINE_BUILDING

	bool RefineItem(LPITEM pkItem, LPITEM pkTarget);
	bool DropItem(TItemPos Cell, WORD wCount = 0);
#if defined(__NEW_DROP_DIALOG__)
	bool DestroyItem(TItemPos Cell);
#endif
	bool GiveRecallItem(LPITEM item);
	void ProcessRecallItem(LPITEM item);

	// void PotionPacket(int iPotionType);
	void EffectPacket(int enumEffectType);
	void SpecificEffectPacket(const char filename[128]);

	// ADD_MONSTER_REFINE
	bool DoRefine(LPITEM item, bool bMoneyOnly = false);
	// END_OF_ADD_MONSTER_REFINE

	bool DoRefineWithScroll(LPITEM item);
	bool RefineInformation(BYTE bCell, BYTE bType, int iAdditionalCell = -1);

	void SetRefineMode(int iAdditionalCell = -1);
	void ClearRefineMode();
	bool IsRefineWindowOpen() noexcept { return m_bUnderRefine ? true : false; };

	bool GiveItem(LPCHARACTER victim, TItemPos Cell);
	bool CanReceiveItem(LPCHARACTER from, LPITEM item) const;
	void ReceiveItem(LPCHARACTER from, LPITEM item);
	//bool GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector<DWORD>& dwItemVnums,
	//	std::vector<DWORD>& dwItemCounts, std::vector<LPITEM>& item_gets, int& count);
	bool GiveItemFromSpecialItemGroup(DWORD dwGroupNum);

	bool MoveItem(TItemPos pos, TItemPos change_pos, WORD num);
	bool PickupItem(DWORD vid);
#ifdef PET_AUTO_PICKUP
	bool PickupItemByPet(uint32_t vid);
#endif
	bool EquipItem(LPITEM item, int iCandidateCell = -1);
	bool UnequipItem(LPITEM item);
	// ���� item�� ������ �� �ִ� �� Ȯ���ϰ�, �Ұ��� �ϴٸ� ĳ���Ϳ��� ������ �˷��ִ� �Լ�
	bool CanEquipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);

	// �������� item�� ���� �� �ִ� �� Ȯ���ϰ�, �Ұ��� �ϴٸ� ĳ���Ϳ��� ������ �˷��ִ� �Լ�
	bool CanUnequipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);
	// �������� item�� ���� �� �ִ� �� Ȯ���ϰ�, �Ұ��� �ϴٸ� ĳ���Ϳ��� ������ �˷��ִ� �Լ�
	// bool CanUnequipNow(const LPITEM item, const TItemPos& swapCell = NPOS);

	bool SwapItem(UINT bCell, UINT bDestCell);
#if defined(__WJ_PICKUP_ITEM_EFFECT__)
	LPITEM AutoGiveItem(DWORD dwItemVnum, WORD bCount = 1, int iRarePct = -1, bool bMsg = true, bool isHighLight = false);
	void AutoGiveItem(LPITEM item, bool longOwnerShip = false, bool isHighLight = false);
#else
	LPITEM AutoGiveItem(DWORD dwItemVnum, WORD wCount = 1, int iRarePct = -1, bool bMsg = true);
	void AutoGiveItem(LPITEM item, bool longOwnerShip = false);
#endif

	int GetEmptyInventoryCube(LPITEM item) const;

	int GetEmptyInventory(BYTE size) const;
	int GetEmptyInventoryCount(BYTE size) const;
	int GetEmptyDragonSoulInventory(LPITEM pItem) const;
	int GetEmptyDragonSoulInventoryWithExceptions(LPITEM pItem, std::vector<WORD>& vec /*= -1*/) const;
#if defined(__SPECIAL_INVENTORY_SYSTEM__)
	int GetEmptySkillBookInventory(BYTE size) const;
	int GetEmptyUpgradeItemsInventory(BYTE size) const;
	int GetEmptyStoneInventory(BYTE size) const;
	int GetEmptyGiftBoxInventory(BYTE size) const;
	int GetEmptyAttributeInventory(BYTE size) const;
#endif
	void CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const;

/*** Ikarus function created for offlineshop & special inventory ***/
	bool CanTakeInventoryItem(LPITEM item, TItemPos* cell);
/*** End ***/

	int CountEmptyInventory() const;

#if defined(ENABLE_CUBE_RENEWAL) && defined(ENABLE_SET_ITEM)
	int CountSpecifyItem(DWORD vnum, int iExceptionCell = -1, bool bIgnoreSetValue = false) const;
	void RemoveSpecifyItem(DWORD vnum, DWORD count = 1, int iExceptionCell = -1, bool bIgnoreSetValue = false);
#else
	int CountSpecifyItem(DWORD vnum, int iExceptionCell = -1) const;
	void RemoveSpecifyItem(DWORD vnum, DWORD count = 1, int iExceptionCell = -1);
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	int CountSpecifyPetFeedItem(uint32_t vnum, int iExceptionCell = -1) const;
	void RemoveSpecifyPetFeedItem(uint32_t vnum, uint32_t count = 1, int iExceptionCell = -1);
#endif

#if defined(ENABLE_CUBE_RENEWAL) && defined(ENABLE_SET_ITEM)
	LPITEM FindSpecifyItem(DWORD vnum, bool bIgnoreSetValue = false) const;
#else
	LPITEM FindSpecifyItem(DWORD vnum) const;
#endif

	LPITEM FindItemByID(DWORD id) const;

	int CountSpecifyTypeItem(BYTE type) const;
	void RemoveSpecifyTypeItem(BYTE type, DWORD count = 1);

	bool IsEquipUniqueItem(DWORD dwItemVnum) const;

	// CHECK_UNIQUE_GROUP
	bool IsEquipUniqueGroup(DWORD dwGroupVnum) const;
	// END_OF_CHECK_UNIQUE_GROUP

	void SendEquipment(LPCHARACTER ch);
	// End of Item
	
#ifdef __ENABLE_NEW_OFFLINESHOP__
	public:
		offlineshop::CShop* GetOfflineShop() {return m_pkOfflineShop;}
		void SetOfflineShop(offlineshop::CShop* pkShop) {m_pkOfflineShop = pkShop;}

		offlineshop::CShop* GetOfflineShopGuest() {return m_pkOfflineShopGuest;}
		void SetOfflineShopGuest(offlineshop::CShop* pkShop) {m_pkOfflineShopGuest = pkShop;}

		offlineshop::CShopSafebox* GetShopSafebox() {return m_pkShopSafebox;}
		void SetShopSafebox(offlineshop::CShopSafebox* pk);

		void SetAuction(offlineshop::CAuction* pk) {m_pkAuction = pk;}
		void SetAuctionGuest(offlineshop::CAuction* pk) {m_pkAuctionGuest = pk;}

		offlineshop::CAuction* GetAuction() {return m_pkAuction;}
		offlineshop::CAuction* GetAuctionGuest() {return m_pkAuctionGuest;}

		void SetLookingOfflineshopOfferList(bool is) { m_bIsLookingOfflineshopOfferList = is; }
		bool IsLookingOfflineshopOfferList() { return m_bIsLookingOfflineshopOfferList; }

	private:
		offlineshop::CShop* m_pkOfflineShop;
		offlineshop::CShop*	m_pkOfflineShopGuest;
		offlineshop::CShopSafebox* m_pkShopSafebox;
		offlineshop::CAuction* m_pkAuction;
		offlineshop::CAuction* m_pkAuctionGuest;

		bool m_bIsLookingOfflineshopOfferList;
#endif
protected:
	/**
	* @param [in] dwItemVnum ������ vnum
	* @param [in] dwItemPrice ������ ����
	**/
#if defined(__CHEQUE_SYSTEM__)
	void SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice, DWORD dwItemPriceCheque);
#else
	void SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice);
#endif

	bool m_bNoOpenedShop; ///< �̹� ���� �� ���λ����� �� ���� �ִ����� ����(������ ���� ���ٸ� true)

	bool m_bItemLoaded;
	int m_iRefineAdditionalCell;

public:
	bool IsUnderRefine() const { return m_bUnderRefine; }
	void SetUnderRefine(bool bState) { m_bUnderRefine = bState; }
#if defined(ENABLE_12ZI) && defined(ENABLE_12ZI_SHOP_LIMIT)
public:
	typedef struct SShopPriceLimitCount
	{
		typedef struct SPurchaseData
		{
			uint32_t dwLimitedCount;
			uint32_t dwLimitedPurchaseCount;
			SPurchaseData() noexcept :
				dwLimitedCount(0),
				dwLimitedPurchaseCount(0)
			{}
		} TPurchaseData;

		std::map<uint32_t, TPurchaseData> data;
	} TShopPriceLimitCount;

	void SetPurchaseItemLimit(LPCHARACTER ch, uint32_t dwVnum, uint32_t dwLimitedCount, uint32_t dwLimitedPurchaseCount);
	void ShopPurchaseLimitItem(LPCHARACTER ch, uint32_t dwVnum);
	bool CanShopPurchaseLimitedItem(LPCHARACTER ch, uint32_t dwVnum);
	uint32_t GetShopLimitedCount(uint32_t dwPlayerID, uint32_t dwVnum);
	uint32_t GetShopLimitedPurchaseCount(uint32_t dwPlayerID, uint32_t dwVnum);

	void BroadcastUpdateLimitedPurchase(uint32_t dwVnum, uint32_t dwLimitedCount, uint32_t dwLimitedPurchaseCount);
protected:
	std::map<uint32_t, TShopPriceLimitCount> m_ShopLimitedPurchaseInfo;
#endif
protected:
	bool m_bUnderRefine;
	DWORD m_dwRefineNPCVID;

	int m_sortInventoryPulse;
#if defined(__SPECIAL_INVENTORY_SYSTEM__)
	int m_sortSpecialInventoryPulse;
#endif

public:
	////////////////////////////////////////////////////////////////////////////////////////
	// Money related
	long long GetGold() const { return m_points.gold; }
	void SetGold(long long gold) { m_points.gold = gold; }
	bool DropGold(INT gold);
	long long GetAllowedGold() const;
	void GiveGold(long long llAmount); // ��Ƽ�� ������ ��Ƽ �й�, �α� ���� ó��
	// End of Money

	////////////////////////////////////////////////////////////////////////////////////////
	// Shop related
#if defined(__CHEQUE_SYSTEM__)
////////////////////////////////////////////////////////////////////////////////////////
// Cheque related
	INT GetCheque() const { return m_points.cheque; }
	void SetCheque(INT cheque) { m_points.cheque = cheque; }
	bool DropCheque(INT cheque);
	void GiveCheque(INT iAmount);
	// End of Cheque
#endif

#if defined(__GEM_SYSTEM__)
	////////////////////////////////////////////////////////////////////////////////////////
	// Gem related
	INT GetGem() const { return m_points.gem; }
	void SetGem(INT gem) { m_points.gem = gem; }
	void GiveGem(INT iAmount);
	// End of Gem
#endif

	////////////////////////////////////////////////////////////////////////////////////////
	// Shop related
public:
	void SetShop(LPSHOP pkShop);
	LPSHOP GetShop() const { return m_pkShop; }
	void ShopPacket(BYTE bSubHeader);

	void SetShopOwner(LPCHARACTER ch) { m_pkChrShopOwner = ch; }
	LPCHARACTER GetShopOwner() const { return m_pkChrShopOwner; }

	void OpenMyShop(const char* c_pszSign, TShopItemTable* pTable, WORD wItemCount);
	LPSHOP GetMyShop() const { return m_pkMyShop; }
	void CloseMyShop();

protected:
	LPSHOP m_pkShop;
	LPSHOP m_pkMyShop;
	std::string m_stShopSign;
	LPCHARACTER m_pkChrShopOwner;
	// End of shop

#if defined(__SKILL_COLOR_SYSTEM__)
public:
	void SetSkillColor(DWORD* dwSkillColor);
	DWORD* GetSkillColor() { return m_dwSkillColor[0]; }

protected:
	DWORD m_dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif

#ifdef ENABLE_GUILDWAR_BUTTON
public:
	void SendWarTeleportButton(bool show = false);
#endif

// #ifdef ENABLE_SET_ITEM
// public:
// 	void RefreshSetBonus();
// 	void ClearAffectSetBonus();
// #endif

#ifdef ENABLE_ATTR_6TH_7TH_EXTEND
public:
	uint8_t GetAlignRank();
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
protected:
	std::unordered_map<uint32_t, TGrowthPetInfo> m_GrowthPetInfo;
public:
	std::vector<TGrowthPetInfo> GetPetList() const;
	void SetGrowthPetInfo(TGrowthPetInfo petInfo);
	void SendGrowthPetInfoPacket();
#endif

	////////////////////////////////////////////////////////////////////////////////////////
	// Exchange related
public:
	bool ExchangeStart(LPCHARACTER victim);
	void SetExchange(CExchange* pkExchange);
	CExchange* GetExchange() const { return m_pkExchange; }

protected:
	CExchange* m_pkExchange;
	// End of Exchange

public:
	void SetDamage(int iDmg) { iDamage = iDmg; }
	int GetDamage() { return iDamage; }

private:
	int iDamage;

	////////////////////////////////////////////////////////////////////////////////////////
	// Battle
public:
	struct TBattleInfo
	{
		int iTotalDamage;
		int iAggro;

		TBattleInfo(int iTot, int iAggr)
			: iTotalDamage(iTot), iAggro(iAggr)
		{}
	};
	typedef std::map<VID, TBattleInfo> TDamageMap;

	typedef struct SAttackLog
	{
		DWORD dwVID;
		DWORD dwTime;
	} AttackLog;

	bool Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
	bool __Profile__Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
	void DeathPenalty(BYTE bExpLossPercent);
	void ReviveInvisible(int iDur);

	bool Attack(LPCHARACTER pkVictim, BYTE bType = 0);
	bool IsAlive() const { return m_pointsInstant.position == POS_DEAD ? false : true; }
	bool CanFight() const;

	bool CanBeginFight() const;
	void BeginFight(LPCHARACTER pkVictim); // pkVictimr�� �ο�� �����Ѵ�. (��������, ������ �� �ֳ� üũ�Ϸ��� CanBeginFight�� ���)

	bool CounterAttack(LPCHARACTER pkChr); // �ݰ��ϱ� (���͸� ���)

	bool IsStun() const;
	void Stun(bool bImmediate = false);
	bool IsDead() const;
	void Dead(LPCHARACTER pkKiller = NULL, bool bImmediateDead = false);

	void Reward(bool bItemDrop);
	void RewardGold(LPCHARACTER pkAttacker);

	bool Shoot(BYTE bType);
	void FlyTarget(DWORD dwTargetVID, long x, long y, BYTE bHeader);

	void ForgetMyAttacker();
	void AggregateMonster();
	void AttractRanger();
	void PullMonster();

	int GetArrowAndBow(LPITEM* ppkBow, LPITEM* ppkArrow, int iArrowCount = 1);
	void UseArrow(LPITEM pkArrow, DWORD dwArrowCount);

	void AttackedByPoison(LPCHARACTER pkAttacker);
	void RemovePoison();

	void AttackedByBleeding(LPCHARACTER pkAttacker);
	void RemoveBleeding();

	void AttackedByFire(LPCHARACTER pkAttacker, int amount, int count);
	void RemoveFire();

	void UpdateAlignment(int iAmount);
	int GetAlignment() const;

	// ����ġ ��� 
	int GetRealAlignment() const;
	void ShowAlignment(bool bShow);

	void SetKillerMode(bool bOn);
	bool IsKillerMode() const;
	void UpdateKillerMode();

	BYTE GetPKMode() const;
	void SetPKMode(BYTE bPKMode);

	void ItemDropPenalty(LPCHARACTER pkKiller);

	void UpdateAggrPoint(LPCHARACTER ch, EDamageType type, int dam);

#ifdef ENABLE_GROWTH_PET_SYSTEM
	bool CanAttack() { return FindAffect(AFFECT_IMPOSSIBLE_ATTACK) ? false : true; };
#endif

	////////////////////////////////////////////////////////////////////////////////////////
	// HACK
public:
	void SetComboSequence(BYTE seq);
	BYTE GetComboSequence() const;

	void SetLastComboTime(DWORD time);
	DWORD GetLastComboTime() const;

	int GetValidComboInterval() const;
	void SetValidComboInterval(int interval);

	BYTE GetComboIndex() const;

	void IncreaseComboHackCount(int k = 1);
	void ResetComboHackCount();
	void SkipComboAttackByTime(int interval);
	DWORD GetSkipComboAttackByTime() const;

protected:
	BYTE m_bComboSequence;
	DWORD m_dwLastComboTime;
	int m_iValidComboInterval;
	BYTE m_bComboIndex;
	int m_iComboHackCount;
	DWORD m_dwSkipComboAttackByTime;

protected:
	void UpdateAggrPointEx(LPCHARACTER ch, EDamageType type, int dam, TBattleInfo& info);
	void ChangeVictimByAggro(int iNewAggro, LPCHARACTER pNewVictim);

	DWORD m_dwFlyTargetID;
	std::vector<DWORD> m_vec_dwFlyTargets;
	TDamageMap m_map_kDamage; // � ĳ���Ͱ� ������ �󸶸�ŭ�� �������� �־��°�?
	//AttackLog m_kAttackLog;
	DWORD m_dwKillerPID;

	int m_iAlignment; // Lawful / Chaotic value -200000 ~ 200000
	int m_iRealAlignment;
	int m_iKillerModePulse;
	BYTE m_bPKMode;

	// Aggro
	DWORD m_dwLastVictimSetTime;
	int m_iMaxAggro;
	// End of Battle

	// Stone
public:
	void SetStone(LPCHARACTER pkChrStone);
	void ClearStone();
	void DetermineDropMetinStone();
	DWORD GetDropMetinStoneVnum() const { return m_dwDropMetinStone; }
	BYTE GetDropMetinStonePct() const { return m_bDropMetinStonePct; }

protected:
	LPCHARACTER m_pkChrStone; // ���� ������ ��
	CHARACTER_SET m_set_pkChrSpawnedBy; // ���� ������ ���
	DWORD m_dwDropMetinStone;
	BYTE m_bDropMetinStonePct;
	// End of Stone

public:
	enum
	{
		SKILL_UP_BY_POINT,
		SKILL_UP_BY_BOOK,
		SKILL_UP_BY_TRAIN,

		// ADD_GRANDMASTER_SKILL
		SKILL_UP_BY_QUEST,
		// END_OF_ADD_GRANDMASTER_SKILL
	};

	void SkillLevelPacket();
#if defined(__7AND8TH_SKILLS__)
	bool SkillCanUp(DWORD dwVnum);
#endif
	void SkillLevelUp(DWORD dwVnum, BYTE bMethod = SKILL_UP_BY_POINT);
	bool SkillLevelDown(DWORD dwVnum);
	// ADD_GRANDMASTER_SKILL
	bool UseSkill(DWORD dwVnum, LPCHARACTER pkVictim, bool bUseGrandMaster = true);
	void ResetSkill();
#if defined(__SKILL_COOLTIME_UPDATE__)
	void ResetSkillCoolTimes();
#endif
	void SetSkillLevel(DWORD dwVnum, BYTE bLev);
	int GetUsedSkillMasterType(DWORD dwVnum);

	bool IsLearnableSkill(DWORD dwSkillVnum) const;
	// END_OF_ADD_GRANDMASTER_SKILL

	bool CheckSkillHitCount(const BYTE SkillID, const VID dwTargetVID);
	bool CanUseSkill(DWORD dwSkillVnum) const;
	bool IsUsableSkillMotion(DWORD dwMotionIndex) const;
	int GetSkillLevel(DWORD dwVnum) const;
	int GetSkillMasterType(DWORD dwVnum) const;
	int GetSkillPower(DWORD dwVnum, BYTE bLevel = 0) const;

	time_t GetSkillNextReadTime(DWORD dwVnum) const;
	void SetSkillNextReadTime(DWORD dwVnum, time_t time);
	void SkillLearnWaitMoreTimeMessage(DWORD dwVnum);

	void ComputePassiveSkill(DWORD dwVnum);
#ifdef ENABLE_QUEEN_NETHIS
	int ComputeSnakeSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel);
#endif
	int ComputeSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
	int ComputeSkillParty(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
	int ComputeSkillAtPosition(DWORD dwVnum, const PIXEL_POSITION& posTarget, BYTE bSkillLevel = 0);
	void ComputeSkillPoints();

	void SetSkillGroup(BYTE bSkillGroup);
	BYTE GetSkillGroup() const { return m_points.skill_group; }

	int ComputeCooltime(int time);

	void GiveRandomSkillBook();
	void GiveSkillBook(DWORD dwSkillVnum, WORD wCount);

	void DisableCooltime();
	bool LearnSkillByBook(DWORD dwSkillVnum, BYTE bProb = 0);
	bool LearnGrandMasterSkill(DWORD dwSkillVnum);

private:
	bool m_bDisableCooltime;
	DWORD m_dwLastSkillTime; ///< ���������� skill �� �� �ð�(millisecond).
	// End of Skill

	// MOB_SKILL
public:
	bool HasMobSkill() const;
	size_t CountMobSkill() const;
	const TMobSkillInfo* GetMobSkill(unsigned int idx) const;
	bool CanUseMobSkill(unsigned int idx) const;
	bool UseMobSkill(unsigned int idx);
	void ResetMobSkillCooltime();
protected:
	DWORD m_adwMobSkillCooltime[MOB_SKILL_MAX_NUM];
	// END_OF_MOB_SKILL

	// for SKILL_MUYEONG
public:
	void StartMuyeongEvent();
	void StopMuyeongEvent();

#if defined(__9TH_SKILL__)
	void StartCheonunEvent(long lApplyValue);
	void StopCheonunEvent();
#endif

#ifdef ENABLE_ATTR_6TH_7TH_EXTEND
	// Timer running when an item have hit buff element bonus
	void StartHitBuffElementEvent();
	void StopHitBuffElementEvent();
#endif

#ifdef ENABLE_ELEMENTAL_WORLD
	void StartElementalWorldEvent();
	void StopElementalWorldEvent();
#endif

private:
	LPEVENT m_pkMuyeongEvent;
#ifdef ENABLE_QUEEN_NETHIS
	LPEVENT m_pkSnakeSkillEvent;
#endif
#if defined(__9TH_SKILL__)
	LPEVENT m_pkCheonunEvent;
#endif

#ifdef ENABLE_ATTR_6TH_7TH_EXTEND
	LPEVENT m_pkHitBuffElementEvent;
#endif

#ifdef ENABLE_ELEMENTAL_WORLD
public:
	LPEVENT m_pkElementalWorldEvent;
	uint32_t hp_reduce_count;
#endif

	// for SKILL_CHAIN lighting
public:
	int GetChainLightningIndex() const { return m_iChainLightingIndex; }
	void IncChainLightningIndex() { ++m_iChainLightingIndex; }
	void AddChainLightningExcept(LPCHARACTER ch) { m_setExceptChainLighting.insert(ch); }
	void ResetChainLightningIndex() { m_iChainLightingIndex = 0; m_setExceptChainLighting.clear(); }
	int GetChainLightningMaxCount() const;
	const CHARACTER_SET& GetChainLightingExcept() const { return m_setExceptChainLighting; }

private:
	int m_iChainLightingIndex;
	CHARACTER_SET m_setExceptChainLighting;

	// for SKILL_EUNHYUNG
public:
	void SetAffectedEunhyung();
	void ClearAffectedEunhyung() { m_dwAffectedEunhyungLevel = 0; }
	bool GetAffectedEunhyung() const { return m_dwAffectedEunhyungLevel; }

private:
	DWORD m_dwAffectedEunhyungLevel;

	//
	// Skill levels
	//
protected:
	TPlayerSkill* m_pSkillLevels;
	boost::unordered_map<BYTE, int> m_SkillDamageBonus;
	std::map<int, TSkillUseInfo> m_SkillUseInfo;

	////////////////////////////////////////////////////////////////////////////////////////
	// AI related
public:
	void AssignTriggers(const TMobTable* table);
	LPCHARACTER GetVictim() const; // ������ ��� ����
	void SetVictim(LPCHARACTER pkVictim);
	LPCHARACTER GetNearestVictim(LPCHARACTER pkChr);
	LPCHARACTER GetProtege() const; // ��ȣ�ؾ� �� ��� ����

	bool Follow(LPCHARACTER pkChr, float fMinimumDistance = 150.0f);
	bool Return();
	bool IsGuardNPC() const;
	bool IsChangeAttackPosition(LPCHARACTER target) const;
	void ResetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time() - AI_CHANGE_ATTACK_POISITION_TIME_NEAR; }
	void SetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time(); }

	bool OnIdle();

	void OnAttack(LPCHARACTER pkChrAttacker);
	void OnClick(LPCHARACTER pkChrCauser);

	VID m_kVIDVictim;

protected:
	DWORD m_dwLastChangeAttackPositionTime;
	CTrigger m_triggerOnClick;
	// End of AI

	////////////////////////////////////////////////////////////////////////////////////////
	// Target
protected:
	LPCHARACTER m_pkChrTarget; // �� Ÿ��
	CHARACTER_SET m_set_pkChrTargetedBy; // ���� Ÿ������ ������ �ִ� �����

public:
	void SetTarget(LPCHARACTER pkChrTarget);
	void BroadcastTargetPacket();
	void ClearTarget();
	void CheckTarget();
	LPCHARACTER GetTarget() const { return m_pkChrTarget; }

	////////////////////////////////////////////////////////////////////////////////////////
	// Safebox
public:
	int GetSafeboxSize() const;
	void QuerySafeboxSize();
	void SetSafeboxSize(int size);

	CSafebox* GetSafebox() const;
	void LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem* pItems);
	void ChangeSafeboxSize(BYTE bSize);
	void CloseSafebox();

	/// â�� ���� ��û
	/**
	* @param [in] pszPassword 1�� �̻� 6�� ������ â�� ��й�ȣ
	*
	* DB �� â�����⸦ ��û�Ѵ�.
	* â���� �ߺ����� ���� ���ϸ�, �ֱ� â���� ���� �ð����� ���� 10�� �̳����� �� �� ���Ѵ�.
	**/
	void ReqSafeboxLoad(const char* pszPassword);

	/// â�� ���� ��û�� ���
	/**
	* ReqSafeboxLoad �� ȣ���ϰ� CloseSafebox ���� �ʾ��� �� �� �Լ��� ȣ���ϸ� â���� �� �� �ִ�.
	* â�������� ��û�� DB �������� ���������� �޾��� ��� �� �Լ��� ����ؼ� ��û�� �� �� �ְ� ���ش�.
	**/
	void CancelSafeboxLoad(void) { m_bOpeningSafebox = false; }

	void SetMallLoadTime(int t) { m_iMallLoadTime = t; }
	int GetMallLoadTime() const { return m_iMallLoadTime; }

	CSafebox* GetMall() const;
	void LoadMall(int iItemCount, TPlayerItem* pItems);
	void CloseMall();

	void SetSafeboxOpenPosition();
	float GetDistanceFromSafeboxOpen() const;

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
	int GetGuildstorageSize() const;
	void QueryGuildstorageSize();
	void SetGuildstorageSize(int iSize);
	void ChangeGuildstorageSize(uint8_t bSize);

	CSafebox* GetGuildstorage() const;
	void LoadGuildstorage(int iSize, int iItemCount, TPlayerItem* pItems);
	void CloseGuildstorage();
	void ReqGuildstorageLoad();

	void SetGuildstorageOpenPosition();
	float GetDistanceFromGuildstorageOpen() const;
#endif

	void LoadSafeboxBuff();
	void SetSafeboxBuff();

protected:
	CSafebox* m_pkSafebox;
	int m_iSafeboxSize;
	int m_iSafeboxLoadTime;
	bool m_bOpeningSafebox; ///< â���� ���� ��û ���̰ų� �����ִ°� ����, true �� ��� �����û�̰ų� ��������.

	CSafebox* m_pkMall;
	int m_iMallLoadTime;

	PIXEL_POSITION m_posSafeboxOpen;

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
	CSafebox* m_pkGuildstorage;
	int m_iGuildstorageSize;
	int m_iGuildstorageLoadTime;
	bool m_bOpeningGuildstorage;

	PIXEL_POSITION m_posGuildstorageOpen;
#endif

	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////
	// Mounting
public:
	void UnMount(bool bUnequipItem = false);
	void MountVnum(DWORD vnum);
	DWORD GetMountVnum() const { return m_dwMountVnum; }
	DWORD GetLastMountTime() const { return m_dwMountTime; }

	bool CanUseHorseSkill();

	// Horse
	virtual void SetHorseLevel(int iLevel);

	virtual bool StartRiding();
	virtual bool StopRiding();

	virtual DWORD GetMyHorseVnum() const;

	virtual void HorseDie();
	virtual bool ReviveHorse();

	virtual void SendHorseInfo();
	virtual void ClearHorseInfo();

	void HorseSummon(bool bSummon, bool bFromFar = false, DWORD dwVnum = 0, const char* name = 0);

	LPCHARACTER GetHorse() const { return m_chHorse; } // ���� ��ȯ���� ��
	LPCHARACTER GetRider() const; // rider on horse
	void SetRider(LPCHARACTER ch);

	bool IsRiding() const;

#if defined(__PET_SYSTEM__)
public:
	CPetSystem* GetPetSystem() { return m_petSystem; }

protected:
	CPetSystem* m_petSystem;

public:
#endif

#ifdef ENABLE_PET_SYSTEM
public:
	void PetSummon(LPITEM petItem);
	void PetUnsummon(LPITEM petItem);
	void CheckPet();
	bool IsPetSummon();
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
public:
	CGrowthPetSystem* GetGrowthPetSystem() noexcept { return m_GrowthPetSystem; }
protected:
	CGrowthPetSystem* m_GrowthPetSystem;
#endif

protected:
	LPCHARACTER m_chHorse;
	LPCHARACTER m_chRider;

	DWORD m_dwMountVnum;
	DWORD m_dwMountTime;

	BYTE m_bSendHorseLevel;
	BYTE m_bSendHorseHealthGrade;
	BYTE m_bSendHorseStaminaGrade;

	////////////////////////////////////////////////////////////////////////////////////////
	// Detailed Log
public:
	void DetailLog() { m_bDetailLog = !m_bDetailLog; }
	void ToggleMonsterLog();
	void MonsterLog(const char* format, ...);

private:
	bool m_bDetailLog;
	bool m_bMonsterLog;

	////////////////////////////////////////////////////////////////////////////////////////
	// Empire
public:
	void SetEmpire(BYTE bEmpire);
	BYTE GetEmpire() const { return m_bEmpire; }

protected:
	BYTE m_bEmpire;

	////////////////////////////////////////////////////////////////////////////////////////
	// Regen
public:
	void SetRegen(LPREGEN pkRegen);

protected:
	PIXEL_POSITION m_posRegen;
	float m_fRegenAngle;
	LPREGEN m_pkRegen;
	size_t regen_id_; // to help dungeon regen identification
	// End of Regen

	////////////////////////////////////////////////////////////////////////////////////////
	// Resists & Proofs
public:
	bool CannotMoveByAffect() const; // Ư�� ȿ���� ���� ������ �� ���� �����ΰ�?
	bool IsImmune(DWORD dwImmuneFlag);
	void SetImmuneFlag(DWORD dw) { m_pointsInstant.dwImmuneFlag = dw; }

protected:
	void ApplyMobAttribute(const TMobTable* table);
	// End of Resists & Proofs

	////////////////////////////////////////////////////////////////////////////////////////
	// QUEST
public:
	void SetQuestNPCID(DWORD vid);
	DWORD GetQuestNPCID() const { return m_dwQuestNPCVID; }
	LPCHARACTER GetQuestNPC() const;

	void SetQuestItemPtr(LPITEM item);
	void ClearQuestItemPtr();
	LPITEM GetQuestItemPtr() const;

	void SetQuestBy(DWORD dwQuestVnum) { m_dwQuestByVnum = dwQuestVnum; }
	DWORD GetQuestBy() const { return m_dwQuestByVnum; }

	int GetQuestFlag(const std::string& flag) const;
	void SetQuestFlag(const std::string& flag, int value);

	void ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID);

private:
	DWORD m_dwQuestNPCVID;
	DWORD m_dwQuestByVnum;
	LPITEM m_pQuestItem;
	DWORD m_dwQuestItemVID{};

	// Events
public:
	bool StartStateMachine(int iPulse = 1);
	void StopStateMachine();
	void UpdateStateMachine(DWORD dwPulse);
	void SetNextStatePulse(int iPulseNext);

	// ĳ���� �ν��Ͻ� ������Ʈ �Լ�. ������ �̻��� ��ӱ����� CFSM::Update �Լ��� ȣ���ϰų� UpdateStateMachine �Լ��� ����ߴµ�, ������ ������Ʈ �Լ� �߰���.
	void UpdateCharacter(DWORD dwPulse);

protected:
	DWORD m_dwNextStatePulse;

	// Marriage
public:
	LPCHARACTER GetMarryPartner() const;
	void SetMarryPartner(LPCHARACTER ch);
	int GetMarriageBonus(DWORD dwItemVnum, bool bSum = true);

	bool IsWearingDress() const;

	void SetWeddingMap(marriage::WeddingMap* pMap);
	marriage::WeddingMap* GetWeddingMap() const { return m_pWeddingMap; }

private:
	marriage::WeddingMap* m_pWeddingMap;
	LPCHARACTER m_pkChrMarried;

	// Warp Character
public:
	void StartWarpNPCEvent();

public:
	void StartSaveEvent();
	void StartRecoveryEvent();
	void StartCheckSpeedHackEvent();
	void StartDestroyWhenIdleEvent();

	LPEVENT m_pkDeadEvent;
	LPEVENT m_pkStunEvent;
	LPEVENT m_pkSaveEvent;
	LPEVENT m_pkRecoveryEvent;
	LPEVENT m_pkTimedEvent;
	LPEVENT m_pkFishingEvent;
	LPEVENT m_pkAffectEvent;
	LPEVENT m_pkPoisonEvent;
	LPEVENT m_pkBleedingEvent;
	LPEVENT m_pkFireEvent;
	LPEVENT m_pkWarpNPCEvent;
	// DELAYED_WARP
	// END_DELAYED_WARP

	// MINING
	LPEVENT m_pkMiningEvent;
	// END_OF_MINING
	LPEVENT m_pkWarpEvent;
	LPEVENT m_pkCheckSpeedHackEvent;
	LPEVENT m_pkDestroyWhenIdleEvent;
	LPEVENT m_pkPetSystemUpdateEvent;

	//bool IsWarping() const { return m_bWarping; }
	bool IsWarping() const;
	
	bool m_bHasPoisoned;
	bool m_bHasBled;

	const CMob* m_pkMobData;
	CMobInstance* m_pkMobInst;

	MobSkillEventMap m_mapMobSkillEvent;

	friend struct FuncSplashDamage;
	friend struct FuncSplashAffect;
	friend class CFuncShoot;

public:
	int GetPremiumRemainSeconds(BYTE bType) const;

private:
	bool m_bWarping{};
	int m_aiPremiumTimes[PREMIUM_MAX_NUM];

	// CHANGE_ITEM_ATTRIBUTES
	static const DWORD msc_dwDefaultChangeItemAttrCycle; ///< ����Ʈ ������ �Ӽ����� ���� �ֱ�
	static const char msc_szLastChangeItemAttrFlag[]; ///< �ֱ� ������ �Ӽ��� ������ �ð��� Quest Flag �̸�
	static const char msc_szChangeItemAttrCycleFlag[]; ///< ������ �Ӽ����� ���� �ֱ��� Quest Flag �̸�
	// END_OF_CHANGE_ITEM_ATTRIBUTES

	// PC_BANG_ITEM_ADD
private:
	bool m_isinPCBang;

public:
	bool SetPCBang(bool flag) { m_isinPCBang = flag; return m_isinPCBang; }
	bool IsPCBang() const { return m_isinPCBang; }
	// END_PC_BANG_ITEM_ADD

	// NEW_HAIR_STYLE_ADD
public:
	bool ItemProcess_Hair(LPITEM item, int iDestCell);
	// END_NEW_HAIR_STYLE_ADD

public:
	void ClearSkill();
	void ClearSubSkill();

	// RESET_ONE_SKILL
	bool ResetOneSkill(DWORD dwVnum);
	// END_RESET_ONE_SKILL

private:
	void SendDamagePacket(LPCHARACTER pAttacker, int Damage, BYTE DamageFlag);

	// ARENA
private:
	CArena* m_pArena;
	bool m_ArenaObserver;
	int m_nPotionLimit;

public:
	void SetArena(CArena* pArena) { m_pArena = pArena; }
	void SetArenaObserverMode(bool flag) { m_ArenaObserver = flag; }

	CArena* GetArena() const { return m_pArena; }
	bool GetArenaObserverMode() const { return m_ArenaObserver; }

	void SetPotionLimit(int count) { m_nPotionLimit = count; }
	int GetPotionLimit() const { return m_nPotionLimit; }
	// END_ARENA

	// PREVENT_TRADE_WINDOW
public:
	bool IsOpenSafebox() const { return m_isOpenSafebox ? true : false; }
	void SetOpenSafebox(bool b) { m_isOpenSafebox = b; }

	int GetSafeboxLoadTime() const { return m_iSafeboxLoadTime; }
	void SetSafeboxLoadTime() { m_iSafeboxLoadTime = thecore_pulse(); }

#ifdef ENABLE_GUILDSTORAGE_SYSTEM
	bool IsOpenGuildstorage() const { return m_isOpenGuildstorage ? true : false; }
	void SetOpenGuildstorage(bool b) { m_isOpenGuildstorage = b; }

	int GetGuildstorageLoadTime() const { return m_iGuildstorageLoadTime; }
	void SetGuildstorageLoadTime() { m_iGuildstorageLoadTime = thecore_pulse(); }
#endif

private:
	bool m_isOpenSafebox;
#ifdef ENABLE_GUILDSTORAGE_SYSTEM
	bool m_isOpenGuildstorage;
#endif
	// END_PREVENT_TRADE_WINDOW

public:
	int GetSkillPowerByLevel(int level, bool bMob = false) const;

	// PREVENT_REFINE_HACK
	int GetRefineTime() const { return m_iRefineTime; }
	void SetRefineTime() { m_iRefineTime = thecore_pulse(); }
	int m_iRefineTime;
	// END_PREVENT_REFINE_HACK

	// RESTRICT_USE_SEED_OR_MOONBOTTLE
	int GetUseSeedOrMoonBottleTime() const { return m_iSeedTime; }
	void SetUseSeedOrMoonBottleTime() { m_iSeedTime = thecore_pulse(); }
	int m_iSeedTime;
	// END_RESTRICT_USE_SEED_OR_MOONBOTTLE

	// PREVENT_PORTAL_AFTER_EXCHANGE
	int GetExchangeTime() const { return m_iExchangeTime; }
	void SetExchangeTime() { m_iExchangeTime = thecore_pulse(); }
	int m_iExchangeTime;
	// END_PREVENT_PORTAL_AFTER_EXCHANGE
	
#ifdef ENABLE_ANTI_SPAM
	int analyze_protect;
	int analyze_protect_count;

	int ap_pickup;
	int apc_pickup;

	int ap_open_items;
	int apc_open_items;

	int ap_attack;
	int apc_attack;
	int ap_move;
	int apc_move;
	int ap_click_target;
	int apc_click_target;
	int ap_drop_target;
	int apc_drop_target;
#endif

	int m_iMyShopTime;
	int GetMyShopTime() const { return m_iMyShopTime; }
	void SetMyShopTime() { m_iMyShopTime = thecore_pulse(); }

#if defined(__MYSHOP_DECO__)
public:
	BYTE GetMyShopDecoType() const { return m_bMyShopDecoType; }
	void SetMyShopDecoType(BYTE bType) { m_bMyShopDecoType = bType; }

	DWORD GetMyShopDecoPolyVnum() const { return m_dwMyShopDecoPolyVnum; }
	void SetMyShopDecoPolyVnum(DWORD dwPolyVnum) { m_dwMyShopDecoPolyVnum = dwPolyVnum; }

	BYTE GetMyShopTabs() const { return m_bMyShopTabs; }
	void SetMyShopTabs(BYTE bTabs) { m_bMyShopTabs = bTabs; }

private:
	BYTE m_bMyShopDecoType;
	DWORD m_dwMyShopDecoPolyVnum;
	BYTE m_bMyShopTabs;

public:
#endif

	// PREVENT_TRADE_WINDOW
	bool PreventTradeWindow(int flags, bool except = false) const;
	// END_PREVENT_TRADE_WINDOW

	// Hack ������ ���� üũ.
	bool IsHack(bool bSendMsg = true, bool bCheckShopOwner = true, int limittime = g_nPortalLimitTime);

	// MONARCH
	BOOL IsMonarch() const;
	// END_MONARCH
	void Say(const std::string& s);

	enum MONARCH_COOLTIME
	{
		MC_HEAL = 10,
		MC_WARP = 60,
		MC_TRANSFER = 60,
		MC_TAX = (60 * 60 * 24 * 7),
		MC_SUMMON = (60 * 60),
	};

	enum MONARCH_INDEX
	{
		MI_HEAL = 0,
		MI_WARP,
		MI_TRANSFER,
		MI_TAX,
		MI_SUMMON,
		MI_MAX
	};

	DWORD m_dwMonarchCooltime[MI_MAX];
	DWORD m_dwMonarchCooltimelimit[MI_MAX];

	void InitMC();
	DWORD GetMC(enum MONARCH_INDEX e) const;
	void SetMC(enum MONARCH_INDEX e);
	bool IsMCOK(enum MONARCH_INDEX e) const;
	DWORD GetMCL(enum MONARCH_INDEX e) const;
	DWORD GetMCLTime(enum MONARCH_INDEX e) const;

public:
	bool ItemProcess_Polymorph(LPITEM item);

	// by mhh
#ifndef ENABLE_CUBE_RENEWAL
	LPITEM* GetCubeItem() noexcept { return m_pointsInstant.pCubeItems; }
#endif
	bool IsCubeOpen() const { return (m_pointsInstant.pCubeNpc ? true : false); }
	void SetCubeNpc(LPCHARACTER npc) { m_pointsInstant.pCubeNpc = npc; }
	bool CanDoCube() const;

#ifdef ENABLE_CUBE_RENEWAL
	void SetTempCubeNPC(uint32_t vnum) { tempCubeNPC = vnum; }
	uint32_t GetTempCubeNPC() { return tempCubeNPC; }

	int GetCubeLastTime() const { return m_iCubeTime; }
	void SetCubeTime() { m_iCubeTime = thecore_pulse(); }

private:
	uint32_t tempCubeNPC;
	int m_iCubeTime;
#endif

public:
	bool IsSiegeNPC() const;

private:
	// �߱� ����
	// 18�� �̸� ����
	// 3�ð� : 50 % 5 �ð� 0%
	e_overtime m_eOverTime;

public:
	bool IsOverTime(e_overtime e) const { return (e == m_eOverTime); }
	void SetOverTime(e_overtime e) { m_eOverTime = e; }

private:
	int m_deposit_pulse;

public:
	void UpdateDepositPulse();
	bool CanDeposit() const;

private:
	void __OpenPrivateShop();
#if defined(__MYSHOP_DECO__)
	void __OpenMyShopDeco();
#endif

public:
	struct AttackedLog
	{
		DWORD dwPID;
		DWORD dwAttackedTime;

		AttackedLog() : dwPID(0), dwAttackedTime(0)
		{
		}
	};

	AttackLog m_kAttackLog;
	AttackedLog m_AttackedLog;
	int m_speed_hack_count;

private:
	std::string m_strNewName;

public:
	const std::string GetNewName() const { return this->m_strNewName; }
	void SetNewName(const std::string name) { this->m_strNewName = name; }

public:
	void GoHome();

private:
	std::set<DWORD> m_known_guild;

public:
	void SendGuildName(CGuild* pGuild);
	void SendGuildName(DWORD dwGuildID);

private:
	DWORD m_dwLogOffInterval;
	DWORD m_dwLastPlay;

public:
	DWORD GetLogOffInterval() const { return m_dwLogOffInterval; }
	DWORD GetLastPlay() const { return m_dwLastPlay; }

public:
	bool UnEquipSpecialRideUniqueItem();

	bool CanWarp() const;
	bool IsInSafezone() const;
	bool IsInBlockedArea(long x = 0, long y = 0) const;

private:
	DWORD m_dwLastGoldDropTime;
#if defined(__CHEQUE_SYSTEM__)
	DWORD m_dwLastChequeDropTime;
#endif

public:
	void AutoRecoveryItemProcess(const EAffectTypes);

public:
	void BuffOnAttr_AddBuffsFromItem(LPITEM pItem);
	void BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem);

private:
	void BuffOnAttr_ValueChange(uint16_t bType, BYTE bOldValue, BYTE bNewValue); //@fixme532
	void BuffOnAttr_ClearAll();

	typedef std::map <uint16_t, CBuffOnAttributes*> TMapBuffOnAttrs; //@fixme532
	TMapBuffOnAttrs m_map_buff_on_attrs;
	// ���� : ��Ȱ�� �׽�Ʈ�� ���Ͽ�.

public:
	void SetArmada() { cannot_dead = true; }
	void ResetArmada() { cannot_dead = false; }

private:
	bool cannot_dead;

#if defined(__BL_67_ATTR__)
	public:
		void Open67Attr();
		void Set67Attr(bool b) { b67Attr = b; }
		bool Is67AttrOpen() const { return b67Attr; }
	private:
		bool b67Attr;
#endif

#if defined(__PET_SYSTEM__)
private:
	bool m_bIsPet;

public:
	void SetPet() { m_bIsPet = true; }
	bool IsPet() { return m_bIsPet; }
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
private:
	bool m_bIsGrowthPet;
	int m_GrowthPetEggVID;
	int m_GrowthPetEvolution;
	bool m_GrowthPetHachWindowIsOpen;
	uint8_t m_GrowthPetWindowType;
protected:
	int m_iGrowthPetDetermineLoadTime;
public:
	void SetGrowthPet() noexcept { m_bIsGrowthPet = true; }
	bool IsGrowthPet() const noexcept { return m_bIsGrowthPet ? true : false; }
	bool IsPetPay() const noexcept { return m_bCharType == CHAR_TYPE_PET_PAY; }

	void SetGrowthPetEggVID(int EggVID) noexcept { m_GrowthPetEggVID = EggVID; }
	int GetGrowthPetEggVID() noexcept { return m_GrowthPetEggVID; }
	void SetPetEvolution(int PetEvolution) noexcept { m_GrowthPetEvolution = PetEvolution; }
	int GetPetEvolution() noexcept { return m_GrowthPetEvolution; }

	void SendGrowthPetHatching(uint8_t bResult, uint8_t pos);
	void SetGrowthPetHatchingWindow(bool isOpen) noexcept { m_GrowthPetHachWindowIsOpen = isOpen; };
	bool IsGrowthPetHatchingWindow() const noexcept { return m_GrowthPetHachWindowIsOpen; };

	void SetPetWindowType(uint8_t pet_window_type);
	uint8_t GetPetWindowType() const noexcept { return m_GrowthPetWindowType; };

	void SendGrowthPetUpgradeSkillRequest(uint8_t bSkillSlot, uint8_t bSkillIndex, int iPrice);
#	ifdef ENABLE_PET_ATTR_DETERMINE
	void PetAttrChange(uint8_t bPetSlotIndex, uint8_t bMaterialSlotIndex);
	bool IsGrowthPetDetermineWindow() const noexcept { return m_GrowthPetWindowType == 1; };
	int GetGrowthPetDetermineLoadTime() const noexcept { return m_iGrowthPetDetermineLoadTime; }
	void SetGrowthPetDetermineLoadTime() noexcept { m_iGrowthPetDetermineLoadTime = thecore_pulse(); }
#	endif
#	ifdef ENABLE_PET_PRIMIUM_FEEDSTUFF
	bool IsGrowthPetPrimiumFeedWindow() const noexcept { return m_GrowthPetWindowType == 2; };
	void RevivePet(const TPacketCGGrowthPetReviveRequest* revivePacket, LPITEM pSummonItem);
	void Revive(LPITEM pSummonItem, uint8_t bType);
#	endif
#endif

	//Final damage correction.
private:
	float m_fAttMul;
	float m_fDamMul;

public:
	float GetAttMul() { return this->m_fAttMul; }
	void SetAttMul(float newAttMul) { this->m_fAttMul = newAttMul; }
	float GetDamMul() { return this->m_fDamMul; }
	void SetDamMul(float newDamMul) { this->m_fDamMul = newDamMul; }

private:
	bool IsValidItemPosition(TItemPos Pos) const;

	// ���� ���� ��� ��Ŷ �ӽ� ����
private:
	unsigned int itemAward_vnum;
	char itemAward_cmd[20];
	//bool itemAward_flag;
public:
	unsigned int GetItemAward_vnum() { return itemAward_vnum; }
	char* GetItemAward_cmd() { return itemAward_cmd; }
	//bool GetItemAward_flag() { return itemAward_flag; }
	void SetItemAward_vnum(unsigned int vnum) { itemAward_vnum = vnum; }
	void SetItemAward_cmd(char* cmd) { strcpy(itemAward_cmd, cmd); }
	//void SetItemAward_flag(bool flag) { itemAward_flag = flag; }
	void SetBasicCsotumeHair(LPCHARACTER ch);
	void SetBasicCsotume(LPCHARACTER ch);

#if defined(__MINI_GAME_CATCH_KING__)
public:
	void MiniGameCatchKingSetFieldCards(std::vector<TCatchKingCard> vec) { m_vecCatchKingFieldCards = vec; }

	DWORD MiniGameCatchKingGetScore() const { return dwCatchKingTotalScore; }
	void MiniGameCatchKingSetScore(DWORD dwScore) { dwCatchKingTotalScore = dwScore; }

	DWORD MiniGameCatchKingGetBetNumber() const { return bCatchKingBetSetNumber; }
	void MiniGameCatchKingSetBetNumber(BYTE bSetNr) { bCatchKingBetSetNumber = bSetNr; }

	BYTE MiniGameCatchKingGetHandCard() const { return bCatchKingHandCard; }
	void MiniGameCatchKingSetHandCard(BYTE bKingCard) { bCatchKingHandCard = bKingCard; }

	BYTE MiniGameCatchKingGetHandCardLeft() const { return bCatchKingHandCardLeft; }
	void MiniGameCatchKingSetHandCardLeft(BYTE bHandCard) { bCatchKingHandCardLeft = bHandCard; }

	bool MiniGameCatchKingGetGameStatus() const { return dwCatchKingGameStatus; }
	void MiniGameCatchKingSetGameStatus(bool bStatus) { dwCatchKingGameStatus = bStatus; }

	std::vector<TCatchKingCard> m_vecCatchKingFieldCards;

protected:
	BYTE bCatchKingHandCard;
	BYTE bCatchKingHandCardLeft;
	bool dwCatchKingGameStatus;

	BYTE bCatchKingBetSetNumber;
	DWORD dwCatchKingTotalScore;
#endif

public:
	// ��ȥ��

	// ĳ������ affect, quest�� load �Ǳ� ���� DragonSoul_Initialize�� ȣ���ϸ� �ȵȴ�.
	// affect�� ���� �������� �ε�Ǿ� LoadAffect���� ȣ����.
	void DragonSoul_Initialize();

	bool DragonSoul_IsQualified() const;
	void DragonSoul_GiveQualification();

	int DragonSoul_GetActiveDeck() const;
	bool DragonSoul_IsDeckActivated() const;
	bool DragonSoul_ActivateDeck(int deck_idx);

	void DragonSoul_DeactivateAll();
	// �ݵ�� ClearItem ���� �ҷ��� �Ѵ�.
	// �ֳ��ϸ�....
	// ��ȥ�� �ϳ� �ϳ��� deactivate�� ������ ���� active�� ��ȥ���� �ִ��� Ȯ���ϰ�,
	// active�� ��ȥ���� �ϳ��� ���ٸ�, ĳ������ ��ȥ�� affect��, Ȱ�� ���¸� �����Ѵ�.
	// 
	// ������ ClearItem ��, ĳ���Ͱ� �����ϰ� �ִ� ��� �������� unequip�ϴ� �ٶ���,
	// ��ȥ�� Affect�� ���ŵǰ�, �ᱹ �α��� ��, ��ȥ���� Ȱ��ȭ���� �ʴ´�.
	// (Unequip�� ������ �α׾ƿ� ��������, �ƴ��� �� �� ����.)
	// ��ȥ���� deactivate��Ű�� ĳ������ ��ȥ�� �� Ȱ�� ���´� �ǵ帮�� �ʴ´�.
	void DragonSoul_CleanUp();

#if defined(__DS_SET__)
	void DragonSoul_HandleSetBonus(/*bool bSet = true*/);
#endif

	// ��ȥ�� ��ȭâ
public:
	bool DragonSoul_RefineWindow_Open(LPENTITY pEntity);
#if defined(__DS_CHANGE_ATTR__)
	bool DragonSoul_RefineWindow_ChangeAttr_Open(LPENTITY pEntity);
#endif
	bool DragonSoul_RefineWindow_Close();
	LPENTITY DragonSoul_RefineWindow_GetOpener() { return m_pointsInstant.m_pDragonSoulRefineWindowOpener; }
	bool DragonSoul_RefineWindow_CanRefine();

private:
	// SyncPosition�� �ǿ��Ͽ� Ÿ������ �̻��� ������ ������ �� ����ϱ� ���Ͽ�,
	// SyncPosition�� �Ͼ ���� ���.
	timeval m_tvLastSyncTime;
	int m_iSyncHackCount;
#ifdef ENABLE_SWITCHBOT
	DWORD use_item_anti_flood_count_;
	int use_item_anti_flood_pulse_;
#endif
public:
	void SetLastSyncTime(const timeval& tv) { memcpy(&m_tvLastSyncTime, &tv, sizeof(timeval)); }
	const timeval& GetLastSyncTime() { return m_tvLastSyncTime; }
	void SetSyncHackCount(int iCount) { m_iSyncHackCount = iCount; }
	int GetSyncHackCount() { return m_iSyncHackCount; }

	// @fixme188 BEGIN
	public:
		void ResetRewardInfo() {
			m_map_kDamage.clear();
			if (!IsPC())
				SetExp(0);
		}
		void DeadNoReward() {
			if (!IsDead())
				ResetRewardInfo();
			Dead();
		}
	// @fixme188 END

#if defined(ENABLE_BATTLE_FIELD) || defined(ENABLE_MEDAL_OF_HONOR)
public:
	int GetShopExUsablePoint(uint8_t coin_type) const noexcept { return m_points.aiShopExUsablePoint[coin_type]; }
	void SetShopExUsablePoint(uint8_t coin_type, int usable_point) noexcept { m_points.aiShopExUsablePoint[coin_type] = usable_point; }

	int GetShopExDailyTimePoint(uint8_t coin_type) const noexcept { return m_points.aiShopExDailyUse[coin_type]; }
	void SetShopExDailyTimePoint(uint8_t coin_type, int dayli_time) noexcept { m_points.aiShopExDailyUse[coin_type] = dayli_time; }
#endif

#ifdef ENABLE_MEDAL_OF_HONOR
public:
	long GetMedalHonor() const noexcept { return m_points.medal_honor; }
	void SetMedalHonor(long medalHonor) noexcept { m_points.medal_honor = medalHonor; }
	void CheckMedals();
#endif

#if defined(__HIDE_COSTUME_SYSTEM__)
public:
	void SetHideCostumePulse(int iPulse) { m_HideCostumePulse = iPulse; }
	int GetHideCostumePulse() { return m_HideCostumePulse; }

	void SetBodyCostumeHidden(bool hidden);
	bool IsBodyCostumeHidden() const { return m_bHideBodyCostume; };

	void SetHairCostumeHidden(bool hidden);
	bool IsHairCostumeHidden() const { return m_bHideHairCostume; };

#if defined(__ACCE_COSTUME_SYSTEM__)
	void SetAcceCostumeHidden(bool hidden);
	bool IsAcceCostumeHidden() const { return m_bHideAcceCostume; };
#endif

#if defined(__WEAPON_COSTUME_SYSTEM__)
	void SetWeaponCostumeHidden(bool hidden);
	bool IsWeaponCostumeHidden() const { return m_bHideWeaponCostume; };
#endif

	void SetAuraCostumeHidden(bool hidden);
	bool IsAuraCostumeHidden() const { return m_bHideAuraCostume; };

private:
	int m_HideCostumePulse;
	bool m_bHideBodyCostume;
	bool m_bHideHairCostume;
#if defined(__ACCE_COSTUME_SYSTEM__)
	bool m_bHideAcceCostume;
#endif
#if defined(__WEAPON_COSTUME_SYSTEM__)
	bool m_bHideWeaponCostume;
#endif
	bool m_bHideAuraCostume;
#endif

#if defined(__GEM_MARKET_SYSTEM__)
public:
	struct Gem_Shop_Values
	{
		int value_1;
		int value_2;
		int value_3;
		int value_4;
		int value_5;
		int value_6;
		bool operator == (const Gem_Shop_Values& b)
		{
			return (this->value_1 == b.value_1) && (this->value_2 == b.value_2) &&
				(this->value_3 == b.value_3) && (this->value_4 == b.value_4) &&
				(this->value_5 == b.value_5) && (this->value_6 == b.value_6);
		}
	};

	struct Gem_Load_Values
	{
		DWORD items;
		DWORD gem;
		DWORD count;
		DWORD glimmerstone;
		DWORD gem_expansion;
		DWORD gem_refresh;
		DWORD glimmerstone_count;
		DWORD gem_expansion_count;
		DWORD gem_refresh_count;
		DWORD grade_stone;
		DWORD give_gem;
		DWORD prob_gem;
		DWORD cost_gem_yang;
	};

	bool CheckItemsFull();
	void UpdateItemsGemMarker0();
	void UpdateItemsGemMarker();
	void InfoGemMarker();
	void ClearGemMarket();
	bool CheckSlotGemMarket(int slot);
	void UpdateSlotGemMarket(int slot);
	void BuyItemsGemMarket(int slot);
	void RefreshItemsGemMarket();
	void CraftGemItems(int slot);
	void MarketGemItems(int slot);
	void RefreshGemItems();
	void LoadGemSystem();
	int GetGemState(const std::string& state) const;
	void SetGemState(const std::string& state, int szValue);
	void StartCheckTimeMarket();
	void StartCheckTimeMarketLogin();

private:
	std::vector<Gem_Shop_Values> info_items;
	std::vector<Gem_Shop_Values> info_slots;
	std::vector<Gem_Load_Values> load_gem_items;
	Gem_Load_Values load_gem_values;
	LPEVENT GemUpdateTime;
#endif

#if defined(__ACCE_COSTUME_SYSTEM__)
protected:
	bool m_bAcceCombination, m_bAcceAbsorption;

public:
	bool IsOpenAcce();
	bool isAcceOpened(bool bCombination) const { return bCombination ? m_bAcceCombination : m_bAcceAbsorption; }
	void OpenAcce(bool bCombination);
	void CloseAcce();
	void ClearAcceMaterials();
	bool CleanAcceAttr(LPITEM pkItem, LPITEM pkTarget);
	LPITEM* GetAcceMaterials() { return m_pointsInstant.pAcceMaterials; }
	bool AcceIsSameGrade(long lGrade);
	DWORD GetAcceCombinePrice(long lGrade);
	void GetAcceCombineResult(DWORD& dwItemVnum, DWORD& dwMinAbs, DWORD& dwMaxAbs);
	BYTE CheckEmptyMaterialSlot();
	void AddAcceMaterial(TItemPos tPos, BYTE bPos);
	void RemoveAcceMaterial(BYTE bPos);
	BYTE CanRefineAcceMaterials();
	void RefineAcceMaterials();
#endif

#if defined(__MINI_GAME_OKEY__)
public:
	struct S_CARD
	{
		DWORD type;
		DWORD value;
	};

	struct CARDS_INFO
	{
		S_CARD cards_in_hand[MAX_CARDS_IN_HAND];
		S_CARD cards_in_field[MAX_CARDS_IN_FIELD];
		DWORD cards_left;
		DWORD field_points;
		DWORD points;
	};

	void Cards_open(DWORD safemode);
	void Cards_clean_list();
	DWORD GetEmptySpaceInHand();
	void Cards_pullout();
	void RandomizeCards();
	bool CardWasRandomized(DWORD type, DWORD value);
	void SendUpdatedInformations();
	void SendReward();
	void CardsDestroy(DWORD reject_index);
	void CardsAccept(DWORD accept_index);
	void CardsRestore(DWORD restore_index);
	DWORD GetEmptySpaceInField();
	DWORD GetAllCardsCount();
	bool TypesAreSame();
	bool ValuesAreSame();
	bool CardsMatch();
	DWORD GetLowestCard();
	bool CheckReward();
	void CheckCards();
	void RestoreField();
	void ResetField();
	void CardsEnd();
	void GetGlobalRank(char* buffer, size_t buflen);
	void GetRundRank(char* buffer, size_t buflen);

protected:
	CARDS_INFO character_cards;
	S_CARD randomized_cards[24];
#endif

#if defined(__PRIVATESHOP_SEARCH_SYSTEM__)
public:
	BYTE GetPrivateShopSearchState() const { return bPrivateShopSearchState; }
	void SetPrivateShopSearchState(BYTE bState) { bPrivateShopSearchState = bState; }
	void OpenPrivateShopSearch(DWORD dwVnum);
protected:
	BYTE bPrivateShopSearchState;
#endif

#if defined(__CHANGE_LOOK_SYSTEM__)
public:
	void SetChangeLook(CChangeLook* c);
	CChangeLook* GetChangeLook() const;
protected:
	CChangeLook* m_pkChangeLook;
#endif

#if defined(__MAILBOX__)
public:
	int GetMyMailBoxTime() const { return m_iMyMailBoxTime; }
	void SetMyMailBoxTime() { m_iMyMailBoxTime = thecore_pulse(); }

	void SetMailBox(CMailBox* m);

	void SetMailBoxLoading(const bool b) { bMailBoxLoading = b; }
	bool IsMailBoxLoading() const { return bMailBoxLoading; }

	CMailBox* GetMailBox() const { return m_pkMailBox; }

private:
	CMailBox* m_pkMailBox;
	bool bMailBoxLoading;
	int m_iMyMailBoxTime;
#endif

#if defined(__CONQUEROR_LEVEL__)
public:
	bool IsNewWorldMap(long lMapIndex);
	void SetSungMaWill();
	uint8_t GetSungMaWill(uint8_t type) const;
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	protected:
		BYTE	m_BiologActualMission;
		WORD	m_BiologCollectedItems;
		BYTE	m_BiologCooldownReminder;
		long	m_BiologCooldown;

	public:
		CBiologSystem* GetBiologManager() const { return m_pkBiologManager; }

		BYTE	GetBiologMissions() const { return m_BiologActualMission; }
		WORD	GetBiologCollectedItems() const { return m_BiologCollectedItems; }
		BYTE	GetBiologCooldownReminder() const { return m_BiologCooldownReminder; }
		long	GetBiologCooldown() const { return m_BiologCooldown; }

		void	SetBiologMissions(BYTE value) { m_BiologActualMission = value; }
		void	SetBiologCollectedItems(WORD value) { m_BiologCollectedItems = value; }
		void	SetBiologCooldownReminder(BYTE value) { m_BiologCooldownReminder = value; }
		void	SetBiologCooldown(long value) { m_BiologCooldown = value; }

	private:
		CBiologSystem* m_pkBiologManager;
#endif

	public:
#ifdef ENABLE_WHITE_DRAGON
		bool			IsWhiteMap();
#endif
#ifdef ENABLE_QUEEN_NETHIS
		bool			IsSnakeMap();
#endif

#ifdef ENABLE_GUILD_TOKEN_AUTH
	public:
		bool IsGuildMaster() const;
		uint64_t GetGuildToken() const;
		void SendGuildToken();
#endif
#if defined(__ENABLE_RIDING_EXTENDED__)
	public:
		void SetMountUpGradeExp(uint32_t exp) {m_mount_up_grade_exp = exp;}
		uint32_t GetMountUpGradeExp() const { return m_mount_up_grade_exp; }

		void SetMountUpGradeFail(uint8_t fail) {m_mount_up_grade_fail = fail;}
		uint8_t IsMountUpGradeFail() const { return m_mount_up_grade_fail; }

	protected:
		uint32_t m_mount_up_grade_exp;
		uint8_t	 m_mount_up_grade_fail;
#endif
#ifdef ENABLE_PET_SUMMON_AFTER_REWARP
	public:
		void SetSummonGrowthPet(LPITEM pPetItem);
		LPITEM GetSummonGrowthPet() const;
		LPITEM GetSummonGrowthPetSystem() const { return m_pkSummonGrowthPet; }
	//private:
		LPITEM m_pkSummonGrowthPet;
#endif
};

ESex GET_SEX(LPCHARACTER ch);

#endif // __INC_CHAR_H__
