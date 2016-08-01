// PowerManager.h: interface for the CPowerManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POWERMANAGER_H__0DC626C8_B21B_47A4_9D5C_F36F5A0E0F8D__INCLUDED_)
#define AFX_POWERMANAGER_H__0DC626C8_B21B_47A4_9D5C_F36F5A0E0F8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SE_BACKUP_PRIVILEGE   0x11
#define SE_RESTORE_PRIVILEGE  0x12
#define SE_SHUTDOWN_PRIVILEGE 0x13
#define SE_DEBUG_PRIVILEGE    0x14

typedef enum _SHUTDOWN_ACTION {
	ShutdownNoReboot,
	ShutdownReboot,
	ShutdownPowerOff
} SHUTDOWN_ACTION, *PSHUTDOWN_ACTION;

class CPowerManager  
{
	typedef LONG (NTAPI *fnShutdownSystem)(INT);
	typedef LONG (NTAPI *fnAdjustPrivilege)(ULONG,BOOL,BOOL,PBOOL);
public:
	CPowerManager();
	virtual ~CPowerManager();

public:
	BOOL LogOff();
	BOOL Reboot(BOOL bQuick = FALSE);
	BOOL Shutdown(BOOL bQuick = FALSE);

protected:
	HMODULE m_hDll;
	BOOL m_bOldPrivilege;
	fnAdjustPrivilege m_pAdjustPrivilege;
	fnShutdownSystem  m_pShutdownSystem;
};

#endif // !defined(AFX_POWERMANAGER_H__0DC626C8_B21B_47A4_9D5C_F36F5A0E0F8D__INCLUDED_)
