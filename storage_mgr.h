#ifndef STORAGE_MGR_H
#define STORAGE_MGR_H

#include "./dberror.h"

#define PAGE_SIZE 4096

/************************************************************
 *                    handle data structures                *
 ************************************************************/
typedef struct SM_FileHandle {
    char *fileName;
    int totalNumPages;
    int curPagePos;
    void *mgmtInfo;
} SM_FileHandle;

typedef char *SM_PageHandle;

/**********************************************************
 *                    interface                           *
 **********************************************************/
/* manipulating page files */
void initStorageManager(void);
RC createPageFile(char *fileName);
RC openPageFile(char *fileName, SM_FileHandle *fHandle);
RC closePageFile(SM_FileHandle *fHandle);
RC destroyPageFile(char *fileName);

/* reading blocks from disc */
RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage);
RC getBlockPos(SM_FileHandle *fHandle);
RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage);
RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage);
RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage);
RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage);
RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage);

/* writing blocks to a page file */
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage);
RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage);
RC appendEmptyBlock(SM_FileHandle *fHandle);
RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle);

#endif
