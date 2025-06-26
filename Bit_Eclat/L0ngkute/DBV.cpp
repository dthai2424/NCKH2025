#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>

using namespace std;

int MIN_SUPPORT = 2; // Minimum support

// DBV - Dynamic Bit Vector
struct DBV
{
    vector<bool> bits;

    DBV(int size = 0) : bits(size, false) {}

    void set(int tid)
    {
        if (tid >= bits.size())
            bits.resize(tid + 1, false);
        bits[tid] = true;
    }

    // Giao hai DBV
    DBV intersect(const DBV &other) const
    {
        int size = min(bits.size(), other.bits.size());
        DBV result(size);
        for (int i = 0; i < size; ++i)
        {
            result.bits[i] = bits[i] && other.bits[i];
        }
        return result;
    }

    int support() const
    {
        return count(bits.begin(), bits.end(), true);
    }

    void print() const
    {
        for (bool b : bits)
            cout << (b ? "1" : "0");
        cout << " (support: " << support() << ")";
    }
};

// In tập mục kèm support
void printItemset(const vector<string> &itemset, const DBV &dbv)
{
    cout << "{ ";
    for (const string &item : itemset)
        cout << item << " ";
    cout << "} ";
    dbv.print();
    cout << endl;
}

// Hàm đệ quy Eclat
void eclat(vector<string> prefix, vector<pair<string, DBV>> items)
{
    for (size_t i = 0; i < items.size(); ++i)
    {
        vector<string> new_prefix = prefix;
        new_prefix.push_back(items[i].first);

        int sup = items[i].second.support();
        if (sup >= MIN_SUPPORT)
        {
            printItemset(new_prefix, items[i].second);

            // Sinh các tập con lớn hơn
            vector<pair<string, DBV>> suffix;
            for (size_t j = i + 1; j < items.size(); ++j)
            {
                DBV inter = items[i].second.intersect(items[j].second);
                if (inter.support() >= MIN_SUPPORT)
                {
                    suffix.emplace_back(items[j].first, inter);
                }
            }

            // Đệ quy
            if (!suffix.empty())
            {
                eclat(new_prefix, suffix);
            }
        }
    }
}

// Chuyển đổi giao dịch → Map<item, DBV>
map<string, DBV> buildDBV(const vector<vector<string>> &transactions)
{
    map<string, DBV> dbvMap;
    for (size_t tid = 0; tid < transactions.size(); ++tid)
    {
        for (const string &item : transactions[tid])
        {
            dbvMap[item].set(tid);
        }
    }
    return dbvMap;
}

int main()
{
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    vector<vector<string>> transactions;
    string line;
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
            transaction.push_back(line); // Thêm phần cuối cùng nếu có
        transactions.push_back(transaction);
    }

    map<string, DBV> dbvMap = buildDBV(transactions);

    // Chuyển về dạng vector để duyệt
    vector<pair<string, DBV>> items;
    // [item, DBV]
    for (auto hihi : dbvMap)
    {
        if (hihi.second.support() >= MIN_SUPPORT)
            items.emplace_back(hihi.first, hihi.second);
    }

    // Bắt đầu Eclat
    eclat({}, items);

    return 0;
}
