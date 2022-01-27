#pragma once

#pragma once
#include "hooks.h"
#include "global.h"
#include "Menu.h"
#include "BacktrackingHelper.h"
#include "xor.h"
#include <intrin.h>
#include "SpoofedConvar.h"
#include "Math.h"
#include "Skinchanger.h"
#include "GameUtils.h"
#include "Items.h"
#include "Resolver.h"

extern float FAKE_LATENCY_AMOUNT;
extern float EST_FAKE_LATENCY_ON_SERVER;
#define LAG_COMPENSATION_EPS_SQR ( 0.1f * 0.1f )
// Allow 4 units of error ( about 1 / 8 bbox width )
#define LAG_COMPENSATION_ERROR_EPS_SQR ( 4.0f * 4.0f )
extern bool BackTracked;
extern ConVar *sv_unlag;
extern ConVar *sv_maxunlag;
extern ConVar *sv_lagflushbonecache;
extern ConVar *sv_lagpushticks;
extern ConVar *sv_client_min_interp_ratio;
extern ConVar *sv_client_max_interp_ratio;
extern ConVar *sv_maxupdaterate;
extern ConVar *sv_minupdaterate;
extern ConVar *cl_updaterate;
extern ConVar *cl_interp_ratio;
class INetChannel;


/*class CIncomingSequence {
public:
	int in_reliable_state,
		out_reliable_state,
		in_sequence_nr;
	float realtime;

	CIncomingSequence() {
		in_reliable_state = (*ctx::client_state)->m_NetChannel->m_nInReliableState;
		out_reliable_state = (*ctx::client_state)->m_NetChannel->m_nOutReliableState;
		in_sequence_nr = (*ctx::client_state)->m_NetChannel->m_nInSequenceNr;
		realtime = g_pGlobals->realtime;
	}

	CIncomingSequence(int irs, int ors, int isn, float _realtime) {
		in_reliable_state = irs;
		out_reliable_state = ors;
		in_sequence_nr = isn;
		realtime = _realtime;
	}
};

class CFakeLatency {
public:
	int SendDatagram(INetChannel* netchan, void* datagram);
	void UpdateIncomingSequences(ClientFrameStage_t curStage);
	void AddLatency(INetChannel* netchan, float latency);

	std::deque <CIncomingSequence> m_sequences;
	float lastincomingsequencenumber;

	INetChannel* m_netchan;
	int ticks = 0;
};

extern CFakeLatency* g_FakeLatency;*/