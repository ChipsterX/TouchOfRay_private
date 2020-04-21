#include "pch.hpp"
#include "AISpaceBase.hpp"
#include "Navigation/game_graph.h"
#include "Navigation/level_graph.h"
#include "Navigation/graph_engine.h"
#include "Navigation/PatrolPath/patrol_path_storage.h"

AISpaceBase::AISpaceBase() { GEnv.AISpace = this; }
AISpaceBase::~AISpaceBase()
{
    xr_delete(m_patrol_path_storage);
    xr_delete(m_graph_engine);
    VERIFY(!m_game_graph);
    GEnv.AISpace = nullptr;
}

void AISpaceBase::Load(const char* levelName)
{
    const CGameGraph::SLevel& currentLevel = game_graph().header().level(levelName);
    m_level_graph = new CLevelGraph();
    game_graph().set_current_level(currentLevel.id());
    auto& crossHeader = cross_table().header();
    auto& levelHeader = level_graph().header();
    auto& gameHeader = game_graph().header();
    R_ASSERT2(crossHeader.level_guid() == levelHeader.guid(), "cross_table doesn't correspond to the AI-map");
    R_ASSERT2(crossHeader.game_guid() == gameHeader.guid(), "graph doesn't correspond to the cross table");
    u32 vertexCount = _max(gameHeader.vertex_count(), levelHeader.vertex_count());
    m_graph_engine = new CGraphEngine(vertexCount);
    R_ASSERT2(currentLevel.guid() == levelHeader.guid(), "graph doesn't correspond to the AI-map");
    level_graph().level_id(currentLevel.id());
}

void AISpaceBase::Unload(bool reload)
{
    if (GEnv.isDedicatedServer)
        return;
    xr_delete(m_graph_engine);
    xr_delete(m_level_graph);
    if (!reload && m_game_graph)
        m_graph_engine = new CGraphEngine(game_graph().header().vertex_count());
}

void AISpaceBase::Initialize()
{
    if (GEnv.isDedicatedServer)
        return;
    VERIFY(!m_graph_engine);
    m_graph_engine = new CGraphEngine(1024);
    VERIFY(!m_patrol_path_storage);
    m_patrol_path_storage = new CPatrolPathStorage();
}

void AISpaceBase::patrol_path_storage_raw(IReader& stream)
{
    if (GEnv.isDedicatedServer)
        return;
    xr_delete(m_patrol_path_storage);
    m_patrol_path_storage = new CPatrolPathStorage();
    m_patrol_path_storage->load_raw(get_level_graph(), get_cross_table(), get_game_graph(), stream);
}

void AISpaceBase::patrol_path_storage(IReader& stream)
{
    if (GEnv.isDedicatedServer)
        return;
    xr_delete(m_patrol_path_storage);
    m_patrol_path_storage = new CPatrolPathStorage();
    m_patrol_path_storage->load(stream);
}

void AISpaceBase::SetGameGraph(CGameGraph* gameGraph)
{
    if (gameGraph)
    {
        VERIFY(!m_game_graph);
        m_game_graph = gameGraph;
        xr_delete(m_graph_engine);
        m_graph_engine = new CGraphEngine(game_graph().header().vertex_count());
    }
    else
    {
        VERIFY(m_game_graph);
        m_game_graph = nullptr;
        xr_delete(m_graph_engine);
    }
}

const CGameLevelCrossTable& AISpaceBase::cross_table() const { return game_graph().cross_table(); }
const CGameLevelCrossTable* AISpaceBase::get_cross_table() const { return &game_graph().cross_table(); }
