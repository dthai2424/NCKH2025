#include <bits/stdc++.h>

using namespace std;

struct DBV
{
    int pos;
    vector<int> bit;

    DBV() : pos(-1) {}
};

map<string, DBV> dbv_map;
map<string, vector<int>> tidset;

DBV intersection(const DBV &a, const DBV &b)
{
    DBV res;
    res.pos = max(a.pos, b.pos);
    int i = a.pos < b.pos ? b.pos - a.pos : 0;
    int j = a.pos < b.pos ? 0 : a.pos - b.pos;
    int count = min(a.bit.size() - i, b.bit.size() - j);

    while (count > 0 && !(a.bit[i] && b.bit[j]))
    {
        i++;
        j++;
        res.pos++;
        count--;
    }

    int i1 = i + count - 1, j1 = j + count - 1;
    while (count > 0 && !(a.bit[i1] && b.bit[j1]))
    {
        i1--;
        j1--;
        count--;
    }

    if (count <= 0)
    {
        res.pos = 0;
        res.bit.clear();
        return res;
    }

    res.bit.resize(count);
    for (int k = 0; k < count; k++)
    {
        res.bit[k] = a.bit[i + k] && b.bit[j + k];
    }

    return res;
}

void initTidset(const vector<vector<string>> &transactions)
{
    for (int i = 0; i < transactions.size(); i++)
    {
        for (const auto &item : transactions[i])
        {
            tidset[item].push_back(i);
        }
    }
}

void initDBV()
{
    for (const auto &pair : tidset)
    {
        DBV dbv;
        dbv.pos = 0;
        dbv.bit.resize(pair.second[pair.second.size() - 1], false);
        for (int tid : pair.second)
        {
            dbv.bit[tid] = true;
        }
        dbv_map[pair.first] = dbv;
    }
}

void print_frequent(const vector<vector<int>> &frequent_itemsets)
{
    cout << "\nFrequent itemsets:\n";
    for (const auto &itemset : frequent_itemsets)
    {
        cout << "{ ";
        for (int item : itemset)
        {
            cout << item << " ";
        }
        cout << "}\n";
    }
}

void eclat(const vector<DBV> &tidset, int min_freq)
{
    vector<vector<int>> frequent_itemsets;
    for (int i = 0; i < tidset.size(); i++)
    {
        frequent_itemsets.push_back(tidset[i].bit);
        print_frequent(frequent_itemsets);
        vector<DBV> new_tidset;
        for (int j = i + 1; j < tidset.size(); j++)
        {
            DBV intersect_tid = intersection(tidset[i], tidset[j]);
            int sup = count(intersect_tid.bit.begin(), intersect_tid.bit.end(), 1);
            if (sup >= min_freq)
            {
                new_tidset.push_back(intersect_tid);
            }
        }
        if (!new_tidset.empty())
        {
            eclat(new_tidset, min_freq);
        }
    }
}

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

    initTidset(transactions);
    initDBV();

    cout << "Frequent itemsets:\n";

    eclat(vector<DBV>(dbv_map.begin(), dbv_map.end()), 2); // Giả sử min_freq = 2
}