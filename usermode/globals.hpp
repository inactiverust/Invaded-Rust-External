#pragma once

#include "common.hpp"
#include "BasePlayer.hpp"
#include "AdminConVar.hpp"
#include "Sky.hpp"
#include "List.hpp"
#include "OcclusionCulling.hpp"

namespace misc
{
	float width;
	float height;
}

struct DrawingInfo
{
	Vector2 HeadScreenPos;
	Vector2 ToeScreenPos;
	std::string name;
	int distance;
	float health;
	bool isTarget;
};

struct PlayerInfo
{
	std::string name;
	float distance;
	float hp;
	std::string slot[6];
};

namespace vars
{
	uint32_t target_pid;
	BasePlayer* AimPlayer;
	std::vector<BasePlayer*> playerList;
	std::vector<DrawingInfo> playerPosList;
	std::vector<BasePlayer*> set_players;
	PlayerInfo aim_player_info;
}

namespace pointers
{
	uintptr_t game_assembly;
	uintptr_t fov_pointer;
	Matrix4x4* view_matrix_pointer;
	BasePlayer* local_player;
	AdminConVar* admin_convar_static;
	Sky* tod_sky_instance;
	OcclusionCulling* occlusion_culling_static;
	namespace cham_mats
	{
		uintptr_t pink = 0;
		uintptr_t black = 0;
		uintptr_t white = 0;
		uintptr_t world = 0;
	}
}

namespace settings {
	bool chams = false;
	bool waterWalk = false;
	bool heliShoot = false;
	bool noSway = false;
	bool automatic = false;
	bool FOVChanger = false;
	bool superJump = false;
	bool Spiderman = false;
	bool timeChanger = false;
	bool adminFlags = false;
	bool superEoka = false;
	bool noRecoil = false;
	bool aimBot = false;
	bool noSpread = false;
	bool noHeavy = false;
	bool esp = false;
	bool full_bright = false;
	float recoilPercentage = 100;
	float time = 12.f;
	float fov = 120.f;
	namespace aim
	{
		float fov = 300.f;
		bool move_prediction = false;
		bool target_wounded = false;
		bool target_sleeping = false;
		bool target_npc = false;
		bool vischeck = false;
		bool silent = false;
		float aim_distance = 200.f;
		BasePlayer::bone_list aim_bone = BasePlayer::bone_list::head;
	}
	namespace ESP
	{
		bool show_wounded = false;
		bool show_sleeping = false;
		bool show_npc = false;
		bool show_health = false;
		bool show_name = false;
		bool show_distance = false;
		bool show_panel = false;
		float esp_distance = 200.f;
	}
}
namespace menu_settings
{
	Vector3 aim_target_color = Vector3(1, 0, 0);
	Vector3 normal_esp_color = Vector3(1, 1, 1);
	Vector3 fov_circle_color = Vector3(1, 1, 1);
}

namespace keybinds
{
	int aimkey = VK_RBUTTON;
}