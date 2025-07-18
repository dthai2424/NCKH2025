#include <vector>
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

const int minSupport = 2;
const double minConfidence = 0.6;

struct Itemset
{
    vector<string> items;
    int support;

    bool operator==(const Itemset &other) const
    {
        return items == other.items;
    }
};

// Danh sách giao dịch
vector<vector<string>> transactions = {
    {"milk", "bread", "butter"},
    {"bread", "butter"},
    {"milk", "bread"},
    {"milk", "bread", "butter"},
    {"milk", "butter"}};

// Sắp xếp itemset để so sánh chính xác
void sortItemset(vector<string> &items)
{
    sort(items.begin(), items.end());
}

// Kiểm tra items có nằm trong transaction không
bool contains(const vector<string> &transaction, const vector<string> &items)
{
    for (const string &item : items)
    {
        if (find(transaction.begin(), transaction.end(), item) == transaction.end())
            return false;
    }
    return true;
}

// Đếm support cho tập ứng viên
vector<Itemset> countSupport(const vector<vector<string>> &candidates)
{
    vector<Itemset> result;
    for (const auto &cand : candidates)
    {
        int count = 0;
        for (const auto &trans : transactions)
        {
            if (contains(trans, cand))
            {
                count++;
            }
        }
        if (count >= minSupport)
        {
            result.push_back({cand, count});
        }
    }
    return result;
}

// Sinh tập ứng viên từ tập phổ biến trước đó
vector<vector<string>> generateCandidates(const vector<Itemset> &prevFreq, int k)
{
    vector<vector<string>> candidates;
    int n = prevFreq.size();

    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            vector<string> a = prevFreq[i].items;
            vector<string> b = prevFreq[j].items;

            vector<string> merged = a;
            for (const auto &item : b)
            {
                if (find(merged.begin(), merged.end(), item) == merged.end())
                {
                    merged.push_back(item);
                }
            }

            sortItemset(merged);
            if (merged.size() == k && find(candidates.begin(), candidates.end(), merged) == candidates.end())
            {
                candidates.push_back(merged);
            }
        }
    }

    return candidates;
}

// In itemset
void printItemset(const vector<string> &items)
{
    cout << "{ ";
    for (size_t i = 0; i < items.size(); ++i)
    {
        cout << items[i];
        if (i < items.size() - 1)
            cout << ", ";
    }
    cout << " }";
}

// Sinh luật kết hợp
void generateAssociationRules(const vector<Itemset> &freqItemsets)
{
    cout << "\n== Các luật kết hợp (minConfidence = " << minConfidence << ") ==\n";
    for (const auto &item : freqItemsets)
    {
        if (item.items.size() < 2)
            continue;
        int supAB = item.support;

        int n = item.items.size();
        for (int mask = 1; mask < (1 << n) - 1; ++mask)
        {
            vector<string> A, B;
            for (int i = 0; i < n; ++i)
            {
                if (mask & (1 << i))
                    A.push_back(item.items[i]);
                else
                    B.push_back(item.items[i]);
            }

            // Tìm support của A
            int supportA = 0;
            for (const auto &t : freqItemsets)
            {
                if (t.items == A)
                {
                    supportA = t.support;
                    break;
                }
            }

            if (supportA == 0)
                continue;

            double conf = (double)supAB / supportA;
            if (conf >= minConfidence)
            {
                printItemset(A);
                cout << " => ";
                printItemset(B);
                cout << " (confidence: " << conf << ")\n";
            }
        }
    }
}

int main()
{
    vector<Itemset> allFrequentItemsets;

    // Bước 1: đếm các item đơn
    vector<string> allItems;
    for (const auto &t : transactions)
    {
        for (const auto &item : t)
        {
            if (find(allItems.begin(), allItems.end(), item) == allItems.end())
            {
                allItems.push_back(item);
            }
        }
    }

    // Tạo L1
    vector<vector<string>> candidates;
    for (const auto &item : allItems)
    {
        candidates.push_back({item});
    }

    vector<Itemset> freq = countSupport(candidates);
    int k = 2;

    while (!freq.empty())
    {
        for (auto &it : freq)
        {
            allFrequentItemsets.push_back(it);
        }

        vector<vector<string>> newCandidates = generateCandidates(freq, k);
        freq = countSupport(newCandidates);
        k++;
    }

    // In tất cả tập phổ biến
    cout << "== Các tập phổ biến (minSupport = " << minSupport << ") ==\n";
    for (const auto &item : allFrequentItemsets)
    {
        printItemset(item.items);
        cout << " -> support: " << item.support << "\n";
    }

    // In luật kết hợp
    // generateAssociationRules(allFrequentItemsets);

    return 0;
}
