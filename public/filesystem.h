//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include <limits.h>

#include "tier0/threadtools.h"
#include "tier0/memalloc.h"
#include "tier0/tslist.h"
#include "tier1/interface.h"
#include "tier1/utlsymbol.h"
#include "tier1/utlstring.h"
#include "tier1/functors.h" 
#include "tier1/checksum_crc.h"
#include "tier1/utlqueue.h"
#include "appframework/IAppSystem.h"
#include "tier2/tier2.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

class CUtlBuffer;
class KeyValues;
class IFileList;

typedef void * FileHandle_t;
typedef int FileFindHandle_t;
typedef void (*FileSystemLoggingFunc_t)( const char *fileName, const char *accessType );
typedef int WaitForResourcesHandle_t;

#ifdef _X360
typedef void* HANDLE;
#endif

//-----------------------------------------------------------------------------
// Enums used by the interface
//-----------------------------------------------------------------------------

#define FILESYSTEM_MAX_SEARCH_PATHS 128

enum FileSystemSeek_t
{
	FILESYSTEM_SEEK_HEAD	= SEEK_SET,
	FILESYSTEM_SEEK_CURRENT = SEEK_CUR,
	FILESYSTEM_SEEK_TAIL	= SEEK_END,
};

enum
{
	FILESYSTEM_INVALID_FIND_HANDLE = -1
};

enum FileWarningLevel_t
{
	// A problem!
	FILESYSTEM_WARNING = -1,

	// Don't print anything
	FILESYSTEM_WARNING_QUIET = 0,

	// On shutdown, report names of files left unclosed
	FILESYSTEM_WARNING_REPORTUNCLOSED,

	// Report number of times a file was opened, closed
	FILESYSTEM_WARNING_REPORTUSAGE,

	// Report all open/close events to console ( !slow! )
	FILESYSTEM_WARNING_REPORTALLACCESSES,

	// Report all open/close/read events to the console ( !slower! )
	FILESYSTEM_WARNING_REPORTALLACCESSES_READ,

	// Report all open/close/read/write events to the console ( !slower! )
	FILESYSTEM_WARNING_REPORTALLACCESSES_READWRITE,

	// Report all open/close/read/write events and all async I/O file events to the console ( !slower(est)! )
	FILESYSTEM_WARNING_REPORTALLACCESSES_ASYNC,

};

// search path filtering
enum PathTypeFilter_t
{
	FILTER_NONE        = 0,	// no filtering, all search path types match
	FILTER_CULLPACK    = 1,	// pack based search paths are culled (maps and zips)
	FILTER_CULLNONPACK = 2,	// non-pack based search paths are culled
};

// search path querying (bit flags)
enum
{
	PATH_IS_NORMAL      = 0x00, // normal path, not pack based
	PATH_IS_PACKFILE    = 0x01, // path is a pack file
	PATH_IS_MAPPACKFILE = 0x02, // path is a map pack file
	PATH_IS_DVDDEV		= 0x04, // path is the dvddev cache
};
typedef uint32 PathTypeQuery_t;

#define IS_PACKFILE( n ) ( n & ( PATH_IS_PACKFILE | PATH_IS_MAPPACKFILE ) )
#define IS_DVDDEV( n )   ( n & PATH_IS_DVDDEV )

enum DVDMode_t
{
	DVDMODE_OFF    = 0, // not using dvd
	DVDMODE_STRICT = 1, // dvd device only
	DVDMODE_DEV    = 2, // dev mode, mutiple devices ok
	DVDMODE_DEV_VISTA = 3, // dev mode from a vista host, mutiple devices ok
};

// In non-retail builds, enable the file blocking access tracking stuff...
#if defined( TRACK_BLOCKING_IO )
enum FileBlockingWarning_t
{
	// Report how long synchronous i/o took to complete
	FILESYSTEM_BLOCKING_SYNCHRONOUS = 0,
	// Report how long async i/o took to complete if AsyncFileFinished caused it to load via "blocking" i/o
	FILESYSTEM_BLOCKING_ASYNCHRONOUS_BLOCK,
	// Report how long async i/o took to complete
	FILESYSTEM_BLOCKING_ASYNCHRONOUS,
	// Report how long the async "callback" took
	FILESYSTEM_BLOCKING_CALLBACKTIMING,

	FILESYSTEM_BLOCKING_NUMBINS,
};

#pragma pack(1)
class FileBlockingItem
{
public:
	enum
	{
		FB_ACCESS_OPEN = 1,
		FB_ACCESS_CLOSE = 2,
		FB_ACCESS_READ = 3,
		FB_ACCESS_WRITE = 4,
		FB_ACCESS_APPEND = 5,
		FB_ACCESS_SIZE = 6
	};

	FileBlockingItem() :
		m_ItemType( (FileBlockingWarning_t)0 ),
		m_flElapsed( 0.0f ),
		m_nAccessType( 0 )
	{
		SetFileName( NULL );
	}

	FileBlockingItem( int type, char const *filename, float elapsed, int accessType ) :
		m_ItemType( (FileBlockingWarning_t)type ),
		m_flElapsed( elapsed ),
		m_nAccessType( accessType )
	{
		SetFileName( filename );
	}

	void SetFileName( char const *filename )
	{
		if ( !filename )
		{
			m_szFilename[ 0 ] = 0;
			return;
		}

		int len = Q_strlen( filename );
		if ( len >= sizeof( m_szFilename ) )
		{
			Q_strncpy( m_szFilename, &filename[ len - sizeof( m_szFilename ) + 1 ], sizeof( m_szFilename ) );
		}
		else
		{
			Q_strncpy( m_szFilename, filename, sizeof( m_szFilename ) );
		}
	}

	char const *GetFileName() const
	{
		return m_szFilename;
	}

	FileBlockingWarning_t	m_ItemType;
	float					m_flElapsed;
	byte					m_nAccessType;
private:

	char					m_szFilename[ 32 ];
};
#pragma pack()

class IBlockingFileItemList
{
public:
	
	// You can't call any of the below calls without locking first
	virtual void	LockMutex() = 0;
	virtual void	UnlockMutex() = 0;

	virtual int		First() const = 0;
	virtual int		Next( int i ) const = 0;
	virtual int		InvalidIndex() const = 0;

	virtual const	FileBlockingItem& Get( int index ) const = 0;

	virtual void	Reset() = 0;
};

#endif // TRACK_BLOCKING_IO

enum FilesystemMountRetval_t
{
	FILESYSTEM_MOUNT_OK = 0,
	FILESYSTEM_MOUNT_FAILED,
};

enum SearchPathAdd_t
{
	PATH_ADD_TO_HEAD,			// First path searched
	PATH_ADD_TO_TAIL,			// Last path searched
	PATH_ADD_TO_TAIL_ATINDEX,	// First path searched
};

enum FilesystemOpenExFlags_t
{
	FSOPEN_UNBUFFERED		= (1 << 0),
	FSOPEN_FORCE_TRACK_CRC	= (1 << 1),		// This makes it calculate a CRC for the file (if the file came from disk) regardless 
											// of the IFileList passed to RegisterFileWhitelist.
	FSOPEN_NEVERINPACK	    = (1 << 2),		// 360 only, hint to FS that file is not allowed to be in pack file
};

#define FILESYSTEM_INVALID_HANDLE	( FileHandle_t )0

//-----------------------------------------------------------------------------
// Structures used by the interface
//-----------------------------------------------------------------------------

struct FileSystemStatistics
{
	CInterlockedUInt	nReads,		
						nWrites,		
						nBytesRead,
						nBytesWritten,
						nSeeks;
};

//-----------------------------------------------------------------------------
// File system allocation functions. Client must free on failure
//-----------------------------------------------------------------------------
typedef void *(*FSAllocFunc_t)( const char *pszFilename, unsigned nBytes );


//-----------------------------------------------------------------------------
// Used to display dirty disk error functions
//-----------------------------------------------------------------------------
typedef void (*FSDirtyDiskReportFunc_t)();


//-----------------------------------------------------------------------------
// Asynchronous support types
//-----------------------------------------------------------------------------
DECLARE_POINTER_HANDLE(FSAsyncControl_t);
DECLARE_POINTER_HANDLE(FSAsyncFile_t);
const FSAsyncFile_t FS_INVALID_ASYNC_FILE = (FSAsyncFile_t)(0x0000ffff);


//---------------------------------------------------------
// Async file status
//---------------------------------------------------------
enum FSAsyncStatus_t
{
	FSASYNC_ERR_ALIGNMENT    = -6,	// read parameters invalid for unbuffered IO
	FSASYNC_ERR_FAILURE      = -5,	// hard subsystem failure
	FSASYNC_ERR_READING      = -4,	// read error on file
	FSASYNC_ERR_NOMEMORY     = -3,	// out of memory for file read
	FSASYNC_ERR_UNKNOWNID    = -2,	// caller's provided id is not recognized
	FSASYNC_ERR_FILEOPEN     = -1,	// filename could not be opened (bad path, not exist, etc)
	FSASYNC_OK               = 0,	// operation is successful
	FSASYNC_STATUS_PENDING,			// file is properly queued, waiting for service
	FSASYNC_STATUS_INPROGRESS,		// file is being accessed
	FSASYNC_STATUS_ABORTED,			// file was aborted by caller
	FSASYNC_STATUS_UNSERVICED,		// file is not yet queued
};

//---------------------------------------------------------
// Async request flags
//---------------------------------------------------------
enum FSAsyncFlags_t
{
	FSASYNC_FLAGS_ALLOCNOFREE		= ( 1 << 0 ),	// do the allocation for dataPtr, but don't free
	FSASYNC_FLAGS_FREEDATAPTR		= ( 1 << 1 ),	// free the memory for the dataPtr post callback
	FSASYNC_FLAGS_SYNC				= ( 1 << 2 ),	// Actually perform the operation synchronously. Used to simplify client code paths
	FSASYNC_FLAGS_NULLTERMINATE		= ( 1 << 3 ),	// allocate an extra byte and null terminate the buffer read in
};

//---------------------------------------------------------
// Return value for CheckFileCRC.
//---------------------------------------------------------
enum EFileCRCStatus
{
	k_eFileCRCStatus_CantOpenFile,		// We don't have this file. 
	k_eFileCRCStatus_GotCRC
};

// Used in CacheFileCRCs.
enum ECacheCRCType
{
	k_eCacheCRCType_SingleFile,
	k_eCacheCRCType_Directory,
	k_eCacheCRCType_Directory_Recursive
};

//---------------------------------------------------------
// Optional completion callback for each async file serviced (or failed)
// call is not reentrant, async i/o guaranteed suspended until return
// Note: If you change the signature of the callback, you will have to account for it in FileSystemV12 (toml [4/18/2005] )
//---------------------------------------------------------
struct FileAsyncRequest_t;
typedef void (*FSAsyncCallbackFunc_t)(const FileAsyncRequest_t &request, int nBytesRead, FSAsyncStatus_t err);

//-----------------------------------------------------------------------------
// Used to add results from async directory scans
//-----------------------------------------------------------------------------
typedef void (*FSAsyncScanAddFunc_t)( void* pContext, char* pFoundPath, char* pFoundFile );
typedef void (*FSAsyncScanCompleteFunc_t)( void* pContext, FSAsyncStatus_t err );


//---------------------------------------------------------
// Description of an async request
//---------------------------------------------------------
struct FileAsyncRequest_t
{
	FileAsyncRequest_t()	{ memset( this, 0, sizeof(*this) ); hSpecificAsyncFile = FS_INVALID_ASYNC_FILE;	}
	const char *			pszFilename;		// file system name
	void *					pData;				// optional, system will alloc/free if NULL
	int						nOffset;			// optional initial seek_set, 0=beginning
	int						nBytes;				// optional read clamp, -1=exist test, 0=full read
	FSAsyncCallbackFunc_t	pfnCallback;		// optional completion callback
	void *					pContext;			// caller's unique file identifier
	int						priority;			// inter list priority, 0=lowest
	unsigned				flags;				// behavior modifier
	const char *			pszPathID;			// path ID (NOTE: this field is here to remain binary compatible with release HL2 filesystem interface)
	FSAsyncFile_t			hSpecificAsyncFile; // Optional hint obtained using AsyncBeginRead()
	FSAllocFunc_t			pfnAlloc;			// custom allocator. can be null. not compatible with FSASYNC_FLAGS_FREEDATAPTR
};


class CUnverifiedCRCFile
{
public:
	char m_PathID[MAX_PATH];
	char m_Filename[MAX_PATH];
	CRC32_t m_CRC;
};


// Spew flags for SetWhitelistSpewFlags (set with the fs_whitelist_spew_flags cvar).
// Update the comment for the fs_whitelist_spew_flags cvar if you change these.
#define WHITELIST_SPEW_WHILE_LOADING		0x0001	// list files as they are added to the CRC tracker
#define WHITELIST_SPEW_RELOAD_FILES			0x0002	// show files the filesystem is telling the engine to reload
#define WHITELIST_SPEW_DONT_RELOAD_FILES	0x0004	// show files the filesystem is NOT telling the engine to reload



// DLC license mask flags is 32 publisher defined bits
// MSW 16 bits in 8.8: Type.SubVersion
// LSW 16 bits: Flags

// return id component
#define DLC_LICENSE_ID( x )				( ( ( (unsigned int)( x ) ) >> 24 ) & 0x000000FF )
// returns minor version component (not generally used, i.e. we dont rev dlc's yet)
#define DLC_LICENSE_MINORVERSION( x )	( ( ( (unsigned int)( x ) ) >> 16 ) & 0x000000FF )
// returns license flags
#define DLC_LICENSE_FLAGS( x )			( ( ( (unsigned int)( x ) ) & 0x0000FFFF ) )

#define DLCFLAGS_PRESENCE_ONLY			0x0001	// causes no search path loadout



//-----------------------------------------------------------------------------
// Base file system interface
//-----------------------------------------------------------------------------

// This is the minimal interface that can be implemented to provide access to
// a named set of files.
#define BASEFILESYSTEM_INTERFACE_VERSION		"VBaseFileSystem011"

abstract_class IBaseFileSystem
{
public:
	virtual int				Read( void* pOutput, int size, FileHandle_t file ) = 0;
	virtual int				Write( void const* pInput, int size, FileHandle_t file ) = 0;

	// if pathID is NULL, all paths will be searched for the file
	virtual FileHandle_t	Open( const char *pFileName, const char *pOptions, const char *pathID = 0 ) = 0;
	virtual void			Close( FileHandle_t file ) = 0;


	virtual void			Seek( FileHandle_t file, int pos, FileSystemSeek_t seekType ) = 0;
	virtual unsigned int	Tell( FileHandle_t file ) = 0;
	virtual unsigned int	Size( FileHandle_t file ) = 0;
	virtual unsigned int	Size( const char *pFileName, const char *pPathID = 0 ) = 0;

	virtual void			Flush( FileHandle_t file ) = 0;
	virtual bool			Precache( const char *pFileName, const char *pPathID = 0 ) = 0;

	virtual bool			FileExists( const char *pFileName, const char *pPathID = 0 ) = 0;
	virtual bool			IsFileWritable( char const *pFileName, const char *pPathID = 0 ) = 0;
	virtual bool			SetFileWritable( char const *pFileName, bool writable, const char *pPathID = 0 ) = 0;

	virtual long			GetFileTime( const char *pFileName, const char *pPathID = 0 ) = 0;

	//--------------------------------------------------------
	// Reads/writes files to utlbuffers. Use this for optimal read performance when doing open/read/close
	//--------------------------------------------------------
	virtual bool			ReadFile( const char *pFileName, const char *pPath, CUtlBuffer &buf, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = NULL ) = 0;
	virtual bool			WriteFile( const char *pFileName, const char *pPath, CUtlBuffer &buf ) = 0;
	virtual bool			UnzipFile( const char *pFileName, const char *pPath, const char *pDestination ) = 0;
};


//-----------------------------------------------------------------------------
// Main file system interface
//-----------------------------------------------------------------------------
abstract_class IFileSystem : public IAppSystem, public IBaseFileSystem
{
public:
	//--------------------------------------------------------
	// Steam operations
	//--------------------------------------------------------

	virtual bool			IsSteam() const = 0;

	// Supplying an extra app id will mount this app in addition 
	// to the one specified in the environment variable "steamappid"
	// 
	// If nExtraAppId is < -1, then it will mount that app ID only.
	// (Was needed by the dedicated server b/c the "SteamAppId" env var only gets passed to steam.dll
	// at load time, so the dedicated couldn't pass it in that way).
	virtual	FilesystemMountRetval_t MountSteamContent( int nExtraAppId = -1 ) = 0;

	//--------------------------------------------------------
	// Search path manipulation
	//--------------------------------------------------------

	// Add paths in priority order (mod dir, game dir, ....)
	// If one or more .pak files are in the specified directory, then they are
	//  added after the file system path
	// If the path is the relative path to a .bsp file, then any previous .bsp file 
	//  override is cleared and the current .bsp is searched for an embedded PAK file
	//  and this file becomes the highest priority search path ( i.e., it's looked at first
	//   even before the mod's file system path ).
	virtual void			AddSearchPath( const char *pPath, const char *pathID, SearchPathAdd_t addType = PATH_ADD_TO_TAIL ) = 0;
	virtual bool			RemoveSearchPath( const char *pPath, const char *pathID = 0 ) = 0;

	// Remove all search paths (including write path?)
	virtual void			RemoveAllSearchPaths( void ) = 0;

	// Remove search paths associated with a given pathID
	virtual void			RemoveSearchPaths( const char *szPathID ) = 0;

	// This is for optimization. If you mark a path ID as "by request only", then files inside it
	// will only be accessed if the path ID is specifically requested. Otherwise, it will be ignored.
	// If there are currently no search paths with the specified path ID, then it will still
	// remember it in case you add search paths with this path ID.
	virtual void			MarkPathIDByRequestOnly( const char *pPathID, bool bRequestOnly ) = 0;

	// converts a partial path into a full path
	virtual const char		*RelativePathToFullPath( const char *pFileName, const char *pPathID, char *pLocalPath, int localPathBufferSize, PathTypeFilter_t pathFilter = FILTER_NONE, PathTypeQuery_t *pPathType = NULL ) = 0;

	// Returns the search path, each path is separated by ;s. Returns the length of the string returned
	virtual int				GetSearchPath( const char *pathID, bool bGetPackFiles, char *pPath, int nMaxLen ) = 0;

	// interface for custom pack files > 4Gb
	virtual bool			AddPackFile( const char *fullpath, const char *pathID ) = 0;

	//--------------------------------------------------------
	// File manipulation operations
	//--------------------------------------------------------

	// Deletes a file (on the WritePath)
	virtual void			RemoveFile( char const* pRelativePath, const char *pathID = 0 ) = 0;

	// Renames a file (on the WritePath)
	virtual bool			RenameFile( char const *pOldPath, char const *pNewPath, const char *pathID = 0 ) = 0;

	// create a local directory structure
	virtual void			CreateDirHierarchy( const char *path, const char *pathID = 0 ) = 0;

	// File I/O and info
	virtual bool			IsDirectory( const char *pFileName, const char *pathID = 0 ) = 0;

	virtual void			FileTimeToString( char* pStrip, int maxCharsIncludingTerminator, long fileTime ) = 0;

	//--------------------------------------------------------
	// Open file operations
	//--------------------------------------------------------

	virtual void			SetBufferSize( FileHandle_t file, unsigned nBytes ) = 0;

	virtual bool			IsOk( FileHandle_t file ) = 0;

	virtual bool			EndOfFile( FileHandle_t file ) = 0;

	virtual char			*ReadLine( char *pOutput, int maxChars, FileHandle_t file ) = 0;
	virtual int				FPrintf( FileHandle_t file, const char *pFormat, ... ) FMTFUNCTION( 3, 4 ) = 0;

	//--------------------------------------------------------
	// Dynamic library operations
	//--------------------------------------------------------

	// load/unload modules
	virtual CSysModule 		*LoadModule( const char *pFileName, const char *pPathID = 0, bool bValidatedDllOnly = true ) = 0;
	virtual void			UnloadModule( CSysModule *pModule ) = 0;

	//--------------------------------------------------------
	// File searching operations
	//--------------------------------------------------------

	// FindFirst/FindNext. Also see FindFirstEx.
	virtual const char		*FindFirst( const char *pWildCard, FileFindHandle_t *pHandle ) = 0;
	virtual const char		*FindNext( FileFindHandle_t handle ) = 0;
	virtual bool			FindIsDirectory( FileFindHandle_t handle ) = 0;
	virtual void			FindClose( FileFindHandle_t handle ) = 0;

	// Same as FindFirst, but you can filter by path ID, which can make it faster.
	virtual const char		*FindFirstEx( 
		const char *pWildCard, 
		const char *pPathID,
		FileFindHandle_t *pHandle
		) = 0;

	virtual void			FindFileAbsoluteList(
		CUtlVector<CUtlString, CUtlMemory<CUtlString, int>> &output,
		const char *pWildCard,
		const char *pPathID
		) = 0;

	//--------------------------------------------------------
	// File name and directory operations
	//--------------------------------------------------------

	// FIXME: This method is obsolete! Use RelativePathToFullPath instead!
	// converts a partial path into a full path
	virtual const char		*GetLocalPath( const char *pFileName, char *pLocalPath, int localPathBufferSize ) = 0;

	// Returns true on success ( based on current list of search paths, otherwise false if 
	//  it can't be resolved )
	virtual bool			FullPathToRelativePath( const char *pFullpath, char *pRelative, int maxlen ) = 0;

	// Gets the current working directory
	virtual bool			GetCurrentDirectory( char* pDirectory, int maxlen ) = 0;

	//--------------------------------------------------------
	// Filename dictionary operations
	//--------------------------------------------------------

	virtual FileNameHandle_t	FindOrAddFileName( char const *pFileName ) = 0;
	virtual bool				String( const FileNameHandle_t& handle, char *buf, int buflen ) = 0;

	//--------------------------------------------------------
	// Asynchronous file operations
	//--------------------------------------------------------

	//------------------------------------
	// Global operations
	//------------------------------------
			FSAsyncStatus_t	AsyncRead( const FileAsyncRequest_t &request, FSAsyncControl_t *phControl = NULL )	{ return AsyncReadMultiple( &request, 1, phControl ); 	}
	virtual FSAsyncStatus_t	AsyncReadMultiple( const FileAsyncRequest_t *pRequests, int nRequests,  FSAsyncControl_t *phControls = NULL ) = 0;
	virtual FSAsyncStatus_t	AsyncAppend(const char *pFileName, const void *pSrc, int nSrcBytes, bool bFreeMemory, FSAsyncControl_t *pControl = NULL ) = 0;
	virtual FSAsyncStatus_t	AsyncAppendFile(const char *pAppendToFileName, const char *pAppendFromFileName, FSAsyncControl_t *pControl = NULL ) = 0;
	virtual void			AsyncFinishAll( int iToPriority = 0 ) = 0;
	virtual void			AsyncFinishAllWrites() = 0;
	virtual FSAsyncStatus_t	AsyncFlush() = 0;
	virtual bool			AsyncSuspend() = 0;
	virtual bool			AsyncResume() = 0;

	//------------------------------------
	// Functions to hold a file open if planning on doing mutiple reads. Use is optional,
	// and is taken only as a hint
	//------------------------------------
	virtual FSAsyncStatus_t	AsyncBeginRead( const char *pszFile, FSAsyncFile_t *phFile ) = 0;
	virtual FSAsyncStatus_t	AsyncEndRead( FSAsyncFile_t hFile ) = 0;

	//------------------------------------
	// Request management
	//------------------------------------
	virtual FSAsyncStatus_t	AsyncFinish( FSAsyncControl_t hControl, bool wait = true ) = 0;
	virtual FSAsyncStatus_t	AsyncGetResult( FSAsyncControl_t hControl, void **ppData, int *pSize ) = 0;
	virtual FSAsyncStatus_t	AsyncAbort( FSAsyncControl_t hControl ) = 0;
	virtual FSAsyncStatus_t	AsyncStatus( FSAsyncControl_t hControl ) = 0;
	// set a new priority for a file already in the queue
	virtual FSAsyncStatus_t	AsyncSetPriority(FSAsyncControl_t hControl, int newPriority) = 0;
	virtual void			AsyncAddRef( FSAsyncControl_t hControl ) = 0;
	virtual void			AsyncRelease( FSAsyncControl_t hControl ) = 0;

	//--------------------------------------------------------
	// Remote resource management
	//--------------------------------------------------------

	// starts waiting for resources to be available
	// returns FILESYSTEM_INVALID_HANDLE if there is nothing to wait on
	virtual WaitForResourcesHandle_t WaitForResources( const char *resourcelist ) = 0;
	// get progress on waiting for resources; progress is a float [0, 1], complete is true on the waiting being done
	// returns false if no progress is available
	// any calls after complete is true or on an invalid handle will return false, 0.0f, true
	virtual bool			GetWaitForResourcesProgress( WaitForResourcesHandle_t handle, float *progress /* out */ , bool *complete /* out */ ) = 0;
	// cancels a progress call
	virtual void			CancelWaitForResources( WaitForResourcesHandle_t handle ) = 0;

	// hints that a set of files will be loaded in near future
	// HintResourceNeed() is not to be confused with resource precaching.
	virtual int				HintResourceNeed( const char *hintlist, int forgetEverything ) = 0;
	// returns true if a file is on disk
	virtual bool			IsFileImmediatelyAvailable(const char *pFileName) = 0;
	
	// copies file out of pak/bsp/steam cache onto disk (to be accessible by third-party code)
	virtual void			GetLocalCopy( const char *pFileName ) = 0;

	//--------------------------------------------------------
	// Debugging operations
	//--------------------------------------------------------

	// Dump to printf/OutputDebugString the list of files that have not been closed
	virtual void			PrintOpenedFiles( void ) = 0;
	virtual void			PrintSearchPaths( void ) = 0;

	// output
	virtual void			SetWarningFunc( void (*pfnWarning)( const char *fmt, ... ) ) = 0;
	virtual void			SetWarningLevel( FileWarningLevel_t level ) = 0;
	virtual void			AddLoggingFunc( void (*pfnLogFunc)( const char *fileName, const char *accessType ) ) = 0;
	virtual void			RemoveLoggingFunc( FileSystemLoggingFunc_t logFunc ) = 0;

	// Returns the file system statistics retreived by the implementation.  Returns NULL if not supported.
	virtual const FileSystemStatistics *GetFilesystemStatistics() = 0;

	//--------------------------------------------------------
	// Start of new functions after Lost Coast release (7/05)
	//--------------------------------------------------------

	virtual FileHandle_t	OpenEx( const char *pFileName, const char *pOptions, unsigned flags = 0, const char *pathID = 0, char **ppszResolvedFilename = NULL ) = 0;

	// Extended version of read provides more context to allow for more optimal reading
	virtual int				ReadEx( void* pOutput, int sizeDest, int size, FileHandle_t file ) = 0;
	virtual int				ReadFileEx( const char *pFileName, const char *pPath, void **ppBuf, bool bNullTerminate = false, bool bOptimalAlloc = false, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = NULL ) = 0;

	virtual FileNameHandle_t	FindFileName( char const *pFileName ) = 0;

#if defined( TRACK_BLOCKING_IO )
	virtual void			EnableBlockingFileAccessTracking( bool state ) = 0;
	virtual bool			IsBlockingFileAccessEnabled() const = 0;

	virtual IBlockingFileItemList *RetrieveBlockingFileAccessInfo() = 0;
#endif

	virtual void SetupPreloadData() = 0;
	virtual void DiscardPreloadData() = 0;

	// Fixme, we could do these via a string embedded into the compiled data, etc...
	enum KeyValuesPreloadType_t
	{
		TYPE_VMT,
		TYPE_SOUNDEMITTER,
		TYPE_SOUNDSCAPE,
		NUM_PRELOAD_TYPES
	};

	// If the "PreloadedData" hasn't been purged, then this'll try and instance the KeyValues using the fast path of compiled keyvalues loaded during startup.
	// Otherwise, it'll just fall through to the regular KeyValues loading routines
	virtual KeyValues	*LoadKeyValues( KeyValuesPreloadType_t type, char const *filename, char const *pPathID = 0 ) = 0;
	virtual bool		LoadKeyValues( KeyValues& head, KeyValuesPreloadType_t type, char const *filename, char const *pPathID = 0 ) = 0;

	virtual FSAsyncStatus_t	AsyncWrite(const char *pFileName, const void *pSrc, int nSrcBytes, bool bFreeMemory, bool bAppend = false, FSAsyncControl_t *pControl = NULL ) = 0;
	virtual FSAsyncStatus_t	AsyncWriteFile(const char *pFileName, const CUtlBuffer *pSrc, int nSrcBytes, bool bFreeMemory, bool bAppend = false, FSAsyncControl_t *pControl = NULL ) = 0;
	// Async read functions with memory blame
	FSAsyncStatus_t			AsyncReadCreditAlloc( const FileAsyncRequest_t &request, const char *pszFile, int line, FSAsyncControl_t *phControl = NULL )	{ return AsyncReadMultipleCreditAlloc( &request, 1, pszFile, line, phControl ); 	}
	virtual FSAsyncStatus_t	AsyncReadMultipleCreditAlloc( const FileAsyncRequest_t *pRequests, int nRequests, const char *pszFile, int line, FSAsyncControl_t *phControls = NULL ) = 0;

	virtual FSAsyncStatus_t AsyncDirectoryScan( const char* pSearchSpec, bool recurseFolders,  void* pContext, FSAsyncScanAddFunc_t pfnAdd, FSAsyncScanCompleteFunc_t pfnDone, FSAsyncControl_t *pControl = NULL ) = 0;

	virtual bool			GetFileTypeForFullPath( char const *pFullPath, wchar_t *buf, size_t bufSizeInBytes ) = 0;

	//--------------------------------------------------------
	//--------------------------------------------------------
	virtual bool		ReadToBuffer( FileHandle_t hFile, CUtlBuffer &buf, int nMaxBytes = 0, FSAllocFunc_t pfnAlloc = NULL ) = 0;

	//--------------------------------------------------------
	// Optimal IO operations
	//--------------------------------------------------------
	virtual bool		GetOptimalIOConstraints( FileHandle_t hFile, unsigned *pOffsetAlign, unsigned *pSizeAlign, unsigned *pBufferAlign ) = 0;
	inline unsigned		GetOptimalReadSize( FileHandle_t hFile, unsigned nLogicalSize );
	virtual void		*AllocOptimalReadBuffer( FileHandle_t hFile, unsigned nSize = 0, unsigned nOffset = 0 ) = 0;
	virtual void		FreeOptimalReadBuffer( void * ) = 0;

	//--------------------------------------------------------
	//
	//--------------------------------------------------------
	virtual void		BeginMapAccess() = 0;
	virtual void		EndMapAccess() = 0;

	// Returns true on success, otherwise false if it can't be resolved
	virtual bool		FullPathToRelativePathEx( const char *pFullpath, const char *pPathId, char *pRelative, int maxlen ) = 0;

	virtual int			GetPathIndex( const FileNameHandle_t &handle ) = 0;
	virtual long		GetPathTime( const char *pPath, const char *pPathID ) = 0;

	virtual DVDMode_t	GetDVDMode() = 0;

	//--------------------------------------------------------
	// Whitelisting for pure servers.
	//--------------------------------------------------------

	// This should be called ONCE at startup. Multiplayer games (gameinfo.txt does not contain singleplayer_only)
	// want to enable this so sv_pure works.
	virtual void			EnableWhitelistFileTracking( bool bEnable ) = 0;

	// This is called when the client connects to a server using a pure_server_whitelist.txt file.
	//
	// Files listed in pWantCRCList will have CRCs calculated for them IF they come off disk
	// (and those CRCs will come out of GetUnverifiedCRCFiles).
	//
	// Files listed in pAllowFromDiskList will be allowed to load from disk. All other files will
	// be forced to come from Steam.
	//
	// The filesystem hangs onto the whitelists you pass in here, and it will Release() them when it closes down
	// or when you call this function again.
	//
	// NOTE: The whitelists you pass in here will be accessed from multiple threads, so make sure the 
	//       IsFileInList function is thread safe.
	//
	// If pFilesToReload is non-null, the filesystem will hand back a list of files that should be reloaded because they
	// are now "dirty". For example, if you were on a non-pure server and you loaded a certain model, and then you connected
	// to a pure server that said that model had to come from Steam, then pFilesToReload would specify that model
	// and the engine should reload it so it can come from Steam.
	//
	// Be sure to call Release() on pFilesToReload.
	virtual void			RegisterFileWhitelist( IFileList *pWantCRCList, IFileList *pAllowFromDiskList, IFileList **pFilesToReload ) = 0;

	// Called when the client logs onto a server. Any files that came off disk should be marked as 
	// unverified because this server may have a different set of files it wants to guarantee.
	virtual void			MarkAllCRCsUnverified() = 0;

	// As the server loads whitelists when it transitions maps, it calls this to calculate CRCs for any files marked
	// with check_crc.   Then it calls CheckCachedFileCRC later when it gets client requests to verify CRCs.
	virtual void			CacheFileCRCs( const char *pPathname, ECacheCRCType eType, IFileList *pFilter ) = 0;
	virtual EFileCRCStatus	CheckCachedFileCRC( const char *pPathID, const char *pRelativeFilename, CRC32_t *pCRC ) = 0;

	// Fills in the list of files that have been loaded off disk and have not been verified.
	// Returns the number of files filled in (between 0 and nMaxFiles).
	//
	// This also removes any files it's returning from the unverified CRC list, so they won't be
	// returned from here again.
	// The client sends batches of these to the server to verify.
	virtual int				GetUnverifiedCRCFiles( CUnverifiedCRCFile *pFiles, int nMaxFiles ) = 0;
	
	// Control debug message output.
	// Pass a combination of WHITELIST_SPEW_ flags.
	virtual int				GetWhitelistSpewFlags() = 0;
	virtual void			SetWhitelistSpewFlags( int flags ) = 0;

	// Installs a callback used to display a dirty disk dialog
	virtual void			InstallDirtyDiskReportFunc( FSDirtyDiskReportFunc_t func ) = 0;

	virtual bool			IsLaunchedFromXboxHDD() = 0;
	virtual bool			IsInstalledToXboxHDDCache() = 0;
	virtual bool			IsDVDHosted() = 0;
	virtual bool			IsInstallAllowed() = 0;

	virtual int				GetSearchPathID( char *pPath, int nMaxLen ) = 0;
	virtual bool			FixupSearchPathsAfterInstall() = 0;
	
	virtual FSDirtyDiskReportFunc_t		GetDirtyDiskReportFunc() = 0;

	virtual void AddVPKFile( char const *pszName, SearchPathAdd_t addType = PATH_ADD_TO_TAIL ) = 0;
	virtual void RemoveVPKFile( char const *pszName ) = 0;
	virtual void GetVPKFileNames( CUtlVector<CUtlString> &destVector ) = 0;
	virtual void			RemoveAllMapSearchPaths() = 0;
	virtual void			SyncDvdDevCache() = 0;

	virtual bool			GetStringFromKVPool( CRC32_t poolKey, unsigned int key, char *pOutBuff, int buflen ) = 0;

	virtual bool			DiscoverDLC( int iController ) = 0;
	virtual int				IsAnyDLCPresent( bool *pbDLCSearchPathMounted = NULL ) = 0;
	virtual bool			GetAnyDLCInfo( int iDLC, unsigned int *pLicenseMask, wchar_t *pTitleBuff, int nOutTitleSize ) = 0;
	virtual int				IsAnyCorruptDLC() = 0;
	virtual bool			GetAnyCorruptDLCInfo( int iCorruptDLC, wchar_t *pTitleBuff, int nOutTitleSize ) = 0;
	virtual bool			AddDLCSearchPaths() = 0;
	virtual bool			IsSpecificDLCPresent( unsigned int nDLCPackage ) = 0;

	// call this to look for CPU-hogs during loading processes. When you set this, a breakpoint
	// will be issued whenever the indicated # of seconds go by without an i/o request.  Passing
	// 0.0 will turn off the functionality.
	virtual void            SetIODelayAlarm( float flThreshhold ) = 0;

};

//-----------------------------------------------------------------------------

#if defined( _X360 ) && !defined( _CERT )
extern char g_szXboxProfileLastFileOpened[MAX_PATH];
#define SetLastProfileFileRead( s ) Q_strncpy( g_szXboxProfileLastFileOpened, sizeof( g_szXboxProfileLastFileOpened), pFileName )
#define GetLastProfileFileRead() (&g_szXboxProfileLastFileOpened[0])
#else
#define SetLastProfileFileRead( s ) ((void)0)
#define GetLastProfileFileRead() NULL
#endif

#if defined( _X360 ) && defined( _BASETSD_H_ )
class CXboxDiskCacheSetter
{
public:
	CXboxDiskCacheSetter( SIZE_T newSize )
	{
		m_oldSize = XGetFileCacheSize();
		XSetFileCacheSize( newSize );
	}

	~CXboxDiskCacheSetter()
	{
		XSetFileCacheSize( m_oldSize );
	}
private:
	SIZE_T m_oldSize;
};
#define DISK_INTENSIVE() CXboxDiskCacheSetter cacheSetter( 1024*1024 )
#else
#define DISK_INTENSIVE() ((void)0)
#endif

//-----------------------------------------------------------------------------

inline unsigned IFileSystem::GetOptimalReadSize( FileHandle_t hFile, unsigned nLogicalSize ) 
{ 
	unsigned align; 
	if ( GetOptimalIOConstraints( hFile, &align, NULL, NULL ) ) 
		return AlignValue( nLogicalSize, align );
	else
		return nLogicalSize;
}

//-----------------------------------------------------------------------------

// We include this here so it'll catch compile errors in VMPI early.
#include "filesystem_passthru.h"

//-----------------------------------------------------------------------------
// Async memory tracking
//-----------------------------------------------------------------------------

#if (defined(_DEBUG) || defined(USE_MEM_DEBUG))
#define AsyncRead( a, b ) AsyncReadCreditAlloc( a, __FILE__, __LINE__, b )
#define AsyncReadMutiple( a, b, c ) AsyncReadMultipleCreditAlloc( a, b, __FILE__, __LINE__, c )
#endif

//-----------------------------------------------------------------------------
// Globals Exposed
//-----------------------------------------------------------------------------
DECLARE_TIER2_INTERFACE( IFileSystem, g_pFullFileSystem );




#endif // FILESYSTEM_H
