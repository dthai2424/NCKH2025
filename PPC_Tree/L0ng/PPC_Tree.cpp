#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <math.h>
#include <iterator>

using namespace std;

// Cấu trúc đại diện một nút trong N-List
struct NListNode
{
    int pre;
    int post;
    int count;

    NListNode(int pre, int post, int count)
        : pre(pre), post(post), count(count) {}
};

// Kiểm tra node a có phải tổ tiên của node b không
bool isAncestor(const NListNode &a, const NListNode &b)
{
    return a.pre < b.pre && a.post > b.post;
}

// Giao NList
// Chưa tối ưu lắm tí em tối ưu sau
vector<NListNode> intersectNList(const vector<NListNode> &A, const vector<NListNode> &B)
{
    vector<NListNode> result;
    int i = 0, j = 0;
    int n = A.size(), m = B.size();
    while (i < n && j < m)
    {
        if (A[i].pre < B[j].pre)
        {
            if (A[i].post > B[j].post)
            {
                result.emplace_back(A[i].pre, A[i].post, min(A[i].count, B[j].count));
                ++j;
            }
            else
                ++i;
        }
        else
            ++j;
    }
    auto ptr1 = result.begin();
    auto ptr2 = next(ptr1);
    while (ptr1 != result.end() && ptr2 != result.end())
    {
        if (ptr1->pre == ptr2->pre && ptr1->post == ptr2->post)
        {
            ptr1->count += ptr2->count;
            result.erase(ptr2);
            ptr2 = next(ptr1);
        }
        else
        {
            ptr1 = ptr2;
            ptr2 = next(ptr1);
        }
    }
    return result;
}

// Tính support
int getSupport(const vector<NListNode> &nlist)
{
    int total = 0;
    for (const auto &node : nlist)
    {
        total += node.count;
    }
    return total;
}

// Cấu trúc Node của cây PPC
struct Node
{
    string item;
    int count;
    int pre = 0, post = 0;
    Node *parent;
    vector<Node *> children;

    Node(string itm = "", Node *par = nullptr)
        : item(itm), count(1), parent(par) {}
};

// Cấu trúc cây PPC
struct PPCTree
{
    Node *root;
    unordered_map<string, vector<NListNode>> nlists;
    unordered_map<string, int> frequency;
    int preorderCounter = 0;
    int postorderCounter = 0;
    int minSupport;

    PPCTree(int minSup) : minSupport(minSup)
    {
        root = new Node();
    }

    // Đếm tần suất từng item
    void countFrequency(const vector<vector<string>> &transactions)
    {
        for (const auto &trans : transactions)
        {
            for (const auto &item : trans)
            {
                frequency[item]++;
            }
        }
    }

    // Lọc và sắp xếp lại giao dịch theo tần suất giảm dần
    vector<string> filterAndSort(const vector<string> &transaction)
    {
        vector<string> result;

        for (const auto &item : transaction)
        {
            if (frequency[item] >= minSupport)
            {
                result.push_back(item);
            }
        }

        // Sắp xếp theo tần suất giảm dần
        sort(result.begin(), result.end(), [&](const string &a, const string &b)
             {
                 if (frequency[a] != frequency[b])
                     return frequency[a] > frequency[b];
                 return a < b; // thứ tự từ điển nếu bằng nhau
             });

        return result;
    }

    void insertTransaction(const vector<string> &transaction)
    {
        Node *current = root;
        for (const auto &item : transaction)
        {
            Node *child = nullptr;

            for (Node *c : current->children)
            {
                if (c->item == item)
                {
                    c->count++;
                    child = c;
                    break;
                }
            }

            if (!child)
            {
                child = new Node(item, current);
                current->children.push_back(child);
            }

            current = child;
        }
    }

    void assignPrePost(Node *node)
    {
        if (!node)
            return;
        node->pre = preorderCounter++;

        for (Node *child : node->children)
        {
            assignPrePost(child);
        }

        node->post = postorderCounter++;

        if (!node->item.empty() && frequency[node->item] >= minSupport)
        {
            nlists[node->item].emplace_back(node->pre, node->post, node->count);
        }
    }

    void buildNLists(const vector<vector<string>> &transactions)
    {
        // Bước 1: đếm tần suất
        countFrequency(transactions);

        // Bước 2: lọc và chèn các giao dịch
        for (const auto &trans : transactions)
        {
            auto filtered = filterAndSort(trans);
            if (!filtered.empty())
            {
                insertTransaction(filtered);
            }
        }

        // Bước 3: gán pre/post và sinh NList
        assignPrePost(root);
    }

    void printNLists()
    {
        cout << "=== N-Lists (filtered by minSup = " << minSupport << ") ===" << endl;
        for (const auto &pair : nlists)
        {
            cout << "Item: " << pair.first << endl;
            for (const auto &node : pair.second)
            {
                cout << "  (pre=" << node.pre << ", post=" << node.post << ", count=" << node.count << ")" << endl;
            }
        }
    }
};

int main()
{
    vector<vector<string>> transactions = {
        {"A", "C", "G", "F"},
        {"E", "A", "C", "B"},
        {"E", "C", "B", "I"},
        {"B", "F", "H"},
        {"B", "F", "E", "C", "D"}};

    int minSupport = ceil(0.25 * transactions.size());

    // Xây cây PPC và sinh NList
    PPCTree tree(minSupport);
    tree.buildNLists(transactions);
    tree.printNLists();

    // Lấy NList của B và F
    auto B = tree.nlists["B"];
    auto F = tree.nlists["F"];

    // Giao NList(B) và NList(F)
    auto BF = intersectNList(B, F);

    // In NList(BF)
    cout << "\nNList(BF):" << endl;
    for (const auto &node : BF)
    {
        cout << "(" << node.pre << ", " << node.post << ", " << node.count << ")" << endl;
    }

    cout << "Support(BF): " << getSupport(BF) << endl;

    return 0;
}
