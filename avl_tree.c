/*============================================================================*/
/* avl_tree.c                                                                 */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include "avl_tree.h"

//______________________________________________________________________________

#define nDEBUG_AVL

//______________________________________________________________________________

static int insertCnt = 0;

static inline
void clearTreeMatters( NodeAVL *node ){
    node->left = node->right = node->parent = node->tmp = 0;
    node->height = 1;
}

static inline
NodeAVL* createNode( const double key, void *ptr ){
    NodeAVL *node = (NodeAVL*)malloc(sizeof(NodeAVL));
    node->key = key; node->ptr = ptr;
node->id = insertCnt; insertCnt++;
    clearTreeMatters( node );
    return node;
}

static inline
int avlHeight( NodeAVL* nd ){
    return nd?nd->height:0;
}

static inline
int avlBfactor( NodeAVL* nd ){
    return avlHeight(nd->right) - avlHeight(nd->left);
}

static
void avlFixHeight( NodeAVL* nd ){
    int hl = avlHeight(nd->left);
    int hr = avlHeight(nd->right);
    nd->height = (hl>hr?hl:hr)+1;
}

static
NodeAVL* avlRotateLeft( NodeAVL* q ){
    #ifdef DEBUG_AVL
      printf( "avlRotateLeft id=%d\n", q->id );
    #endif
    NodeAVL* p = q->right;
    q->right = p->left;
    if( p->left ) p->left->parent = q;
    p->left = q;
    NodeAVL* qp = q->parent;
    q->parent = p;
    p->parent = qp;
    avlFixHeight(q);
    avlFixHeight(p);
    return p;
}

static
NodeAVL* avlRotateRight( NodeAVL* p ){
    #ifdef DEBUG_AVL
      printf( "avlRotateRight id=%d\n", p->id );
    #endif
    NodeAVL* q = p->left;
    p->left = q->right;
    if( q->right ) q->right->parent = p;
    q->right = p;
    NodeAVL* pp = p->parent;
    p->parent = q;
    q->parent = pp;
    avlFixHeight(p);
    avlFixHeight(q);
    return q;
}

static
NodeAVL* avlBalance( NodeAVL* nd ){
    #ifdef DEBUG_AVL
      printf( "avlBalance id=%d\n", nd->id );
    #endif
    avlFixHeight(nd);
    if( avlBfactor(nd)==2 ){
        if( avlBfactor(nd->right) < 0 ) nd->right = avlRotateRight(nd->right);
        return avlRotateLeft(nd);
    }
    if( avlBfactor(nd)==-2 ){
        if( avlBfactor(nd->left) > 0  ) nd->left = avlRotateLeft(nd->left);
        return avlRotateRight(nd);
    }
    return nd;
}

static
NodeAVL* avlInsert0( NodeAVL* nd, NodeAVL *newNd ){
    if( !nd ) return newNd;
    if( newNd->key < nd->key ){
        nd->left = avlInsert0( nd->left, newNd );
        nd->left->parent = nd;
    } else {
        nd->right = avlInsert0( nd->right, newNd );
        nd->right->parent = nd;
    }
    return avlBalance(nd);
}

static
NodeAVL* avlFindMin( NodeAVL* nd ){
    #ifdef DEBUG_AVL
      printf( "avlFindMin id=%d\n", nd->id );
    #endif
    return nd->left?avlFindMin( nd->left ):nd;
}

static
NodeAVL* avlRemoveMin( NodeAVL* p ){
    #ifdef DEBUG_AVL
      printf( "avlRemoveMin id=%d\n", p->id );
    #endif
    if( !p->left ) return p->right;
    p->left = avlRemoveMin( p->left );
    #ifdef DEBUG_AVL
      printf( "bude balance z avlRemoveMin id=%d\n", p->id );
    #endif
    return avlBalance( p );
}

NodeAVL* avlFindAndDeleteMin0( NodeAVL *nd, void **ptr ){
    #ifdef DEBUG_AVL
      printf( "avlFindAndDeleteMin0 id=%d\n", nd->id );
    #endif
    if( !nd->left ){
        *ptr = nd->ptr;
        NodeAVL *r = nd->right;
        if (r) r->parent = nd->parent;
        free(nd);
        return r;
    }
    nd->left = avlFindAndDeleteMin0( nd->left, ptr );
    return avlBalance(nd);
}

static
NodeAVL* avlRemoveThisNode( NodeAVL* nd, const int freeMem ){

    #ifdef DEBUG_AVL
      printf( "avlRemoveThisNode id=%d\n", nd->id );
      printf( "=============== ND tree from avlRemoveThisNode  ===============\n" );
      printTree( "", nd );
      printf( "===============================================================\n" );
    #endif

    NodeAVL* l = nd->left;
    NodeAVL* r = nd->right;
    NodeAVL* p = nd->parent;
    if ( freeMem ){ free( nd ); nd = 0; }
    if( !r ){ if ( l ) l->parent = p; return l; }
    NodeAVL* min = avlFindMin(r);

    #ifdef DEBUG_AVL
      if ( min ){
          printf( "*** r->id=%d min->id=%d min->parent->id=%d\n", r->id, min->id, min->parent->id );
          if ( min->left ) printf( "min->left->id=%d\n", min->left->id );
          if ( min->right ) printf( "min->right->id=%d\n", min->right->id );
      }
    #endif

    if ( min->right ) min->right->parent = min->parent;

    min->right = avlRemoveMin(r);
    min->left = l;

    #ifdef DEBUG_AVL
      if ( min->right ){
          printf( "=========  MIN->RIGHT tree from avlRemoveThisNode  ============\n" );
          printf( "min->right=%d\n", min->right->id );
          printTree( "", min->right );
          printf( "===============================================================\n" );
      }
    #endif

    min->parent = p;
    if ( min->left ) min->left->parent = min;
    if ( min->right ) min->right->parent = min;

    #ifdef DEBUG_AVL
      printf( "============== MIN tree from avlRemoveThisNode  ===============\n" );
      printTree( "", min );
      printf( "===============================================================\n" );
    #endif

    return avlBalance(min);
}

static
NodeAVL* avlGoUp( NodeAVL* nd ){
    #ifdef DEBUG_AVL
      printf( "avlGoUp id=%d\n", nd->id );
    #endif
    if ( !nd->parent ) return nd;
    nd->parent->tmp = nd;
    return avlGoUp( nd->parent );
}

static
NodeAVL* avlRemove0( NodeAVL* nd, const int freeMem ){
    if ( nd->tmp ){
        if ( nd->tmp == nd->left ) nd->left = avlRemove0( nd->left, freeMem );
        if ( nd->tmp == nd->right ) nd->right = avlRemove0( nd->right, freeMem );
        nd->tmp = 0;
        return avlBalance( nd ); // FIXME ma tu byt ten return?
    } else return avlRemoveThisNode( nd, freeMem );
}

static
void printTree0( NodeAVL *nd, const int nest ){
    if ( !nd ) return;
    int i; for ( i = 0; i < 10*nest; ++i ) printf( " " );
    int node = nd->id, lSon = -1, rSon=-1, parent = -1;
    if ( nd->parent ) parent = nd->parent->id;
    if ( nd->left ) lSon = nd->left->id;
    if ( nd->right ) rSon = nd->right->id;
    printf( "n=%3d k=%6.4f p=%3d l=%3d r=%3d\n", node, nd->key, parent, lSon, rSon );
    printTree0( nd->left, nest+1 );
    printTree0( nd->right, nest+1 );
}

static
void avlCheckNode( NodeAVL *nd, int *err ){
    if ( !nd ) return;
    if ( nd->left ){
        if ( nd->left->key > nd->key ){
            printf( "Nespravne poradi nd=%d nd->left=%d (%6.4f %6.4f)\n", nd->id, nd->left->id, nd->key, nd->left->key );
            *err = 1;
        }
        if ( nd->left->parent != nd ){
            printf( "Nespravny rodic u nd->left=%d (je %d ma byt %d)\n", nd->left->id, nd->left->parent->id, nd->id );
            *err = 1; }
        avlCheckNode( nd->left, err );
    }

    if ( nd->right ){
        if ( nd->right->key < nd->key ){
            printf( "Nespravne poradi nd=%d nd->right=%d (%6.4f %6.4f)\n", nd->id, nd->right->id, nd->key, nd->right->key );
            *err = 1;
        }
        if ( nd->right->parent != nd ){
            printf( "Nespravny rodic u nd->right=%d (je %d ma byt %d)\n", nd->right->id, nd->right->parent->id, nd->id );
            *err = 1;
        }
        avlCheckNode( nd->right, err );
    }
}
//______________________________________________________________________________

NodeAVL* avlInsert( NodeAVL **tree, const double key, void *ptr ){
    NodeAVL *newNd = createNode( key, ptr );
    *tree = avlInsert0( *tree, newNd );
    return newNd;
}

void* avlFindAndDeleteMin( NodeAVL **tree ){
    #ifdef DEBUG_AVL
      printf( "avlFindAndDeleteMin\n" );
    #endif
    void *ptr;
    *tree = avlFindAndDeleteMin0( *tree, &ptr );
    return ptr;
}

void avlRemoveNode( NodeAVL **tree, NodeAVL* nd ){
    *tree = avlRemove0( avlGoUp(nd), 1 );
}

void avlUpdateNode( NodeAVL **tree, NodeAVL* nd, const double key ){
    #ifdef DEBUG_AVL
      printf( "avlUpdateNode id=%d\n", nd->id );
    #endif
    *tree = avlRemove0( avlGoUp( nd ), 0 );
    nd->key = key;
    clearTreeMatters( nd );
    *tree = avlInsert0( *tree, nd );
}

void printTree( const char *heading, NodeAVL *nd ){
    printf( "%s\n", heading );
    printTree0( nd, 0 );
}

void avlCheckTree( const char *heading, NodeAVL *tree ){
    int err = 0;
    avlCheckNode( tree, &err );
    if ( err ){ printTree( heading, tree ); exit(1); }
}
/*============================================================================*/

