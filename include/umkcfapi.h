#ifndef _UMKCFAPI_H
#define _UMKCFAPI_H

// This file contains UMKCF definitions shared across
// kernel-mode and user-mode.

#define KCF_DEVICE_SHORT_NAME L"UMKCF"
#define KCF_DEVICE_TYPE 0x9999
#define KCF_DEVICE_NAME (L"\\Device\\" KCF_DEVICE_SHORT_NAME)
#define KCF_VERSION 1

// Parameters

typedef enum _KCF_SECURITY_LEVEL
{
    KcfSecurityNone = 0, // all clients are allowed
    KcfSecurityPrivilegeCheck = 1, // require SeDebugPrivilege
    KcfMaxSecurityLevel
} KCF_SECURITY_LEVEL, *PKCF_SECURITY_LEVEL;

typedef ULONG KCF_CALLBACK_ID, *PKCF_CALLBACK_ID;

#define KCF_CATEGORY_FRAMEWORK 0x1
#define KCF_CATEGORY_PROCESS 0x2
#define KCF_CATEGORY_OBJECT 0x4
#define KCF_CATEGORY_REGISTRY 0x8
#define KCF_CATEGORY_FILE 0x10

// Process
#define KCF_PROCESS_EVENT_CREATE_PROCESS 0
#define KCF_PROCESS_EVENT_EXIT_PROCESS 1
#define KCF_PROCESS_EVENT_CREATE_THREAD 2
#define KCF_PROCESS_EVENT_EXIT_THREAD 3
#define KCF_PROCESS_EVENT_LOAD_IMAGE 4

typedef struct _KCF_EVENT_ID
{
    union
    {
        struct
        {
            USHORT Category;
            USHORT Event;
        };
        ULONG Value;
    };
} KCF_EVENT_ID, *PKCF_EVENT_ID;

#define KCF_MAKE_EVENT_ID_VALUE(Category, Event) ((ULONG)(USHORT)(Category) + ((ULONG)(USHORT)(Event) << 16))

FORCEINLINE KCF_EVENT_ID KcfMakeEventId(
    __in USHORT Category,
    __in USHORT Event
    )
{
    KCF_EVENT_ID eventId;

    eventId.Category = Category;
    eventId.Event = Event;

    return eventId;
}

FORCEINLINE BOOLEAN KcfEqualEventId(
    __in KCF_EVENT_ID EventId1,
    __in KCF_EVENT_ID EventId2
    )
{
    return EventId1.Value == EventId2.Value;
}

typedef struct _KCF_CALLBACK_DATA
{
    KCF_EVENT_ID EventId;
    CLIENT_ID ClientId; // ID of source thread
    LARGE_INTEGER TimeStamp;

    union
    {
        struct
        {
            HANDLE ProcessId;
            HANDLE ParentProcessId;
            CLIENT_ID CreatingThreadId;
            UNICODE_STRING ImageFileName;
            UNICODE_STRING CommandLine;
            BOOLEAN FileOpenNameAvailable;
        } CreateProcess;
        struct
        {
            HANDLE ProcessId;
        } ExitProcess;
    } Parameters;
} KCF_CALLBACK_DATA, *PKCF_CALLBACK_DATA;

typedef struct _KCF_CALLBACK_RETURN_DATA
{
    KCF_EVENT_ID EventId;

    union
    {
        struct
        {
            NTSTATUS CreationStatus;
        } CreateProcess;
    } Parameters;
} KCF_CALLBACK_RETURN_DATA, *PKCF_CALLBACK_RETURN_DATA;

typedef enum _KCF_FILTER_KEY
{
    FilterKeyNone,
    FilterKeyProcessId, // i: source process ID
    FilterKeyProcessName, // s: source process name
    FilterKeyProcessFileName, // s: source process file name
    FilterKeyPath // s: file name or registry path
} KCF_FILTER_KEY;

// Control codes

#define KCF_CTL_CODE(x) CTL_CODE(KCF_DEVICE_TYPE, 0x800 + x, METHOD_NEITHER, FILE_ANY_ACCESS)

#define KCF_QUERYVERSION KCF_CTL_CODE(0)
#define KCF_REMOVECALLBACK KCF_CTL_CODE(1)
#define KCF_RETURNCALLBACK KCF_CTL_CODE(2)

#endif