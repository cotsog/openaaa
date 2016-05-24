
#include <nss/pkcs11.h>
#include <sys/compiler.h>
#include <sys/cpu.h>
#include <sys/decls.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <ctypes/lib.h>
#include <ctypes/string.h>
#include <mem/stack.h>
#include <mem/pool.h>
#include <hash/fn.h>

#include <aaa/lib.h>
#include <aaa/net/tls/lib.h>

#define PKCS11_LIBRARY_DESC    "OpenAAA PKCS11 Module"
#define PKCS11_MANUFACTURER_ID "OpenAAA"
#define PKCS11_SLOT_DESC       "OpenAAA PKCS11 Slot"

#define AAA_HANDLER   "qrview.exe"
#define AAA_AUTHORITY "orangebox-p2.aducid.com"

static volatile bool pkcs11_running = false;

static CK_FUNCTION_LIST pkcs11 = {
	.C_Initialize        = C_Initialize,
	.C_Finalize          = C_Finalize,
	.C_GetInfo           = C_GetInfo,
	.C_GetFunctionList   = C_GetFunctionList,
	.C_GetSlotList       = C_GetSlotList,
	.C_GetSlotInfo       = C_GetSlotInfo,
	.C_GetMechanismList  = C_GetMechanismList,
	.C_GetMechanismInfo  = C_GetMechanismInfo,
	.C_InitToken         = C_InitToken,
	.C_OpenSession       = C_OpenSession,
	.C_CloseSession      = C_CloseSession,
	.C_CloseAllSessions  = C_CloseAllSessions,
	.C_GetFunctionStatus = C_GetFunctionStatus,
	.C_CancelFunction    = C_CancelFunction,	
	.C_WaitForSlotEvent  = C_WaitForSlotEvent
};

#ifdef CONFIG_WINDOWS
#include <windows.h>
#include <wingdi.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void
win32_io_init(void)
{
	int hConHandle;
	long lStdHandle;
	int iVar;

	if (!AttachConsole (ATTACH_PARENT_PROCESS))
		return;

	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	FILE *fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
}
#endif

static FILE *fp;

static void
pkcs11_sys_log(void *usr, unsigned level, const char *msg)
{
	fprintf(fp, "%s", msg);
	fflush(fp);
}

CK_RV C_Initialize
(CK_VOID_PTR pInitArgs)
{
	pkcs11_running = true;

#ifdef CONFIG_WINDOWS
	win32_io_init();
	fp = stdout;
#endif

//	fp = fopen ("aaa-pkcs11.log", "w");

	sys_log_custom(pkcs11_sys_log, NULL);

	sys_dbg("openaaa:pkcs11:%s", __func__);

	tls_set_authority(AAA_AUTHORITY);
	tls_set_handler(AAA_HANDLER);

	struct tls_ekm tls_ekm = (struct tls_ekm) {
		.label_size = strlen(TLS_RFC5705_LABEL),
		.size = 16
	};

        snprintf(tls_ekm.label, sizeof(tls_ekm.label) - 1, "%s", 
	         TLS_RFC5705_LABEL);
        tls_set_ekm_attrs(&tls_ekm);	

	aaa_init();
	tls_init();
	return CKR_OK;
}

CK_RV
C_Finalize(CK_VOID_PTR pReserved)
{
	sys_dbg("openaaa:pkcs11:%s", __func__);
	tls_fini();
	aaa_fini();
	//fclose(fp);
	pkcs11_running = false;
	return CKR_OK;
}

CK_RV
C_GetInfo(CK_INFO_PTR info)
{
	sys_dbg("openaaa:pkcs11:%s", __func__);

	memset(info->libraryDescription, ' ', 32);
	memset(info->manufacturerID,     ' ', 32);
	sprintf((char *)info->libraryDescription, "%.*s", 
	        (int)strlen(PKCS11_LIBRARY_DESC), PKCS11_LIBRARY_DESC);
	sprintf((char *)info->manufacturerID,     "%.*s", 
	        (int)strlen(PKCS11_MANUFACTURER_ID), PKCS11_MANUFACTURER_ID);

	info->libraryVersion.major = 1;
	info->libraryVersion.minor = 1;
	info->cryptokiVersion.major = 2;
	info->cryptokiVersion.minor = 20;
	info->flags = 0;

	return CKR_OK;
}

CK_RV
C_GetSlotList(CK_BBOOL tok, CK_SLOT_ID_PTR id, CK_ULONG_PTR count)
{
	sys_dbg("openaaa:pkcs11:%s token=%d list=%p, count = %p/%d", __func__, 
	        tok, id, count, (int)*count);

	if (tok == CK_TRUE) {
		*count = 0;
		return CKR_OK;
	}

	*count = 1;

	if (!id)
		return CKR_OK;

	*id = (CK_SLOT_ID)(0x1);
	return CKR_OK;
}

CK_RV
C_GetSlotInfo(CK_SLOT_ID id, CK_SLOT_INFO_PTR slot)
{
	sys_dbg("openaaa:pkcs11:%s id=%p", __func__, (void *)id);

	memset(slot->slotDescription, ' ', 64);
	memset(slot->manufacturerID,  ' ', 32);

	sprintf((char *)slot->slotDescription, "%.*s", 
	        (int)strlen(PKCS11_SLOT_DESC), PKCS11_SLOT_DESC);
	sprintf((char *)slot->manufacturerID, "%.*s", 
	        (int)strlen(PKCS11_MANUFACTURER_ID), PKCS11_MANUFACTURER_ID);

	slot->flags = CKF_REMOVABLE_DEVICE;

	slot->hardwareVersion.major = 1;
	slot->hardwareVersion.minor = 1;
	slot->firmwareVersion.major = 1;
	slot->firmwareVersion.minor = 1;

	return CKR_OK;
}

CK_RV
C_GetTokenInfo(CK_SLOT_ID id, CK_TOKEN_INFO_PTR token)
{
	sys_dbg("openaaa:pkcs11:%s", __func__);
	return CKR_TOKEN_NOT_PRESENT;
}

CK_RV
C_GetMechanismList(CK_SLOT_ID id, CK_MECHANISM_TYPE_PTR type,
                   CK_ULONG_PTR count) 
{
	sys_dbg("openaaa:pkcs11:%s type=%p count=%p/%d", __func__, 
                 type, count, *count);
	*count = 0;
	if (!type)
		return CKR_OK;

	return CKR_OK;
}


CK_RV
C_GetMechanismInfo(CK_SLOT_ID id, CK_MECHANISM_TYPE type, 
                   CK_MECHANISM_INFO_PTR info)
{
	sys_dbg("openaaa:pkcs11:%s", __func__);
	return CKR_OK;
}

CK_RV
C_InitToken(CK_SLOT_ID id, CK_UTF8CHAR_PTR pin,
            CK_ULONG len, CK_UTF8CHAR_PTR label)
{
	sys_dbg("openaaa:pkcs11:%s", __func__);
	return CKR_OK;
}

CK_RV
C_OpenSession(CK_SLOT_ID id,CK_FLAGS  flags, CK_VOID_PTR app, 
              CK_NOTIFY  notify, CK_SESSION_HANDLE_PTR hdl)
{
	sys_dbg("openaaa:pkcs11:%s", __func__);
	return CKR_TOKEN_NOT_PRESENT;
}


CK_RV
C_CloseSession(CK_SESSION_HANDLE hdl)
{
	sys_dbg("openaaa:pkcs11:%s", __func__);
	return CKR_TOKEN_NOT_PRESENT;
}


CK_RV
C_CloseAllSessions(CK_SLOT_ID id)
{
	sys_dbg("openaaa:pkcs11:%s", __func__);
	return CKR_TOKEN_NOT_PRESENT;
}

CK_RV
C_GetFunctionList(CK_FUNCTION_LIST_PTR_PTR list)
{
	*list = &pkcs11;
	return CKR_OK;
}

CK_RV
C_GetFunctionStatus(CK_SESSION_HANDLE sess)
{
	sys_dbg("openaaa:pkcs11:%s", __func__);
	return CKR_OK;
}

CK_RV
C_CancelFunction(CK_SESSION_HANDLE hSession)
{
	sys_dbg("openaaa:pkcs11:%s", __func__);
	return CKR_OK;
}

CK_RV
C_WaitForSlotEvent(CK_FLAGS flags, CK_SLOT_ID_PTR slot, CK_VOID_PTR arg)
{

	sys_dbg("openaaa:pkcs11:%s flag=%d", __func__, flags);
	return CKR_NO_EVENT;

	if (pkcs11_running == false)
		return CKR_CRYPTOKI_NOT_INITIALIZED;

	if (flags & CKF_DONT_BLOCK)
		return CKR_NO_EVENT;

	sys_dbg("openaaa:pkcs11:%s signalling", __func__);
	return CKR_NO_EVENT;
}
