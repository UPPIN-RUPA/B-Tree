#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./storage_mgr.h"

/************************************************************
 *                    handle data structures                *
 ************************************************************/

/**********************************************************
 *                    interface                           *
 **********************************************************/

/* manipulating page files */
void initStorageManager(void) {
    // Nothing to initialize in this simple implementation
}

RC createPageFile(char *fileName) {
    FILE *fp = fopen(fileName, "w");
    if (fp == NULL)
        return RC_FILE_CREATION_FAILED;

    // Create a single page filled with 0 bytes
    SM_PageHandle emptyPage = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char));
    if (emptyPage == NULL) {
        fclose(fp);
        return RC_FILE_CREATION_FAILED;
    }

    fwrite(emptyPage, sizeof(char), PAGE_SIZE, fp);
    free(emptyPage);
    fclose(fp);

    return RC_OK;
}

RC openPageFile(char *fileName, SM_FileHandle *fHandle) {
    FILE *fp = fopen(fileName, "r+");
    if (fp == NULL)
        return RC_FILE_NOT_FOUND;

    fHandle->fileName = fileName;
    fseek(fp, 0L, SEEK_END);
    fHandle->totalNumPages = ftell(fp) / PAGE_SIZE;
    fHandle->curPagePos = 0;
    fHandle->mgmtInfo = fp;

    return RC_OK;
}

RC closePageFile(SM_FileHandle *fHandle) {
    if (fclose(fHandle->mgmtInfo) != 0)
        return RC_FILE_CLOSE_FAILED;
    return RC_OK;
}

RC destroyPageFile(char *fileName) {
    if (remove(fileName) != 0)
        return RC_FILE_DELETION_FAILED;
    return RC_OK;
}

/* reading blocks from disc */
RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (pageNum < 0 || pageNum >= fHandle->totalNumPages)
        return RC_READ_NON_EXISTING_PAGE;

    FILE *fp = fHandle->mgmtInfo;
    fseek(fp, pageNum * PAGE_SIZE, SEEK_SET);
    fread(memPage, sizeof(char), PAGE_SIZE, fp);
    fHandle->curPagePos = pageNum;

    return RC_OK;
}

RC getBlockPos(SM_FileHandle *fHandle) {
    return fHandle->curPagePos;
}

RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(0, fHandle, memPage);
}

RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->curPagePos - 1, fHandle, memPage);
}

RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->curPagePos, fHandle, memPage);
}

RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->curPagePos + 1, fHandle, memPage);
}

RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->totalNumPages - 1, fHandle, memPage);
}

/* writing blocks to a page file */
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (pageNum < 0 || pageNum >= fHandle->totalNumPages)
        return RC_WRITE_FAILED;

    FILE *fp = fHandle->mgmtInfo;
    fseek(fp, pageNum * PAGE_SIZE, SEEK_SET);
    fwrite(memPage, sizeof(char), PAGE_SIZE, fp);
    fHandle->curPagePos = pageNum;

    return RC_OK;
}

RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return writeBlock(fHandle->curPagePos, fHandle, memPage);
}

RC appendEmptyBlock(SM_FileHandle *fHandle) {
    FILE *fp = fHandle->mgmtInfo;
    fseek(fp, 0L, SEEK_END);

    SM_PageHandle emptyPage = (SM_PageHandle)calloc(PAGE_SIZE, sizeof(char));
    if (emptyPage == NULL)
        return RC_WRITE_FAILED;

    fwrite(emptyPage, sizeof(char), PAGE_SIZE, fp);
    free(emptyPage);

    fHandle->totalNumPages++;

    return RC_OK;
}

RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle) {
    if (numberOfPages <= fHandle->totalNumPages)
        return RC_OK;

    int numEmptyPages = numberOfPages - fHandle->totalNumPages;
    for (int i = 0; i < numEmptyPages; i++) {
        RC rc = appendEmptyBlock(fHandle);
        if (rc != RC_OK)
            return rc;
    }

    return RC_OK;
}
