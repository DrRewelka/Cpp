#include <iostream>
#include <stdio.h>

using namespace std;

/*struct node
{
    node *up;
    node *left;
    node *right;
    int data;
    char color;
};

node S = {&S, &S, &S, 0, 'B'};

void rotL(node* &root, node* A)
{
    node *B, *p;
    B = A->right;
    if(B != &S)
    {
        p = A->up;
        A->right = B->left;
        if(A->right != &S) A->right->up = A;

        B->left = A;
        B->up = p;
        A->up = B;

        if(p != &S)
        {
          if(p->left == A) p->left = B; else p->right = B;
        }
        else root = B;
    }
}

void rotR(node* &root, node* A)
{
    node *B, *p;
    B = A->left;
    if(B != &S)
    {
        p = A->up;
        A->left = B->right;
        if(A->left != &S) A->left->up = A;

        B->right = A;
        B->up = p;
        A->up = B;

        if(p != &S)
        {
          if(p->left == A) p->left = B; else p->right = B;
        }
        else root = B;
    }
}

void insertBT(node* &root, int a);
{
    node *X, *Y;
    X = new RBTNode;
    X->left  = &S;
    X->right = &S;
    X->up    = root;
    X->key   = k;
    if(X->up == &S) root = X;
    else
        while(true)
        {
            if(k < X->up->key)
            {
                if(X->up->left == &S)
                {
                    X->up->left = X;
                    break;
                }
                X->up = X->up->left;
            }
            else
            {
                if(X->up->right == &S)
                {
                    X->up->right = X;
                    break;
                }
                X->up = X->up->right;
            }
        }
    X->color = 'R';
    while((X != root) && (X->up->color == 'R'))
    {
        if(X->up == X->up->up->left)
        {
            Y = X->up->up->right;
            if(Y->color == 'R')
            {
                X->up->color = 'B';
                Y->color = 'B';
                X->up->up->color = 'R';
                X = X->up->up;
                continue;
            }
            if(X == X->up->right)
            {
                X = X->up;
                rot_L(root, X);
            }
            X->up->color = 'B';
            X->up->up->color = 'R';
            rot_R(root, X->up->up);
            break;
        }
        else
        {
            Y = X->up->up->left;
            if(Y->color == 'R')
            {
                X->up->color = 'B';
                Y->color = 'B';
                X->up->up->color = 'R';
                X = X->up->up;
                continue;
            }
            if(X == X->up->left)
            {
                X = X->up;
                rot_R(root, X);
            }
            X->up->color = 'B';
            X->up->up->color = 'R';
            rot_L(root, X->up->up);
            break;
        }
    }
    root->color = 'B';
}

void removeBT(node* &root, node* X)
{
    node *W, *Y, *Z;
    if((X->left == &S) || (X->right == &S)) Y = X;
    else Y = succRBT(X);
    if(Y->left != &S) Z = Y->left;
    else Z = Y->right;
    Z->up = Y->up;
    if(Y->up == &S) root = Z;
    else if(Y == Y->up->left) Y->up->left  = Z;
    else Y->up->right = Z;
    if(Y != X) X->key = Y->key;
    if(Y->color == 'B')
        while((Z != root) && (Z->color == 'B'))
            if(Z == Z->up->left)
            {
                W = Z->up->right;
                    if(W->color == 'R')
                    {
                        W->color = 'B';
                        Z->up->color = 'R';
                        rot_L(root, Z->up);
                        W = Z->up->right;
                    }
                    if((W->left->color == 'B') && (W->right->color == 'B'))
                    {
                        W->color = 'R';
                        Z = Z->up;
                        continue;
                    }
                    if(W->right->color == 'B')
                    {
                        W->left->color = 'B';
                        W->color = 'R';
                        rot_R(root, W);
                        W = Z->up->right;
                    }
                    W->color = Z->up->color;
                    Z->up->color = 'B';
                    W->right->color = 'B';
                    rot_L(root, Z->up);
                    Z = root;
            }
            else
            {
                W = Z->up->left;
                if(W->color == 'R')
                {
                    W->color = 'B';
                    Z->up->color = 'R';
                    rot_R(root, Z->up);
                    W = Z->up->left;
                }
                if((W->left->color == 'B') && (W->right->color == 'B'))
                {
                    W->color = 'R';
                    Z = Z->up;
                    continue;
                }
                if(W->left->color == 'B')
                {
                    W->right->color = 'B';
                    W->color = 'R';
                    rot_L(root, W);
                    W = Z->up->left;
                }
                W->color = Z->up->color;
                Z->up->color = 'B';
                W->left->color = 'B';
                rot_R(root, Z->up);
                Z = root;
            }
    Z->color = 'B';
    delete Y;
}

node* n_isin(node* A, int x)
{
    while(A != &S)
    {
        if(A->data == x) return A;
        if(x < A->data) A = A->left;
        else A = A->right;
    }
    return NULL;
}

void n_add(node* &A, int x)
{
    if(!n_isin(A, x)) insertBT(A, x);
}

void n_remove(node* &A, int x)
{
    node *p = n_isin(A, x);
    if(p) removeBT(A, p);
}
*/
int main()
{
    char w;
    int t, n, m, a, b;
    int counterBajtka, counterBitka;
    scanf("%u", t);

    for(int i = 0; i < t; i++)
    {
        scanf("%u %u", n, m);
        for(int i = 0; i < t; i++)
            scanf("%u %c %u", a, w, b);
        counterBajtka = n;
        counterBitka = n;
        for(int j = 0; j < m; j++)
        {
            if(1 <= a <= n && 1 <= b <= n)
            {
                if(w == '<')
                    counterBajtka--;
                else if(w == '>')
                    counterBitka--;
            }
        }
        if(counterBajtka > counterBitka)
            cout << "WYGRANA" << endl;
        else if(counterBajtka == counterBitka)
            cout << "REMIS" << endl;
        else if(counterBajtka < counterBitka)
            cout << "PRZEGRANA" << endl;
    }
    return 0;
}
