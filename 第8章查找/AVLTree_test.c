#include <stdio.h>
#include <malloc.h>

#define NodeNum 10

#define RH -1
#define EH 0
#define LH 1

typedef struct AVLNode {
    int data;
    int bf;                 // 平衡因子
    struct AVLNode *left;   // 左孩子
    struct AVLNode *right;  // 右孩子
}AVLNode, *AVLTree;

// 旋转以t为根节点的二叉树，最后t指向新的根节点
// 左旋
void rotateLeft(AVLTree *t) {
    AVLTree r = (*t)->right;
    (*t)->right = r->left;
    r->left = (*t);
    (*t) = r;
}
// 右旋
void rotateRight(AVLTree *t) {
    AVLTree l = (*t)->left;
    (*t)->left = l->right;
    l->right = (*t);
    (*t) = l;
}

// 平衡左子树
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
        case LH:            // 插入节点是lr的左孩子
            (*t)->bf = RH;  // *t右旋失去左孩子，故(*t)->bf变为-1
            l->bf = EH;     // l左旋先失去右孩子，而lr的左孩子（插入节点）又变成l的右孩子，故l->bf仍是0
            break;
        case EH:            // 插入节点是lr
            (*t)->bf = EH;  // *t和r变为叶子节点，故bf都是0
            l->bf = EH; 
            break;
        case RH:            // 插入节点是lr的右孩子
            (*t)->bf = EH;  // *t右旋先失去左孩子，而lr的右孩子（插入节点）又变成*t的左孩子，故(*t)->仍是0
            l->bf = LH;     // l左旋失去右孩子，故l->bf是1
            break;
        default:
            break;
        }

        lr->bf = EH;        // rl上升1层或2层，bf肯定是0

        rotateLeft(&(*t)->left);
        rotateRight(t);
        break;

    // 删除的时候才用到
    case EH:
        rotateRight(t);
        // l->bf==0比l->bf==1时右子树高一层，故平衡后t的bf变为-1
        // 平衡前的l的bf平衡后不变
        (*t)->bf = RH;
        break;

    default:
        break;
    }
}

// 平衡右子树
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
            (*t)->bf = LH;  // (*t)一定有左孩子，左旋后右孩子没了，所以bf是1
            r->bf = EH;     // rl的右孩子变成r的左孩子，故r的bf变为0
            break;
        case EH:
            (*t)->bf = EH;
            r->bf = EH;     // *t和r变为叶子节点，故bf都是0
            break;
        case LH:
            (*t)->bf = EH;  // (*t)一定有左孩子，rl上升两层，rl的左孩子变为(*t)的右孩子，故(*t)->bf是0
            r->bf = RH;     // 右旋，r失去左孩子，故r->bf变为-1
            break;
        default:
            break;
        }
        rl->bf = 0;         // rl上升1层或2层，bf肯定是0

        rotateRight(&(*t)->right);
        rotateLeft(t);
        break;

    // 删除的时候才用到
    case EH:
        rotateLeft(t);
        // r->bf==0比r->bf==1时左子树高一层，故平衡后t的bf变为1
        // r->bf和平衡前一样
        (*t)->bf = LH;
        break;

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
        //printf("new data: %d\n", data);
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
                *isTaller = 0;
                break;

            // 原本左右子树一样高，插入左节点后左子树高一层
            case EH:
                (*t)->bf = LH;
                (*isTaller) = 1;
                break;

            // 原本左子树高，插入左节点后需要重新平衡
            case LH:
                //printf("balanceLeft\n");
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
                //printf("balanceRight\n");
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

void updateBf(AVLTree *t, int *isShorter, AVLTree *pre) {
    if (NULL != (*t)->right) {
        updateBf(&(*t)->right, isShorter, pre);

        if (*isShorter) {
            switch ((*t)->bf) {
            case LH:
                balanceLeft(t);
                if (EH == (*t)->bf) {
                    *isShorter = 1;
                }
                else {
                    *isShorter = 0;
                }
                break;

            case EH:
                (*t)->bf = LH;
                *isShorter = 0;
                break;

            case RH:
                (*t)->bf = EH;
                *isShorter = 1;
                break;

            default:
                break;
            }
        }
    }
    else {
        *pre = *t;
        // 前驱的左孩子
        (*t) = (*t)->left;
    }
}

void deleteAVLTree(AVLTree *t, int data, int *isShorter) {
    if (NULL == t || NULL == *t) {
        return;
    }
    if (data < (*t)->data) {
        deleteAVLTree(&(*t)->left, data, isShorter);

        if (*isShorter) {
            switch ((*t)->bf)
            {
            case LH:
                (*t)->bf = EH;
                *isShorter = 1;
                break;

            case EH:
                (*t)->bf = RH;
                *isShorter = 0;
                break;

            case RH:
                balanceRight(t);

                if (EH == (*t)->bf) {
                    // 平衡后高度-1
                    *isShorter = 1;
                }
                else {
                    // l有右孩子
                    *isShorter = 0;
                }
                break;

            default:
                break;
            }
        }
    }
    else if (data > (*t)->data) {
        deleteAVLTree(&(*t)->right, data, isShorter);

        if (*isShorter) {
            switch ((*t)->bf)
            {
            case LH:

                balanceLeft(t);

                if (EH == (*t)->bf) {
                    *isShorter = 1;
                }
                else {
                    // r有左孩子
                    *isShorter = 0;
                }
                break;

            case EH:
                (*t)->bf = LH;
                *isShorter = 0;
                break;

            case RH:
                (*t)->bf = EH;
                *isShorter = 1;
                break;

            default:
                break;
            }
        }
    }
    else {
        *isShorter = 1;

        if (NULL == (*t)->left && NULL == (*t)->right) {
            free(*t);
            *t = NULL;
        }
        else if (NULL != (*t)->left && NULL == (*t)->right) {
            (*t)->data = (*t)->left->data;
            (*t)->bf = 0;
            free((*t)->left);
            (*t)->left = NULL;
        }
        else if (NULL == (*t)->left && NULL != (*t)->right) {
            (*t)->data = (*t)->right->data;
            (*t)->bf = 0;
            free((*t)->right);
            (*t)->right = NULL;
        }
        else {
            // 前驱
            AVLTree pre = NULL;
            updateBf(&(*t)->left, isShorter, &pre);

            (*t)->data = pre->data;
            free(pre);

            if (*isShorter) {
                switch ((*t)->bf)
                {
                case LH:
                    (*t)->bf = EH;
                    *isShorter = 1;
                    break;

                case EH:
                    (*t)->bf = RH;
                    *isShorter = 0;
                    break;

                case RH:
                    balanceRight(t);

                    if (EH == (*t)->bf) {
                        // 平衡后高度-1
                        *isShorter = 1;
                    }
                    else {
                        *isShorter = 0;
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }
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
#if 1
    int a[] = { 13,42,61,75,15,6,58,160,19,7 };
    int b[] = { 58,61,75,13,6,160,19,42,7,15 };
#elif 0
    int a[] = { 50,11,32,72,99,41,65,29,20,91 };
    int b[] = { 29,11,99,91,32,41,50,65,20,72 };
#elif 0
    int a[] = { 77, 66, 55, 33, 99, 22, 11, 44, 88, 100 };
    int b[] = { 44,77, 66, 55, 99, 22, 11, 88, 100, 33};
#elif 0
    int a[] = { 42, 61, 96, 43, 36, 31, 45, 87, 6, 41,
        92, 64, 69, 16, 80, 13, 67, 58, 95, 88 };
    int b[] = { 42, 80, 61, 95, 43, 58, 36, 31, 87, 96,
        41, 64, 92, 69, 16, 13, 6, 67, 45, 88 };
#else
    int a[] = {63, 7, 56, 22, 11, 29, 2, 28, 32, 4,
        14, 0, 24, 68, 59, 70, 30, 18, 71, 83 };
    int b[] = { 14, 63, 7, 71, 30, 83, 56, 22,  29, 2, 
        59, 32, 4, 0, 24, 28, 68,  70,  11, 18,};
#endif

    int isTaller = 0;
    AVLTree t = NULL;
    for (int i = 0; i < NodeNum; ++i) {
        insertAVLTree(&t, a[i], &isTaller);
        //traversal(t);
        //printf("\n");
    }
    traversal(t);
    printf("\n");
    
    int isShorter = 0;
    for (int i = 0; i < NodeNum; ++i) {
        printf("delete %d:\n", b[i]);
        deleteAVLTree(&t, b[i], &isShorter);
        traversal(t);
        printf("\n");
    }

    return 0;
}