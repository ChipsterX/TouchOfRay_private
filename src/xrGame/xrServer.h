#pragma once
// xrServer.h: interface for the xrServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
#define AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_
#pragma once

#include "xrNetServer/net_server.h"
#include "game_sv_base.h"
#include "id_generator.h"
#include "xrCommon/xr_unordered_map.h"

class CSE_Abstract;

constexpr u32 NET_Latency = 50; // time in (ms)

// XXX: check if u16 used for entity's id. If true, then this must be changed, if we want to increase the number of ID's.
using xrS_entities = xr_unordered_map<u16, CSE_Abstract*>;

class xrClientData : public IClient
{
public:
    CSE_Abstract* owner;
    BOOL net_Accepted;

    xrClientData();
    virtual ~xrClientData();
    virtual void Clear();
};

// main
class xrServer : public IPureServer
{
private:
    xrS_entities entities;
    xr_vector<u16> conn_spawned_ids;

    u32 m_last_update_time;

private:
    typedef CID_Generator<u32, // time identifier type
        u8, // compressed id type
        u16, // id type
        u8, // block id type
        u16, // chunk id type
        0, // min value
        u16(-2), // max value
        256, // block size
        u16(-1) // invalid id
        >
        id_generator_type;

private:
    id_generator_type m_tID_Generator;
    struct ServerStatistics
    {
        CStatTimer Update;

        ServerStatistics() { FrameStart(); }
        void FrameStart() { Update.FrameStart(); }
        void FrameEnd() { Update.FrameEnd(); }
    };
    ServerStatistics stats;

protected:
    game_sv_GameState* game;

public:
    virtual IServerGameState* GetGameState() override { return game; }
    void Export_game_type(IClient* CL);
    void Perform_game_export();
    BOOL PerformRP(CSE_Abstract* E);
    void PerformMigration(CSE_Abstract* E, xrClientData* from, xrClientData* to);

    IC void clear_ids() { m_tID_Generator = id_generator_type(); }
    virtual u16 PerformIDgen(u16 ID) override { return (m_tID_Generator.tfGetID(ID)); }
    virtual void FreeID(u16 ID, u32 time) override { return (m_tID_Generator.vfFreeID(ID, time)); }
    void Perform_connect_spawn(CSE_Abstract* E, xrClientData* to, NET_Packet& P);
    void Perform_transfer(NET_Packet& PR, NET_Packet& PT, CSE_Abstract* what, CSE_Abstract* from, CSE_Abstract* to);
    void Perform_reject(CSE_Abstract* what, CSE_Abstract* from, int delta);
    void Perform_destroy(CSE_Abstract* tpSE_Abstract);

    CSE_Abstract* Process_spawn(NET_Packet& P, ClientID sender,
                                bool bSpawnWithClientsMainEntityAsParent = false, CSE_Abstract* tpExistedEntity = nullptr) override;
    void Process_update(NET_Packet& P, ClientID sender);
    void Process_save(NET_Packet& P, ClientID sender);
    void Process_event(NET_Packet& P, ClientID sender);
    void Process_event_ownership(NET_Packet& P, ClientID sender, u32 time, u16 ID, BOOL bForced = FALSE);
    bool Process_event_reject(NET_Packet& P, const ClientID sender, const u32 time, const u16 id_parent,
        const u16 id_entity, bool send_message = true);
    void Process_event_destroy(NET_Packet& P, ClientID sender, u32 time, u16 ID, NET_Packet* pEPack);
    void Process_event_activate(NET_Packet& P, const ClientID sender, const u32 time, const u16 id_parent,
        const u16 id_entity, bool send_message = true);

    void __stdcall SendConfigFinished(ClientID const& clientId);

protected:
    void SendConnectionData(IClient* CL);

public:
    // constr / destr
    xrServer();
    virtual ~xrServer();

    // extended functionality
    u32 OnMessage(NET_Packet& P, ClientID sender); // Non-Zero means broadcasting with "flags" as returned
    virtual void OnCL_Connected(IClient* CL);
    void SendTo(ClientID ID, NET_Packet& P);
    void SendBroadcast(ClientID exclude, NET_Packet& P);
    virtual void client_Destroy(IClient* C); // destroy client info

    // utilities
    virtual CSE_Abstract* entity_Create(pcstr name) override;
    virtual void entity_Destroy(CSE_Abstract*& P) override;
    u32 GetEntitiesNum() { return entities.size(); };
    CSE_Abstract* GetEntity(u32 Num);
    xrClientData* ID_to_client(ClientID const& ID, bool ScanAll = false)
    {
        return (xrClientData*)(IPureServer::ID_to_client(ID, ScanAll));
    }
    CSE_Abstract* ID_to_entity(u16 ID);

    // main
    virtual EConnect Connect(shared_str& session_name);
    void Disconnect();
    virtual void Update();
    void SLS_Default();
    void SLS_Clear();
    void SLS_Save(IWriter& fs);
    void SLS_Load(IReader& fs);
    shared_str level_name(const shared_str& server_options) const;
    shared_str level_version(const shared_str& server_options) const;

    void CreateSVClient();

    virtual void GetServerInfo(CServerInfo* si);

public:
    xr_string ent_name_safe(u16 eid);
};

#endif // !defined(AFX_XRSERVER_H__65728A25_16FC_4A7B_8CCE_D798CA5EC64E__INCLUDED_)
