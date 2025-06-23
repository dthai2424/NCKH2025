#include <bits/stdc++.h>

using namespace std;

struct Node
{
    string item_name;
    int count;
    vector<Node *> children;
    int pre_order;
    int post_order;
    Node(string name) : item_name(name), count(1), pre_order(0), post_order(0) {}
};

struct PPCTree
{
    Node *root;

    void constructTree(const vector<vector<string>> &transactions, int minSupport)
    {
        root = new Node("");
        for (const auto &transaction : transactions)
        {
            insertTransaction(transaction, root);
        }
    }

    void insertTransaction(const vector<string> &transaction, Node *node)
    {
        if (transaction.empty())
            return;

        string item = transaction[0];
        Node *child = nullptr;

        // Tìm nút con phù hợp
        for (Node *c : node->children)
        {
            if (c->item_name == item)
            {
                child = c;
                child->count++;
                break;
            }
        }

        // Nếu không tìm thấy, tạo nút mới
        if (!child)
        {
            child = new Node(item);
            node->children.push_back(child);
        }

        // Đệ quy thêm các mục còn lại
        vector<string> subTransaction(transaction.begin() + 1, transaction.end());
        insertTransaction(subTransaction, child);
    }

    void PreOrder_Generate(Node *node, int &order)
    {
        if (!node)
            return;

        node->pre_order = order++;
        for (Node *child : node->children)
        {
            PreOrder_Generate(child, order);
        }
        node->post_order = order++;
    }

    void PostOrder_Generate(Node *node, int &order)
    {
        if (!node)
            return;

        for (Node *child : node->children)
        {
            PostOrder_Generate(child, order);
        }
        node->post_order = order++;
    }
};

int main()
{
}