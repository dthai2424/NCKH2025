#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <unordered_map>

using namespace std;

// Cấu trúc nút trong FP-Tree
struct FPNode
{
    string item;               // Tên mục
    int count;                 // Tần suất
    FPNode *parent;            // Con trỏ đến nút cha
    vector<FPNode *> children; // Danh sách con
    FPNode *next;              // Liên kết ngang trong Header Table
    FPNode(string _item, int _count = 1) : item(_item), count(_count), parent(nullptr), next(nullptr) {}
};

// Lớp FP-Tree
struct FPTree
{
    FPNode *root;                                // Nút gốc của FP-Tree
    unordered_map<string, FPNode *> headerTable; // Header Table
    map<string, int> itemCount;                  // Tần suất các mục
    int minSupport;                              // Ngưỡng support tối thiểu

    FPTree(int _minSupport) : minSupport(_minSupport), root(new FPNode("")) {}

    // Thêm giao dịch vào FP-Tree
    void insertTransaction(const vector<string> &transaction, FPNode *node)
    {
        if (transaction.empty())
            return;
        string item = transaction[0];
        FPNode *child = nullptr;

        // Tìm nút con phù hợp
        for (FPNode *c : node->children)
        {
            if (c->item == item)
            {
                child = c;
                child->count++;
                break;
            }
        }

        // Nếu không tìm thấy, tạo nút mới
        if (!child)
        {
            child = new FPNode(item);
            child->parent = node;
            node->children.push_back(child);

            // Cập nhật Header Table
            if (headerTable.find(item) == headerTable.end())
            {
                headerTable[item] = child;
            }
            else
            {
                FPNode *current = headerTable[item];
                while (current->next)
                    current = current->next;
                current->next = child;
            }
        }

        // Đệ quy thêm các mục còn lại
        vector<string> subTransaction(transaction.begin() + 1, transaction.end());
        insertTransaction(subTransaction, child);
    }

    // Xây dựng conditional pattern base cho một mục
    void getConditionalPatternBase(const string &item, vector<pair<vector<string>, int>> &cpb)
    {
        FPNode *current = headerTable[item];
        while (current)
        {
            vector<string> path;
            FPNode *parent = current->parent;
            while (parent && !parent->item.empty())
            {
                path.push_back(parent->item);
                parent = parent->parent;
            }
            if (!path.empty())
            {
                reverse(path.begin(), path.end());
                cpb.push_back({path, current->count});
            }
            current = current->next;
        }
    }

    // Xây dựng conditional FP-Tree
    FPTree *buildConditionalFPTree(const vector<pair<vector<string>, int>> &cpb)
    {
        FPTree *condTree = new FPTree(minSupport);
        // [path, count]
        for (auto conditional_pattern_base : cpb)
        {
            for (int i = 0; i < conditional_pattern_base.second; i++)
            {
                condTree->insertTransaction(conditional_pattern_base.first);
            }
        }
        return condTree;
    }

    // Thêm giao dịch
    void insertTransaction(const vector<string> &transaction)
    {
        vector<string> sortedTransaction;
        for (const string &item : transaction)
        {
            if (itemCount[item] >= minSupport)
            {
                sortedTransaction.push_back(item);
            }
        }
        sort(sortedTransaction.begin(), sortedTransaction.end(),
             [this](const string &a, const string &b)
             {
                 return itemCount[a] > itemCount[b] ||
                        (itemCount[a] == itemCount[b] && a < b);
             });
        insertTransaction(sortedTransaction, root);
    }

    // Cập nhật tần suất mục
    void updateItemCount(const vector<string> &transaction)
    {
        for (const string &item : transaction)
        {
            itemCount[item]++;
        }
    }

    // Khai thác tập hợp thường xuyên
    void mineFrequentItemsets(vector<vector<string>> &result, vector<string> prefix = {})
    {
        // Duyệt Header Table từ dưới lên
        for (auto it = headerTable.begin(); it != headerTable.end(); ++it)
        {
            string item = it->first;
            if (itemCount[item] < minSupport)
                continue;

            // Thêm mục vào tập hợp hiện tại
            vector<string> newPrefix = prefix;
            newPrefix.push_back(item);
            result.push_back(newPrefix);

            // Xây conditional pattern base
            vector<pair<vector<string>, int>> cpb;
            getConditionalPatternBase(item, cpb);

            // Xây conditional FP-Tree
            FPTree *condTree = buildConditionalFPTree(cpb);

            // Đệ quy khai thác
            condTree->mineFrequentItemsets(result, newPrefix);

            delete condTree;
        }
    }

    // Lấy tần suất mục
    const map<string, int> &getItemCount() const { return itemCount; }
};

// Hàm chính
int main()
{
    // Dữ liệu giao dịch mẫu
    vector<vector<string>> transactions = {
        {"A", "B", "C"},
        {"A", "C"},
        {"A", "B", "D"},
        {"B", "C", "E"},
        {"A", "B", "C", "E"}};

    int minSupport = 3; // Ngưỡng support tối thiểu
    FPTree fptree(minSupport);

    // Quét lần 1: Cập nhật tần suất
    for (const auto &transaction : transactions)
    {
        fptree.updateItemCount(transaction);
    }

    // Quét lần 2: Xây FP-Tree
    for (const auto &transaction : transactions)
    {
        fptree.insertTransaction(transaction);
    }

    // Khai thác tập hợp thường xuyên
    vector<vector<string>> frequentItemsets;
    fptree.mineFrequentItemsets(frequentItemsets);

    // In kết quả
    cout << "Frequent Itemsets (minSupport = " << minSupport << "):\n";
    for (const auto &itemset : frequentItemsets)
    {
        cout << "{ ";
        for (const string &item : itemset)
        {
            cout << item << " ";
        }
        cout << "} : " << fptree.getItemCount().at(itemset[0]) << endl;
    }

    return 0;
}