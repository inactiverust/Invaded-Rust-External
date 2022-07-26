#include "common.hpp"
#include "memory.hpp"
#include "globals.hpp"
#include "features.hpp"
#include "menu.hpp"

#include "auth/skStr.h"
#include "auth/auth.hpp"

bool should_exit;

#define check_rust true;
#define check_auth true;
#define using_signed true;

bool get_local_player()
{
	uintptr_t buffer_list = memory::read_chain(pointers::game_assembly, { classes::oBaseEntity, 0xB8, 0x10, 0x10, 0x28 });
	if(!buffer_list)
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

void cheat_entry()
{
	while (!should_exit)
	{
		Sleep(1);
	}

	Sleep(40);

	features::disable_commands();

	while (true) {
		if (pointers::local_player)
		{
			features::admin_flag();
			features::change_time();
			features::spiderman();
			features::super_jump();
			features::fov_changer();
			features::esp();
			features::no_sway();
			features::weapon_mods();
			features::full_bright();
			features::aim_bot();
			features::no_heavy();
			features::water_walk();
			features::shoot_heli();
		}
		else
		{
			if (get_local_player())
				setup();
		}
	}
}

void load_drv()
{
	std::string path = std::filesystem::current_path().string();
	auth::download_file("206642", "invdriver.sys");
	system(("sc create invaded type= kernel binPath= " + path + "\\invdriver.sys").c_str());
	system("sc start invaded");
	Sleep(300);
	system("sc stop invaded");
	remove("invdriver.sys");
	system("sc delete invaded");
	system("CLS");
}

int main()
{
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
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

#if using_signed
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)load_drv, 0, 0, 0);
#endif;

	Sleep(1000);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)cheat_entry, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)menu::render, 0, 0, 0);

	for (;;) {
		if (settings::connected)
			break;
		Sleep(1);
	}

	vars::target_pid = memory::get_pid(_("RustClient.exe"));

	pointers::game_assembly = memory::find_base_address(vars::target_pid, _(L"GameAssembly.dll"));
	
	if (!pointers::game_assembly)
	{
		ShowWindow(GetConsoleWindow(), SW_SHOW);
		std::cout << _("Error 1");
		Sleep(30000);
		exit(3);
	}
	memory::setup(vars::target_pid);

	should_exit = true;

	memory::loop();
}