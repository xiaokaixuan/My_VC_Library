#include "StdAfx.h"
#include <windows.h>
#include "kxExapi.h"

BOOL kxIsObjectOnStack(LPVOID pObject)
{
	BOOL bRet(FALSE);
	int nStack(0);
	MEMORY_BASIC_INFORMATION mi = {0};
	DWORD dwGet = VirtualQuery(&bRet, &mi, sizeof(mi));
	if (dwGet > 0)
	{
		bRet = ((DWORD)pObject >= (DWORD)mi.BaseAddress && (DWORD)pObject < (DWORD)mi.BaseAddress + mi.RegionSize);
	}
	return bRet;
}

BOOL kxIsObjectOnHeap(LPVOID pObject)
{
    BOOL bRet(FALSE);
    DWORD dwHeaps = GetProcessHeaps(0, NULL);
    LPHANDLE pHeaps = new HANDLE[dwHeaps];
    if(pHeaps != NULL)
    {
        MEMORY_BASIC_INFORMATION mi = {0};
        GetProcessHeaps(dwHeaps, pHeaps);
		
        for(DWORD i=0; i<dwHeaps; ++i)
        {
            VirtualQuery((LPVOID)pHeaps[i], &mi, sizeof(mi));
            
            if((DWORD)pObject >= (DWORD)mi.BaseAddress && (DWORD)pObject < (DWORD)mi.BaseAddress + mi.RegionSize)
            {
                bRet = TRUE;
                break;
            }
        }
		delete []pHeaps;
    }	
    return bRet;
}
