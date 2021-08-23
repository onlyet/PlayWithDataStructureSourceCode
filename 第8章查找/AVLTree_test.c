#include <stdio.h>
#include <malloc.h>

#define NodeNum 10

#define RH -1
#define EH 0
#define LH 1

typedef struct AVLNode {
    int data;
    int bf;
    struct AVLNode *left;
    struct AVLNode *right;
}AVLNode, *AVLTree;

// ��ת��tΪ���ڵ�Ķ����������tָ���µĸ��ڵ�
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
     // LL��
    case LH:
        (*t)->bf = EH;
        l->bf = EH;
        rotateRight(t);
        break;

    // LR��
    case RH:
        lr = l->right;
        switch (lr->bf)
        {
        case LH:            // ����ڵ���lr������
            (*t)->bf = RH;  // *t����ʧȥ���ӣ���(*t)->bf��Ϊ-1
            l->bf = EH;     // l������ʧȥ�Һ��ӣ���lr�����ӣ�����ڵ㣩�ֱ��l���Һ��ӣ���l->bf����0
            break;
        case EH:            // ����ڵ���lr
            (*t)->bf = EH;  // *t��r��ΪҶ�ӽڵ㣬��bf����0
            l->bf = EH; 
            break;
        case RH:            // ����ڵ���lr���Һ���
            (*t)->bf = EH;  // *t������ʧȥ���ӣ���lr���Һ��ӣ�����ڵ㣩�ֱ��*t�����ӣ���(*t)->����0
            l->bf = LH;     // l����ʧȥ�Һ��ӣ���l->bf��1
            break;
        default:
            break;
        }

        lr->bf = EH;        // rl����1���2�㣬bf�϶���0

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
     // RR��
    case RH:
        (*t)->bf = EH;
        r->bf = EH;
        rotateLeft(t);
        break;

    // RL��
    case LH:
        rl = r->left;
        switch (rl->bf) {
        case RH:
            (*t)->bf = LH;  // (*t)һ�������ӣ��������Һ���û�ˣ�����bf��1
            r->bf = EH;     // rl���Һ��ӱ��r�����ӣ���r��bf��Ϊ0
            break;
        case EH:
            (*t)->bf = EH;
            r->bf = EH;     // *t��r��ΪҶ�ӽڵ㣬��bf����0
            break;
        case LH:
            (*t)->bf = EH;  // (*t)һ�������ӣ�rl�������㣬rl�����ӱ�Ϊ(*t)���Һ��ӣ���(*t)->bf��0
            r->bf = RH;     // ������rʧȥ���ӣ���r->bf��Ϊ-1
            break;
        default:
            break;
        }
        rl->bf = 0;         // rl����1���2�㣬bf�϶���0

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
            // ԭ���������ߣ�������ڵ����������һ����
            case RH:
                (*t)->bf = EH;
                *isTaller = 0;
                break;
            // ԭ����������һ���ߣ�������ڵ����������һ��
            case EH:
                (*t)->bf = LH;
                (*isTaller) = 1;
                break;
            // ԭ���������ߣ�������ڵ����Ҫ����ƽ��
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
                // ԭ���������ߣ������ҽڵ����Ҫ����ƽ��
            case RH:
                printf("balanceRight\n");
                balanceRight(t);
                *isTaller = 0;
                break;
                // ԭ����������һ���ߣ������ҽڵ����������һ��
            case EH:
                (*t)->bf = RH;
                *isTaller = 1;
                break;
                // ԭ���������ߣ������ҽڵ����������һ����
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
        // ǰ��������
        (*t) = (*t)->left;
        *isShorter = 0;
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

                if (EH == (*t)->left->bf) {
                    // ƽ���߶�-1
                    *isShorter = 0;
                }
                else {
                    // l���Һ���
                    *isShorter = 1;
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

                if (EH == (*t)->right->bf) {
                    *isShorter = 0;
                }
                else {
                    // r������
                    *isShorter = 1;
                }

                *isShorter = 1;
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
            //AVLTree *l = &(*t)->left;
            // ǰ��
            AVLTree pre = NULL;
            updateBf(&(*t)->left, isShorter, &pre);

            (*t)->data = pre->data;
            free(pre);

            switch ((*t)->bf)
            {
            case RH:
                balanceRight(t);

                if (EH == (*t)->bf) {
                    // ƽ���߶�-1
                    *isShorter = 0;
                }
                else {
                    *isShorter = 1;
                }
            default:
                break;
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
    //int a[] = { 3,2,1,4,5,6,7,10,9,8 };
    //int a[] = { 3,2,1,4,5,6,8,10,9,7 };
    int a[] = { 13,42,61,75,15,6,58,160,19,7 };
    int isTaller = 0;
    AVLTree t = NULL;
    for (int i = 0; i < NodeNum; ++i) {
        insertAVLTree(&t, a[i], &isTaller);
        //traversal(t);
    }
    traversal(t);
    printf("\n\n");
    
    int isShorter = 0;
    deleteAVLTree(&t, 42, &isShorter);
    deleteAVLTree(&t, 61, &isShorter);
    traversal(t);

    return 0;
}