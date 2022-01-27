#pragma once
#include "ttf.h"
#include "hooks.h"
#include "Menu.h"
#include "global.h"
#include "MaterialHelper.h"
#include "xor.h"
class Chams
	: public Singleton<Chams>
{
	friend class Singleton<Chams>;

	Chams();
	~Chams();

public:
	void OnSceneEnd();
	void OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool glass, bool metallic, const Color& rgba);
	void OverrideMaterialTP(bool ignoreZ, bool flat, bool wireframe, bool glass);
private:


	IMaterial* materialRegular = nullptr;
	IMaterial* materialRegularIgnoreZ = nullptr;
	IMaterial* materialFlatIgnoreZ = nullptr;
	IMaterial* materialFlat = nullptr;
	IMaterial* materialMetall = nullptr;
	IMaterial* materialMetallnZ = nullptr;
};