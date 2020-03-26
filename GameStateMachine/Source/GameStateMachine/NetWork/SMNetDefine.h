#pragma once

#define MAX_FLAG_INT32	256

#define ITEM_BOUNDGOLD_ID		10001		// 绑定金币
#define ITEM_GOLD_ID	10002				// 金币
#define ITEM_MONEYBOUND_ID	10003			// 绑定D币
#define ITEM_MONEY_ID	10004				// D币这个只有充值
#define ITEM_LEGENDGOLD_ID 10005			// 传奇币
#define ITEM_EXP_ID		10006				// 经验（暂时没用）

#define ITEM_BATTLEPASS_EXP_ID 10010		// Battlepass 经验
#define ITEM_WEEK_ACTIVE_ID 10011			// 周活跃度
#define ITEM_DAILY_ACTIVE_ID 10012			// 日活跃度

#define PSN_LABEL_TYPE_CHARGE 0
#define PSN_LABEL_TYPE_DLC 1


enum PLAYER_FLAG32
{
	//必须赋值，因为会存盘，中途不能更改索引
	player_flag32_head = 0,
	player_flag32_chest_shirt = 1,
	player_flag32_hands_gloves = 2,
	player_flag32_coat = 3,
	player_flag32_crotch_pants_shorts_skirt = 4,
	player_flag32_lowerLeg_leggings_socks = 5,
	player_flag32_feet_shoes = 6,
	player_flag32_hat = 7,
	player_flag32_hair = 8,
	player_flag32_beard = 9,
	player_flag32_glasses = 10,
	player_flag32_necklace = 11,
	player_flag32_earrings = 12,
	player_flag32_watch_bracelet = 13,
	player_flag32_tatoo = 14,
	player_flag32_helm = 15,//
	player_flag32_vest = 16,//
	player_flag32_backpack = 17,//
	player_flag32_raceskin = 18,
	player_flag32_painting = 19,
	player_flag32_behaivorcontroller = 20,
	player_flag32_belt = 21,

	Player_Flag32_HeadSuit = 27,				//头部套装
	Player_Flag32_BodySuit = 28,				//身体套装
	Player_Flag32_WholeSuit = 29,				//整体套装

	player_flag32_noSexBegin = 30,
	player_flag32_wolf = 30,				//狼皮肤
	player_flag32_parachute = 31,				//降落伞
	player_flag32_smog = 32,					//尾烟
	Player_Flag32_BusinessCard = 33,			//名片
	Player_Flag32_Leopard = 34,					//豹人皮肤
	player_flag32_noSexEnd = 40,


	player_flag32_body_max = 60,				//这里是个标记



	//动作预留
	player_flag32_act_begin = 61,
	player_flag32_act_1 = 61,
	player_flag32_act_2 = 62,
	player_flag32_act_3 = 63,
	player_flag32_act_4 = 64,
	player_flag32_act_5 = 65,
	player_flag32_act_end = 65,

	//动作预留
	player_flag32_points_begin = 71,
	player_flag32_points_used = 71,
	player_flag32_points_unused = 72,
	player_flag32_points_end = 72,


	//匹配相关
	player_flag32_score = 73,

	player_flag32_gc_mail_id = 74,

	//玩家累计充值（加player_data存盘太麻烦了，让我写在这里）
	player_flag32_mall_all_recharge = 75,
	//标记玩家打过的副本(永保大概是是这么描述的，就是个跟角色相关的标记)
	player_flag32_transcripts = 76,

	player_flag32_user_agreement_version = 77,
	player_flag32_privacy_version = 78,

	Player_Flag32_Human_Kill_Human = 81,  // 人杀人总数
	Player_Flag32_Human_Kill_Wolf = 82,  // 人杀狼总数
	Player_Flag32_Human_Win = 83,  // 人类胜利次数
	Player_Flag32_Wolf_Kill_Human = 84,  // 狼杀人总数
	Player_Flag32_Wolf_Prevent_Evacuate = 85,  // 狼人阻止人类撤离的次数
	Player_Flag32_Wolf_Kill_All_Human = 86,  // 狼人全灭人类次数

	Player_Flag32_BattlePass_ID = 94,
	Player_Flag32_BattlePass_Exp = 95,
	Player_Flag32_BattlePass_lv = 96,
	Player_Flag32_BattlePass_Plus = 97,

	player_flag32_max = MAX_FLAG_INT32,

	//下面的递增就可以了，不需要赋值
	player_flag32_tmp_begin = player_flag32_max,//不需要存盘的临时变量

	player_flag32_tmp_match_type,					//匹配类型
	player_flag32_tmp_match_status,					//进入匹配后的状态


	player_flag32_tmp_end
};

enum
{
	//副本战斗的标记名称 用这个取得标记位中的指定副本
	TRANSCRIPT_BEGIN = -1,
	TRANSCRIPT_1 = 0,
	TRANSCRIPT_END,
};

//匹配类型（最多31种类型） 模式->地图  每个模式下可以有不同的地图
enum
{
	/////////////////////////逃生模式//////////////////////////////
	MATCH_TYPE_MIN = 0,
	MATCH_TYPE_QUICK = 1,			//快速匹配
	MATCH_TYPE_SOLO = 2,			//solo匹配
	MATCH_TYPE_2GROUP = 3,			//2人组队
	MATCH_TYPE_4GROUP = 4,			//4人组队  

	////////////////////////三军模式////////////////////////////////////////
	MATCH_TYPE_THREEARMY_10GROUP = 5,			//10人组队

	///////////////////////8人逃杀模式//////////////////////////////////////
	MATCH_TYPE_ICU_QUICK = 6,          //8人逃杀单人匹配
	MATCH_TYPE_ICU_2GROUP = 7,         //8人逃杀2人组队

	MATCH_TYPE_MAX,
};

enum
{
	MOUDLES_MIN = 0,
	MOUDLES_NORMAL = 1,
	MOUDLES_THREEARMY = 2,
};

struct FMailAttach
{
	int8 item_Type;
	int32 item_id;
	int32 item_num;
};

struct FNetMail
{
	int64 mailguid;
	int32 mailID;
	FString title;
	FString content;
	FString mailsender;
	FString filepath; //now just include img path
	int mailType;
	TArray<FMailAttach> attacharr;
	int64 sendtime;
	int64 effectiveTime;
	int8 HaveRead;
};

enum STATE_TYPE
{
	STATE_TYPE_GOLD_PLUS_50 = 1,
};
