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

//          node   StructureType  RequeastElement
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


VOID
ListAllFunc (
);

EFI_STATUS
ShowMemoryMap(
);

EFI_STATUS
ShowFreeMemoryMap(

);
EFI_STATUS
GetKey (
);

BOOLEAN
HandlePressedKey (
 );

VOID
AllcatePage(
);

VOID
AllcatePool(
);

UINT32
SelectMemoryType(
)
;
EFI_STATUS 
PutKey(
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

#endif

