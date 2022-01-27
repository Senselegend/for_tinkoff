#pragma once

extern void GUI_Init();

class ClientVariables
{
public:

	struct TriggerbotList
	{
		bool Enable = false;
		bool AutoFire = false;
		bool FriendlyFire = false;
		int Key = 0;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}deagle;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}duals;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}five7;


		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}glock;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}cz75;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}p250;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}r8;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}tec9;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}p2000;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}usp;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}xm1014;


		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}mag7;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}nova;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}sawed_off;


		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}mac10;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}p90;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}umo45;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}mp7;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}mp9;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}bizon;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}ak47;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}aug;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}famas;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}galil;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}m4a1;


		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}m4a1s;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}sg553;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}awp;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}g3sg1;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}scar20;

		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}ssg080;


		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}m249;


		struct
		{
			bool Enable = false;
			bool AutoFire = false;
			bool FriendlyFire = false;
			int Key = 0;

		}negev;
		////////////////////////////
		bool WeaponConfig = false;

	}Triggerbot;

	struct Ragebot
	{
		bool EnableAimbot = false;
		int AimbotSelection = 0;
		bool AutomaticFire = false;
		bool AutomaticScope = false;
		bool SilentAimbot = false;
		bool NoRecoil = false;
		bool NoSpread = false;
		bool PositionAdjustment = false;
		int Preferbodyaim = 0;
		int Hitbox = 0;
		int pzj = 0;
		int Hitscan = 0;
		bool AutowallHitscan = false;
		bool Autowall = false;
		int Multipoint = 0.f;
		float Headscale = 0.0f;
		float Bodyscale = 0.0f;
		int Mindamage = 1.f;
		bool Hitchance = false;
		int Minhitchance = 0.f;
		bool AutomaticResolver;
		int ResolverStepAngle = 25;
		bool FriendlyFire = false;
		bool Quickstop = false;
		int multimemez;
		int multimemez2;
		bool headaimwalking;
		bool AutomaticRevolver;
		bool FakeLatency;
		float FakeLatencyAmount;

	} Ragebot;

	struct Antiaim
	{
		bool AntiaimEnable = false;
		bool Lines = false;
		bool Indicator = false;
		int Pitch = 0;

		int Yaw = 0;
		int YawAdd = 0;
		int YawRunning = 0;
		int YawRunningAdd = 0;
		int FakeYaw = 0;
		int FakeYawAdd = 0;

		int Fakewalk = 0;

		int breakaa;
		int FreestandingDelta = 0;

		int AtPlayer = 0;
		bool Disableaa = false;
		int Jitterrange = 0.f;
		bool Randomizejitter = false;
		int AntiaimMode = 0;
	} Antiaim;

	struct Visuals
	{
		bool EspEnable = false;
		bool EnemyOnly = false;
		bool BoundingBox = false;
		bool Bones = false;
		bool Health = false;
		bool Armor = false;
		bool Flags = false;
		bool Fake = false;
		bool Dlight = false;
		bool setblend = false;
		bool Name = false;
		bool Weapon = false;
		bool Ammo = false;
		bool AllItems = false;
		bool Rank = false;
		bool Radar = false;
		bool Wins = false;
		bool Glow = false;
		bool LineofSight = false;
		bool SnapLines = false;
		bool GrenadePrediction = false;
		int Crosshair = 0;
		bool SpreadCrosshair = false;
		bool RecoilCrosshair = false;
		bool FartherESP = false;

		//Cbase/filers
		int DroppedWeapons = 0;
		bool Hostage = false;
		bool ThrownNades = false;
		bool LocalPlayer = false;
		bool BulletTracers = false;
		bool Bomb = false;
		bool Spectators = false;
		bool OutOfPOVArrows = false;
		bool DamageIndicators = false;
		bool shaokpidork = false;

		//Effects/world
		bool nightmode = false;
		int Skybox = 0;
		int FlashbangAlpha = 0;
		bool Nosmoke = false;
		bool Noscope = false;
		bool RemoveParticles = false;
		bool Novisrevoil = false;
		bool Hitmarker = false;
		bool ChamsEnable = false;
		bool ShowBacktrack = false;
		int ChamsStyle = 0;
		bool ChamsL = false;
		bool crosssnip = false;
		bool Chamsenemyonly = false;
		bool ChamsPlayer = false;
		bool ChamsPlayerWall = false;
		bool ChamsHands = false;
		bool ChamsHandsWireframe = false;
		bool WeaponWireframe = false;
		bool WeaponChams = false;
	} Visuals;

	struct Misc
	{
		bool AntiUT = true;
		bool WireHand = false;
		bool static_scope = false;
		int PlayerFOV = 0.f;
		int PlayerViewmodel = 0.f;
		int TPangles = 0;
		int TPKey = 0;
		int MenuKey = 0x2d;
		int WalkbotSet = 0x2d;
		int WalkbotDelete = 0x2d;
		int WalkbotStart = 0x2d;
		bool Clantag;
		bool AutoJump = false;
		int buybot1 = 0.f;
		int buybot2 = 0.f;
		int buybot3 = 0.f;
		bool AutoStrafe = false;
		bool AutoAccept = false;
		bool Prespeed = false;
		int Retrack = 0.f;
		int PrespeedKey = 0;
		bool FakelagEnable = false;
		bool FakelagOnground = false;
		int FakelagMode = 0;
		int FakelagAmount = 0.f;
		bool FakelagShoot = false;
		int ConfigSelection = 0;
		bool Walkbot = false;
		bool WalkbotBunnyhop = false;
		//int WalkbotSetPoint = 0;
		//int WalkbotDeletePoint = 0;
		//int WalkbotStart = 0;
		bool FakewalkEnable = false;
		int FakewalkKey = 0;
		bool legitbacktrack;
		bool Spectators = true;
	} Misc;

	struct Skins
	{
		bool Enabled;
		int Knife;
		int gloves;
		int KnifeSkin;
		int AK47Skin;
		int M4A1SSkin;
		int M4A4Skin;
		int AUGSkin;
		int FAMASSkin;
		int AWPSkin;
		int SSG08Skin;
		int SCAR20Skin;
		int P90Skin;
		int UMP45Skin;
		int GlockSkin;
		int USPSkin;
		int DeagleSkin;
		int tec9Skin;
		int P2000Skin;
		int P250Skin;
		int CZ75Skin;
		int RevolverSkin;
		int DualSkins;
		int FiveSevenSkin;
	} Skinchanger;

	struct CPlayerlist
	{
		bool bEnabled;
		int iPlayer;
		char* szPlayers[64] = {
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" ", " ", " ", " ", " ", " ", " ", " ", " ",
			" "
		};
	} Playerlist;

	struct NigColors
	{
		float MenuColor[3] = { 1.f, 1.f, 1.f };


		float BoundingBox[3] = { 1.f, 1.f, 1.f };
		float PlayerChams[3] = { 1.f, 1.f, 1.f };
		float ChamsHistory[3] = { 1.f, 1.f, 1.f };
		float PlayerChamsl[4] = { 1.f, 1.f, 1.f, 1.f };
		float PlayerChamsWall[3] = { 1.f, 1.f, 1.f };
		float Skeletons[3] = { 1.f, 1.f, 1.f };
		float Bulletracer[3] = { 1.f, 1.f, 0.f };

		float WireframeHand[3] = { 1.f, 1.f, 1.f };
		float ChamsHand[3] = { 1.f, 1.f, 1.f };
		float ChamsWeapon[3] = { 1.f, 1.f, 1.f };
		float WireframeWeapon[3] = { 1.f, 1.f, 1.f };
		float Glow[4] = { 1.f, 1.f, 1.f, 1.f };
		float DroppedWeapon[3] = { 1.f, 1.f, 1.f };
		float Bomb[3] = { 1.f, 1.f, 1.f };
		float PlantedBomb[3] = { 1.f, 1.f, 1.f };
		float Hostage[3] = { 1.f, 1.f, 1.f };
		float GrenadePrediction[3] = { 1.f, 1.f, 1.f };
		float FakeAngleGhost[3] = { 1.f, 1.f, 1.f };
		float SpreadCrosshair[4] = { 1.f, 1.f, 1.f, 0.1f };
		float Snaplines[3] = { 1.f, 1.f, 1.f };
		float DamageIndicator[3] = { 1.f, 1.f, 1.f };
		bool Props;
	}Colors;
};

extern ClientVariables* Clientvariables;
