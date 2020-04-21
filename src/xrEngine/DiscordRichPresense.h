#pragma once

class ENGINE_API xrDiscordPresense
{
public:
	enum class StatusId
	{
		Menu,
		In_Game,
        zaton,
        jupiter,
        pripyat,
        k00_marsh,
        l01_escape,
        l02_garbage,
        l03_agroprom,
        k01_darkscape,
        l04_darkvalley,
        l05_bar,
        l06_rostok,
        l07_military,
        l08_yantar,
        l09_deadcity,
        l10_limansk,
        l10_radar,
        l10_red_forest,
        l11_pripyat,
        l12_stancia,
        l12_stancia_2,
        l13_generators,
        l03u_agr_underground,
        l04u_labx18,
        l08u_brainlab,
        l12u_sarcofag,
        l12u_control_monolith,
        l13u_warlab,
        jupiter_underground,
        labx8,
        k02_trucks_cemetery,
        Initialization,
        Load,
        fake_start
	};

public:

	void Initialize();
	void Shutdown();

	void SetStatus(StatusId status);

	~xrDiscordPresense();

private:
	bool bInitialize = false;
};

extern ENGINE_API xrDiscordPresense g_discord;
