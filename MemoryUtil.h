#ifndef _MEMORY_UTIL_H_
#define _MEMORY_UTIL_H_

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SimpleTextOut.h>
#include <Library/BaseLib.h>

#define MEMORYMAP   2
#define ALLCATEPAGE 3
#define ALLCATEPOOL 4
#define ALLCATEINFO 5
#define FREEMEM     6

#define MEMMAP_SIZE 400
#define DESC_SIZE   48
#define PAGESIZE    4096
#define MB          1048576 //Byte


#define AllocateAnyPages    0
#define AllocateMaxAddress  1
#define AllocateAddress     2

#define EXIT 2
#define ENTER 1
#define NOACTION 5
#define ISBACKSPACE 3
#define CONTI 4

#define MAX_TRY_TO_ALLOCATE_PAGES_NUM   65536
#define MAX_TRY_TO_ALLOCATE_BYTE_NUM    268435456

//    (struct*)element   StructureType  member
#define _CR(Record, TYPE, Field)  \
    ((TYPE *) ( (CHAR8 *)(Record) - (CHAR8 *) &( ( (TYPE *) 0)->Field)))

//Read more: https://blog.fpmurphy.com/2013/03/enhanced-linked-list-support-in-gnu-efi.html#ixzz6ogvNlc4y


typedef struct _Memory_Pages_List_Entry{

  EFI_PHYSICAL_ADDRESS   address;
  UINTN                  pages;
  LIST_ENTRY             link;
  
}Memory_Pages_List_Entry;

typedef struct _Memory_Pool_List_Entry{
  
  VOID                   *ptr;
  UINTN                  bytes;
  LIST_ENTRY             link;

}Memory_Pool_List_Entry;

/**
 List all application Function.
**/
VOID
ListAllFunc (
);

/**
 Show current Memory Map.
 
 @retval EFI_SUCCESS            The operation completed successfully.
 @retval EFI_INVALID_PARAMETER  Allocate or Free Fail.
 @retval EFI_OUT_OF_RESOURCES   The pool requested could not be allocated.
**/
EFI_STATUS
ShowMemoryMap(
);

/**
 Show current Free Memory Column.
 
 
 @retval EFI_SUCCESS            The operation completed successfully.
 @retval EFI_INVALID_PARAMETER  Allocate or Free Fail.
 @retval EFI_OUT_OF_RESOURCES   The pool requested could not be allocated.
**/
EFI_STATUS
ShowFreeMemoryMap(

);

/**
 Handle the key.

 @param[IN] Key          A pointer to a key by keyboard, to do something for the key.
 
 @retval EFI_SUCCESS            The operation completed successfully.
 @retval EFI_INVALID_PARAMETER  Time is NULL.
 @retval EFI_DEVICE_ERROR       The time could not be retrieved due to hardware error.
**/
EFI_STATUS
GetKey (
);

/**
 Take the Key from standrand input, and do relative action.

 @param[IN] Key          A pointer to a key by keyboard, to do something for the key.
 
 @retval TRUE                   Exit, do not receive any more.
 @retval FALSE                  Continue read next Key input.
**/
BOOLEAN
HandlePressedKey (
  IN  EFI_INPUT_KEY  Key
 );

/**
 The function to try to allocate by AllocatePages().
**/
VOID
AllcatePage(
);


/**
 The function to try to allocate by AllocatePool().
**/
VOID
AllcatePool(
);

/**
 Select which memory type want to allocate.

 @retval UINT32            0-13 types of memory type.
 
**/
UINT32
SelectMemoryType(
)
;

/**
 Resolve 0~9 , A~F and a~f to add into a UINT32 data. Max value is max. The
 User cannot input more than max

 @param[IN]     Key          A pointer to a key by keyboard, to do something for the key.
 @param[IN OUT] *data        return value into data
 @param[IN]     count        To count how many key has goten
 @param[IN]     max          max value, the input value cannot exceed it.
 
 
 @retval EXIT                   The user do not continue.
 @retval ENTER                  The user press enter.
 @retval ISBACKSPACE            The user press backspace.
 @retval CONTI                  Input valid key.
 @retval NOACTION               Input invalid key.
**/
EFI_STATUS 
PutKey(
    IN      EFI_INPUT_KEY  Key,
    IN OUT  UINT32         *data,
    IN      UINT32         count,
    IN      UINT32         max

)
;
EFI_STATUS
AllcatePageForAllFreeMem(
)
;

EFI_STATUS
AllcatePoolForAllFreeMem(
)
;

VOID
doAllcatePages(
);

VOID
doAllcatePool(
)
;

INTN 
GetPagesNum(
);

EFI_STATUS
SetMemValue(
);

EFI_STATUS
InitialPageList(
);

EFI_STATUS
InitialPoolList(
);

EFI_STATUS
InsertPageList(
);

EFI_STATUS
InsertPoolList(
);

EFI_STATUS
RemovePageEntry(
);

EFI_STATUS
RemovePoolEntry(
);

VOID
ShowMemoryAllocateInfo(
)
;

VOID
FreeMemoryAllcate(
)
;

EFI_STATUS
FreeAllAllocateMemory(
)
;

VOID
ShowTotalSum(
);

#endif

