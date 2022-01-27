#pragma once
#include "GameUtils.h"
#include "AW_hitmarker.h"
#include <chrono>
CAW_hitmaker* hitmarker_2 = new CAW_hitmaker();

void CAW_hitmaker::paint()
{
	if (!Clientvariables->Visuals.Hitmarker)
		return;

	float time = g_pGlobals->curtime;

	for (int i = 0; i < hitmarkers.size(); i++)
	{
		bool expired = time >= hitmarkers.at(i).impact.time + 2.f;
		if (expired)
			hitmarkers.at(i).alpha -= 1;
		if (expired && hitmarkers.at(i).alpha <= 0)
		{
			hitmarkers.erase(hitmarkers.begin() + i);
			continue;
		}

		Vector pos3D = Vector(hitmarkers.at(i).impact.x, hitmarkers.at(i).impact.y, hitmarkers.at(i).impact.z), pos2D;
		if (!GameUtils::WorldToScreen(pos3D, pos2D))
			continue;

		int linesize = 6;


		g_pSurface->SetDrawColor(240, 240, 240, hitmarkers.at(i).alpha);
		g_pSurface->DrawLine(pos2D.x - linesize, pos2D.y - linesize, pos2D.x - (linesize / 3), pos2D.y - (linesize / 3));
		g_pSurface->DrawLine(pos2D.x - linesize, pos2D.y + linesize, pos2D.x - (linesize / 3), pos2D.y + (linesize / 3));
		g_pSurface->DrawLine(pos2D.x + linesize, pos2D.y - linesize, pos2D.x + (linesize / 3), pos2D.y - (linesize / 3));
		g_pSurface->DrawLine(pos2D.x + linesize, pos2D.y + linesize, pos2D.x + (linesize / 3), pos2D.y + (linesize / 3));
	}
}