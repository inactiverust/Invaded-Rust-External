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

namespace vars
{
	uint32_t target_pid;
	BasePlayer* AimPlayer;
	std::vector<BasePlayer*> playerList;
	std::vector<DrawingInfo> playerPosList;
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
}

namespace settings {
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
		float esp_distance = 200.f;
	}
}
namespace menu
{
}
namespace keybinds
{
	int aimkey = VK_RBUTTON;
}