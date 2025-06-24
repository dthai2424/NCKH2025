#include <bits/stdc++.h>

using namespace std;

int transactionSize = 0;
int minSupport = 0;

struct NListEntry
{
    int pre, post, count;
    NListEntry(int p, int q, int c) : pre(p), post(q), count(c) {}
};

struct Node
{
    string item_name;
    int count;
    vector<Node *> children;
    Node *parent;
    int pre_order;
    int post_order;
    Node(string name) : item_name(name), count(1), pre_order(0), post_order(0) {}
};

bool ListCMP(const struct List &a, const struct List &b)
{
    if (a.count != b.count)
        return a.count > b.count; // Sắp xếp giảm dần theo count
    return a.order < b.order;     // Nếu count bằng nhau, sắp xếp theo order
}

struct List
{
    int order;
    int count;
    vector<string> items;

    List(int size) : order(0), count(0), items(size) {}
    List() : order(0), count(0) {}
};

struct PPCTree
{
    Node *root;
    unordered_map<string, vector<NListEntry>> nlists;
    vector<List> list[100];
    int PreOrderCount = 0;
    int PostOrderCount = 0;

    void constructTree(const vector<vector<string>> &transactions, int minSupport)
    {
        root = new Node("");
        for (const auto &transaction : transactions)
        {
            insertTransaction(transaction, root);
        }
        PreOrder_Generate(root, PreOrderCount);
        PostOrder_Generate(root, PostOrderCount);
    }

    void insertTransaction(const vector<string> &transaction, Node *node)
    {
        if (transaction.empty())
            return;
        ++transactionSize;
        string item = transaction[0];
        Node *child = nullptr;

        // Tìm nút con phù hợp
        for (Node *c : node->children)
        {
            if (c->item_name == item)
            {
                child = c;
                child->count++;
                child->parent = node; // Cập nhật parent
                break;
            }
        }

        // Nếu không tìm thấy, tạo nút mới
        if (!child)
        {
            child = new Node(item);
            child->parent = node; // Cập nhật parent
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

        // Lưu thông tin vào nlists
        if (!node->item_name.empty())
        {
            nlists[node->item_name].emplace_back(node->pre_order, node->post_order, node->count);
        }
    }

    void L1_Construction(Node *root)
    {
        for (auto &entry : nlists)
        {
            string item = entry.first;
            // count
            vector<NListEntry> &entries = entry.second;

            // Tạo danh sách L1
            List l1(1);
            l1.count = 0;
            l1.items.push_back(item);
            for (const auto &nle : entries)
            {
                l1.count += nle.count;
            }
            list[1].push_back(l1);
        }
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

    void getPreOrderNodes(Node *node, vector<Node *> &preOrderNodes)
    {
        if (!node)
            return;

        preOrderNodes.push_back(node);
        for (Node *child : node->children)
        {
            getPreOrderNodes(child, preOrderNodes);
        }
    }

    int getItemOrder(const string &item)
    {
        for (int i = 0; i < list[1].size(); ++i)
        {
            if (list[1][i].items[0] == item)
            {
                return list[1][i].order; // Trả về chỉ số của mục trong L1
            }
        }
        return -1; // Nếu không tìm thấy
    }

    void L2_Construction(Node *root)
    {
        sort(list[1].begin(), list[1].end(), ListCMP);
        for (int i = 0; i < list[1].size(); ++i)
        {
            list[1][i].order = i; // Đặt thứ tự cho các mục trong L1
        }
        int size = list[1].size();
        int tmp2[size][size];

        vector<Node *> preOrderNodes;
        getPreOrderNodes(root, preOrderNodes);

        // Duyệt theo pre-order
        for (Node *n : preOrderNodes)
        {
            int i = getItemOrder(n->item_name);
            if (i == -1)
                continue;

            Node *ancestor = n->parent;
            while (ancestor && ancestor->item_name.empty())
            {
                int j = getItemOrder(ancestor->item_name);
                if (j != -1)
                {
                    tmp2[i][j]++;
                }
                ancestor = ancestor->parent; // Tìm nút cha không rỗng
            }
        }

        // Tạo L2
        vector<List> &L2 = list[2];
        L2.clear();
        for (size_t i = 0; i < size; ++i)
        {
            for (size_t j = 0; j < size; ++j)
            {
                if (tmp2[i][j] >= minSupport)
                {
                    L2.emplace_back(list[1][i].items[0], list[1][j].items[0], tmp2[i][j]);
                }
            }
        }
    }
};

int main()
{
    PPCTree tree;
    vector<vector<string>> transactions = {
        {"A", "B", "C"},
        {"A", "B"},
        {"B", "C"},
        {"A", "C"},
        {"B"},
        {"C"}};

    minSupport = 0.2 * transactionSize; // Tính minSupport từ tỷ lệ phần trăm

    tree.constructTree(transactions, minSupport);

    // In ra kết quả
    for (const auto &entry : tree.nlists)
    {
        cout << "Item: " << entry.first << "\n";
        for (const auto &nle : entry.second)
        {
            cout << "<(" << nle.pre << ", " << nle.post << "): " << nle.count << ">\n";
        }
    }

    return 0;
}