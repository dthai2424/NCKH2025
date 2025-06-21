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

// Cấu trúc FP-Tree
struct FPTree
{
    FPNode *root;                                   // Nút gốc của FP-Tree
    unordered_map<string, FPNode *> headerTable;    // Header Table
    map<string, int> itemCount;                     // Tần suất các mục
    int minSupport;                                 // Ngưỡng support tối thiểu
    map<vector<string>, int> frequentItemsetCounts; // Lưu tần suất của tập hợp thường xuyên

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
        // [path, count] là các cặp trong conditional pattern base
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
        // Duyệt qua Header Table để tìm các mục thường xuyên
        for (auto it = headerTable.begin(); it != headerTable.end(); ++it)
        {
            string item = it->first;
            if (itemCount[item] < minSupport)
                continue;

            // Thêm mục vào tập hợp hiện tại
            vector<string> newPrefix = prefix;
            newPrefix.push_back(item);
            result.push_back(newPrefix);
            frequentItemsetCounts[newPrefix] = 0;

            // Xây conditional pattern base
            vector<pair<vector<string>, int>> cpb;
            getConditionalPatternBase(item, cpb);

            // Tính tần suất của tập hợp thường xuyên
            for (const auto &path : cpb)
            {
                frequentItemsetCounts[newPrefix] += path.second;
            }

            // Xây conditional FP-Tree
            FPTree *condTree = buildConditionalFPTree(cpb);

            // Đệ quy khai thác
            condTree->mineFrequentItemsets(result, newPrefix);

            delete condTree;
        }
    }

    // Lấy tần suất mục
    const map<string, int> &getItemCount() const { return itemCount; }

    // Lấy tần suất tập hợp thường xuyên
    int getItemsetCount(const vector<string> &itemset) const
    {
        auto it = frequentItemsetCounts.find(itemset);
        if (it != frequentItemsetCounts.end())
            return it->second;
        return itemCount.at(itemset[0]);
    }
};

// Tạo luật kết hợp từ tập hợp thường xuyên
void generateAssociationRules(const vector<vector<string>> &frequentItemsets,
                              const FPTree &fptree, float minConf, int totalTransactions)
{
    cout << "\nAssociation Rules (minConf = " << minConf * 100 << "%):\n";
    for (const auto &itemset : frequentItemsets)
    {
        if (itemset.size() < 2) // Bỏ qua tập hợp 1 mục
            continue;

        // Tạo tất cả các tập con không rỗng cho tiền đề
        for (int mask = 1; mask < (1 << itemset.size()) - 1; mask++)
        {
            vector<string> antecedent, consequent;
            for (int i = 0; i < itemset.size(); i++)
            {
                if (mask & (1 << i))
                    antecedent.push_back(itemset[i]);
                else
                    consequent.push_back(itemset[i]);
            }

            // Tìm support của tiền đề
            int antCount = 0;
            vector<string> antItemset = antecedent;
            sort(antItemset.begin(), antItemset.end());
            for (const auto &fi : frequentItemsets)
            {
                vector<string> sortedFi = fi;
                sort(sortedFi.begin(), sortedFi.end());
                if (antItemset == sortedFi)
                {
                    antCount = fptree.getItemsetCount(fi);
                    break;
                }
            }
            if (antCount == 0) // Nếu không tìm thấy, thử lấy từ itemCount cho tập 1 mục
            {
                if (antecedent.size() == 1)
                    antCount = fptree.getItemCount().at(antecedent[0]);
            }

            // Tính confidence
            int itemsetCount = fptree.getItemsetCount(itemset);
            float confidence = (float)itemsetCount / antCount;
            float support = (float)itemsetCount / totalTransactions;

            if (confidence >= minConf)
            {
                cout << "{ ";
                for (const auto &item : antecedent)
                    cout << item << " ";
                cout << "} -> { ";
                for (const auto &item : consequent)
                    cout << item << " ";
                cout << "} (support = " << support << ", confidence = " << confidence * 100 << "%)\n";
            }
        }
    }
}

// Hàm chính
int main()
{
    // Đọc dữ liệu từ input.txt
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    vector<vector<string>> transactions;
    string line;
    // Đọc từ input ngăn cách bởi dấu phẩy
    while (getline(cin, line))
    {
        vector<string> transaction;
        size_t pos = 0;
        while ((pos = line.find(',')) != string::npos)
        {
            transaction.push_back(line.substr(0, pos));
            line.erase(0, pos + 1);
        }
        if (!line.empty())
            transaction.push_back(line);
        transactions.push_back(transaction);
    }

    int minSupport = 3;  // Ngưỡng support tối thiểu
    float minConf = 0.7; // Ngưỡng confidence tối thiểu
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

    // In tập hợp thường xuyên
    cout << "Frequent Itemsets (minSupport = " << minSupport << "):\n";
    for (const auto &itemset : frequentItemsets)
    {
        cout << "{ ";
        for (const string &item : itemset)
        {
            cout << item << " ";
        }
        cout << "} : " << fptree.getItemsetCount(itemset) << endl;
    }

    // Sinh và in luật kết hợp
    generateAssociationRules(frequentItemsets, fptree, minConf, transactions.size());

    return 0;
}