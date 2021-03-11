/** @file
//;******************************************************************************
//;* Copyright (c) 1983-2021, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;
//; Abstract:
//;  MemoryUtility
//;------------------------------------------------------------------------------
//;
//; $Log: $
//;
//; Revision History:
//;*
//;******************************************************************************
  This sample application is to find all pci device and read or write the configration
  space.
  
  Copyright (c) 2006 - 2021, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

#include "MemoryUtil.h"

UINT32                gcursorrow = MEMORYMAP;

CHAR16 *mem_types[] = { 
    L"ReservedMemory    ", // 1
    L"LoaderCode        ", 
    L"LoaderData        ", 
    L"BSCode            ", 
    L"BSData            ", 
    L"RSCode            ", 
    L"RSData            ", 
    L"Conventional      ", 
    L"UnusableMemory    ", 
    L"ACPIReclaimMemory ", 
    L"ACPIMemoryNVS     ", 
    L"MemoryMappedIO    ", 
    L"MemMapIOPortSpace ", 
    L"PalCode           ", // 14
    L"PersistentMemory  ",
    L"MaxMemoryType     "  // 16
}; 

Memory_Pages_List_Entry   *PageHead;
Memory_Pool_List_Entry    *PoolHead;

EFI_STATUS
EFIAPI
MemoryUtilMain (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS            Status;
  
  BOOLEAN                          Exit;
  
  EFI_INPUT_KEY                    Key;
  
  
  gST->ConOut->ClearScreen (gST->ConOut);
  
  Print (L"finish\n");
  Exit = FALSE;  
  ListAllFunc ();
  InitialPageList();
  InitialPoolList();
  
  while (!Exit) {
    Status = GetKey (&Key);
    if (!EFI_ERROR (Status)) {
        Exit = HandlePressedKey (Key);
    }
  }
  
  gST->ConOut->ClearScreen (gST->ConOut);
  return EFI_SUCCESS;
}

VOID
ListAllFunc (
)
{
    gST->ConOut->ClearScreen (gST->ConOut);
  
    Print (L"Memory Utility\n");
    Print (L"What you want to do?\n");
    Print (L"1.Get Current Memory Allocation Map.\n");
    Print (L"2.AllocatePages ()\n");
    Print (L"3.AllocatePool ()\n");
    Print (L"4.Show Memory Allocate Information.\n");
    Print (L"5.Free All/Specific Allcated Memory.\n");
    Print (L"Press [ESC] to Quit.\n");
    gST->ConOut->SetCursorPosition (
                   gST->ConOut, 
                   0, 
                   gcursorrow
                   );
}
EFI_STATUS
GetKey (
  OUT  EFI_INPUT_KEY              *Key
  )
{
  EFI_STATUS  Status;
  gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, NULL);
  Status = gST->ConIn->ReadKeyStroke (
                         gST->ConIn,
                         Key
                         );
  return Status;
}


BOOLEAN
HandlePressedKey (
  IN  EFI_INPUT_KEY  Key
  )
{
    
    switch (Key.UnicodeChar) {
    
    case CHAR_CARRIAGE_RETURN:

      switch (gcursorrow) {

      case MEMORYMAP:
        ShowMemoryMap ();
        break;
        
      case ALLCATEPAGE:
        AllcatePage ();
        break;

      case ALLCATEPOOL:
        AllcatePool ();
        break;
        
      case ALLCATEINFO:
        ShowMemoryAllocateInfo();
        break;

      case FREEMEM:
        FreeMemoryAllcate();
        break;
        
      default:
      break;
      }
      ////////////////////////////////////////
      ListAllFunc();
      break;
      
    default:
      break;
    }

    
    switch (Key.ScanCode) {

    case SCAN_UP:
  	  if (gcursorrow != MEMORYMAP){
  	    gcursorrow--;
  	  }
      gST->ConOut->SetCursorPosition (gST->ConOut, 0, gcursorrow);
      break;
    
    case SCAN_DOWN:
      if (gcursorrow != FREEMEM){
  	    gcursorrow++;
  	  }
      gST->ConOut->SetCursorPosition (gST->ConOut, 0, gcursorrow);
      break;
      
    case SCAN_ESC:
      Print (L"Esc\n");
      return TRUE;
      break;

    default:
      break;
    }
   
  
    return FALSE;
}


EFI_STATUS
ShowMemoryMap(
)
{
  EFI_STATUS                        Status;
  UINTN                             MemMapSize = 0;
  UINTN                             MapKey;
  UINTN                             DsSize;
  UINT32                            Ver;
  UINT32                            i;
  UINTN                             MemSize;
  EFI_MEMORY_DESCRIPTOR             *MemMap;
  EFI_MEMORY_DESCRIPTOR             *memmap;
  UINT64                            PagesPerType[16] = {0};
  EFI_INPUT_KEY                     Key;
  UINT64                            TotalMem = 0;
  UINT64                            y=0;
  
  gST->ConOut->ClearScreen (gST->ConOut);
  
  // find out how many size the map want
  Status = gBS->AllocatePool(AllocateAddress,0, &MemMap);
  Status = gBS->GetMemoryMap (
              &MemMapSize,
              MemMap,
              &MapKey,
              &DsSize,
              &Ver
  );
  if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL){
    Print (L"Getmap Error num = %d.\n",Status);  
  }
  Status = gBS->FreePool(MemMap);
  //true get map
  Status = gBS->AllocatePool(AllocateAddress,MemMapSize, &MemMap);
  if (EFI_ERROR (Status)){
    Print (L"AllocPool Error num = %d Size=%d.\n",Status,MemMapSize);  
    return Status;
  }
  Status = gBS->GetMemoryMap (
              &MemMapSize,
              MemMap,
              &MapKey,
              &DsSize,
              &Ver
  );
  
  if (EFI_ERROR (Status)){
    Print (L"Getmap Error num = %d Size=%d.\n",Status,MemMapSize);  
    return Status;
  }
  Print (L"Type               Start            End               #Pages             Attributes\n");  
  for (i = 0; i < MemMapSize/DsSize ; i++){
    memmap = (EFI_MEMORY_DESCRIPTOR*) ((UINT8*)MemMap + i * DESC_SIZE);
    MemSize = memmap->NumberOfPages * PAGESIZE;
    
    PagesPerType[memmap->Type] += memmap->NumberOfPages;
    
    Print (mem_types[memmap->Type]);
    Print (L" %16llX-%16llX ",
      memmap->PhysicalStart,
      memmap->PhysicalStart + MemSize -1
      );
    Print (L" %16X  %16X\n",
      memmap->NumberOfPages,
      memmap->Attribute
      );
  }
  
  Print (L"Press Any Key to Continue. Press [ESC] to Skip Printing...\n");
  gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, NULL);
  Status = GetKey (&Key);
  
  if (Key.ScanCode == SCAN_ESC) {
    Status = gBS->FreePool(MemMap);
    return Status;    
  }
  Print (L"\n");
  for (i = 0; i < MemMapSize/DsSize; i++){
    if (PagesPerType[i] != 0){
      Print (L"    ");
      Print (mem_types[i]);
      Print (L":%8d Pages(%d).\n",PagesPerType[i],PagesPerType[i] * PAGESIZE);
      TotalMem += PagesPerType[i] * PAGESIZE;
      y += PagesPerType[i];
    }
  }
  Print (L"    Total Memory: %lld MiB(%lld)Bytes %d\n",(y*PAGESIZE)/MB, TotalMem, y);
  
  Print (L"\nPress Any Key to Exit...\n");
  gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, NULL);
  Status = GetKey (&Key);

  
  Status = gBS->FreePool(MemMap);
  
  return Status;
  
}
EFI_STATUS
ShowFreeMemoryMap(
)
{
  EFI_STATUS                        Status;
  UINTN                             MemMapSize = 0;
  UINTN                             MapKey;
  UINTN                             DsSize;
  UINT32                            Ver;
  UINT32                            i;
  UINTN                             MemSize;
  EFI_MEMORY_DESCRIPTOR             *MemMap;
  EFI_MEMORY_DESCRIPTOR             *memmap;
  EFI_INPUT_KEY                     Key;
  
  gST->ConOut->ClearScreen (gST->ConOut);
  
  // find out how many size the map want
  Status = gBS->AllocatePool(AllocateAddress,0, &MemMap);
  Status = gBS->GetMemoryMap (
              &MemMapSize,
              MemMap,
              &MapKey,
              &DsSize,
              &Ver
  );
  if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL){
    Print (L"Getmap Error num = %d.\n",Status);  
  }
  Status = gBS->FreePool(MemMap);
  //true get map
  Status = gBS->AllocatePool(AllocateAddress,MemMapSize, &MemMap);
  if (EFI_ERROR (Status)){
    Print (L"AllocPool Error num = %d Size=%d.\n",Status,MemMapSize);  
    return Status;
  }
  Status = gBS->GetMemoryMap (
              &MemMapSize,
              MemMap,
              &MapKey,
              &DsSize,
              &Ver
  );
  
  if (EFI_ERROR (Status)){
    Print (L"Getmap Error num = %d Size=%d.\n",Status,MemMapSize);  
    return Status;
  }
  Print (L"Type               Start            End               #Pages             Attributes\n");  
  for (i = 0; i < MemMapSize/DsSize ; i++){
    memmap = (EFI_MEMORY_DESCRIPTOR*) ((UINT8*)MemMap + i * DESC_SIZE);
    MemSize = memmap->NumberOfPages * PAGESIZE;
      if (memmap->Type == EfiConventionalMemory){
      Print (mem_types[memmap->Type]);
      Print (L" %16llX-%16llX ",
        memmap->PhysicalStart,
        memmap->PhysicalStart + MemSize - 1
        );
      Print (L" %16X  %16X\n",
        memmap->NumberOfPages,
        memmap->Attribute
        );
      }
  }
  
  Print (L"Press Any Key to Continue.\n");
  gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, NULL);
  
  Status = GetKey (&Key);
  
  Status = gBS->FreePool(MemMap);
  
  return Status;
  

}

VOID
AllcatePage(
)
{
  EFI_STATUS                        Status;
  EFI_INPUT_KEY                     Key;
  BOOLEAN                           next = FALSE;
  UINT32                            InputCount = 0;
  UINT32                            Mt;
  EFI_MEMORY_TYPE                   mt;
  EFI_ALLOCATE_TYPE                 at;
  EFI_PHYSICAL_ADDRESS              addr = 0;
  
  gST->ConOut->ClearScreen (gST->ConOut);
  Print (L"Now Process AllocatePages.\n");
  
  Print (L"Supported Allocate Type:\n");
  Print (L"0.AllocateAnyPages\n");
  Print (L"1.AllocateMaxAddress\n");
  Print (L"2.AllocateAdddress\n");
 
  Print (L"Please Choose One. [ESC] Back to Upper Level:");
  while (1){
      Status = GetKey (&Key);
      
      switch (Key.UnicodeChar) {

      case CHAR_CARRIAGE_RETURN:
        if (InputCount){
          next = TRUE;
        }
        break;

      case CHAR_BACKSPACE:
        if (InputCount){
          Print (L"\b");
          InputCount--;
        }
        break;

      case L'0':
        if (!InputCount){
          Print (L"0");     
          at = AllocateAnyPages;
          InputCount ++;
        }
        break;

      case L'1':
        if (!InputCount){
          Print (L"1");
          at = AllocateMaxAddress;
          InputCount ++;
        }
        break;
            

      case L'2':
        if (!InputCount){
          Print (L"2");
          at = AllocateAddress;
          InputCount ++;
        }
        break;

      default:
        break;
      
      }
      switch (Key.ScanCode) {

      case SCAN_ESC:
        return;
        break;       
      }
    if (next){
      break;
    }
  }
  gST->ConOut->ClearScreen (gST->ConOut);
  
  /////////////////////////////////////////////////////////////
  Print (L"Now Process AllocatePages.\n");

  Mt = SelectMemoryType();

  if (Mt == 16)
    return;  
  gST->ConOut->ClearScreen (gST->ConOut);
  Print (L"Now Process AllocatePages.\n");
  mt = Mt;

  doAllcatePages(mt, at, addr);
  
  //Print (L"%r\n",Status);
  Status = GetKey (&Key);
}

VOID
AllcatePool(
)
{
  //EFI_STATUS                        Status;
  //EFI_INPUT_KEY                     Key;
  UINT32                            Mt;
  EFI_MEMORY_TYPE                   mt;
  
  gST->ConOut->ClearScreen (gST->ConOut);

  Print (L"Now Process AllocatePool.\n");
  
  Mt = SelectMemoryType();

  if (Mt == 16)
    return;
  
  gST->ConOut->ClearScreen (gST->ConOut);
  Print (L"Now Process AllocatePool.\n");
  mt = Mt;

  doAllcatePool (mt);
}

UINT32
SelectMemoryType(){
  EFI_STATUS                        Status;
  UINT32                            i;
  EFI_INPUT_KEY                     Key;
  UINT32                            Mt = 0;
  UINT32                            count = 0;
  
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, 1);
  Print (L"Support Memory Type:\n");
  for (i = 0; i<14; i++){
    Print (L"%d.",i);
    Print (mem_types[i]);
    Print (L"\n");
  }
  Print (L"Please Choose one, [ESC] Back to Upper Level:");

  while (1){
        Status = GetKey (&Key);
        Status = PutKey(Key, &Mt, count, 13);
        if (Status == EXIT){
          return 16;
        }
        else if (Status == ENTER && count > 0){
          break;
        }
        else if (Status == ISBACKSPACE){
          count--;
        }
        else if (Status == CONTI){
          count++;
        }
    }
  return Mt;
}

EFI_STATUS
PutKey(
  EFI_INPUT_KEY  Key,
  UINT32         *data,
  UINT32         count,
  UINT32         max
)
{
  UINT32 i;
  //Print (L"c%dc\n",count);
  
  if (Key.ScanCode == SCAN_ESC) {
    return EXIT;
  }
  if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN){
    return ENTER;
  }
  
  if (Key.UnicodeChar == CHAR_BACKSPACE && count > 0){
        *data = *data / 10;
        Print (L"\b");
        return ISBACKSPACE;
  }
  
  if (count > 2){
    return CONTI;
  }
  for (i=48; i<58; i++){
    if (Key.UnicodeChar == i){
      *data = *data * 10 + i - 48;

      if (*data > max){
        *data = *data / 10;
      }
      else {
        Print (L"%c",i);
        return CONTI;
      }
    }
    
  }
  return NOACTION;
}

EFI_STATUS
AllcatePageForAllFreeMem(
  EFI_MEMORY_TYPE       MemoryType
)
{
  EFI_STATUS                        Status;
  UINTN                             pages;
  EFI_INPUT_KEY                     Key;
  EFI_PHYSICAL_ADDRESS              addr = 0;
  Memory_Pages_List_Entry           Entry;
  
  Print (L"Do you want to allocate pages for all available memory?(y/n):");
  
  while (1){
    GetKey(&Key);
    if (Key.ScanCode == SCAN_ESC){
      Print (L"\n");
      return EFI_UNSUPPORTED;
      }
    if (Key.UnicodeChar == L'y'){
      Print (L"%c",Key.UnicodeChar);
      break;
      }
    else if (Key.UnicodeChar == L'n'){
      Print (L"%c",Key.UnicodeChar);
      Print (L"\n");
      return EFI_NOT_READY;
      }
  }
  pages = MAX_TRY_TO_ALLOCATE_PAGES_NUM;
  Print (L"\n\n");
  while (1){
    Status = gBS->AllocatePages(
                      AllocateAnyPages, 
                      MemoryType,
                      pages,
                      &addr
                      );
    Print (L"Allocate %5d Pages ",pages);
    if (Status == EFI_OUT_OF_RESOURCES){
      
      Print(L"Failed!!");
      if (pages != 128){
        Print (L"Try to Allocate %d Pages.\n",pages,pages/2);
        pages = pages / 2;
      }
      else{
        break;
      }
    }
    else if (Status == EFI_SUCCESS){
      
      Entry.address = addr;
      Entry.pages   = pages;
      InsertPageList(&Entry);
      Print (L"at %p OK\n",addr);
    }
  }
  //Try to free all
  Print (L"\nDo you want to free all now? (y/n) :");
  while (1){
    GetKey(&Key);
    if (Key.ScanCode == SCAN_ESC){
      return EFI_SUCCESS;
      }
    if (Key.UnicodeChar == L'y'){
      Print (L"%c",Key.UnicodeChar);
      break;
      }
    else if (Key.UnicodeChar == L'n'){
      Print (L"%c",Key.UnicodeChar);
      return EFI_SUCCESS;
      }
  }
  FreeAllAllocateMemory(TRUE);
  return EFI_SUCCESS;
}

EFI_STATUS
AllcatePoolForAllFreeMem(
  EFI_MEMORY_TYPE       MemoryType

)
{
  EFI_STATUS                        Status;
  UINTN                             bytes;
  EFI_INPUT_KEY                     Key;
  VOID                              *ptr;
  Memory_Pool_List_Entry            Entry;
  
  Print (L"Do you want to allocate pool for all available memory?(y/n):");
  
  while (1){
    GetKey(&Key);
    if (Key.ScanCode == SCAN_ESC){
      Print (L"\n");
      return EFI_UNSUPPORTED;
      }
    if (Key.UnicodeChar == L'y'){
      Print (L"%c",Key.UnicodeChar);
      break;
      }
    else if (Key.UnicodeChar == L'n'){
      Print (L"%c",Key.UnicodeChar);
      Print (L"\n");
      return EFI_NOT_READY;
      }
  }
  bytes = MAX_TRY_TO_ALLOCATE_BYTE_NUM;
  Print (L"\n\n");
  while (1){
    Status = gBS->AllocatePool( 
                      MemoryType,
                      bytes,
                      &ptr
                      );
    Print (L"Allocate %9d bytes ",bytes);
    if (Status == EFI_OUT_OF_RESOURCES){
      
      Print(L"Failed!!");
      if (bytes != 65536){
        Print (L"Try to Allocate %d bytes.\n",bytes,bytes/2);
        bytes = bytes / 2;
      }
      else{
        break;
      }
    }
    else if (Status == EFI_SUCCESS){
      Entry.ptr   = ptr;
      Entry.bytes = bytes;
      InsertPoolList(&Entry);
      Print (L"at %p OK\n",(EFI_PHYSICAL_ADDRESS)ptr);
    }
  }
  //Try to free all
  Print (L"\nDo you want to free all now? (y/n) :");
  while (1){
    GetKey(&Key);
    if (Key.ScanCode == SCAN_ESC){
      return EFI_SUCCESS;
      }
    if (Key.UnicodeChar == L'y'){
      Print (L"%c",Key.UnicodeChar);
      break;
      }
    else if (Key.UnicodeChar == L'n'){
      Print (L"%c",Key.UnicodeChar);
      return EFI_SUCCESS;
      }
  }
  FreeAllAllocateMemory(FALSE);
  return EFI_SUCCESS;
}

VOID
doAllcatePages(
  IN EFI_MEMORY_TYPE       MemoryType,
  IN EFI_ALLOCATE_TYPE     AllocateType,
  IN EFI_PHYSICAL_ADDRESS  address
)
{
  UINTN                             pages = 0;
  EFI_STATUS                        Status;
  EFI_INPUT_KEY                     Key;
  UINT32                            count = 0;
  UINT64                            addr = 0;
  Memory_Pages_List_Entry           Entry;
  count = 0;
  if (AllocateType == AllocateAnyPages){
      addr = address;
      Status = AllcatePageForAllFreeMem(MemoryType);
      if (Status == EFI_SUCCESS || Status == EFI_UNSUPPORTED){
        return;
      }
      
      pages = GetPagesNum(TRUE);
       if (pages == -1){  //modify by global
        return;
      }
      Print (L"\n\nTry to allocate %d Pages.\n",pages);
      Status = gBS->AllocatePages(
                      AllocateType, 
                      MemoryType,
                      pages,
                      &addr
                      );
      if (Status == EFI_SUCCESS){
        ///////////////////////////////////////////////////
        Entry.address = addr;
        Entry.pages   = pages;
        InsertPageList(&Entry);
        ///////////////////////////////////////////////////
        Status = SetMemValue(
                   TRUE,
                   pages,
                   addr
                   );
      }
      else {
        Print (L"Error Allocate because %r.\n",Status);
      }
    }
  else if (AllocateType == AllocateMaxAddress){
    ShowFreeMemoryMap();
    pages = GetPagesNum(TRUE);
    if (pages == -1){
      return;
      }
    Print (L"\nPlease input the \"Max Address\" Value : 0X");
    while (1){
        GetKey(&Key);
        if (Key.ScanCode == SCAN_ESC) 
          return;
        if (Key.UnicodeChar == CHAR_BACKSPACE && count > 0){
          count--;
          addr = addr / 16;
          Print (L"\b");
        }
        else if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN && count > 0){
          break;
        }
        else if (count < 16){
          
          if ( Key.UnicodeChar >= 48  && Key.UnicodeChar <= 57){ //number
            count++;
            addr = addr * 16 + Key.UnicodeChar - 48;
            Print (L"%c",Key.UnicodeChar);
          }
          else if (Key.UnicodeChar >= 65  && Key.UnicodeChar <= 70){ //capital letter
            count++;
            addr = addr * 16 + Key.UnicodeChar - 55;
            Print (L"%c",Key.UnicodeChar);
          }
          else if (Key.UnicodeChar >= 97  && Key.UnicodeChar <= 102){//lower  letter
            count++;
            addr = addr * 16 + Key.UnicodeChar - 87;
            Print (L"%c",Key.UnicodeChar);
          }
        }
    }
    gST->ConOut->ClearScreen (gST->ConOut);
    Print (L"How many pages do you want to allocate : %d\n",pages);
    Print (L"\nPlease input the \"Max Address\" Value : 0X%p\n",addr);
    Status = gBS->AllocatePages(
                    AllocateType,
                    MemoryType,
                    pages,
                    &addr
                    );
    if (Status == EFI_SUCCESS){
        ///////////////////////////////////////////////////
        Entry.address = addr;
        Entry.pages   = pages;
        InsertPageList(&Entry);
        ///////////////////////////////////////////////////
        Status = SetMemValue(
                   TRUE,
                   pages,
                   addr
                   );
      }
    else {
        Print (L"\nError Allocate because %r.\n",Status);
      }
  }
  else {
    ShowFreeMemoryMap();
    pages = GetPagesNum(TRUE);
     if (pages == -1){
      return;
      }
    Print (L"\nPlease input the Address Value : 0X");
    while (1){
        GetKey(&Key);
        if (Key.ScanCode == SCAN_ESC) 
          return;
        if (Key.UnicodeChar == CHAR_BACKSPACE && count > 0){
          count--;
          pages = pages / 16;
          Print (L"\b");
        }
        else if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN && count > 0){
          break;
        }
        else if (count < 16){
          
          if ( Key.UnicodeChar >= 48  && Key.UnicodeChar <= 57){ //number
            count++;
            addr = addr * 16 + Key.UnicodeChar - 48;
            Print (L"%c",Key.UnicodeChar);
          }
          else if (Key.UnicodeChar >= 65  && Key.UnicodeChar <= 70){ //capital letter
            count++;
            addr = addr * 16 + Key.UnicodeChar - 55;
            Print (L"%c",Key.UnicodeChar);
          }
          else if (Key.UnicodeChar >= 97  && Key.UnicodeChar <= 102){//lower  letter
            count++;
            addr = addr * 16 + Key.UnicodeChar - 87;
            Print (L"%c",Key.UnicodeChar);
          }
          
        }
    }
    gST->ConOut->ClearScreen (gST->ConOut);
    Print (L"How many pages do you want to allocate : %d\n",pages);
    Print (L"\nPlease input the  Address Value : 0X%p\n",addr);
    Status = gBS->AllocatePages(
                    AllocateType,
                    MemoryType,
                    pages,
                    &addr
                    );
    if (Status == EFI_SUCCESS){
        ///////////////////////////////////////////////////
        Entry.address = addr;
        Entry.pages   = pages;
        InsertPageList(&Entry);
        ///////////////////////////////////////////////////
        Status = SetMemValue(
                   TRUE,
                   pages,
                   addr
                   );
    }
    else {
      Print (L"Error Allocate because %r.\n",Status);
    }
  }
  Print (L"\n%r\n",Status);
  Print (L"Press Any Key To Continue!\n");
  GetKey(&Key);
  
  //gBS->FreePages (addr, pages);
}

VOID
doAllcatePool(
  IN EFI_MEMORY_TYPE       MemoryType
)
{
  UINTN                             bytes = 0;
  EFI_STATUS                        Status;
  EFI_INPUT_KEY                     Key;
  UINT32                            count = 0;
  VOID                              *ptr;
  Memory_Pool_List_Entry            Entry;
  count = 0;

  Status = AllcatePoolForAllFreeMem(MemoryType);

  if (Status == EFI_SUCCESS || Status == EFI_UNSUPPORTED){
        return;
  }
  bytes = GetPagesNum(FALSE);
       if (bytes == -1){  //modify by global
        return;
      }
      Print (L"\n\nTry to allocate %d bytes.\n",bytes);
      Status = gBS->AllocatePool( 
                      MemoryType,
                      bytes,
                      &ptr
                      );
      if (Status == EFI_SUCCESS){
        ///////////////////////////////////////////////////
        Entry.ptr   = ptr;
        Entry.bytes = bytes;
        InsertPoolList(&Entry);
        ///////////////////////////////////////////////////
        Status = SetMemValue(
                   FALSE,
                   bytes,
                   (EFI_PHYSICAL_ADDRESS)ptr
                   );
      }
      else {
        Print (L"Error Allocate because %r.\n",Status);
      }
  Print (L"\nPress Any Key To Continue!\n");
  GetKey(&Key);
}

EFI_STATUS
SetMemValue(
  BOOLEAN                 Ispages,
  UINTN                   pages,
  EFI_PHYSICAL_ADDRESS    address
  )
{
  UINT32            i;
  UINT32            j;
  UINT8             MemValue[64];
  UINT8             *temaddr;
  UINT32            Value = 0;
  UINT32            count = 0;
  UINTN             amount;
  EFI_INPUT_KEY     Key;

  Print (L"Allocate %10d Pages at 0X%p  OK\n",pages,address);
  for (i = 0; i < 64; i++){
    MemValue[i] = *( ( (UINT8*) address) + i);
    if (i % 16 == 0){
      Print (L"\n%8X: ",i/16);
    }
    Print (L"%2X ",MemValue[i]);
    if (i % 16 == 15){
      Print (L"*");
      
      for (j = 0; j < 16; j++){
        if (MemValue[ i - 15 + j] > 127){
          Print (L".");
        }
        else{
          Print (L"%c",MemValue[ i - 15 + j]);
        }
      }
      Print (L"*");
    }
  }
  Print (L"\nDo you want to fill memory with value? (y/n):");
  while (1){
    GetKey(&Key);
    if (Key.ScanCode == SCAN_ESC){
      return EFI_SUCCESS;
      }
    if (Key.UnicodeChar == L'y'){
      Print (L"%c",Key.UnicodeChar);
      break;
      }
    else if (Key.UnicodeChar == L'n'){
      Print (L"%c",Key.UnicodeChar);
      return EFI_SUCCESS;
      }
  }
  Print (L"\nWhat Value do you want to fill ?(1 Byte): 0x");
  while (1){
    GetKey(&Key);
    if (Key.ScanCode == SCAN_ESC) 
      return EFI_SUCCESS;
    if (Key.UnicodeChar == CHAR_BACKSPACE && count > 0){
      count--;
      Value = Value / 16;
      Print (L"\b");
    }
    else if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN && count > 0){
      break;
    }
    else if (count < 2){
      
      if ( Key.UnicodeChar >= 48  && Key.UnicodeChar <= 57){ //number
        Value = Value * 16 + Key.UnicodeChar - 48;
        count++;
        Print (L"%c",Key.UnicodeChar);
      }
      else if (Key.UnicodeChar >= 65  && Key.UnicodeChar <= 70){ //capital letter
        Value = Value * 16 + Key.UnicodeChar - 55;
        count++;
        Print (L"%c",Key.UnicodeChar);
      }
      else if (Key.UnicodeChar >= 97  && Key.UnicodeChar <= 102){//lower  letter
        Value = Value * 16 + Key.UnicodeChar - 87;
        count++;
        Print (L"%c",Key.UnicodeChar);
      }
    }
  }
  if (Ispages){
    amount = pages * PAGESIZE;
    }
  else{
    amount = pages;
    }
  // Set Value
  for (i = 0; i < amount; i++){
    temaddr = ( (UINT8*)address ) + i; 
    *temaddr  = (UINT8) Value;
  }
  // Print 64 Bytes
  for (i = 0; i < 64; i++){
    MemValue[i] = *( ( (UINT8*) address) + i);
    if (i % 16 == 0){
      Print (L"\n%8X: ",i/16);
    }
    Print (L"%2X ",MemValue[i]);
    if (i % 16 == 15){
      Print (L"*");
      
      for (j = 0; j < 16; j++){
        if (MemValue[ i - 15 + j] > 127){
          Print (L".");
        }
        else{
          Print (L"%c",MemValue[ i - 15 + j]);
        }
      }
      Print (L"*");
    }
  }
  return EFI_SUCCESS;

}

INTN 
GetPagesNum(
  BOOLEAN Ispage
)
{
  UINT32            count = 0;
  EFI_INPUT_KEY     Key;
  INTN             pages = 0;
  if (Ispage){
    Print (L"How many pages do you want to allocate : ");
  }
  else{
    Print (L"How many bytes do you want to allocate : ");
  
  }
  //pages Key count 
  while (1){
    GetKey(&Key);
    if (Key.ScanCode == SCAN_ESC) 
      return -1;
    if (Key.UnicodeChar == CHAR_BACKSPACE && count > 0){
      count--;
      pages = pages / 10;
      Print (L"\b");
    }
    else if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN && count > 0){
      break;
    }
    else if (Key.UnicodeChar >= 48 && Key.UnicodeChar < 58 && count < 10){
      count++;
      pages = pages * 10 + Key.UnicodeChar - 48;
      Print (L"%c",Key.UnicodeChar);
    }
  }
  return pages;
}

EFI_STATUS
InitialPageList(
)
{
  EFI_STATUS  Status;

  Status = gBS->AllocatePool(
         EfiBootServicesData,
         sizeof (Memory_Pages_List_Entry),
         &PageHead
         );
  
  PageHead->address = 0;
  PageHead->pages   = 0;
  
  
  InitializeListHead( &(PageHead->link) );
  
  
  return Status;
}

EFI_STATUS
InitialPoolList(
)
{
  EFI_STATUS  Status;

  Status = gBS->AllocatePool(
         EfiBootServicesData,
         sizeof (Memory_Pool_List_Entry),
         &PoolHead
         );
  
  PoolHead->ptr   = 0;
  PoolHead->bytes = 0;
  
  InitializeListHead( &(PoolHead->link) );
  return Status;
}

EFI_STATUS
InsertPageList(
  Memory_Pages_List_Entry     *Entry
)
{
  Memory_Pages_List_Entry       *NewEntry;

  gBS->AllocatePool(
         EfiBootServicesData,
         sizeof (Memory_Pages_List_Entry),
         &NewEntry
         );
  
  NewEntry->address = Entry->address;
  NewEntry->pages   = Entry->pages;
  
  
  InsertHeadList(&(PageHead->link), &(NewEntry->link));

  return EFI_SUCCESS;
}

EFI_STATUS
InsertPoolList(
  Memory_Pool_List_Entry     *Entry
)
{
  Memory_Pool_List_Entry       *NewEntry;
  gBS->AllocatePool(
         EfiBootServicesData,
         sizeof (Memory_Pool_List_Entry),
         &NewEntry
         );
  
  NewEntry->ptr   = Entry->ptr;
  NewEntry->bytes = Entry->bytes;
  
  InsertHeadList(&(PoolHead->link), &(NewEntry->link));
  return EFI_SUCCESS;
}


EFI_STATUS
RemovePageEntry(
  Memory_Pages_List_Entry     *Entry
)
{
 
  RemoveEntryList ( &(Entry->link) );

  gBS->FreePages ( Entry->address, Entry->pages);
 
  
  gBS->FreePool ( Entry );
  
  
  return EFI_SUCCESS;
}

EFI_STATUS
RemovePoolEntry(
  Memory_Pool_List_Entry     *Entry
)
{
  RemoveEntryList ( &(Entry->link));
  
  gBS->FreePool ( Entry->ptr );
  
  gBS->FreePool ( Entry );
  return EFI_SUCCESS;
}

VOID
ShowMemoryAllocateInfo(
)
{
  LIST_ENTRY              *current = &(PageHead->link);
  EFI_INPUT_KEY           Key;
  UINT32                  i;
  UINT32                  x = 0;
  UINT32                  j;
  UINT8                   MemValue[16];
  EFI_PHYSICAL_ADDRESS    CurrentAddress;
  UINTN                   Currentpages;
  
  gST->ConOut->ClearScreen (gST->ConOut);
  Print (L"Show Allocated Memory Information.\n\n");
  
  while (1){
    //Print (L"head link address = %p\n",&(PageHead->link));
    //Print (L"Current forward link = %p\n",current->link.ForwardLink);
    if (IsListEmpty (&(PageHead->link) )){
      break;
      }
    current = GetNextNode (&(PageHead->link), current);

    Print (L"%d : ",x+1);
    if (x == 0){
      Print (L"The Last ");
    }
    CurrentAddress = _CR((Memory_Pages_List_Entry*)current,
           Memory_Pages_List_Entry, 
           link
         )->address;
    Currentpages   = _CR(
           (Memory_Pages_List_Entry*)current,
           Memory_Pages_List_Entry, 
           link
         )->pages;
    Print (L"Allocate address = %p, number of pages  = %d\n",
      CurrentAddress,
      Currentpages
      );
    for (i = 0; i < 16; i++){
      MemValue[i] = *( ( (UINT8*) CurrentAddress) + i);
      if (i == 0){
        Print (L"%8X: ",i);
      }
      Print (L"%2X ", *( ( (UINT8*) CurrentAddress) + i));
      if (i == 15){
        Print (L"*");
        
        for (j = 0; j < 16; j++){
          if (MemValue[ i - 15 + j] > 127){
            Print (L".");
          }
          else{
            Print (L"%c",MemValue[ i - 15 + j]);
          }
        }
        Print (L"*");
      }
  }
  Print (L"\n");
    x++;
    if (IsNodeAtEnd (&(PageHead->link), current)){
      Print (L"\nIt reach the end\n\n");
      break;
    }
  }
  Print (L"There are no more Allocated Page Memory Exists.\n");
  Print (L"\nPress Any Key To Continue!\n");
  GetKey(&Key);
  ////////////////////////////////////////////////////////
  //                 Show Pool Allocated                //
  ////////////////////////////////////////////////////////
  current = &(PoolHead->link);
  while (1){
    //Print (L"head link address = %p\n",&(PageHead->link));
    //Print (L"Current forward link = %p\n",current->link.ForwardLink);
    if (IsListEmpty (&(PoolHead->link) )){
      Print (L"head address of link = %p\n",&(PoolHead->link));
      Print (L"heaf ForwardLink = %p\n",PoolHead->link.ForwardLink);
      Print (L"no entry\n");
      break;
      }
    current = GetNextNode (&(PoolHead->link), current);

    Print (L"%d : ",x+1);
    if (x == 0){
      Print (L"The Last ");
    }
    CurrentAddress = (EFI_PHYSICAL_ADDRESS)(_CR((Memory_Pool_List_Entry*)current,
           Memory_Pool_List_Entry, 
           link
         )->ptr);
    Currentpages   = _CR(
           (Memory_Pool_List_Entry*)current,
           Memory_Pool_List_Entry, 
           link
         )->bytes;
    Print (L"Allocate address = %p, number of bytes  = %d\n",
      CurrentAddress,
      Currentpages
      );
    for (i = 0; i < 16; i++){
      MemValue[i] = *( ( (UINT8*) CurrentAddress) + i);
      if (i == 0){
        Print (L"%8X: ",i);
      }
      Print (L"%2X ", *( ( (UINT8*) CurrentAddress) + i));
      if (i == 15){
        Print (L"*");
        
        for (j = 0; j < 16; j++){
          if (MemValue[ i - 15 + j] > 127){
            Print (L".");
          }
          else{
            Print (L"%c",MemValue[ i - 15 + j]);
          }
        }
        Print (L"*");
      }
  }
  Print (L"\n");
    x++;
    if (IsNodeAtEnd (&(PoolHead->link), current)){
      Print (L"\nIt reach the end\n\n");
      break;
    }
  }
  Print (L"There are no more Allocated Pool Memory Exists.\n");
  Print (L"\nPress Any Key To Continue!\n");
  GetKey(&Key);
  
}

VOID
FreeMemoryAllcate(
)
{
  EFI_STATUS              Status;
  LIST_ENTRY              *current = &(PageHead->link);
  EFI_PHYSICAL_ADDRESS    CurrentAddress;
  UINTN                   Currentpages;
  UINT32                  x = 0;
  EFI_INPUT_KEY           Key;
  UINT32                  num = 0;
  UINT32                  count = 0;
  BOOLEAN                 Yes_No;
  
  gST->ConOut->ClearScreen (gST->ConOut);
  Print (L"Free Allocated Page Memory\n");

  while(1){
    x     = 0;
    num   = 0;
    count = 0;
    if (IsListEmpty (&(PageHead->link) )){
        Print (L"No allcated Page Memory could be Freed.");
        break;
    }
    Print (L"Do you want to free all Allocated pages? (y/n):");

    //do yes or no key
    while (1){
      GetKey(&Key);
      if (Key.ScanCode == SCAN_ESC){
        return ;
        }
      if (Key.UnicodeChar == L'y'){
        Print (L"%c\n",Key.UnicodeChar);
        FreeAllAllocateMemory(TRUE);
        return;
        }
      else if (Key.UnicodeChar == L'n'){
        Print (L"%c\n",Key.UnicodeChar);
        break;
        }
    }
    current = &(PageHead->link);
    while (1){
      current = GetNextNode (&(PageHead->link), current);

      Print (L"[%d] : ",x);
      CurrentAddress = _CR((Memory_Pages_List_Entry*)current,
             Memory_Pages_List_Entry, 
             link
           )->address;
      Currentpages   = _CR(
             (Memory_Pages_List_Entry*)current,
             Memory_Pages_List_Entry, 
             link
           )->pages;
      Print (L"%8d pages have allocated at 0x%p address.\n",
        Currentpages,
        CurrentAddress
        );
      x++;
      if (IsNodeAtEnd (&(PageHead->link), current)){
        break;
      }
    }
    Print (L"Which Allocated Page Memory you want to free? :");
    //do getkey and put num
    while (1){
        Status = GetKey (&Key);
        Status = PutKey(Key, &num, count, x-1);
        if (Status == EXIT){
          return;
        }
        else if (Status == ENTER && count > 0){
          break;
        }
        else if (Status == ISBACKSPACE){
          count--;
        }
        else if (Status == CONTI){
          count++;
        }
    }
    Print (L"\nFree->%8d pages have allocated at 0x%p address. (y/n):",
        Currentpages,
        CurrentAddress
        );
    //do yes or no key
    while (1){
      GetKey(&Key);
      if (Key.ScanCode == SCAN_ESC){
        return;
        }
      if (Key.UnicodeChar == L'y'){
        Print (L"%c",Key.UnicodeChar);
        Yes_No = TRUE;
        break;
        }
      else if (Key.UnicodeChar == L'n'){
        Print (L"%c",Key.UnicodeChar);
        Yes_No = FALSE;
        break;
        }
    }
    
    current = &(PageHead->link);
    num++;
    if (Yes_No){
      while(num > 0){
        current = GetNextNode (&(PageHead->link), current);
        num--;
      }
      CurrentAddress = _CR((Memory_Pages_List_Entry*)current,
               Memory_Pages_List_Entry, 
               link
             )->address;
      Currentpages   = _CR(
               (Memory_Pages_List_Entry*)current,
               Memory_Pages_List_Entry, 
               link
             )->pages;
      
      Print (L"\nFree%8d pages at %p",Currentpages, CurrentAddress);
      Status = RemovePageEntry (
                 _CR(
                   (Memory_Pages_List_Entry*)current,
                   Memory_Pages_List_Entry, 
                   link
                   )
               );
      if (Status == EFI_SUCCESS){
        Print(L" Finish.\n");
      }
      else{
        Print(L" Fail because %r\n",Status);  
      }
    }
    Print (L"\nPress Any Key To Continue and Clean screen!\n");
    GetKey(&Key);
    gST->ConOut->ClearScreen (gST->ConOut);
    Print (L"Free Allocated Page Memory\n");
  }
  Print (L"\nPress Any Key to Continue!\n");
  GetKey(&Key);
   
  ////////////////////////////////////
  //            FreePool            //
  ////////////////////////////////////
  gST->ConOut->ClearScreen (gST->ConOut);
    Print (L"Free Allocated Pool Memory\n");
  while(1){
    x     = 0;
    num   = 0;
    count = 0;
    if (IsListEmpty (&(PoolHead->link) )){
        Print (L"No allcated Pool Memory could be Freed.");
        break;
    }
    Print (L"Do you want to free all Allocated Pool? (y/n):");

    //do yes or no key
    while (1){
      GetKey(&Key);
      if (Key.ScanCode == SCAN_ESC){
        return ;
        }
      if (Key.UnicodeChar == L'y'){
        Print (L"%c\n",Key.UnicodeChar);
        FreeAllAllocateMemory(FALSE);
        return;
        }
      else if (Key.UnicodeChar == L'n'){
        Print (L"%c\n",Key.UnicodeChar);
        break;
        }
    }
    current = &(PoolHead->link);
    while (1){
      current = GetNextNode (&(PoolHead->link), current);

      Print (L"[%d] : ",x);
      CurrentAddress = (EFI_PHYSICAL_ADDRESS)(_CR((Memory_Pool_List_Entry*)current,
             Memory_Pool_List_Entry, 
             link
           )->ptr);
      Currentpages   = _CR(
             (Memory_Pool_List_Entry*)current,
             Memory_Pool_List_Entry, 
             link
           )->bytes;
      Print (L"%8d bytes have allocated at 0x%p address.\n",
        Currentpages,
        CurrentAddress
        );
      x++;
      if (IsNodeAtEnd (&(PoolHead->link), current)){
        break;
      }
    }
    Print (L"Which Allocated Pool Memory you want to free? :");
    //do getkey and put num
    while (1){
        Status = GetKey (&Key);
        Status = PutKey(Key, &num, count, x-1);
        if (Status == EXIT){
          return;
        }
        else if (Status == ENTER && count > 0){
          break;
        }
        else if (Status == ISBACKSPACE){
          count--;
        }
        else if (Status == CONTI){
          count++;
        }
    }
    Print (L"\nFree->%8d bytes have allocated at 0x%p address. (y/n):",
        Currentpages,
        CurrentAddress
        );
    //do yes or no key
    while (1){
      GetKey(&Key);
      if (Key.ScanCode == SCAN_ESC){
        return;
        }
      if (Key.UnicodeChar == L'y'){
        Print (L"%c",Key.UnicodeChar);
        Yes_No = TRUE;
        break;
        }
      else if (Key.UnicodeChar == L'n'){
        Print (L"%c",Key.UnicodeChar);
        Yes_No = FALSE;
        break;
        }
    }
    
    current = &(PoolHead->link);
    num++;
    if (Yes_No){
      while(num > 0){
        current = GetNextNode (&(PoolHead->link), current);
        num--;
      }
      CurrentAddress = (EFI_PHYSICAL_ADDRESS)(_CR((Memory_Pool_List_Entry*)current,
               Memory_Pool_List_Entry, 
               link
             )->ptr);
      Currentpages   = _CR(
               (Memory_Pool_List_Entry*)current,
               Memory_Pool_List_Entry, 
               link
             )->bytes;
      
      Print (L"\nFree%8d pages at %p",Currentpages, CurrentAddress);
      Status = RemovePoolEntry (
                 _CR(
                   (Memory_Pool_List_Entry*)current,
                   Memory_Pool_List_Entry, 
                   link
                   )
               );
      if (Status == EFI_SUCCESS){
        Print(L" Finish.\n");
      }
      else{
        Print(L" Fail because %r\n",Status);  
      }
    }
    Print (L"\nPress Any Key To Continue and Clean screen!\n");
    GetKey(&Key);
    gST->ConOut->ClearScreen (gST->ConOut);
    Print (L"Free Allocated Page Memory\n");
  }
  Print (L"\nPress [ESC] To Go Back Menu!\n");
  while(1){
    GetKey(&Key);
    if (Key.ScanCode== SCAN_ESC){
      break;
    }
  }
}
EFI_STATUS
FreeAllAllocateMemory(
  BOOLEAN     Ispages
)
{
  EFI_STATUS  Status;
  EFI_INPUT_KEY           Key;
  LIST_ENTRY              *current;
  LIST_ENTRY              *tem;
  EFI_PHYSICAL_ADDRESS    CurrentAddress;
  UINTN                   Currentpages;
  UINT32                  x = 0;

  if (Ispages){
    gST->ConOut->ClearScreen (gST->ConOut);
    Print (L"Free Allocated Page Memory\n");
    
    current = PageHead->link.ForwardLink;

    while (1){
        tem     = GetNextNode (&(PageHead->link), current);
        Print (L"Free->[%d] : ",x);
        CurrentAddress = _CR((Memory_Pages_List_Entry*)current,
               Memory_Pages_List_Entry, 
               link
             )->address;
        Currentpages   = _CR(
               (Memory_Pages_List_Entry*)current,
               Memory_Pages_List_Entry, 
               link
             )->pages;
        Print (L"%8d pages at 0x%p address.\n",
          Currentpages,
          CurrentAddress
          );
        x++;
        Status = RemovePageEntry (
                   _CR(
                     (Memory_Pages_List_Entry*)current,
                     Memory_Pages_List_Entry, 
                     link
                     )
                 );
        
        current = tem;
        
        if (IsListEmpty (&(PageHead->link) )){
          break;
        }
      }
    Print (L"\nPress Any Key To Continue and Clean screen!\n");
      GetKey(&Key);
  }
  else{
    gST->ConOut->ClearScreen (gST->ConOut);
    Print (L"Free Allocated Pool Memory\n");
    
    current = PoolHead->link.ForwardLink;

    while (1){
        tem     = GetNextNode (&(PoolHead->link), current);
        Print (L"Free->[%d] : ",x);
        CurrentAddress = (EFI_PHYSICAL_ADDRESS)(_CR((Memory_Pool_List_Entry*)current,
               Memory_Pool_List_Entry, 
               link
             )->ptr);
        Currentpages   = _CR(
               (Memory_Pool_List_Entry*)current,
               Memory_Pool_List_Entry, 
               link
             )->bytes;
        Print (L"%8d bytes at 0x%p address.\n",
          Currentpages,
          CurrentAddress
          );
        x++;
        Status = RemovePoolEntry (
                   _CR(
                     (Memory_Pool_List_Entry*)current,
                     Memory_Pool_List_Entry, 
                     link
                     )
                 );
        
        current = tem;
        
        if (IsListEmpty (&(PoolHead->link) )){
          break;
        }
      }
    Print (L"\nPress Any Key To Continue and Clean screen!\n");
      GetKey(&Key);
  }
  return EFI_SUCCESS;
}


