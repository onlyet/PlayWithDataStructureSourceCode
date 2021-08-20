#include <stdio.h>
#include <malloc.h>

#define NodeNum 10

//enum BalanceFactor {
//    RH = -1,
//    EH = 0,
//    LH = 1
//};
#define RH -1
#define EH 0
#define LH 1

typedef struct AVLNode {
    int data;
    int bf;
    struct AVLNode *left;
    struct AVLNode *right;
}AVLNode, *AVLTree;

// 旋转以t为根节点的二叉树，最后t指向新的根节点
void rotateLeft(AVLTree *t) {
    AVLTree r = (*t)->right;
    (*t)->right = r->left;
    r->left = (*t);
    (*t) = r;
}

void rotateRight(AVLTree *t) {
    AVLTree l = (*t)->left;
    (*t)->left = l->right;
    l->right = (*t);
    (*t) = l;
}

void balanceLeft(AVLTree *t) {
    AVLTree l = (*t)->left;
    AVLTree lr;
    switch (l->bf) {
     // LL型
    case LH:
        (*t)->bf = EH;
        l->bf = EH;
        rotateRight(t);
        break;

    // LR型
    case RH:
        lr = l->right;
        switch (lr->bf)
        {
        case LH:
            (*t)->bf = RH;
            l->bf = RH;
            break;
        case EH:
            (*t)->bf = EH;
            l->bf = EH;
            break;
        case RH:
            (*t)->bf = EH;
            l->bf = LH;
            break;
        default:
            break;
        }

        lr->bf = EH;

        rotateLeft(&(*t)->left);
        rotateRight(t);
        break;

    default:
        break;
    }
}

void balanceRight(AVLTree *t) {
    AVLTree r = (*t)->right;
    AVLTree rl;
    switch (r->bf) {
     // RR型
    case RH:
        (*t)->bf = EH;
        r->bf = EH;
        rotateLeft(t);
        break;

    // RL型
    case LH:
        rl = r->left;
        switch (rl->bf) {
        case RH:
            r->bf = EH;     // rl的右孩子变成r的左孩子，故r的bf变为0
            (*t)->bf = LH;  // (*t)一定有左孩子，左旋后右孩子没了，所以bf是1
            break;
        case EH:
            r->bf = EH;     // *t和r变为叶子节点，故bf都是0
            (*t)->bf = EH;
            break;
        case LH:
            r->bf = RH;     // 右旋，r失去左孩子，故r->bf变为-1
            (*t)->bf = EH;  // (*t)一定有左孩子，rl上升两层，rl的左孩子变为(*t)的右孩子，故(*t)->bf是0
            break;
        default:
            break;
        }
        rl->bf = 0;         // rl上升1层或2层，bf肯定是0

        rotateRight(&(*t)->right);
        rotateLeft(t);

    default:
        break;
    }
}

int insertAVLTree(AVLTree *t, int data, int *isTaller) {
    if (NULL == *t) {
        *t = (AVLTree)malloc(sizeof(AVLNode));
        (*t)->data = data;
        (*t)->left = (*t)->right = NULL;
        (*t)->bf = EH;
        *isTaller = 1;
        printf("new data: %d\n", data);
        return 1;
    }

    if (data == (*t)->data) {
        *isTaller = 0;
        return 1;
    }
    else if(data < (*t)->data) {
        insertAVLTree(&(*t)->left, data, isTaller);
        if (*isTaller) {
            switch ((*t)->bf) {
            // 原本右子树高，插入左节点后左右子树一样高
            case RH:
                (*t)->bf = EH;
                *isTaller = 1;
                break;
            // 原本左右子树一样高，插入左节点后左子树高一层
            case EH:
                (*t)->bf = LH;
                (*isTaller) = 0;
                break;
            // 原本左子树高，插入左节点后需要重新平衡
            case LH:
                printf("balanceLeft\n");
                balanceLeft(t);
                *isTaller = 0;
                break;
            default:
                break;
            }
        }
    }
    else {
        insertAVLTree(&(*t)->right, data, isTaller);
        if (*isTaller) {
            switch ((*t)->bf) {
                // 原本右子树高，插入右节点后需要重新平衡
            case RH:
                printf("balanceRight\n");
                balanceRight(t);
                *isTaller = 0;
                break;
                // 原本左右子树一样高，插入右节点后右子树高一层
            case EH:
                (*t)->bf = RH;
                *isTaller = 1;
                break;
                // 原本左子树高，插入右节点后左右子树一样高
            case LH:
                (*t)->bf = EH;
                *isTaller = 0;
                break;
            default:
                break;
            }
        }
    }
    return 1;
}

void traversal(AVLTree t) {
    if (NULL == t) {
        return;
    }
    traversal(t->left);
    printf("%d bf:%d\n", t->data, t->bf);
    traversal(t->right);
}

int main() {
    //int a[] = { 3,2,1,4,5,6,7,10,9,8 };
    //int a[] = { 3,2,1,4,5,6,8,10,9,7 };
    int a[] = { 13,42,61,74,15,6,58,160,19,7 };
    int isTaller = 0;
    AVLTree t = NULL;
    for (int i = 0; i < NodeNum; ++i) {
        insertAVLTree(&t, a[i], &isTaller);
        traversal(t);
    }
    //traversal(t);
    return 0;
}