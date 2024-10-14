#include <string.h>
#include <stdlib.h>
#include "btree_mgr.h"
#include "buffer_mgr.h"
#include "record_mgr.h"
#include "storage_mgr.h"
#include "tables.h"

BPTree **bpTree;
int totalNumNodes;
int nextScanNode;

int BPTreeSize = 50;
int BTreeHandleSize = 3;

// Initializes the index manager
RC initIndexManager (void *mgmtData) {
	bpTree = (BPTree **) malloc (sizeof(BPTree *) * BPTreeSize);
	return RC_OK;
}

// Terminates the index manager
RC shutdownIndexManager () {
	free(bpTree);
	return RC_OK;
}

// Creates a B Tree with the given arguments
RC createBtree (char *idxId, DataType keyType, int n) {
	BTreeHandle *btHandle;
	btHandle = (BTreeHandle *)malloc(sizeof(BTreeHandle) * BTreeHandleSize);

	btHandle->keyType = keyType;
	btHandle->idxId = idxId;

	totalNumNodes = 0;
	nextScanNode = 0;

	return RC_OK;
}

// Opens a B Tree identified by its id
RC openBtree (BTreeHandle **btHandle, char *idxId) {
	*btHandle = (BTreeHandle *) malloc (sizeof(BTreeHandle) * BTreeHandleSize);
	(*btHandle)->idxId = (char *) malloc (sizeof(char) * (strlen(idxId) + 1));

	(*btHandle)->idxId = idxId;

	return RC_OK;
}

// Closes a B Tree passed as a parameter
RC closeBtree (BTreeHandle *btHandle) {
	free(btHandle);
	return RC_OK;
}

// Deletes a B Tree identified by its id
RC deleteBtree (char *idxId) {
	totalNumNodes = 0;
	nextScanNode = 0;
	return RC_OK;
}

// Search for the index of given key in tree
int searchKey(BTreeHandle *btHandle, Value *search_key) {
    for (int i = 0; i < totalNumNodes; i++) {
        BPTree *fetched_node = bpTree[i];
        if (fetched_node->dataValue.dt == search_key->dt) {
            switch (fetched_node->dataValue.dt) {
                case DT_INT:
                    if (fetched_node->dataValue.v.intV == search_key->v.intV) {
                        return i;
                    }
                    break;
                case DT_FLOAT:
                    if (fetched_node->dataValue.v.floatV == search_key->v.floatV) {
                        return i;
                    }
                    break;
                case DT_STRING:
                    if (strcmp(fetched_node->dataValue.v.stringV, search_key->v.stringV) == 0) {
                        return i;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return -1;
}


// Gets the total number of nodes in the B Tree
RC getNumNodes(BTreeHandle *btHandle, int *total_count) {
    int number = 0;
    int i = 0;
    while (i < totalNumNodes) {
        int j = i - 1;
        while (j >= 0) {
            if (bpTree[j]->rid.page == bpTree[i]->rid.page) {
                number++;
                break;
            }
            j--;
        }
        i++;
    }
    *total_count = totalNumNodes - number;
    return RC_OK;
}

// Get the number of nodes
RC getNumEntries (BTreeHandle *btHandle, int *total_count) {
	*total_count = totalNumNodes;
	return RC_OK;
}

// Get data type of tree
RC getKeyType (BTreeHandle *btHandle, DataType *dataType) {
	*dataType = btHandle->keyType;
	return RC_OK;
}

// Look for a given key in tree
RC findKey (BTreeHandle *btHandle, Value *value, RID *rid) {
	int search_key_index = searchKey(btHandle, value);
	if (search_key_index == -1) {
		return RC_IM_KEY_NOT_FOUND;
	}
	rid->page = bpTree[search_key_index]->rid.page;
	rid->slot = bpTree[search_key_index]->rid.slot;
	return RC_OK;	
}

// Insert new node to tree
RC insertKey(BTreeHandle *btHandle, Value *key, RID rid) {
    if (searchKey(btHandle, key) != -1) {
        return RC_IM_KEY_ALREADY_EXISTS;
    }

    BPTree *newBPTreeNode = createBPTreeNode(key, rid);
    if (newBPTreeNode == NULL) {
        return RC_ERROR_CREATING_NODE;
    }

    bpTree[totalNumNodes] = newBPTreeNode;
    totalNumNodes++;
    
    return RC_OK;
}

BPTree *createBPTreeNode(Value *key, RID rid) {
    BPTree *newBPTreeNode = (BPTree *)malloc(sizeof(BPTree));
    if (newBPTreeNode == NULL) {
        return NULL;
    }

    newBPTreeNode->rid.page = rid.page;
    newBPTreeNode->rid.slot = rid.slot;

    newBPTreeNode->dataValue.dt = key->dt;
    if (key->dt == DT_INT) {
        newBPTreeNode->dataValue.v.intV = key->v.intV;
    }
    else if (key->dt == DT_FLOAT) {
        newBPTreeNode->dataValue.v.floatV = key->v.floatV;
    }
    else if (key->dt == DT_STRING) {
        strcpy(newBPTreeNode->dataValue.v.stringV, key->v.stringV);
    }

    return newBPTreeNode;
}

// Delete key from tree
RC deleteKey(BTreeHandle *tree, Value *key) {
    int key_index = searchKey(tree, key);
    if (key_index == -1) {
        return RC_IM_KEY_NOT_FOUND;
    }
    for (int i = key_index; i < totalNumNodes - 1; i++) {
        *bpTree[i] = *bpTree[i + 1];
    }
    free(bpTree[--totalNumNodes]);
    return RC_OK;
}


// Sort key in tree
RC openTreeScan(BTreeHandle *tree, BT_ScanHandle **handle) {
    int i = 0;
    while (i < totalNumNodes - 1) {
        int minIndex = i;
        int j = i + 1;
        while (j < totalNumNodes) {
            if (bpTree[j]->dataValue.v.intV < bpTree[minIndex]->dataValue.v.intV) {
                minIndex = j;
            }
            j++;
        }
        BPTree *tmp = bpTree[i];
        bpTree[i] = bpTree[minIndex];
        bpTree[minIndex] = tmp;
        i++;
    }
    return RC_OK;
}

// Calculates the next entry in the given scan
RC nextEntry (BT_ScanHandle *handle, RID *res_rid) {
	if (nextScanNode >= totalNumNodes) {
		return RC_IM_NO_MORE_ENTRIES;
	}
	
	res_rid->page = bpTree[nextScanNode]->rid.page;
	res_rid->slot = bpTree[nextScanNode]->rid.slot;
	nextScanNode++;
	return RC_OK;
}

// free allocated memory from handle
RC closeTreeScan (BT_ScanHandle *bt_scan_handler) {
	free(bt_scan_handler);
	return RC_OK;
}

// Debug method
char *printTree (BTreeHandle *btTreeHandle) {
	return btTreeHandle->idxId;
}
