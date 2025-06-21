//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Source Engine Add Entities plugin for Portal 2
// Warning: This plugin is highly experimental and can break things
// Do NOT unload the plugin, restart the game
// All RVA offsets (Real Virtual Addresses) MUST be changed after an update to server.dll
//
// $NoKeywords: $
//
//===========================================================================//

#include <stdio.h>
#include "windows.h"
#undef GAME_DLL
#ifdef GAME_DLL
#include "cbase.h"
#endif
#include <stdio.h>
#include "interface.h"
#include "filesystem.h"
#include "engine/iserverplugin.h"
#include "eiface.h"
#include "igameevents.h"
#include "convar.h"
#include "Color.h"
#include "vstdlib/random.h"
#include "engine/IEngineTrace.h"
#include "tier2/tier2.h"
//#include "game/server/pluginvariant.h"
#include "game/server/iplayerinfo.h"
//#include "game/server/igameinfo.h"
#include "entlist_stub.h"
#include "E:\vs2022ide\DIA SDK\include/dia2.h"
#include "addresses.h"
#include "pdbparser.h"
#include "baseentity_stub.h"

#include "lib.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Interfaces from the engine
IVEngineServer	*engine = NULL; // helper functions (messaging clients, loading content, making entities, running commands, etc)
IGameEventManager *gameeventmanager_ = NULL; // game events interface
#ifndef GAME_DLL
#define gameeventmanager gameeventmanager_
#endif


IPlayerInfoManager *playerinfomanager = NULL; // game dll interface to interact with players
//IEntityInfoManager *entityinfomanager = NULL; // game dll interface to interact with all entities (like IPlayerInfo)
//IGameInfoManager *gameinfomanager = NULL; // game dll interface to get data from game rules directly
IBotManager *botmanager = NULL; // game dll interface to interact with bots
IServerPluginHelpers *helpers = NULL; // special 3rd party plugin helpers from the engine
IUniformRandomStream *randomStr = NULL;
IEngineTrace *enginetrace = NULL;


CGlobalVars *gpGlobals = NULL;



// useful helper func
#if 0//ndef GAME_DLL
inline bool FStrEq(const char *sz1, const char *sz2)
{
	return(Q_stricmp(sz1, sz2) == 0);
}
#endif
//---------------------------------------------------------------------------------
// Purpose: a sample 3rd party plugin class
//---------------------------------------------------------------------------------
class CAddEntServerPlugin: public IServerPluginCallbacks, public IGameEventListener
{
public:
	CAddEntServerPlugin();
	~CAddEntServerPlugin();

	// IServerPluginCallbacks methods
	virtual bool			Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory );
	virtual void			Unload( void );
	virtual void			Pause( void );
	virtual void			UnPause( void );
	virtual const char     *GetPluginDescription( void );      
	virtual void			LevelInit( char const *pMapName );
	virtual void			ServerActivate( edict_t *pEdictList, int edictCount, int clientMax );
	virtual void			GameFrame( bool simulating );
	virtual void			LevelShutdown( void );
	virtual void			ClientActive( edict_t *pEntity );
	virtual void			ClientFullyConnect( edict_t* pEntity );
	virtual void			ClientDisconnect( edict_t *pEntity );
	virtual void			ClientPutInServer( edict_t *pEntity, char const *playername );
	virtual void			SetCommandClient( int index );
	virtual void			ClientSettingsChanged( edict_t *pEdict );
	virtual PLUGIN_RESULT	ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen );
	virtual PLUGIN_RESULT	ClientCommand( edict_t *pEntity, const CCommand &args );
	virtual PLUGIN_RESULT	NetworkIDValidated( const char *pszUserName, const char *pszNetworkID );
	virtual void			OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue );
	virtual void			OnEdictAllocated( edict_t *edict );
	virtual void			OnEdictFreed( const edict_t *edict  );

	// IGameEventListener Interface
	virtual void FireGameEvent( KeyValues * event );

	virtual int GetCommandIndex() { return m_iClientCommandIndex; }
private:
	int m_iClientCommandIndex;
};


// 
// The plugin is a static singleton that is exported as an interface
//
CAddEntServerPlugin g_AddEntServerPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CAddEntServerPlugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_AddEntServerPlugin );

//---------------------------------------------------------------------------------
// Purpose: constructor/destructor
//---------------------------------------------------------------------------------
CAddEntServerPlugin::CAddEntServerPlugin()
{
	m_iClientCommandIndex = 0;
}

CAddEntServerPlugin::~CAddEntServerPlugin()
{
}


void CAddEntServerPlugin::ClientFullyConnect(edict_t* pEntity) 
{

}

CBaseEntityList* g_pEntityList = NULL;

HMODULE serverDLL;

extern void FillRVAs();

void LoadLib() 
{
	if (serverDLL)
		return;
	serverDLL = GetModuleHandleA("server.dll");
	FillRVAs();
	g_pEntityList = *((CBaseEntityList**)((DWORD_PTR)serverDLL + ENTLIST_RVA));
	
}




//---------------------------------------------------------------------------------
// Purpose: called when the plugin is loaded, load the interface we need from the engine
//---------------------------------------------------------------------------------
bool CAddEntServerPlugin::Load(	CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory )
{

	ConnectTier1Libraries( &interfaceFactory, 1 );
	ConnectTier2Libraries( &interfaceFactory, 1 );

	//entityinfomanager = (IEntityInfoManager *)gameServerFactory(INTERFACEVERSION_ENTITYINFOMANAGER,NULL);
	//if ( !entityinfomanager )
	//{
	//	Warning( "Unable to load entityinfomanager, ignoring\n" ); // this isn't fatal, we just won't be able to access entity data
	//}

	playerinfomanager = (IPlayerInfoManager *)gameServerFactory(INTERFACEVERSION_PLAYERINFOMANAGER,NULL);
	if ( !playerinfomanager )
	{
		Warning( "Unable to load playerinfomanager, ignoring\n" ); // this isn't fatal, we just won't be able to access specific player data
	}

	botmanager = (IBotManager *)gameServerFactory(INTERFACEVERSION_PLAYERBOTMANAGER, NULL);
	if ( !botmanager )
	{
		Warning( "Unable to load botcontroller, ignoring\n" ); // this isn't fatal, we just won't be able to access specific bot functions
	}
	//gameinfomanager = (IGameInfoManager *)gameServerFactory(INTERFACEVERSION_GAMEINFOMANAGER, NULL);
	//if (!gameinfomanager)
	//{
	//	Warning( "Unable to load gameinfomanager, ignoring\n" );
	//}

	engine = (IVEngineServer*)interfaceFactory(INTERFACEVERSION_VENGINESERVER, NULL);
	gameeventmanager = (IGameEventManager *)interfaceFactory(INTERFACEVERSION_GAMEEVENTSMANAGER,NULL);
	helpers = (IServerPluginHelpers*)interfaceFactory(INTERFACEVERSION_ISERVERPLUGINHELPERS, NULL);
	enginetrace = (IEngineTrace *)interfaceFactory(INTERFACEVERSION_ENGINETRACE_SERVER,NULL);
	randomStr = (IUniformRandomStream *)interfaceFactory(VENGINE_SERVER_RANDOM_INTERFACE_VERSION, NULL);

	// get the interfaces we want to use
	if(	! ( engine && gameeventmanager && g_pFullFileSystem && helpers && enginetrace && randomStr ) )
	{
		return false; // we require all these interface to function
	}

	if ( playerinfomanager )
	{
		gpGlobals = playerinfomanager->GetGlobalVars();
	}

	MathLib_Init( 2.2f, 2.2f, 0.0f, 2.0f );
	ConVar_Register( 0 );
	return true;
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unloaded (turned off)
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::Unload( void )
{
	gameeventmanager->RemoveListener( this ); // make sure we are unloaded from the event system

	ConVar_Unregister( );
	DisconnectTier2Libraries( );
	DisconnectTier1Libraries( );
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is paused (i.e should stop running but isn't unloaded)
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::Pause( void )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when the plugin is unpaused (i.e should start executing again)
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::UnPause( void )
{
}

//---------------------------------------------------------------------------------
// Purpose: the name of this plugin, returned in "plugin_print" command
//---------------------------------------------------------------------------------
const char *CAddEntServerPlugin::GetPluginDescription( void )
{
	return "Entity Adder Plugin for Portal 2, theaperturecat";
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::LevelInit( char const *pMapName )
{
	//Msg( "Level \"%s\" has been loaded\n", pMapName );
	gameeventmanager->AddListener( this, true );
}

//---------------------------------------------------------------------------------
// Purpose: called on level start, when the server is ready to accept client connections
//		edictCount is the number of entities in the level, clientMax is the max client count
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
}

//---------------------------------------------------------------------------------
// Purpose: called once per server frame, do recurring work here (like checking for timeouts)
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::GameFrame( bool simulating )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level end (as the server is shutting down or going to a new map)
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::LevelShutdown( void ) // !!!!this can get called multiple times per map change
{
	gameeventmanager->RemoveListener( this );
}

//---------------------------------------------------------------------------------
// Purpose: called when a client spawns into a server (i.e as they begin to play)
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::ClientActive( edict_t *pEntity )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client leaves a server (or is timed out)
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::ClientDisconnect( edict_t *pEntity )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on 
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::ClientPutInServer( edict_t *pEntity, char const *playername )
{
}

//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::SetCommandClient( int index )
{
	m_iClientCommandIndex = index;
}

void ClientPrint( edict_t *pEdict, char *format, ... )
{
	va_list		argptr;
	static char		string[1024];
	
	va_start (argptr, format);
	Q_vsnprintf(string, sizeof(string), format,argptr);
	va_end (argptr);

	engine->ClientPrintf( pEdict, string );
}
//---------------------------------------------------------------------------------
// Purpose: called on level start
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::ClientSettingsChanged( edict_t *pEdict )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when a client joins a server
//---------------------------------------------------------------------------------
PLUGIN_RESULT CAddEntServerPlugin::ClientConnect( bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen )
{
	return PLUGIN_CONTINUE;
}
//---------------------------------------------------------------------------------
// Purpose: called when a client types in a command (only a subset of commands however, not CON_COMMAND's)
//---------------------------------------------------------------------------------
PLUGIN_RESULT CAddEntServerPlugin::ClientCommand( edict_t *pEntity, const CCommand &args )
{
	//const char *pcmd = args[0];

	if ( !pEntity || pEntity->IsFree() ) 
	{
		return PLUGIN_CONTINUE;
	}

	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a client is authenticated
//---------------------------------------------------------------------------------
PLUGIN_RESULT CAddEntServerPlugin::NetworkIDValidated( const char *pszUserName, const char *pszNetworkID )
{
	return PLUGIN_CONTINUE;
}

//---------------------------------------------------------------------------------
// Purpose: called when a cvar value query is finished
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::OnQueryCvarValueFinished( QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue )
{

}
void CAddEntServerPlugin::OnEdictAllocated( edict_t *edict )
{
}
void CAddEntServerPlugin::OnEdictFreed( const edict_t *edict  )
{
}

//---------------------------------------------------------------------------------
// Purpose: called when an event is fired
//---------------------------------------------------------------------------------
void CAddEntServerPlugin::FireGameEvent( KeyValues * event )
{
}
/*
class IRecipientFilter
{
public:
	virtual			~IRecipientFilter() {}

	virtual bool	IsReliable(void) const = 0;
	virtual bool	IsInitMessage(void) const = 0;

	virtual int		GetRecipientCount(void) const = 0;
	virtual int		GetRecipientIndex(int slot) const = 0;
};

class CRecipientFilter : public IRecipientFilter
{
public:
	bool IsReliable() const
	{
		return false;
	}
	bool IsInitMessage() const
	{
		return false;
	}
	int GetRecipientCount() const 
	{
		return m_Recipients.Count();
	}
	int GetRecipientIndex(int slot) const
	{
		if (slot < 0 || slot >= GetRecipientCount())
			return -1;

		return m_Recipients[slot];
	}

	void AddRecipient(CBaseEntity* player)
	{
		Assert(player);

		int index = IndexOfEdict(player->GetNetworkable()->GetEdict());

		// If we're predicting and this is not the first time we've predicted this sound
		//  then don't send it to the local player again.
		//if (false)//m_bUsingPredictionRules)
		//{
			// Only add local player if this is the first time doing prediction
			//if (g_RecipientFilterPredictionSystem.GetSuppressHost() == player)
			//{
			//	return;
			//}
		//}

		// Already in list
		if (m_Recipients.Find(index) != m_Recipients.InvalidIndex())
			return;

		m_Recipients.AddToTail(index);
	}

	CUtlVector< int >	m_Recipients;
};

extern CBaseEntity* UTIL_GetLocalPlayer();

CON_COMMAND(flash_indicator, "test flash f-stop indicator")
{
	int msg_type = 35;//usermessages->LookupUserMessage(messagename);

	CRecipientFilter filter;
	filter.AddRecipient(UTIL_GetLocalPlayer());
	bf_write * buffer = engine->UserMessageBegin(&filter, msg_type, "ControlHelperAnimate");
	buffer->WriteChar(true);
	buffer->WriteChar(1);
	engine->MessageEnd();
}
*/