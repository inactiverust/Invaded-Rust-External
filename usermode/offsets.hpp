#pragma once
#define iauth true

#if iauth
namespace classes
{
	uint64_t oBaseEntity = 0;  //BaseEntity_c*
	uint64_t oAdminConVar = 0; //ConVar_Admin_c*
	uint64_t oTODSky = 0; //TOD_Sky_c*
	uint64_t oGraphicConVar = 0; //ConVar_Graphics_c*
	uint64_t oOcclusionCulling = 0; //OcclusionCulling_c*
	uint64_t oConsoleSystem = 0; //ConsoleSystem_Index_c*
	uint64_t oMainCamera = 0; //MainCamera_c*
}
#else
namespace classes
{
	uint64_t oBaseEntity = 52898488;  //BaseEntity_c*
	uint64_t oAdminConVar = 52893784; //ConVar_Admin_c*
	uint64_t oTODSky = 52912736; //TOD_Sky_c*
	uint64_t oGraphicConVar = 52904424; //ConVar_Graphics_c*
	uint64_t oOcclusionCulling = 52903136; //OcclusionCulling_c*
	uint64_t oConsoleSystem = 52962984; //ConsoleSystem_Index_c*
	uint64_t oMainCamera = 52905720; //MainCamera_c*
}
#endif

#define oPlayerFlags 0x690
#define oPlayerModel 0x4d0
#define oNeedsClothesRebuild 0x530
#define oLifestate 0x224 
#define oHealth 0x22c
#define oMaxHealth 0x230 
#define oPlayerWalkMovement 0x4f8
#define oClothingAccuracyBonus 0x76c
#define oActiveItem 0x5d8
#define oInput 0x4f0
#define oInventory 0x6a0
#define oPlayerEyes 0x698
#define oModel 0x130
#define oDisplayName 0x6f0

#define oTransforms 0x48

#define oNight 0x58
#define oDay 0x50
#define oAmbientMult 0x50

#define oBodyAngle 0x3c
#define oBodyRotation 0x44

#define oGroundAngle 0xc4
#define oGroundAngleNew 0xc8
#define oJumpTime 0xd0
#define oLandTime 0xd4
#define oGroundTime 0xcc
#define oGravity 0x84
#define oGravitySwimming 0x88
#define oGrounded 0x140
#define oJumping 0x145
#define oWasJumping 0x146

#define oIsLocalPlayer 0x299
#define oIsNPC 0x320
#define oIsVisible 0x288

#define oTimeSinceAmbientUpdate 0x234
#define oSunVisibility 0xE4

#define oFOV 0x18

#define oDebugShow 0x94

#define oAllowRunFromServer 0x58

#define oBeltContainer 0x28

#define oItemID 0x28

#define oHeldEnt 0x98
#define oRecoilProperties 0x2E0
#define oEokaRecoilProperties 0x380
#define oProjectileList 0x370
#define oItemModProjectile 0xE8
#define oProjectileSpread 0x30
#define oOverride 0x78
#define oYAWMax 0x1c
#define oYAWMin 0x18
#define oPitchMax 0x24
#define oAutomatic 0x290
#define oPitchMin 0x20
#define oSightAimConeScale 0x330
#define oHipAimConeScale 0x338
#define oMagazine 0x2c0

#define oPosition 0x218

#define oModelState 0x210
#define oModelFlags 0x24

#define oCanWeildItem 0x2B8
#define oBaseMountable 0x608