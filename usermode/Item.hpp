#pragma once
#include "common.hpp"
#include "memory.hpp"
#include "offsets.hpp"
#include "math.hpp"

struct bullet_info
{
	float velocity = 333;
	float drag = 1.f;
	float gravity = 1.f;
	float velocity_scale = 1.f;
	float velocity_scalar = 1.f;
};

struct ProjectileWeaponModModifier
{
	bool enabled;
	float scalar;
	float offset;
};

uintptr_t get_component(uintptr_t game_object, const char* name_str)
{
	if (!game_object)
		return NULL;

	uintptr_t list = memory::read<uintptr_t>(game_object + 0x30);
	for (int i = 0; i < 20; i++)
	{
		uintptr_t component = memory::read<uintptr_t>(list + (0x10 * i + 0x8));

		if (!component)
			continue;

		uintptr_t unk1 = memory::read<uintptr_t>(component + 0x28);

		if (!unk1)
			continue;

		uintptr_t name_ptr = memory::read<uintptr_t>(unk1);
		std::string name = memory::read_str(memory::read<uintptr_t>(name_ptr + 0x10), 18);
		if (!strcmp(name.c_str(), name_str))
			return unk1;
	}

	return NULL;
}

class Item
{
public:
	
	void set_shotgun_nospread()
	{
		uintptr_t base_projectile = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oHeldEnt);
		uintptr_t magazine = memory::read<uintptr_t>(base_projectile + oMagazine);
		uintptr_t item_def = memory::read<uintptr_t>(magazine + 0x20);
		int itemid = memory::read<int>(item_def + 0x18);
		if (itemid == -1036635990)
			return;
		uintptr_t unity_class = memory::read_chain(item_def, { 0x10, 0x30 });
		uintptr_t itemModProjectile = get_component(unity_class, "ItemModProjectile");
		memory::write<float>(itemModProjectile + 0x30, 0.f);
	}

	//Clean Up
	bullet_info getBulletInfo()
	{
		bullet_info info{};

		uintptr_t base_projectile = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oHeldEnt);

		info.velocity_scale = memory::read<float>(base_projectile + 0x28c);

		uintptr_t magazine = memory::read<uintptr_t>(base_projectile + oMagazine);
		uintptr_t unity_class = memory::read_chain(magazine, { 0x20, 0x10, 0x30 });
		uintptr_t itemModProjectile = get_component(unity_class, "ItemModProjectile");

		info.velocity = memory::read<float>(itemModProjectile + 0x34);

		uintptr_t projectileObject = memory::read<uintptr_t>(itemModProjectile + 0x18);

		uintptr_t unk0 = memory::read<uintptr_t>(projectileObject + 0x18);
		uintptr_t unk1 = memory::read<uintptr_t>(unk0 + 0x10);

		uintptr_t projectile = get_component(unk1, "Projectile");

		Vector2 projectile_info = memory::read<Vector2>(projectile + 0x24);

		info.drag = projectile_info.x;
		info.gravity = projectile_info.y;

		uintptr_t children_list = memory::read<uintptr_t>(base_projectile + 0x40);

		int children_list_size = memory::read<int>(children_list + 0x18);
		children_list = memory::read<uintptr_t>(children_list + 0x10);

		for (int i = 0; i < children_list_size; ++i)
		{
			uintptr_t child_entity = memory::read<uintptr_t>(children_list + (0x20 + (i * 0x8)));
			std::string child_entity_name = memory::read_str(memory::read<uintptr_t>(child_entity + 0x10));

			if (child_entity_name == "ProjectileWeaponMod")
			{
				ProjectileWeaponModModifier velocity_scalar = memory::read<ProjectileWeaponModModifier>((child_entity + 0x180));
				if (velocity_scalar.enabled)
					info.velocity_scalar = velocity_scalar.scalar;
			}
		}

		return info;
	}

	uintptr_t get_item_id()
	{
		return memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oItemID);
	}
	void set_strike_frac(float strike_frac) {
		int id = get_rust_id();
		if (id == -75944661) {
			uintptr_t base_projectile = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oHeldEnt);
			memory::write<float>(base_projectile + 0x378, strike_frac);
		}
	}
	void set_no_spread()
	{
		int id = get_rust_id();
		if (id == -75944661 || id == -1367281941 || id == -41440462 || id == 795371088 || id == -765183617)
		{
			uintptr_t base_projectile = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oHeldEnt);
			memory::write<float>(base_projectile + oSightAimConeScale, -1.f);
			memory::write<float>(base_projectile + oHipAimConeScale, -1.f);
			set_shotgun_nospread();
		}
		else if (is_valid_weapon(id))
		{
			uintptr_t base_projectile = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oHeldEnt);
			memory::write<float>(base_projectile + oSightAimConeScale, -1.f);
			memory::write<float>(base_projectile + oHipAimConeScale, -1.f);
		}
	}
	void set_no_recoil(float percent)
	{
		int id = get_rust_id();
		if (is_valid_weapon(id))
		{
			if (id != -75944661)
			{
				Vector4 orig_values = RecoilValues(id);
				uintptr_t base_projectile = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oHeldEnt);
				uintptr_t recoil_properties_old = memory::read<uintptr_t>(base_projectile + oRecoilProperties);
				uintptr_t recoil_properties_new = memory::read<uintptr_t>(recoil_properties_old + oOverride);
				if (id == -904863145 || id == 28201841 || id == -852563019 || id == 1373971859 || id == 649912614 || id == 818877484)
				{
					memory::write<float>(recoil_properties_old + oYAWMin, percent / 100 * orig_values.x);
					memory::write<float>(recoil_properties_old + oYAWMax, percent / 100 * orig_values.y);
					memory::write<float>(recoil_properties_old + oPitchMin, percent / 100 * orig_values.z);
					memory::write<float>(recoil_properties_old + oPitchMax, percent / 100 * orig_values.w);
				}
				else
				{
					memory::write<float>(recoil_properties_new + oYAWMin, percent / 100 * orig_values.x);
					memory::write<float>(recoil_properties_new + oYAWMax, percent / 100 * orig_values.y);
					memory::write<float>(recoil_properties_new + oPitchMin, percent / 100 * orig_values.z);
					memory::write<float>(recoil_properties_new + oPitchMax, percent / 100 * orig_values.w);
				}
			}
		}
	}
	void set_automatic(bool is_auto)
	{
		int id = get_rust_id();
		if (id == -904863145 || id == 28201841 || id == -852563019 || id == 1373971859 || id == 649912614 || id == 818877484)
		{
			uintptr_t base_projectile = memory::read<uintptr_t>(reinterpret_cast<uintptr_t>(this) + oHeldEnt);
			memory::write<bool>(base_projectile + oAutomatic, is_auto);
		}
	}
private:
	inline static std::vector<int> weapon_ids = { 1545779598 /*ak*/, -1335497659 /*ak ice*/, -1812555177 /*lr*/, 28201841 /*m39*/, -904863145 /*semi*/, 1796682209 /*custom*/, 1318558775 /*mp5*/, -1758372725 /*thommy*/, -75944661 /*eoka*/, -852563019 /*m92*/, 1373971859 /*python*/, 649912614 /*revolver*/, 818877484 /*sap*/, -1214542497 /*hmlmg*/, -2069578888 /*m249*/ };
	int get_rust_id()
	{
		uintptr_t a0 = memory::read<uintptr_t>((uintptr_t)this + 0x20);
		return memory::read<int>(a0 + 0x18);
	}
	bool is_valid_weapon(int item_id)
	{
		if (std::count(weapon_ids.begin(), weapon_ids.end(), item_id)) {
			return true;
		}
		else {
			return false;
		}
	}
	/*Return Weapon Recoil Values in Format Yaw Min, Yaw Max, Pitch Min, Pitch Max*/
	Vector4 RecoilValues(int itemId)
	{
		switch (itemId)
		{
		case 1545779598: /*ak*/
			return Vector4(1.5, 2.5, -2.5, -3.5);
		case -1335497659: /*ak*/
			return Vector4(1.5, 2.5, -2.5, -3.5);
		case -1812555177: /*lr*/
			return Vector4(-1, 1, -2, -3);
		case 28201841: /*m39*/
			return Vector4(-1.5, 1.5, -5, -7);
		case -904863145: /*semi*/
			return Vector4(-0.5, 0.5, -2, -3);
		case 1796682209: /*custom*/
			return Vector4(-1, 1, -1.5, -2);
		case 1318558775: /*mp5*/
			return Vector4(-1, 1, -1, -3);
		case -1758372725: /*thommy*/
			return Vector4(-1, 1, -1.5, -2);
		case -852563019: /*m92*/
			return Vector4(-1, 1, -7, -8);
		case 1373971859: /*python*/
			return Vector4(-2.0, 2.0, -15.0, -16.0);
		case 649912614: /*revolver*/
			return Vector4(-1, 1, -3, -6);
		case 818877484: /*sap*/
			return Vector4(-1.0, 1.0, -2.0, -2.5);
		case -1214542497: /*hmlmhg*/
			return Vector4(-1, -1.5, -3, -4);
		case -2069578888: /*m249*/
			return Vector4(-1, 1.0, -3.0, -3.0);
		default:
			return Vector4(0, 0, 0, 0);
		}
	}
};