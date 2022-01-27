#include "sdk.h"
#include "ESP.h"
#include "global.h"
#include "xor.h"
#include "Math.h"
#include "Menu.h"
#include "Misc.h"
#include "BacktrackingHelper.h"
#include "GameUtils.h"
#include "Draw.h"
#include "AW_hitmarker.h"
#include <cmath>
#include <d3dx9.h>
#include <amp.h>

CESP* g_ESP = new CESP;
#define TICK_INTERVAL			( g_pGlobals->interval_per_tick )
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )
char* GetWeaponName(CBaseCombatWeapon* pWeapon)
{
	int ID = pWeapon->WeaponID();

	switch (ID) {
	case 4:
		return "glock";
	case 2:
		return "duals";
	case 36:
		return "p250";
	case 30:
		return "tec9";
	case 1:
		return "deagle";
	case 32:
		return "p2000";
	case 3:
		return "fiveseven";
	case 64:
		return "revolver";
	case 63:
		return "cz75";
	case 61:
		return "usp";
	case 35:
		return "nova";
	case 25:
		return "xm1014";
	case 29:
		return "sawed off";
	case 14:
		return "m249";
	case 28:
		return "negev";
	case 27:
		return "mag7";
	case 17:
		return "mac10";
	case 33:
		return "mp7";
	case 24:
		return "ump45";
	case 19:
		return "p90";
	case 26:
		return "�bizon";
	case 34:
		return "mp9";
	case 10:
		return "famas";
	case 16:
		return "m4a4";
	case 40:
		return "ssg08";
	case 8:
		return "aug";
	case 9:
		return "awp";
	case 38:
		return "scar20";
	case 13:
		return "galil";
	case 7:
		return "ak47";
	case 39:
		return "sg553";
	case 11:
		return "g3sg1";
	case 60:
		return "m4a1s";
	case 46:
	case 48:
		return "molotov";
	case 44:
		return "grenade";
	case 43:
		return "flashbang";
	case 45:
		return "smoke";
	case 47:
		return "decoy";
	case 31:
		return "taser";
	default:
		return "knife";
	}
}



template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}

bool GetCBaseEntityBox(CBaseEntity* pBaseEntity, Vector& BotCenter, Vector& TopCenter, float& Left, float& Right, bool is_player)
{
	if (!pBaseEntity)
		return false;

	if (!is_player)
	{
		const VMatrix& trans = pBaseEntity->GetCollisionBoundTrans();

		CollisionProperty* collision = pBaseEntity->GetCollision();

		if (!collision)
			return false;

		Vector min, max;

		//pBaseEntity->GetRenderable()->GetRenderBounds(min, max);
		min = collision->VecMins();
		max = collision->VecMaxs();

		Vector points[] = { Vector(min.x, min.y, min.z),
			Vector(min.x, max.y, min.z),
			Vector(max.x, max.y, min.z),
			Vector(max.x, min.y, min.z),
			Vector(max.x, max.y, max.z),
			Vector(min.x, max.y, max.z),
			Vector(min.x, min.y, max.z),
			Vector(max.x, min.y, max.z) };

		auto vector_transform = [](const Vector in1, const VMatrix& in2)
		{
			auto dot_product = [](const Vector &v1, const float *v2)
			{
				float ret = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
				return ret;
			};

			auto out = Vector();
			out[0] = dot_product(in1, in2[0]) + in2[0][3];
			out[1] = dot_product(in1, in2[1]) + in2[1][3];
			out[2] = dot_product(in1, in2[2]) + in2[2][3];
			return out;
		};

		Vector pointsTransformed[8];
		for (int i = 0; i < 8; i++)
		{
			pointsTransformed[i] = vector_transform(points[i], trans);
		}

		Vector pos = pBaseEntity->GetAbsOrigin();
		Vector flb;
		Vector brt;
		Vector blb;
		Vector frt;
		Vector frb;
		Vector brb;
		Vector blt;
		Vector flt;


		if (!GameUtils::WorldToScreen(pointsTransformed[3], flb) || !GameUtils::WorldToScreen(pointsTransformed[5], brt)
			|| !GameUtils::WorldToScreen(pointsTransformed[0], blb) || !GameUtils::WorldToScreen(pointsTransformed[4], frt)
			|| !GameUtils::WorldToScreen(pointsTransformed[2], frb) || !GameUtils::WorldToScreen(pointsTransformed[1], brb)
			|| !GameUtils::WorldToScreen(pointsTransformed[6], blt) || !GameUtils::WorldToScreen(pointsTransformed[7], flt))
			return false;

		Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
		//+1 for each cuz of borders at the original box
		float left = flb.x;        // left
		float top = flb.y;        // top
		float right = flb.x;    // right
		float bottom = flb.y;    // bottom

		for (int i = 1; i < 8; i++)
		{
			if (left > arr[i].x)
				left = arr[i].x;
			if (top < arr[i].y)
				top = arr[i].y;
			if (right < arr[i].x)
				right = arr[i].x;
			if (bottom > arr[i].y)
				bottom = arr[i].y;
		}
		BotCenter = Vector(right - ((right - left) / 2), top, 0);
		TopCenter = Vector(right - ((right - left) / 2), bottom, 0);
		Left = left;
		Right = right;
	}
	else
	{
		Vector org = pBaseEntity->GetAbsOrigin();
		Vector head;
		if ((*pBaseEntity->GetFlags() & FL_DUCKING))
			head = org + Vector(0.f, 0.f, 52.f);
		else
			head = org + Vector(0.f, 0.f, 72.f);

		Vector org_screen, head_screen;
		if (!GameUtils::WorldToScreen(org, org_screen) || !GameUtils::WorldToScreen(head, head_screen))
			return false;

		int height = int(org_screen.y - head_screen.y);
		int width = int(height / 2);
		Left = int(head_screen.x - width / 2);
		Right = int(head_screen.x + width / 2);
		BotCenter = Vector(head_screen.x, org_screen.y);
		TopCenter = BotCenter; TopCenter.y = head_screen.y;
	}
	return true;
}

void CESP::DrawBones(CBaseEntity* pBaseEntity, int r, int g, int b, int a)
{
	float Red = Clientvariables->Colors.Skeletons[0] * 255;
	float Green = Clientvariables->Colors.Skeletons[1] * 255;
	float Blue = Clientvariables->Colors.Skeletons[2] * 255;


	int index = pBaseEntity->Index();
	studiohdr_t* pStudioModel = g_pModelInfo->GetStudioModel((model_t*)pBaseEntity->GetModel());
	if (pStudioModel && g_BacktrackHelper->PlayerRecord[index].records.size() > 0)
	{

		if (g_BacktrackHelper->PlayerRecord[index].records.size() > 0)
		{
			tick_record record = g_BacktrackHelper->PlayerRecord[index].records.at(0);

			if (g_BacktrackHelper->IsTickValid(record))
			{

				for (int i = 0; i < pStudioModel->numbones; i++)
				{
					mstudiobone_t* pBone = pStudioModel->pBone(i);
					if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
						continue;

					Vector sBonePos1 = GameUtils::GetBonePosition(pBaseEntity, i, record.boneMatrix), vBonePos1; //.back() for nice skeleton
					if (sBonePos1 == Vector(0, 0, 0))
						continue;
					if (!GameUtils::WorldToScreen(sBonePos1, vBonePos1))
						continue;

					Vector sBonePos2 = GameUtils::GetBonePosition(pBaseEntity, pBone->parent, record.boneMatrix), vBonePos2;//.back() for nice skeleton
					if (sBonePos2 == Vector(0, 0, 0))
						continue;
					if (!GameUtils::WorldToScreen(sBonePos2, vBonePos2))
						continue;

					g_Draw.LineRGBA((int)vBonePos1.x, (int)vBonePos1.y, (int)vBonePos2.x, (int)vBonePos2.y, 255, 255, 255, 255);

				}
			}
		}
	}
}
RECT GetViewport()
{
	RECT Viewport = { 0, 0, 0, 0 };
	int w, h;
	g_pEngine->GetScreenSize(w, h);
	Viewport.right = w; Viewport.bottom = h;
	return Viewport;
}

void CESP::WalkbotPoints() {

	if (Clientvariables->Misc.Walkbot)
	{

		if (G::wbpoints == -1)
			return;

		Vector pos;
		Vector draw;

		for (int i = 0; i < G::walkpoints.size(); i++)
		{
			pos = G::walkpoints[i];
			pos.z += 5.f;

			if (!GameUtils::WorldToScreen(pos, draw))
				continue;

			Color clr = Color(255, 255, 255, 50);

			if (i == G::wbcurpoint)
				clr = Color(0, 255, 0, 50);

			g_Draw.StringA(g_Draw.font_esp, true, draw.x + 5, draw.y + 15, 255, 255, 255, 255, "%d", i);
		}
	}
}


void TexturedPolygon(int n, std::vector<Vertex_t> vertice, Color color)
{
	static int texture_id = g_pSurface->CreateNewTextureID(true); // 
	static unsigned char buf[4] = { 255, 255, 255, 255 };
	g_pSurface->DrawSetTextureRGBA(texture_id, buf, 1, 1); //
	g_pSurface->SetDrawColor(color); //
	g_pSurface->DrawSetTexture(texture_id); //
	g_pSurface->DrawTexturedPolygon(n, vertice.data()); //
}
void DrawTexturedPoly(int n, Vertex_t* vertice, Color col)
{
	static int texture_id = g_pSurface->CreateNewTextureID(true);
	static unsigned char buf[4] = { 255, 255, 255, 255 };
	g_pSurface->DrawSetTextureRGBA(texture_id, buf, 1, 1);
	g_pSurface->SetDrawColor(col);
	g_pSurface->DrawSetTexture(texture_id);
	g_pSurface->DrawTexturedPolygon(n, vertice);
}

void DrawFilledCircle(Vector2D center, Color color, float radius, float points)
{
	std::vector<Vertex_t> vertices;
	float step = (float)M_PI * 2.0f / points;

	for (float a = 0; a < (M_PI * 2.0f); a += step)
		vertices.push_back(Vertex_t(Vector2D(radius * cosf(a) + center.x, radius * sinf(a) + center.y)));

	DrawTexturedPoly((int)points, vertices.data(), color);
}

enum circle_type {
	full = 1, half, quarter
};
struct CUSTOMVERTEX
{
	float x, y, z, rhw;
	DWORD color;
};
static LPDIRECT3DVERTEXBUFFER9 g_pVB2;
#define D3DX_PI2 3.1415926535897932384626

void CircleFilledDualColor(float x, float y, float rad, float rotate, int type, int resolution, DWORD color, DWORD color2, IDirect3DDevice9* m_device)
{

	std::vector<CUSTOMVERTEX> circle(resolution + 2);

	float angle = rotate * D3DX_PI2 / 180, pi = D3DX_PI2;

	if (type == full)
		pi = D3DX_PI2; // Full circle 
	if (type == half)
		pi = D3DX_PI2 / 2; // 1/2 circle 
	if (type == quarter)
		pi = D3DX_PI2 / 4; // 1/4 circle 

	pi = D3DX_PI2 / type; // 1/4 circle 

	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0;
	circle[0].rhw = 1;
	circle[0].color = color2;

	for (int i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - rad * cos(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - rad * sin(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0;
		circle[i].rhw = 1;
		circle[i].color = color;
	}

	// Rotate matrix 
	int _res = resolution + 2;
	for (int i = 0; i < _res; i++)
	{
		circle[i].x = x + cos(angle)*(circle[i].x - x) - sin(angle)*(circle[i].y - y);
		circle[i].y = y + sin(angle)*(circle[i].x - x) + cos(angle)*(circle[i].y - y);
	}

	m_device->CreateVertexBuffer((resolution + 2) * sizeof(CUSTOMVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, NULL);

	VOID* pVertices;
	g_pVB2->Lock(0, (resolution + 2) * sizeof(CUSTOMVERTEX), (void**)&pVertices, 0);
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(CUSTOMVERTEX));
	g_pVB2->Unlock();

	m_device->SetTexture(0, NULL);
	m_device->SetPixelShader(NULL);
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_device->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX));
	m_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	m_device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);
	if (g_pVB2 != NULL)
		g_pVB2->Release();
}


void CESP::Crosshair(IDirect3DDevice9* m_device)
{
	int width = 0;
	int height = 0;

	g_pEngine->GetScreenSize(width, height);


	if (Clientvariables->Visuals.SpreadCrosshair)
	{
		if (G::LocalPlayer->GetHealth() > 0 && G::LocalPlayer)
		{
			int r, g, b, a;
			r = Clientvariables->Colors.SpreadCrosshair[0] * 255;
			g = Clientvariables->Colors.SpreadCrosshair[1] * 255;
			b = Clientvariables->Colors.SpreadCrosshair[2] * 255;
			a = Clientvariables->Colors.SpreadCrosshair[3] * 255;

			int x = width / 2;
			int y = height / 2;

			auto cone = G::MainWeapon->GetSpread() + G::MainWeapon->GetInaccuracy() * 550.f;

			if (G::MainWeapon->IsGun())
				CircleFilledDualColor(x, y, cone, 10, full, 100, D3DCOLOR_ARGB(a, r, g, b), D3DCOLOR_ARGB(0, r, g, b), m_device);
		}
	}
}

bool IsOnScreen(Vector origin, Vector& screen)
{
	if (!GameUtils::WorldToScreen(origin, screen)) return false;
	int iScreenWidth, iScreenHeight;
	g_pEngine->GetScreenSize(iScreenWidth, iScreenHeight);
	bool xOk = iScreenWidth > screen.x > 0, yOk = iScreenHeight > screen.y > 0;
	return xOk && yOk;
}

mstudiobbox_t* get_hitbox(CBaseEntity* entity, int hitbox_index)
{
	if (entity->IsDormant() || entity->GetHealth() <= 0)
		return NULL;

	const auto pModel = entity->GetModel();
	if (!pModel)
		return NULL;

	auto pStudioHdr = g_pModelInfo->GetStudioModel(pModel);
	if (!pStudioHdr)
		return NULL;

	auto pSet = pStudioHdr->pHitboxSet(0);
	if (!pSet)
		return NULL;

	if (hitbox_index >= pSet->numhitboxes || hitbox_index < 0)
		return NULL;

	return pSet->pHitbox(hitbox_index);
}

void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
{
	Math::VectorTransform2(&in1.x, in2, &out.x);
}

Vector get_hitbox_pos(CBaseEntity* entity, int hitbox_id)
{
	auto hitbox = get_hitbox(entity, hitbox_id);
	if (!hitbox)
		return Vector(0, 0, 0);

	auto bone_matrix = entity->GetBoneMatrix(hitbox->bone);

	Vector bbmin, bbmax;
	VectorTransform(hitbox->bbmin, bone_matrix, bbmin);
	VectorTransform(hitbox->bbmax, bone_matrix, bbmax);

	return (bbmin + bbmax) * 0.5f;
}

void CESP::DrawBarrel(CBaseEntity* entity)
{
	auto local_player = G::LocalPlayer;

	if (!local_player)
		return;

	if (entity->IsDormant())
		return;
	if (!local_player->isAlive())
		return;

	if (!Clientvariables->Visuals.Radar)
		return;

	Vector screenPos;
	if (IsOnScreen(get_hitbox_pos(entity, (int)CSGOHitboxID::PELVIS), screenPos)) return;

	auto client_viewangles = Vector();
	auto screen_width = 0, screen_height = 0;

	g_pEngine->GetViewAngles(client_viewangles);
	g_pEngine->GetScreenSize(screen_width, screen_height);

	auto radius = 350;
	Vector local_position = local_player->GetOrigin() + local_player->GetViewOffset();

	const auto screen_center = Vector(screen_width / 2.f, screen_height / 2.f, 0);
	const auto rot = DEG2RAD(client_viewangles.y - GameUtils::CalculateAngle(local_position, get_hitbox_pos(entity, (int)CSGOHitboxID::PELVIS)).y - 90);


	std::vector<Vertex_t> vertices;
	vertices.push_back(Vertex_t(Vector2D(screen_center.x + cosf(rot) * radius, screen_center.y + sinf(rot) * radius)));
	vertices.push_back(Vertex_t(Vector2D(screen_center.x + cosf(rot + DEG2RAD(2)) * (radius - 35), screen_center.y + sinf(rot + DEG2RAD(2)) * (radius - 35))));
	vertices.push_back(Vertex_t(Vector2D(screen_center.x + cosf(rot - DEG2RAD(2)) * (radius - 35), screen_center.y + sinf(rot - DEG2RAD(2)) * (radius - 35))));

	static float rainbow;
	rainbow += 0.0005f;
	if (rainbow > 1.f) rainbow = 0.f;
	Color col;

	col = Color::FromHSB(rainbow, 1.f, 1.f);
	col.SetAlpha(flPlayerAlpha[entity->GetIndex()] - 55);
	TexturedPolygon(3, vertices, col);
}

void DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...)
{


	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int width, height;
	g_pSurface->GetTextSize(font, wbuf, width, height);

	if (alignment & 1)
		x -= width;
	if (alignment & 2)
		x -= width / 2;

	g_pSurface->DrawSetTextFont(font);
	g_pSurface->DrawSetTextColor(r, g, b, a);
	g_pSurface->DrawSetTextPos(x, y - height / 2);
	g_pSurface->DrawPrintText(wbuf, wcslen(wbuf));
}

RECT GetTextSize1(unsigned long font, const char* text)
{
	size_t origsize = strlen(text) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);

	RECT rect; int x, y;
	g_pSurface->GetTextSize(font, wcstring, x, y);
	rect.left = x; rect.bottom = y;
	rect.right = x;
	return rect;
}


void CESP::SpecList()
{

}



void CESP::DrawPlayer(CBaseEntity* pPlayer, CBaseEntity* pLocalPlayer)
{
	Vector m_position = pPlayer->GetAbsOrigin();
	Vector m_top_position = m_position;

	if (*pPlayer->GetFlags() & IN_DUCK)
		m_top_position += Vector(0, 0, 52);
	else
		m_top_position += Vector(0, 0, 72);

	Vector footpos, headpos;

	if (pPlayer->IsDormant() && flPlayerAlpha[pPlayer->GetIndex()] != 0.f)
		if (flPlayerAlpha[pPlayer->GetIndex()] < 0.f)
		{


			flPlayerAlpha[pPlayer->GetIndex()] = 0.f;
		}
		else
			flPlayerAlpha[pPlayer->GetIndex()] -= 0.5f;
	else if (flPlayerAlpha[pPlayer->GetIndex()] != 255.f && !pPlayer->IsDormant())
		flPlayerAlpha[pPlayer->GetIndex()] = 255.f;
	float alpha = flPlayerAlpha[pPlayer->GetIndex()];
	clamp(alpha, 0, 255);
	if (pPlayer != G::LocalPlayer && !pPlayer->IsDormant() && Clientvariables->Visuals.SnapLines) // Crosshair to abs origin
	{
		RECT View = GetViewport();
		int MidX = View.right / 1.9999999999999;
		int MidY = View.bottom / 2;

		Vector VecOrigin = pPlayer->GetAbsOrigin(), SecondVector;

		if (GameUtils::WorldToScreen(VecOrigin, SecondVector))
		{
			g_Draw.LineRGBA(MidX, MidY, SecondVector.x, SecondVector.y, 255, 255, 255, 255);
		}
	}

	if (GameUtils::WorldToScreen(m_position, footpos) && GameUtils::WorldToScreen(m_top_position, headpos))
	{
		Vector Bot, Top;
		float Left, Right;
		GetCBaseEntityBox(pPlayer, Bot, Top, Left, Right, true);
		int height = Bot.y - Top.y;
		int width = Right - Left;
		int x = Left;
		int y = Top.y;

		CBaseCombatWeapon* pMainWeapon = pPlayer->GetWeapon();

		if (Clientvariables->Visuals.Bones)
			DrawBones(pPlayer, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()]);

		if (Clientvariables->Visuals.BoundingBox)
		{

			Vector min, max;
			Vector pos, pos3D, top, top3D;
			pos3D = pPlayer->GetAbsOrigin() - Vector(0, 0, 10);
			top3D = pos3D + Vector(0, 0, max.z + 10);
			float Red = Clientvariables->Colors.BoundingBox[0] * 255;
			float Green = Clientvariables->Colors.BoundingBox[1] * 255;
			float Blue = Clientvariables->Colors.BoundingBox[2] * 255;

			if (pPlayer->IsDormant())
			{
				g_Draw.Box(x - 1, y - 1, width + 2, height + 2, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.Box(x, y, width, height, 140, 140, 140, flPlayerAlpha[pPlayer->GetIndex()]); // Main
				g_Draw.Box(x + 1, y + 1, width - 2, height - 2, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
			}
			else if (!pPlayer->IsDormant())
			{
				g_Draw.Box(x - 1, y - 1, width + 2, height + 2, 0, 21, 0, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.Box(x, y, width, height, Red, Green, Blue, flPlayerAlpha[pPlayer->GetIndex()]); // Main
				g_Draw.Box(x + 1, y + 1, width - 2, height - 2, 21, 21, 21, flPlayerAlpha[pPlayer->GetIndex()]);
			}
			else
			{

				g_Draw.Box(x - 1, y - 1, width + 2, height + 2, 0, 21, 0, 255);
				g_Draw.Box(x, y, width, height, Red, Green, Blue, 255); // Main
				g_Draw.Box(x + 1, y + 1, width - 2, height - 2, 21, 21, 21, 255);
			}

		}




		if (Clientvariables->Visuals.Health)
		{
			int health = 100 - pPlayer->GetHealth();
			int w = 4;
			if (width < 4)
				w = width;

			int hr, hg, hb;

			hr = 255 - (pPlayer->GetHealth()*2.55);
			hg = pPlayer->GetHealth() * 2.55;
			hb = 0;

			if (pPlayer->IsDormant())
			{
				g_Draw.FillRGBA(x - (5), y, w, height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.FillRGBA(x - (5), y + health * height / 100, w, pPlayer->GetHealth()*height / 100, 140, 140, 140, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.Box(x - (5), y, w, height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
			}
			else if (!pPlayer->IsDormant())
			{
				g_Draw.FillRGBA(x - (5), y, w, height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.FillRGBA(x - (5), y + health * height / 100, w, pPlayer->GetHealth()*height / 100, 0, 253, 0, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.Box(x - (5), y, w, height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
			}
			else
			{
				g_Draw.FillRGBA(x - (5), y, w, height, 0, 0, 0, 255);
				g_Draw.FillRGBA(x - (5), y + health * height / 100, w, pPlayer->GetHealth()*height / 100, 0, 253, 0, 255);
				g_Draw.Box(x - (5), y, w, height, 0, 0, 0, 255);
			}

			if (pPlayer->GetHealth() <= 100)
			{
				char hp[20];
				sprintf_s(hp, sizeof(hp), "%i", pPlayer->GetHealth());

				g_Draw.StringA(g_Draw.font_esp, false, x - g_Draw.getWidht(hp, g_Draw.font_espnum), y + health * height / 100 - 6, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()], "%i", pPlayer->GetHealth());
			}



			/* int health = 100 - pPlayer->GetHealth();
			int w = 4;
			if (width < 4)
			w = width;

			int hr, hg, hb;

			hr = 255 - (pPlayer->GetHealth()*2.55);
			hg = pPlayer->GetHealth() * 2.55;
			hb = 0;

			if (pPlayer->IsDormant())
			{
			g_Draw.Box(x - (5), y, w, height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
			g_Draw.FillRGBA(x - (5), y, w, height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
			g_Draw.FillRGBA(x - (5), y + health * height / 100, w, pPlayer->GetHealth()*height / 100, 140, 140, 140, flPlayerAlpha[pPlayer->GetIndex()]);
			}
			else if (!pPlayer->IsDormant())

			{	g_Draw.Box(x - (5), y, w, height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
			g_Draw.FillRGBA(x - (5), y, w, height, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
			g_Draw.FillRGBA(x - (5), y + health * height / 100, w, pPlayer->GetHealth()*height / 100, 0, 253, 0, flPlayerAlpha[pPlayer->GetIndex()]);

			}
			else
			{
			g_Draw.Box(x - (5), y, w, height, 0, 0, 0, 255);
			g_Draw.FillRGBA(x - (5), y, w, height, 0, 0, 0, 255);
			g_Draw.FillRGBA(x - (5), y + health * height / 100, w, pPlayer->GetHealth()*height / 100, 0, 253, 0, 255);
			} */

			/*	if (pPlayer->GetHealth() <= 100)
			{
			char hp[20];
			sprintf_s(hp, sizeof(hp), "%i", pPlayer->GetHealth());

			g_Draw.StringA(g_Draw.font_esp, false, x - g_Draw.getWidht(hp, g_Draw.font_espnum), y + health * height / 100 - 6, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()], "", pPlayer->GetHealth());
			}*/
		}

		int w1 = 4;
		if (width < 4)
			w1 = width;
			static float next_lby_update[65];
			static float last_lby[65];
			if (pPlayer->GetVelocity().Length2D() > 28)
				next_lby_update[pPlayer->GetIndex()] = pPlayer->GetSimulationTime() + 0.22f;
			else
			{
				if (next_lby_update[pPlayer->GetIndex()] <= pPlayer->GetSimulationTime())
					next_lby_update[pPlayer->GetIndex()] = pPlayer->GetSimulationTime() + 1.1f;

			}

			last_lby[pPlayer->GetIndex()] = pPlayer->LowerBodyYaw();

			if (pPlayer->GetVelocity().Length2D() < 28)
			{
				int da = flPlayerAlpha[pPlayer->GetIndex()];
				float _y = y + height + 2;
				float boxHeight = 3;
				float time_remain_to_update = next_lby_update[pPlayer->GetIndex()] - pPlayer->GetSimulationTime();
				float ammoBarWidth = width - (time_remain_to_update * (width / 1.1f));
				//
				//

				g_Draw.Box(x, _y, width, boxHeight, 0, 0, 0, flPlayerAlpha[pPlayer->GetIndex()]);
				g_Draw.FillRGBA(x + 1, y + height + 2 + 1, ammoBarWidth - 2, boxHeight - 2, 45, 83, 188, flPlayerAlpha[pPlayer->GetIndex()]);

			}
		
		int bot_add = 0;
		int top_add = 0;

		if (Clientvariables->Visuals.Name)
		{
			player_info_s info;
			g_pEngine->GetPlayerInfo(pPlayer->Index(), &info);
			g_Draw.StringA(g_Draw.font_esp, true, x + width / 2, y - 12 - top_add, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()], "%s", info.m_szPlayerName);
			top_add += 12;
		}
		
			if (pPlayer->IsScoped())
				g_Draw.StringA(g_Draw.font_esp, false, x + width + 3, y + 16, 45, 83, 188, flPlayerAlpha[pPlayer->GetIndex()], "ZOOM");


			if (abs(pPlayer->GetEyeAnglesPtr()->y - pPlayer->LowerBodyYaw()) > 35.f)
				g_Draw.StringA(g_Draw.font_esp, false, x + width + 3, y, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()], "FAKE");

			if (pPlayer->GetArmor() > 0)
			{
				char hp[50];
				sprintf_s(hp, sizeof(hp), "%i", pPlayer->GetArmor());
				if (pPlayer->HasHelmet())
					g_Draw.StringA(g_Draw.font_esp, false, x + width + 3, y + 8, 62, 242, 65, flPlayerAlpha[pPlayer->GetIndex()], "HK");
				else
					g_Draw.StringA(g_Draw.font_esp, false, x + width + 3, y + 8, 62, 242, 65, flPlayerAlpha[pPlayer->GetIndex()], "K");
			}

	
		if (Clientvariables->Visuals.Weapon) {
			UINT* hWeapons = pPlayer->GetWeapons();
			if (hWeapons) {
				if (pMainWeapon) {
					std::string s1 = pMainWeapon->GetGunIcon();
					std::string ammo;
					if (Clientvariables->Visuals.Ammo && pMainWeapon->Clip1() != -1 && pMainWeapon->GetWeaponType() != WEAPONCLASS::WEPCLASS_INVALID) {
						ammo.append(XorStr("("));
						ammo.append(to_string(pMainWeapon->Clip1()));
						ammo.append(XorStr("/"));
						ammo.append(to_string(pMainWeapon->GetMaxAmmoReserve()));
						ammo.append(XorStr(")"));
					}
					g_Draw.StringA(g_Draw.icon_esp, true, x + width / 2, y + height + bot_add + 5, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()], XorStr("%s"), s1.c_str());
					g_Draw.StringA(g_Draw.font_esp, true, x + width / 2, y + height + bot_add + 15, 255, 255, 255, flPlayerAlpha[pPlayer->GetIndex()], XorStr("%s"), ammo.c_str());
				}
			}
		}
	}
}

void CESP::DrawC4(CBaseEntity* pBomb, bool is_planted, CBaseEntity* pLocalPlayer)
{
	Vector Bot, Top;
	float Left, Right;
	GetCBaseEntityBox(pBomb, Bot, Top, Left, Right, false);
	int height = Bot.y - Top.y;
	int width = Right - Left;

	DWORD col = is_planted ? D3DCOLOR_RGBA(250, 42, 42, 255) : D3DCOLOR_RGBA(255, 255, 0, 255);

	g_Draw.StringA(g_Draw.font_espinfo, true, Left + width / 2, Bot.y, 255, 255, 255, 255, "%s", "Bomb");

	if (is_planted)
	{
		CBomb* bomb = (CBomb*)pBomb;
		float flBlow = bomb->GetC4BlowTime();
		float TimeRemaining = flBlow - (g_pGlobals->interval_per_tick * pLocalPlayer->GetTickBase());
		if (TimeRemaining < 0)
			TimeRemaining = 0;
		char buffer[64];

		if (bomb->IsDefused())
			sprintf_s(buffer, XorStr("Defused"));
		else
			sprintf_s(buffer, XorStr("%.1fs remaining!"), TimeRemaining);


		//g_pRender->String(Left + width / 2, Bot.y + 13, centered, g_pRender->Fonts.esp, true, WHITE(255), "%s", buffer);
		g_Draw.StringA(g_Draw.font_esp, true, Left + width / 2, Bot.y + 13, 255, 255, 255, 255, "%s", buffer);

		if (!bomb->IsDefused())
		{
			float a = 450.7f;
			float b = 75.68f;
			float c = 789.2f;
			float d = ((pLocalPlayer->GetAbsOrigin().DistTo(pBomb->GetOrigin()) - b) / c);
			float flDamage = a * expf(-d * d);

			auto GetArmourHealth = [](float flDamage, int ArmorValue)
			{
				float flCurDamage = flDamage;

				if (flCurDamage == 0.0f || ArmorValue == 0)
					return flCurDamage;

				float flArmorRatio = 0.5f;
				float flArmorBonus = 0.5f;
				float flNew = flCurDamage * flArmorRatio;
				float flArmor = (flCurDamage - flNew) * flArmorBonus;

				if (flArmor > ArmorValue)
				{
					flArmor = ArmorValue * (1.0f / flArmorBonus);
					flNew = flCurDamage - flArmor;
				}

				return flNew;
			};

			float damage = max((int)ceilf(GetArmourHealth(flDamage, pLocalPlayer->GetArmor())), 0);


			//g_pRender->String(Left + width / 2, Bot.y + 26, centered, g_pRender->Fonts.esp, true, WHITE(255), "Damage: %f", damage);
			g_Draw.StringA(g_Draw.font_espinfo, true, Left + width / 2, Bot.y + 26, 255, 255, 255, 255, "Damage: %f", damage);
		}
	}
}

void CESP::set_hitmarker_time(float time)
{
	G::flHurtTime = time;
}

void CESP::DrawHitmarker()
{
	auto local_player = G::LocalPlayer;

	if (!local_player)
		return;

	if (local_player->GetHealth() <= 0)
		return;

	static int lineSize = 6;

	static float alpha = 0;
	float step = 255.f / 0.3f * g_pGlobals->frametime;


	if (G::flHurtTime + 0.4f >= g_pGlobals->curtime)
		alpha = 255.f;
	else
		alpha -= step;

	if (alpha > 0) {
		int screenSizeX, screenCenterX;
		int screenSizeY, screenCenterY;
		g_pEngine->GetScreenSize(screenSizeX, screenSizeY);

		screenCenterX = screenSizeX / 2;
		screenCenterY = screenSizeY / 2;
		Color col = Color(255, 255, 255, alpha);
		g_pSurface->SetDrawColor(col);
		g_pSurface->DrawLine(screenCenterX - lineSize * 2, screenCenterY - lineSize * 2, screenCenterX - (lineSize), screenCenterY - (lineSize));
		g_pSurface->DrawLine(screenCenterX - lineSize * 2, screenCenterY + lineSize * 2, screenCenterX - (lineSize), screenCenterY + (lineSize));
		g_pSurface->DrawLine(screenCenterX + lineSize * 2, screenCenterY + lineSize * 2, screenCenterX + (lineSize), screenCenterY + (lineSize));
		g_pSurface->DrawLine(screenCenterX + lineSize * 2, screenCenterY - lineSize * 2, screenCenterX + (lineSize), screenCenterY - (lineSize));
	}
}

void CESP::DrawThrowable(CBaseEntity* pThrowable)
{
	const model_t* nadeModel = pThrowable->GetModel();

	if (!nadeModel)
		return;

	studiohdr_t* hdr = g_pModelInfo->GetStudioModel(nadeModel);

	if (!hdr)
		return;

	if (!strstr(hdr->name, XorStr("thrown")) && !strstr(hdr->name, XorStr("dropped")))
		return;

	std::string nadeName = XorStr("Unknown Grenade");

	IMaterial* mats[32];
	g_pModelInfo->GetModelMaterials(nadeModel, hdr->numtextures, mats);

	for (int i = 0; i < hdr->numtextures; i++)
	{
		IMaterial* mat = mats[i];
		if (!mat)
			continue;

		/*	if (strstr(mat->GetName(), XorStr("flashbang")))
		{
		nadeName = XorStr("Flashbang");

		break;
		}*/
		else if (strstr(mat->GetName(), XorStr("m67_grenade")) || strstr(mat->GetName(), XorStr("hegrenade")))
		{
			nadeName = XorStr("HE Grenade");

			break;
		}
		else if (strstr(mat->GetName(), XorStr("smoke")))
		{
			nadeName = XorStr("Smoke");

			break;
		}
		else if (strstr(mat->GetName(), XorStr("decoy")))
		{
			nadeName = XorStr("Decoy");

			break;
		}
		else if (strstr(mat->GetName(), XorStr("incendiary")) || strstr(mat->GetName(), XorStr("molotov")))
		{
			nadeName = XorStr("Molotov");

			break;
		}
	}
	Vector Bot, Top;
	float Left, Right;
	GetCBaseEntityBox(pThrowable, Bot, Top, Left, Right, false);
	int height = Bot.y - Top.y;
	int width = Right - Left;

	//g_pRender->Text((char*)nadeName.c_str(), Left + width / 2, Bot.y, centered, g_pRender->Fonts.esp, true, WHITE(255), BLACK(255));
	g_Draw.StringA(g_Draw.font_espinfo, true, Left + width / 2, Bot.y, 255, 255, 255, 255, nadeName.c_str());
}

void CESP::DrawDroppedWeapon(CBaseCombatWeapon* pWeapon)
{
	CBaseEntity* pCBaseEntity = (CBaseEntity*)pWeapon;
	CBaseCombatWeapon* Weapon = (CBaseCombatWeapon*)pWeapon;

	if (!pCBaseEntity || !g_pModelInfo)
		return;

	std::string sCurWeapon = g_pModelInfo->GetModelName(pCBaseEntity->GetModel());
	if (!sCurWeapon.empty() && sCurWeapon.find(XorStr("w_")) != std::string::npos)
	{
		std::string name;

		if (sCurWeapon.find(XorStr("defuser")) != std::string::npos/* insert check for defuser here*/)
			name = XorStr("defuser");
		else
		{
			CBaseCombatWeapon* pWeapon = (CBaseCombatWeapon*)pCBaseEntity;
			if (!pWeapon)return;
			name = GetWeaponName(pWeapon);//pWeapon->GetName(false);
		}

		Vector Bot, Top;
		float Left, Right;
		GetCBaseEntityBox(pCBaseEntity, Bot, Top, Left, Right, false);
		int width = Right - Left;

		float Red = Clientvariables->Colors.DroppedWeapon[0] * 255;
		float Green = Clientvariables->Colors.DroppedWeapon[1] * 255;
		float Blue = Clientvariables->Colors.DroppedWeapon[2] * 255;

		g_Draw.StringA(g_Draw.font_espinfo, true, Left + width / 2, Bot.y, Red, Green, Blue, 255, name.c_str());
	}
}

void CESP::DrawLinesAA()
{
	/*show lby and shit*/
	Vector vecLby, vecFake, vecCircle;
	Vector org = G::LocalPlayer->GetOrigin(), vOrg;
	Vector end, vEnd;
	Math::AngleVectors(Vector(0, Math::NormalizeYaw(G::LocalPlayer->LowerBodyYaw()), 0), &vecLby);
	//Math::AngleVectors(Vector(0, Math::NormalizeYaw(G::RealAngle.y), 0), &vecReal);
	Math::AngleVectors(Vector(0, Math::NormalizeYaw(G::FakeAngle.y), 0), &vecFake);
	Vector circle_angle = Vector(0, Math::NormalizeYaw(g_Misc->m_circle_yaw), 0);
	vecCircle = circle_angle.Direction();


	end = org + (vecFake * 40);

	if (GameUtils::WorldToScreen(org, vOrg) && GameUtils::WorldToScreen(end, vEnd))
	{
		g_Draw.LineRGBA(vOrg.x, vOrg.y, vEnd.x, vEnd.y, 255, 0, 0, 255);
	}

	end = org + (vecLby * 40);

	if (GameUtils::WorldToScreen(org, vOrg) && GameUtils::WorldToScreen(end, vEnd))
	{
		g_Draw.LineRGBA(vOrg.x, vOrg.y, vEnd.x, vEnd.y, 0, 0, 255, 255);
	}
}

void CESP::Loop()
{
	if (!G::LocalPlayer)
		return;

	if (Clientvariables->Misc.Walkbot)
		WalkbotPoints();

	if (Clientvariables->Visuals.Hitmarker && G::LocalPlayer->isAlive()) {
		hitmarker_2->paint();
	}

	if (Clientvariables->Antiaim.Lines && G::LocalPlayer->isAlive()) {
		DrawLinesAA();
	}

	for (int i = 1; i <= g_pEntitylist->GetHighestEntityIndex(); i++)
	{
		CBaseEntity* pBaseEntity = g_pEntitylist->GetClientEntity(i);

		if (!pBaseEntity)
			continue;

		ClientClass* pClass = (ClientClass*)pBaseEntity->GetClientClass();

		if (pClass->m_ClassID == int(EClassIds::CCSPlayer) && Clientvariables->Visuals.EspEnable)
		{
			if (pBaseEntity->GetHealth() < 1)
				continue;

			bool is_friendly = pBaseEntity->GetTeamNum() == G::LocalPlayer->GetTeamNum();

			if (!is_friendly) {
				g_ESP->DrawPlayer(pBaseEntity, G::LocalPlayer);
				g_ESP->DrawBarrel(pBaseEntity);
			}
		}

		Vector buf, pos = pBaseEntity->GetAbsOrigin();

		if (pos.x == 0 || pos.y == 0 || pos.z == 0 || !GameUtils::WorldToScreen(pos, buf))
			continue;

		if ((pClass->m_ClassID != int(EClassIds::CBaseWeaponWorldModel) && (strstr(pClass->m_pNetworkName, XorStr("Weapon")) || pClass->m_ClassID == int(EClassIds::CDEagle) || pClass->m_ClassID == int(EClassIds::CAK47))) && Clientvariables->Visuals.DroppedWeapons == 1)
			DrawDroppedWeapon((CBaseCombatWeapon*)pBaseEntity);

		if ((pClass->m_ClassID == int(EClassIds::CC4) || pClass->m_ClassID == int(EClassIds::CPlantedC4)))
		{
			if (Clientvariables->Visuals.Bomb)
				DrawC4(pBaseEntity, pClass->m_ClassID == int(EClassIds::CPlantedC4), G::LocalPlayer);
		}


	}
}

void CESP::DrawScope(CBaseEntity* pLocalPlayer)
{
	CBaseCombatWeapon* pLocalWeapon = pLocalPlayer->GetWeapon();

	if (pLocalWeapon)
	{
		if (pLocalWeapon->IsSniper())
		{
			if (pLocalPlayer->IsScoped())
			{
				int width = 0;
				int height = 0;
				g_pEngine->GetScreenSize(width, height);
				int centerX = static_cast<int>(width * 0.5f);
				int centerY = static_cast<int>(height * 0.5f);
				g_pSurface->SetDrawColor(Color(0, 0, 0, 255));
				g_pSurface->DrawLine(0, centerY, width, centerY);
				g_pSurface->DrawLine(centerX, 0, centerX, height);
			}
		}
	}
}