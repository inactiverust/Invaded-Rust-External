#pragma once
#include "common.hpp"
#include "memory.hpp"
#include "offsets.hpp"
#include "math.hpp"

#include "PlayerModel.hpp"
#include "PlayerWalkMovement.hpp"
#include "Item.hpp"

#include <xmmintrin.h>
#include <emmintrin.h>

class BasePlayer
{
public:
	enum bone_list
	{
		l_hip = 1,
		l_knee,
		l_foot,
		l_toe,
		l_ankle_scale,
		pelvis,
		penis,
		GenitalCensor,
		GenitalCensor_LOD0,
		Inner_LOD0,
		GenitalCensor_LOD1,
		GenitalCensor_LOD2,
		r_hip,
		r_knee,
		r_foot,
		r_toe,
		r_ankle_scale,
		spine1,
		spine1_scale,
		spine2,
		spine3,
		spine4,
		l_clavicle,
		l_upperarm,
		l_forearm,
		l_hand,
		l_index1,
		l_index2,
		l_index3,
		l_little1,
		l_little2,
		l_little3,
		l_middle1,
		l_middle2,
		l_middle3,
		l_prop,
		l_ring1,
		l_ring2,
		l_ring3,
		l_thumb1,
		l_thumb2,
		l_thumb3,
		IKtarget_righthand_min,
		IKtarget_righthand_max,
		l_ulna,
		neck,
		head,
		jaw,
		eyeTranform,
		l_eye,
		l_Eyelid,
		r_eye,
		r_Eyelid,
		r_clavicle,
		r_upperarm,
		r_forearm,
		r_hand,
		r_index1,
		r_index2,
		r_index3,
		r_little1,
		r_little2,
		r_little3,
		r_middle1,
		r_middle2,
		r_middle3,
		r_prop,
		r_ring1,
		r_ring2,
		r_ring3,
		r_thumb1,
		r_thumb2,
		r_thumb3,
		IKtarget_lefthand_min,
		IKtarget_lefthand_max,
		r_ulna,
		l_breast,
		r_breast,
		BoobCensor,
		BreastCensor_LOD0,
		BreastCensor_LOD1,
		BreastCensor_LOD2,
		collision,
		displacement
	};
	enum player_flags
	{
		Unused1 = 1,
		Unused2 = 2,
		IsAdmin = 4,
		ReceivingSnapshot = 8,
		Sleeping = 16,
		Spectating = 32,
		Wounded = 64,
		IsDeveloper = 128,
		Connected = 256,
		ThirdPersonViewmode = 1024,
		EyesViewmode = 2048,
		ChatMute = 4096,
		NoSprint = 8192,
		Aiming = 16384,
		DisplaySash = 32768,
		Relaxed = 65536,
		SafeZone = 131072,
		ServerFall = 262144,
		Incapacitated = 524288,
		Workbench1 = 1048576,
		Workbench2 = 2097152,
		Workbench3 = 4194304
	};

	uintptr_t base_player() { return reinterpret_cast<uintptr_t>(this); }
	/* Classes */
	PlayerModel* player_model() { return memory::read<PlayerModel*>(reinterpret_cast<uintptr_t>(this) + oPlayerModel); }

	PlayerWalkMovement* player_movement() { return memory::read<PlayerWalkMovement*>(reinterpret_cast<uintptr_t>(this) + oPlayerWalkMovement); }

	void remove_heavy_effects()
	{
		uintptr_t inventory = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oInventory);
		uintptr_t container_wear = memory::read<uintptr_t>(inventory + 0x30); //Container Wear
		uintptr_t item_list = memory::read<uintptr_t>(container_wear + 0x38);
		int sz = memory::read<int>(item_list + 0x18);
		uintptr_t list = memory::read<uintptr_t>(item_list + 0x10);
		for (int i = 0; i < sz; i++)
		{
			uintptr_t item = memory::read<uintptr_t>(list + 0x20 + (i * 0x8));
			uintptr_t item_definition = memory::read<uintptr_t>(item + 0x20);
			uintptr_t item_mod_wearable = memory::read<uintptr_t>(item_definition + 0x100);
			memory::write<bool>(item_mod_wearable + 0x4C, false); //blocks aiming
			memory::write<bool>(item_mod_wearable + 0x54, false); //blocks equipping
			memory::write<float>(item_mod_wearable + 0x48, 64);
		}
	}

	Item* get_active_weapon()
	{
		int active_id = memory::read<int>(reinterpret_cast<uintptr_t>(this) + oActiveItem);
		uintptr_t inventory = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oInventory);
		uintptr_t container_belt = memory::read<uintptr_t>(inventory + oBeltContainer);
		uintptr_t item_list = memory::read<uintptr_t>(container_belt + 0x38);
		item_list = memory::read<uintptr_t>(item_list + 0x10);
		for (int i = 0; i < 6; i++)
		{
			uintptr_t item_class = memory::read<uintptr_t>(item_list + 0x20 + (i * 0x8));
			uintptr_t item_id = memory::read<uintptr_t>(item_class + oItemID);
			if (item_id == active_id)
			{
				return reinterpret_cast<Item*>(item_class);
			}
		}
		return 0;
	}
	/* BasePlayer Mutators */
	void set_cloth_rebuild(bool input) { memory::write<bool>(reinterpret_cast<uintptr_t>(this) + oNeedsClothesRebuild, input); }

	std::string get_name()
	{
		wchar_t player_name[36] = { '\0' };
		memory::copy_memory(memory::read<uintptr_t>((uintptr_t)this + oDisplayName) + 0x14, (uintptr_t)&player_name, sizeof(player_name));
		std::wstring ws(player_name);
		return std::string(ws.begin(), ws.end());
	}
	void set_flag(player_flags flag)
	{
		int current_flags = memory::read<int>(reinterpret_cast<uintptr_t>(this) + oPlayerFlags);
		if (!(current_flags & flag))
			memory::write<int>(reinterpret_cast<uintptr_t>(this) + oPlayerFlags, current_flags |= flag);
	}

	void unset_flag(player_flags flag)
	{
		int player_flags = memory::read<int>(reinterpret_cast<uintptr_t>(this) + oPlayerFlags);

		if (has_flag(flag))
			player_flags -= flag;

		memory::write<int>(reinterpret_cast<uintptr_t>(this) + oPlayerFlags, player_flags);
	}

	void set_accuracy(float accuracy) { memory::write<float>(reinterpret_cast<uintptr_t>(this) + oClothingAccuracyBonus, accuracy); }
	/* BasePlayer Accessors */
	float get_health() { return memory::read<float>(reinterpret_cast<uintptr_t>(this) + oHealth); }

	bool is_dead() { return memory::read<bool>(reinterpret_cast<uintptr_t>(this) + oLifestate); }

	bool has_flag(player_flags flag) 
	{
		return memory::read<int>(reinterpret_cast<uintptr_t>(this) + oPlayerFlags) & flag;
	};

	void set_ba(Vector3 input_angle)
	{
		uintptr_t input = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oInput);
		memory::write<Vector3>(input + oBodyAngle, input_angle);
	}

	void set_br(Vector4 input_quat)
	{
		uintptr_t eyes = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oPlayerEyes);
		memory::write<Vector4>(eyes + oBodyRotation, input_quat);
	}

	Vector3 get_position(bone_list target_bone)
	{
		uintptr_t model = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oModel);
		uintptr_t bone_transforms = memory::read<uintptr_t>(model + oTransforms);
		uintptr_t bone_value = memory::read<uintptr_t>(bone_transforms + 0x20 + target_bone * 0x8);
		uintptr_t bone = memory::read<uintptr_t>(bone_value + 0x10);
		return get_transform_value(bone);
	}

private:
	Vector3 get_transform_value(DWORD64 pTransform)
	{
		if (!pTransform || !(uintptr_t)this) return Vector3{ 0.f, 0.f, 0.f };

		static struct Matrix34
		{
			BYTE vec0[16];
			BYTE vec1[16];
			BYTE vec2[16];
		};

		const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
		const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
		const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

		unsigned int Index = memory::read<unsigned int>(pTransform + 0x40);

		if (Index >= 1000 || Index <= 0)
		{
			//std::cout << "invalid index: " << Index << std::endl;
			return Vector3{ 0.f, 0.f, 0.f };
		}


		DWORD64 pTransformData = memory::read<DWORD64>(pTransform + 0x38);


		DWORD64 transformData[2];

		memory::copy_memory(pTransformData + 0x18, (DWORD64)transformData, 16);

		size_t sizeMatriciesBuf = 48 * Index + 48;
		size_t sizeIndicesBuf = 4 * Index + 4;


		Matrix34* pMatriciesBuf = new Matrix34[sizeMatriciesBuf];
		int* pIndicesBuf = new int[sizeIndicesBuf];


		memory::copy_memory(transformData[0], (ULONG_PTR)pMatriciesBuf, sizeMatriciesBuf);
		memory::copy_memory(transformData[1], (ULONG_PTR)pIndicesBuf, sizeIndicesBuf);


		__m128 result = *(__m128*)((ULONG_PTR)pMatriciesBuf + 0x30 * Index);
		int transformIndex = *(int*)((ULONG_PTR)pIndicesBuf + 0x4 * Index);

		byte pSafe = 0;
		while (transformIndex >= 0 && pSafe++ < 200)
		{
			Matrix34 matrix34 = *(Matrix34*)((ULONG_PTR)pMatriciesBuf + 0x30 * transformIndex);

			__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));
			__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));
			__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));
			__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));
			__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));
			__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));
			__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

			result = _mm_add_ps(
				_mm_add_ps(
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
								_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
					_mm_add_ps(
						_mm_mul_ps(
							_mm_sub_ps(
								_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
								_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
							_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))), tmp7)), *(__m128*)(&matrix34.vec0));

			transformIndex = *(int*)((ULONG_PTR)pIndicesBuf + 0x4 * transformIndex);
		}
		delete[]pMatriciesBuf;
		delete[]pIndicesBuf;
		return Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
	}
};