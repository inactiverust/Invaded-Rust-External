#pragma once

#include "common.hpp"
#include "globals.hpp"
#include "memory.hpp"
#include "offsets.hpp"
#include "Aimbot.hpp"
namespace features
{
	void no_heavy()
	{
		if (settings::noHeavy)
			pointers::local_player->remove_heavy_effects();
	}
	bool disable_commands()
	{
		int counter = 0;
		uintptr_t command_list = memory::read_chain(pointers::game_assembly, { classes::oConsoleSystem, 0xB8, 0x10 });
		int sz = memory::read<int>(command_list + 0x18);
		for (int i = 0; i < sz; i++)
		{
			uintptr_t command = memory::read<uintptr_t>(command_list + (i * 0x8));
			uintptr_t p_command_name = memory::read<uintptr_t>(command + 0x10);
			wchar_t command_name[36] = { '\0' };
			memory::copy_memory(p_command_name + 0x14, (uintptr_t)&command_name, sizeof(command_name));
			if (wcscmp(command_name, _(L"noclip")) == 0 || wcscmp(command_name, _(L"camspeed")) == 0 || wcscmp(command_name, _(L"camlerp")) == 0)
			{
				counter++;
				memory::write<bool>(command + oAllowRunFromServer, false);
				if (counter == 3)
					return true;
			}
		}
		return false;
	}
	void fov_changer()
	{
		if (settings::FOVChanger)
		{
			memory::write<float>(pointers::fov_pointer + oFOV, settings::fov);
		}
	}
	void change_time()
	{
		if (!settings::timeChanger)
			settings::time = -1;
		pointers::admin_convar_static->set_admin_time(settings::time);
	}
	void admin_flag()
	{
		if (settings::adminFlags)
		{
			pointers::local_player->set_flag(BasePlayer::player_flags::IsAdmin);
		}
	}

	void shoot_heli()
	{
		if (settings::heliShoot)
		{
			uintptr_t mounted = memory::read<uintptr_t>((uintptr_t)pointers::local_player + oBaseMountable);
			memory::write<bool>(mounted + oCanWeildItem, true);
		}
	}

	void water_walk()
	{
		if (pointers::local_player->player_model()->get_position().y < 1 && settings::waterWalk)
		{
			pointers::local_player->player_movement()->set_gravity_multiplier(0);
			pointers::local_player->player_movement()->set_gravity_multiplier(0);
			pointers::local_player->player_movement()->set_ground_angles(0);
			memory::write<bool>((uintptr_t)pointers::local_player->player_movement() + oJumping, false);
			memory::write<bool>((uintptr_t)pointers::local_player->player_movement() + oWasJumping, false);
		}
		else
		{
			pointers::local_player->player_movement()->set_gravity_multiplier(2.5);
			pointers::local_player->player_movement()->set_gravity_multiplier_swimming(0.10);
		}

	}
	void spiderman()
	{
		if (settings::Spiderman)
		{
			pointers::local_player->player_movement()->set_ground_angles(0.f);
		}
	}
	void super_jump()
	{
		if (settings::superJump)
		{
			pointers::local_player->player_movement()->set_jump_time(0.51);
			pointers::local_player->player_movement()->set_land_time(0.3);
			pointers::local_player->player_movement()->set_ground_time(99999.f);
		}
	}
	bool do_once = false;
	void full_bright()
	{
		uintptr_t ambient_parameters = memory::read<uintptr_t>((uintptr_t)pointers::tod_sky_instance + 0x90);
		if (settings::full_bright)
		{
			if (do_once)
			{
				memory::write<float>(ambient_parameters + 0x18, 0.01f);
				pointers::admin_convar_static->set_admin_time(12);
				Sleep(50);
				memory::write<float>(ambient_parameters + 0x18, 5.f);
				do_once = false;
			}
			memory::write<float>((uintptr_t)pointers::tod_sky_instance + 0x234, 0.f);
		}
		else
		{
			memory::write<float>(ambient_parameters + 0x18, 0.01f);
			do_once = true;
		}

	}
	void no_sway()
	{
		if (settings::noSway)
			pointers::local_player->set_accuracy(1.f);
	}
	void weapon_mods() {
		if (settings::noRecoil)
			pointers::local_player->get_active_weapon()->set_no_recoil(settings::recoilPercentage);
		if (settings::noSpread)
			pointers::local_player->get_active_weapon()->set_no_spread();
		if (settings::superEoka)
			pointers::local_player->get_active_weapon()->set_strike_frac(1.f);
		pointers::local_player->get_active_weapon()->set_automatic(settings::automatic);
	}

	void aim_bot()
	{	
		if (settings::aimBot)
		{
			Aim::find_target();
			if (GetAsyncKeyState(keybinds::aimkey) != 0)
			{
				if (settings::aim::silent)
					Aim::do_silent();
				else
					Aim::do_aim();
			}
		}
	}
}