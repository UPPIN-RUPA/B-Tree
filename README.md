**B+ Tree Index Manager**
This repository contains an implementation of a B+ Tree index manager in C. The B+ Tree index manager provides functionalities to create, open, close, delete, insert, and search for keys in a B+ Tree index structure. Additionally, it supports scanning through the entries of the index.

**B+ Tree**
A B+ Tree is a type of self-balancing search tree used in computer science, typically to implement associative arrays, databases, and filesystems. In a B+ Tree, each internal node can have a variable number of child nodes within a certain range. This structure allows efficient insertion, deletion, and retrieval operations.

**Implemented Functions**
initIndexManager: Initializes the index manager.

shutdownIndexManager: Shuts down the index manager.

createBtree: Creates a new B+ Tree index.

openBtree: Opens an existing B+ Tree index.

closeBtree: Closes a B+ Tree index.

deleteBtree: Deletes a B+ Tree index.

getNumNodes: Retrieves the total number of nodes in the B+ Tree.

getNumEntries: Retrieves the total number of entries in the B+ Tree.

getKeyType: Retrieves the data type of the keys in the B+ Tree.

findKey: Finds a key in the B+ Tree and retrieves its corresponding RID.

insertKey: Inserts a key and its corresponding RID into the B+ Tree.

deleteKey: Deletes a key from the B+ Tree.

openTreeScan: Opens a scan handle for traversing the B+ Tree entries.

nextEntry: Retrieves the next entry in the B+ Tree scan.

closeTreeScan: Closes a B+ Tree scan handle.

**Contribution**
Manju: Implemented all functions except createNode, createBPTreeNode, and printTree.

Rupa: Implemented createNode.

Vanaja: Implemented createBPTreeNode.

Mohit: Implemented printTree.