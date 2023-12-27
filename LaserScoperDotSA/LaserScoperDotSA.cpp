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

void Init() {
	CTxdStore::PushCurrentTxd();
	int32_t slot2 = CTxdStore::AddTxdSlot("VCLaserScopeDot");
	CTxdStore::LoadTxd(slot2, PLUGIN_PATH((char*)"MODELS\\VCLASERSCOPEDOT.TXD"));
	int32_t slot = CTxdStore::FindTxdSlot("VCLaserScopeDot");
	CTxdStore::SetCurrentTxd(slot);
	gpLaserDotTex = RwTextureRead("laserdot", "laserdotm");
	CTxdStore::PopCurrentTxd();
}

void Shutdown() {
	if (gpLaserDotTex) {
		RwTextureDestroy(gpLaserDotTex);
		gpLaserDotTex = nullptr;
	}
}
			
void DoLaserScopeDot() {
	SpriteBrightness = min(SpriteBrightness + 1, 30);
	float size = 25.0f;
	CVector source = 0.5f * TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront + TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecSource;
	int16_t camMode;
	camMode = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;
	if (camMode == MODE_SNIPER && FindPlayerPed()->m_aWeapons[FindPlayerPed()->m_nActiveWeaponSlot].LaserScopeDot(&source, &size)) {
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
		CSprite::RenderOneXLUSprite(source.x, source.y, source.z,
			SCREEN_SCALE_X(size), SCREEN_SCALE_Y(size), intensity - 36, 0, 0, 127, 1.0f, intensity - 36, 0, 0);

		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)FALSE);
	}
	else {
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
		RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
		SpriteBrightness = 0;
	}
}


class LaserScopeDotSA {
public:
	LaserScopeDotSA() {

		Events::initRwEvent += []() {
			Init();
		};

		Events::shutdownRwEvent += []() {
			Shutdown();
		};

		Events::drawHudEvent += []() {
			DoLaserScopeDot();
		};
		Patch<BYTE>(0x73AA0C + 1, 0); // May not necessary?!?
		Patch<BYTE>(0x73AA06 + 1, 0);
	}
} laserScopeDotSA;

