#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

struct Item
{
    string name;
    vector<int> tidList; // danh sách TID
};

// Giao hai danh sách TID
vector<int> intersect(const vector<int> &a, const vector<int> &b)
{
    vector<int> result;
    int i = 0, j = 0;
    while (i < a.size() && j < b.size())
    {
        if (a[i] == b[j])
        {
            result.push_back(a[i]);
            i++;
            j++;
        }
        else if (a[i] < b[j])
        {
            i++;
        }
        else
        {
            j++;
        }
    }
    return result;
}

// Đệ quy khai thác tập mục
void eclat(string prefix, vector<Item> items, int minSupport)
{
    for (size_t i = 0; i < items.size(); i++)
    {
        string newPrefix = prefix + items[i].name;
        int support = items[i].tidList.size();

        if (support >= minSupport)
        {
            cout << "Itemset: {" << newPrefix << "} - Support: " << support << endl;

            vector<Item> suffix;
            for (size_t j = i + 1; j < items.size(); j++)
            {
                vector<int> intersected = intersect(items[i].tidList, items[j].tidList);
                if (!intersected.empty())
                {
                    Item newItem;
                    newItem.name = items[j].name;
                    newItem.tidList = intersected;
                    suffix.push_back(newItem);
                }
            }
            // Đệ quy tiếp
            eclat(newPrefix, suffix, minSupport);
        }
    }
}

int main()
{
    // Giao dịch:
    // T1: A B C
    // T2: A C D
    // T3: B C
    // T4: A B
    // T5: A C
    // T6: B C D

    map<string, vector<int>> verticalDB;
    verticalDB["A"] = {1, 2, 4, 5};
    verticalDB["B"] = {1, 3, 4, 6};
    verticalDB["C"] = {1, 2, 3, 5, 6};
    verticalDB["D"] = {2, 6};

    vector<Item> items;
    for (auto &p : verticalDB)
    {
        Item it;
        it.name = p.first;
        it.tidList = p.second;
        items.push_back(it);
    }

    int minSupport = 2;

    cout << "Frequent Itemsets with minSupport = " << minSupport << ":\n";
    eclat("", items, minSupport);

    return 0;
}
