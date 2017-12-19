/*============================================================================*/
/* avl_tree.h                                                                 */
/*============================================================================*/

#ifndef _AVL_TREE_H_
#define _AVL_TREE_H_

//______________________________________________________________________________

typedef struct NodeAVL NodeAVL;
struct NodeAVL {
    int id; double key; void *ptr; int height;
    NodeAVL *left; NodeAVL *right; NodeAVL *parent; NodeAVL *tmp;
};

NodeAVL* avlInsert( NodeAVL **tree, const double key, void *ptr );
void* avlFindAndDeleteMin( NodeAVL **tree );
void avlRemoveNode( NodeAVL **tree, NodeAVL* nd );
void avlUpdateNode( NodeAVL **tree, NodeAVL* nd, const double key );

void printTree( const char *heading, NodeAVL *nd );
void avlCheckTree(  const char *heading, NodeAVL *tree );

/*============================================================================*/
#endif

