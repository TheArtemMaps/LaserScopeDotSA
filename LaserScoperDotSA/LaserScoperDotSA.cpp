#include "plugin.h"
#include "CWeapon.h"
#include "CCamera.h"
#include <game_sa/CSprite.h>
#include "CDraw.h"
#include "CTxdStore.h"
#include "CColourset.h"
#include "CWorld.h"
#include "CTimeCycle.h"
#include "MemoryMgr.h"
#include "CCoronas.h"
using namespace plugin;
RwTexture* gpLaserDotTex;
float SpriteBrightness = CTimeCycle::m_CurrentColours.m_fSpriteBrightness;
#define DEFAULT_SCREEN_WIDTH  (640.0f)
#define DEFAULT_SCREEN_HEIGHT (480.0f)
#define SCREEN_ASPECT_RATIO (CDraw::ms_fAspectRatio)
#define DEFAULT_ASPECT_RATIO (4.0f/3.0f)
#define SCREEN_SCALE_AR(a) ((a) * DEFAULT_ASPECT_RATIO / SCREEN_ASPECT_RATIO)
#define SCREEN_STRETCH_X(a)   ((a) * (float) SCREEN_WIDTH / DEFAULT_SCREEN_WIDTH)
#define SCREEN_STRETCH_Y(a)   ((a) * (float) SCREEN_HEIGHT / DEFAULT_SCREEN_HEIGHT)
#define SCREEN_SCALE_X(a) SCREEN_SCALE_AR(SCREEN_STRETCH_X(a))
#define SCREEN_SCALE_Y(a) SCREEN_STRETCH_Y(a)

#define FIX_BUGS // Undefine to play with bugs

static uint16_t GetRandomNumber(void)
{
	return rand() & RAND_MAX;
}
static bool GetRandomTrueFalse(void)
{
	return GetRandomNumber() < RAND_MAX / 2;
}
static float GetRandomNumberInRange(float low, float high)
{
	return low + (high - low) * (GetRandomNumber() / float(RAND_MAX + 1));
}

static int32_t GetRandomNumberInRange(int32_t low, int32_t high)
{
	return low + (high - low) * (GetRandomNumber() / float(RAND_MAX + 1));
}

static RwTexture* RwTextureRead(const char* name, const char* mask) {
	return plugin::CallAndReturn<RwTexture*, 0x07F3AC0>(name, mask);
}

void RegisterCorona(unsigned int id, CEntity* attachTo, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, CVector const& posn, float radius, float farClip, int coronaType, int flaretype, bool enableReflection, bool checkObstacles, int _param_not_used, float angle, bool longDistance, float nearClip, unsigned char fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay)
{
	((void(__cdecl*)(unsigned int, CEntity*, unsigned char, unsigned char, unsigned char, unsigned char, CVector const&, float, float, int, int, bool, bool, int, float, bool, float, unsigned char, float, bool, bool))0x6FC580)(id, attachTo, red, green, blue, alpha, posn, radius, farClip, coronaType, flaretype, enableReflection, checkObstacles, _param_not_used, angle, longDistance, nearClip, fadeState, fadeSpeed, onlyFromBelow, reflectionDelay);
}

struct tIniData
{
	bool bEnable[512];
	CRGBA Color[512];
	int nType[512];
	int flareType[512];
	float fRangeMul[512];
	float fRadius[512];
	float fFarClip[512];
	int32_t WeaponID[512];
	int16_t CamMode[512];
	bool bEnableDot[512];
	CRGBA DotColor[512];
	unsigned char nDotIntensity[512];
};

tIniData Sniper, AK47, Colt45, Spas12, Deagle, M4, Minigun, MP5, CuntRifle, RH, RHS, Sawedoff, Shotgun, Silenced, Tec9, Uzi;

void Init() {
	CTxdStore::PushCurrentTxd();
	int32_t slot2 = CTxdStore::AddTxdSlot("VCLaserScopeDot");
	CTxdStore::LoadTxd(slot2, PLUGIN_PATH((char*)"MODELS\\VCLASERSCOPEDOT.TXD"));
	int32_t slot = CTxdStore::FindTxdSlot("VCLaserScopeDot");
	CTxdStore::SetCurrentTxd(slot);
	gpLaserDotTex = RwTextureRead("laserdot", "laserdotm");
	CTxdStore::PopCurrentTxd();
	//Sniper rifle
	for (int32_t i = 22; i < 512; i++)
	{
		Sniper.bEnable[i] = (bool)GetPrivateProfileIntA("MAIN", "Enable", Sniper.bEnable[i], "VCLaserScopeDot.ini");
		Sniper.WeaponID[i] = GetPrivateProfileIntA("MAIN", "WeaponID", 358, "VCLaserScopeDot.ini");
		Sniper.Color[i] = CRGBA(GetPrivateProfileIntA("MAIN", "Red", 128, "VCLaserScopeDot.ini"), GetPrivateProfileIntA("MAIN", "Green", 0, "VCLaserScopeDot.ini"), GetPrivateProfileIntA("MAIN", "Blue", 0, "VCLaserScopeDot.ini"), GetPrivateProfileIntA("MAIN", "Alpha", 255, "VCLaserScopeDot.ini"));
		Sniper.nType[i] = GetPrivateProfileIntA("MAIN", "CoronaType", 0, "VCLaserScopeDot.ini");
		Sniper.flareType[i] = GetPrivateProfileIntA("MAIN", "CoronaFlareType", 0, "VCLaserScopeDot.ini");
		Sniper.fRangeMul[i] = (float)GetPrivateProfileIntA("MAIN", "RangeMultiplier", 3.0f, "VCLaserScopeDot.ini");
		Sniper.fRadius[i] = (float)GetPrivateProfileIntA("MAIN", "Radius", 1.2f, "VCLaserScopeDot.ini");
		Sniper.CamMode[i] = GetPrivateProfileIntA("MAIN", "CamMode", 7, "VCLaserScopeDot.ini");
		Sniper.fFarClip[i] = (float)GetPrivateProfileIntA("MAIN", "FarClip", 50.0f, "VCLaserScopeDot.ini");
		Sniper.bEnableDot[i] = (bool)GetPrivateProfileIntA("MAIN", "EnableDot", Sniper.bEnableDot[i], "VCLaserScopeDot.ini");
		Sniper.DotColor[i] = CRGBA(GetPrivateProfileIntA("MAIN", "DotRed", 0, "VCLaserScopeDot.ini"), GetPrivateProfileIntA("MAIN", "DotGreen", 255, "VCLaserScopeDot.ini"), GetPrivateProfileIntA("MAIN", "DotBlue", 0, "VCLaserScopeDot.ini"), GetPrivateProfileIntA("MAIN", "DotAlpha", 255, "VCLaserScopeDot.ini"));
		Sniper.nDotIntensity[i] = GetPrivateProfileIntA("MAIN", "DotIntensity", 127, "VCLaserScopeDot.ini");
	}
	//Sniper rifle
	//Sniper.WeaponID = GetPrivateProfileIntA("MAIN", "WeaponID", 351, "VCLaserScopeDot.ini");
	/*Sniper.bEnable = (bool)GetPrivateProfileIntA("MAIN", "Enable", Sniper.bEnable, "VCLaserScopeDot.ini");
	Sniper.Color = CRGBA(GetPrivateProfileIntA("MAIN", "Red", 128, "VCLaserScopeDot.ini"), GetPrivateProfileIntA("MAIN", "Green", 0, "VCLaserScopeDot.ini"), GetPrivateProfileIntA("MAIN", "Blue", 0, "VCLaserScopeDot.ini"), GetPrivateProfileIntA("MAIN", "Alpha", 255, "VCLaserScopeDot.ini"));
	Sniper.nType = GetPrivateProfileIntA("MAIN", "CoronaType", 0, "VCLaserScopeDot.ini");
	Sniper.flareType = GetPrivateProfileIntA("MAIN", "CoronaFlareType", 0, "VCLaserScopeDot.ini");
	Sniper.fRangeMul = (float)GetPrivateProfileIntA("MAIN", "RangeMultiplier", 10.0f, "VCLaserScopeDot.ini");
	Sniper.fRadius = (float)GetPrivateProfileIntA("MAIN", "Radius", 1.2f, "VCLaserScopeDot.ini");
	Sniper.CamMode = GetPrivateProfileIntA("MAIN", "CamMode", 53, "VCLaserScopeDot.ini");
	Sniper.fFarClip = (float)GetPrivateProfileIntA("MAIN", "FarClip", 500.0f, "VCLaserScopeDot.ini");
	Sniper.bEnableDot = (bool)GetPrivateProfileIntA("MAIN", "EnableDot", Sniper.bEnableDot, "VCLaserScopeDot.ini");
	Sniper.DotColor = CRGBA(GetPrivateProfileIntA("MAIN", "DotRed", 0, "VCLaserScopeDot.ini"), GetPrivateProfileIntA("MAIN", "DotGreen", 255, "VCLaserScopeDot.ini"), GetPrivateProfileIntA("MAIN", "DotBlue", 0, "VCLaserScopeDot.ini"), GetPrivateProfileIntA("MAIN", "DotAlpha", 255, "VCLaserScopeDot.ini"));
	Sniper.nDotIntensity = GetPrivateProfileIntA("MAIN", "DotIntensity", 127, "VCLaserScopeDot.ini");*/
}

void Shutdown() {
	if (gpLaserDotTex) {
		RwTextureDestroy(gpLaserDotTex);
		gpLaserDotTex = nullptr;
	}
}

class CWep : public CWeapon
{
public:
	bool LaserScopeDot(CVector* pOut, float* fDist, CRGBA const& Color, int nType, int flareType, float fRangeMul, float fRadius, float fFarClip, bool bEnable)
	{
		bool result;
		float fRange;
		CVector CamFront, Cam;
		CColPoint Point;
		RwV3d out;
		float w;
		float h;
		CEntity* pEnt;

		fRange = CWeaponInfo::GetWeaponInfo(this->m_eWeaponType, FindPlayerPed()->GetWeaponSkill())->m_fWeaponRange * fRangeMul;

		Cam.x = 0.5f * TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront.x + TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecSource.x;
		Cam.y = 0.5f * TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront.y + TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecSource.y;
		Cam.z = 0.5f * TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront.z + TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecSource.z;
		CamFront.x = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront.x;
		CamFront.y = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront.y;
		CamFront.z = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront.z;
		CamFront.Normalise();
		CamFront.x = CamFront.x * fRange;
		CamFront.y = CamFront.y * fRange;
		CamFront.z = CamFront.z * fRange;
		CamFront.x = CamFront.x + Cam.x;
		CamFront.y = CamFront.y + Cam.y;
		CamFront.z = CamFront.z + Cam.z;
		if (CWorld::ProcessLineOfSight(Cam, CamFront, Point, pEnt, true, true, true, true, true, true, true, true) && (out.x = Point.m_vecPoint.x, out.y = Point.m_vecPoint.y, out.z = Point.m_vecPoint.z, CSprite::CalcScreenCoors(reinterpret_cast<RwV3d const&>(Point.m_vecPoint), &out, &w, &h, true, true)))
		{
			pOut->x = out.x;
			pOut->y = out.y;
			pOut->z = out.z;
			*fDist = w * 0.050000001f;
			if (bEnable)
			{
					RegisterCorona(
					this->m_nState + 3,
					nullptr,
					Color.r,
					Color.g,
					Color.b,
					Color.a,
					Point.m_vecPoint,
					fRadius,
					fFarClip,
					nType,
					flareType,
					true,
					false,
					0,
					0.0f,
					false,
					1.5f,
					0,
					15.0f,
					false,
					false
					);
			}
			result = true;
		}
		else
		{
			result = false;
		}
		return result;
	}

};

void DoLaserScopeDot() {;
	SpriteBrightness = min(SpriteBrightness + 1, 30);
	CPlayerPed* player = FindPlayerPed();
	//CVector source = 0.5f * TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront + TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecSource;
	int16_t camMode;
	CVector pos;
	float size = 25.0f;
	const auto z = CDraw::ms_fFarClipZ;
	const auto rhw = 1.0f / z;
	eWeaponType wepType;
	wepType = player->m_aWeapons[player->m_nActiveWeaponSlot].m_eWeaponType;
	int weapModel = CWeaponInfo::GetWeaponInfo((eWeaponType)player->m_aWeapons[player->m_nActiveWeaponSlot].m_eWeaponType, FindPlayerPed()->GetWeaponSkill())->m_nModelId1;
	camMode = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;
		//if (camMode == MODE_SNIPER && FindPlayerPed()->m_aWeapons[FindPlayerPed()->m_nActiveWeaponSlot].LaserScopeDot(&source, &size)) {
	if (weapModel == Sniper.WeaponID[wepType] && camMode == Sniper.CamMode[wepType] && reinterpret_cast<CWep&>(FindPlayerPed()->m_aWeapons[FindPlayerPed()->m_nActiveWeaponSlot]).LaserScopeDot(&pos, &size, Sniper.Color[wepType], Sniper.nType[wepType], Sniper.flareType[wepType], Sniper.fRangeMul[wepType], Sniper.fRadius[wepType], Sniper.fFarClip[wepType], Sniper.bEnable[wepType]))
	{
			if (Sniper.bEnable[wepType])
			{
				//if (camMode == MODE_SNIPER) {
				RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
				RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
				RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
				RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
#ifdef FIX_BUGS
				RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
#else
				RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVDESTALPHA);
#endif
				RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(gpLaserDotTex));
#ifdef FIX_BUGS
				int intensity = GetRandomNumberInRange(0, 37);
#else
				int intensity = GetRandomNumberInRange(0, 35);
#endif
				CSprite::RenderOneXLUSprite(pos.x, pos.y, z,
					size, size, Sniper.DotColor[wepType].r, Sniper.DotColor[wepType].g, Sniper.DotColor[wepType].b, Sniper.DotColor[wepType].a, rhw, Sniper.nDotIntensity[wepType], 0, 0);

				RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
			}
			else {
				RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
				RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
				RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
				SpriteBrightness = 0;
			}
		}
	}

		
/*void Process(CPed* ped) {
	if (ped) {
		float size = 25.0f;
		CVector pos;
		CVector pedPos = ped->GetPosition();
		eWeaponType wepType;
		const auto z = CDraw::ms_fFarClipZ;
		const auto rhw = 1.0f / z;
		wepType = ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_eWeaponType;
		CWeaponInfo* weapModel = CWeaponInfo::GetWeaponInfo((eWeaponType)ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_eWeaponType, 1);
		if (reinterpret_cast<CWep&>(FindPlayerPed()->m_aWeapons[FindPlayerPed()->m_nActiveWeaponSlot]).LaserScopeDot(&pedPos, &size, Sniper.Color[wepType], Sniper.nType[wepType], Sniper.flareType[wepType], Sniper.fRangeMul[wepType], Sniper.fRadius[wepType], Sniper.fFarClip[wepType], Sniper.bEnable[wepType])) {
			CSprite::RenderOneXLUSprite(pedPos.x, pedPos.y, pedPos.z,
				size, size, Sniper.DotColor[wepType].r, Sniper.DotColor[wepType].g, Sniper.DotColor[wepType].b, Sniper.DotColor[wepType].a, rhw, Sniper.nDotIntensity[wepType], 0, 0);
		}
	}
}*/

	


class LaserScopeDotSA {
public:
	LaserScopeDotSA() {

		Events::initRwEvent += []() {
			Init();
		};

		Events::shutdownRwEvent += []() {
			Shutdown();
		};

		Events::reInitGameEvent += []() { // To reload ini by loading save file
			Init();
		};

		Events::drawHudEvent += []() {
			DoLaserScopeDot();
		};

		Patch<BYTE>(0x73AA0C + 1, 0); // May not be necessary?!?
		Patch<BYTE>(0x73AA06 + 1, 0);
	}
} laserScopeDotSA;