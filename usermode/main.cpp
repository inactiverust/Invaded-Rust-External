#include "common.hpp"
#include "memory.hpp"
#include "globals.hpp"
#include "features.hpp"

#include "auth/skStr.h"
#include "auth/auth.hpp"

#include "drawing.hpp"

bool should_exit;

#define check_rust false
#define check_auth false
#define using_signed false

bool get_local_player()
{
	uintptr_t buffer_list = memory::read_chain(pointers::game_assembly, { classes::oBaseEntity, 0xB8, 0x10, 0x10, 0x28 });
	if (!buffer_list)
		buffer_list = memory::read_chain(pointers::game_assembly, { classes::oBaseEntity, 0xB8, 0x40, 0x10, 0x28 });
	int sz = memory::read<int>(buffer_list + 0x10);
	uintptr_t p_object_list = memory::read<uintptr_t>(buffer_list + 0x18);
	std::vector<uintptr_t> object_list = List::get_list(p_object_list, sz);
	for (const auto& object : object_list)
	{
		if (!object) continue;
		uintptr_t game_object = memory::read_chain(object, { 0x10, 0x30 });
		WORD tag = memory::read<WORD>(game_object + 0x54);
		if (tag == 6)
		{
			BasePlayer* Player = reinterpret_cast<BasePlayer*>(memory::read_chain(game_object, { 0x30, 0x18, 0x28 }));
			if (Player->player_model()->is_local_player())
			{
				pointers::local_player = Player;
				return true;
			}
		}
	}
	return false;
}


void setup()
{
	misc::height = GetSystemMetrics(SM_CYSCREEN);
	misc::width = GetSystemMetrics(SM_CXSCREEN);
	pointers::admin_convar_static = reinterpret_cast<AdminConVar*>(memory::read_chain(pointers::game_assembly, { classes::oAdminConVar, 0xB8 }));
	pointers::tod_sky_instance = reinterpret_cast<Sky*>(memory::read_chain(pointers::game_assembly, { classes::oTODSky, 0xB8, 0x0, 0x10, 0x20 }));
	pointers::fov_pointer = memory::read_chain(pointers::game_assembly, { classes::oGraphicConVar, 0xB8 });
	pointers::view_matrix_pointer = reinterpret_cast<Matrix4x4*>(memory::read_chain(pointers::game_assembly, { classes::oMainCamera, 0xB8, 0x0, 0x10 }) + 0x2E4);
	pointers::occlusion_culling_static = reinterpret_cast<OcclusionCulling*>(memory::read_chain(pointers::game_assembly, { classes::oOcclusionCulling, 0xB8 }));
	pointers::occlusion_culling_static->disable_animals();
	
}

void esp()
{
	vars::playerList.clear();
	std::vector<DrawingInfo> temp_info;
	uintptr_t buffer_list = memory::read_chain(pointers::game_assembly, { classes::oBaseEntity, 0xB8, 0x10, 0x10, 0x28 });
	if (!buffer_list)
		buffer_list = memory::read_chain(pointers::game_assembly, { classes::oBaseEntity, 0xB8, 0x40, 0x10, 0x28 });
	int sz = memory::read<int>(buffer_list + 0x10);
	uintptr_t p_object_list = memory::read<uintptr_t>(buffer_list + 0x18);
	std::vector<uintptr_t> object_list = List::get_list(p_object_list, sz);
	for (const auto& object : object_list)
	{
		if (!object) continue;
		uintptr_t game_object = memory::read_chain(object, { 0x10, 0x30 });
		WORD tag = memory::read<WORD>(game_object + 0x54);
		if (tag == 6)
		{
			BasePlayer* Player = reinterpret_cast<BasePlayer*>(memory::read_chain(game_object, { 0x30, 0x18, 0x28 }));
			if (Player->player_model()->is_local_player())
			{
				if (Player != pointers::local_player)
				{
					pointers::local_player = Player;
					setup();
				}
			}
			else
			{
				if (settings::aimBot)
				{
					if (!settings::aim::target_sleeping && Player->has_flag(BasePlayer::player_flags::Sleeping))
						goto label;
					else if (!settings::aim::target_wounded && Player->has_flag(BasePlayer::player_flags::Wounded))
						goto label;
					else if (!settings::aim::target_npc && Player->player_model()->is_npc())
						goto label;
					vars::playerList.push_back(Player);
				}
				label:
				if (settings::esp)
				{
					if (!settings::ESP::show_sleeping && Player->has_flag(BasePlayer::player_flags::Sleeping))
						continue;
					else if (!settings::ESP::show_wounded && Player->has_flag(BasePlayer::player_flags::Wounded))
						continue;
					else if (!settings::ESP::show_npc && Player->player_model()->is_npc())
						continue;

					DrawingInfo info;

					Vector3 hpos = Player->get_position(BasePlayer::head);
					Vector3 tpos = Player->player_model()->get_position();
					Vector3 lpos = pointers::local_player->player_model()->get_position();

					float dist = Calc3D_Dist(tpos, lpos);

					if (dist > settings::ESP::esp_distance)
						continue;

					Aim::world_to_screen(Vector3(hpos.x, hpos.y + 0.3, hpos.z), info.HeadScreenPos);
					Aim::world_to_screen(Vector3(tpos.x, tpos.y - 0.15, tpos.z), info.ToeScreenPos);

					info.name = Player->get_name();
					info.distance = dist;

					if (Player == vars::AimPlayer)
						info.isTarget = true;
					else
						info.isTarget = false;

					info.health = Player->get_health();

					temp_info.push_back(info);
				}
			}
		}
	}
	vars::playerPosList = std::move(temp_info);
}

void fill_player_info()
{
	if (settings::ESP::show_panel)
	{
		PlayerInfo* info = &vars::aim_player_info;
		if (vars::AimPlayer)
		{
			info->hp = vars::AimPlayer->get_health();
			info->distance = Calc3D_Dist(vars::AimPlayer->player_model()->get_position(), pointers::local_player->player_model()->get_position());
			info->name = vars::AimPlayer->get_name();
			
			uintptr_t inventory = memory::read<uintptr_t>((uintptr_t)vars::AimPlayer + oInventory);
			uintptr_t belt = memory::read<uintptr_t>(inventory + 0x28);
			uintptr_t item_list = memory::read<uintptr_t>(belt + 0x38);
			uintptr_t list = memory::read<uintptr_t>(item_list + 0x10);
			int sz = memory::read<int>(item_list + 0x18);

			std::vector<uintptr_t> item_arr = List::get_list(list, sz);
			int counter = 0;
			for (auto& entry : item_arr)
			{
				uintptr_t ItemDefinition = memory::read<uintptr_t>(entry + 0x20);
				wchar_t item_name[36] = { '\0' };
				memory::copy_memory(memory::read<uintptr_t>((uintptr_t)ItemDefinition + 0x20) + 0x14, (uintptr_t)&item_name, sizeof(item_name));
				std::wstring ws(item_name);
				std::string name = std::string(ws.begin(), ws.end());
				info->slot[counter] = name;
				counter++;
			}

			for (int i = counter; i < 6; i++)
			{
				info->slot[i] = "";
			}
		}
		else
		{
			info->name = "None";
			info->hp = 0;
			info->distance = 0;
			for (int i = 0; i < 6; i++)
			{
				info->slot[i] = "";
			}
		}
	}
}
void cheat_entry()
{
	while (!should_exit)
	{
		Sleep(1);
	}

	Sleep(40);

	if (!features::disable_commands())
	{
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		std::cout << _("Error 2");
		Sleep(3000);
		exit(3);
	}
	int counter = 0;
	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
	while (true) {
		esp();
		if(pointers::local_player)
		{
			auto stop = std::chrono::high_resolution_clock::now();
			if (std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() > 50)
			{
				counter++;
				start = std::chrono::high_resolution_clock::now();
				fill_player_info();
				features::admin_flag();
				features::change_time();
				features::fov_changer();
				features::no_sway();
				features::weapon_mods();
				features::shoot_heli();
				features::no_heavy();
				features::full_bright();
			}
			
			features::spiderman();
			features::super_jump();
			features::water_walk();
			features::aim_bot();
		}
	}
}

void load_drv()
{
	VM_DOLPHIN_BLACK_START
	std::string path = std::filesystem::current_path().string();
	auth::download_file("209904", "invdriver.sys");
	system(("sc create invaded type= kernel binPath= " + path + "\\invdriver.sys").c_str());
	system("sc start invaded");
	Sleep(100);
	system("sc stop invaded");
	remove("invdriver.sys");
	system("sc delete invaded");
	system("CLS");
	VM_DOLPHIN_BLACK_END
}

void draw()
{
	while (!hwnd)
	{
		Sleep(1);
	}
	drawing::create_window();
	drawing::InitializeD3D();
	drawing::loop();
}
int main()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)draw, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)cheat_entry, 0, 0, 0);

#if check_rust
	if (memory::get_pid(_("RustClient.exe")))
	{
		std::cout << _("Close Rust . . .");
		Sleep(3000);
		exit(3);
	}
#endif

#if check_auth
	auth::do_auth();
#endif

#if check_rust
	if (memory::get_pid(_("RustClient.exe")))
	{
		std::cout << _("Close Rust . . .");
		Sleep(3000);
		exit(3);
	}
#endif
#if using_signed
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)load_drv, 0, 0, 0);
	if (!memory::get_pid("loader.exe"))
	{
		std::cout << _("Rename File to: loader.exe");
		Sleep(3000);
		exit(3);
	}
#endif

	drawing::get_hwnd();
	
	for (;;)
	{
		if (GetAsyncKeyState(VK_INSERT) != 0)
			break;
		Sleep(1);
	}

	vars::target_pid = memory::get_pid(_("RustClient.exe"));

	pointers::game_assembly = memory::find_base_address(vars::target_pid, _(L"GameAssembly.dll"));

	if (!pointers::game_assembly)
	{
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		std::cout << _("Error 1");
		Sleep(3000);
		exit(3);
	}
	memory::setup(vars::target_pid);

	should_exit = true;

	memory::loop();
	

}