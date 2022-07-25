#pragma once

#include "common.hpp"
#include "globals.hpp"
#include "math.hpp"
#include "memory.hpp"



namespace Aim
{
	float calculate_bullet_drop(float height, float distance, float velocity, float gravity)
	{
		float pitch = std::atan2(height, distance);
		float bullet_velocity = velocity * std::cos(pitch);
		float time = distance / bullet_velocity;

		return (0.5f * gravity * time * time) * 9.1;
	}

	Vector3 prediction(Vector3 AimPos, Vector3 PlayerPos)
	{
		Item* item = pointers::local_player->get_active_weapon();
		Vector3 new_target = AimPos;

		bullet_info info{};

		info = item->getBulletInfo();

		float distance = Calc3D_Dist(PlayerPos, AimPos);
		float bullet_speed = info.velocity * (info.velocity_scale * info.velocity_scalar);
		float bullet_gravity = info.gravity;
		float bullet_time = distance / bullet_speed;
		float bullet_drag = info.drag;
		
		Vector3 direction = AimPos - PlayerPos;
		float target_distance = std::sqrt((direction.x) * (direction.x) + (direction.z) * (direction.z));
		float bullet_drop = calculate_bullet_drop(direction.y, target_distance, bullet_speed, bullet_gravity);

		if (settings::aim::move_prediction)
		{
			const float time_step = 0.015625f;

			float travelled = 0;
			float speed = 0;
			float time = 0;
			float divider;

			for (float distance_to_travel = 0.f; distance_to_travel < distance;)
			{
				float speed_modifier = 1.f - time_step * bullet_drag;
				bullet_speed *= speed_modifier;

				if (bullet_speed <= 0.f || bullet_speed >= 10000.f || travelled >= 10000.f || travelled < 0.f)
					break;

				if (time > 8.f)
					break;

				speed += (9.81f * bullet_gravity) * time_step;
				speed *= speed_modifier;

				distance_to_travel += bullet_speed * time_step;
				travelled += speed * time_step;
				time += time_step;
			}
			Vector3 velocity = memory::read<Vector3>((uintptr_t)vars::AimPlayer->player_model() + 0x224) * 0.75f;

			if (velocity.y > 0.f)
				velocity.y /= 3.25;

			new_target += velocity * time;
		}


		new_target.y += bullet_drop;
		return new_target;
	}

	bool world_to_screen(const Vector3& EntityPos, Vector2& ScreenPos)
	{
		if (!pointers::view_matrix_pointer) return false;
		Vector3 TransVec = Vector3(memory::read<float>((uintptr_t)&pointers::view_matrix_pointer->_14), memory::read<float>((uintptr_t)&pointers::view_matrix_pointer->_24), memory::read<float>((uintptr_t)&pointers::view_matrix_pointer->_34));
		Vector3 RightVec = Vector3(memory::read<float>((uintptr_t)&pointers::view_matrix_pointer->_11), memory::read<float>((uintptr_t)&pointers::view_matrix_pointer->_21), memory::read<float>((uintptr_t)&pointers::view_matrix_pointer->_31));
		Vector3 UpVec = Vector3(memory::read<float>((uintptr_t)&pointers::view_matrix_pointer->_12), memory::read<float>((uintptr_t)&pointers::view_matrix_pointer->_22), memory::read<float>((uintptr_t)&pointers::view_matrix_pointer->_32));
		float w = Dot(TransVec, EntityPos) + memory::read<float>((uintptr_t)&pointers::view_matrix_pointer->_44);
		if (w < 0.098f) return false;
		float y = Dot(UpVec, EntityPos) + memory::read<float>((uintptr_t)&pointers::view_matrix_pointer->_42);
		float x = Dot(RightVec, EntityPos) + memory::read<float>((uintptr_t)&pointers::view_matrix_pointer->_41);
		ScreenPos = Vector2((misc::width / 2) * (1.f + x / w), (misc::height / 2) * (1.f - y / w));
		return true;
	}

	void fill_player_list()
	{
		vars::playerList.clear();
		uintptr_t buffer_list = memory::read_chain(pointers::game_assembly, { oBaseEntity, 0xB8, 0x10, 0x10, 0x28 });
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
				if (Player != pointers::local_player)
				{
					if (!settings::aim::target_sleeping && Player->has_flag(BasePlayer::player_flags::Sleeping))
						continue;
					if (!settings::aim::target_wounded && Player->has_flag(BasePlayer::player_flags::Wounded))
						continue;
					if (!settings::aim::target_npc && Player->player_model()->is_npc())
						continue;
					vars::playerList.push_back(Player);
				}
			}
		}
	}

	void find_target()
	{
		BasePlayer* returnPlayer = NULL;
		float min_fov = 9999;
		for (int i = 0; i < vars::playerList.size(); i++)
		{
			Vector2 ScreenPos;
			Vector3 tempPos = vars::playerList[i]->player_model()->get_position();
			Vector3 playerPos = pointers::local_player->player_model()->get_position();
			world_to_screen(tempPos, ScreenPos);
			float fov = Calc2D_Dist(Vector2(misc::width / 2, misc::height / 2), ScreenPos);
			if (fov < min_fov && fov < 500 && fov > 0)
			{
				returnPlayer = vars::playerList[i];
				min_fov = fov;
			}

		}
		vars::AimPlayer = returnPlayer;
	}

	void do_silent()
	{
		if (!vars::AimPlayer)
			return;
		Vector3 AimPos = vars::AimPlayer->get_position(settings::aim::aim_bone);
		Vector3 PlayerPos = pointers::local_player->get_position(BasePlayer::bone_list::head);
		Vector2 angles = CalcAngle(PlayerPos, prediction(AimPos, PlayerPos));
		if (angles.x == 0 || angles.y == 0)
			return;
		Vector4 quat = to_quat(Vector3(angles.x, angles.y, 0));
		pointers::local_player->set_br(quat);
	}

	void do_aim()
	{
		if (!vars::AimPlayer)
			return;
		Vector3 AimPos = vars::AimPlayer->get_position(settings::aim::aim_bone);
		Vector3 PlayerPos = pointers::local_player->get_position(BasePlayer::bone_list::head);
		Vector2 angles = CalcAngle(PlayerPos, prediction(AimPos, PlayerPos));
		if (angles.x == 0 || angles.y == 0 || isnan(angles.x) || isnan(angles.y))
			return;
		pointers::local_player->set_ba(Vector3(angles.x, angles.y, 0));
	}
}