#include <bits/stdc++.h>
#define endl "\n"

using namespace std;

map<string, set<int>> convert(vector<vector<string>> &transaction){
    map<string, set<int>> res;
    for(int i=0;i<(int)transaction.size();i++){
        for(string item : transaction[i]){
            res[item].insert(i+1);
        }
    }
    return res;
}

void ECLAT(vector<pair<string, set<int>>> items, set<int> prefixTids, vector<string> prefix, int minSup,
    vector<pair<vector<string>, int>>& results){
    for(size_t i = 0; i < items.size(); ++i){
        const string item = items[i].first;
        const set<int> itemTids = items[i].second;

        set<int> newTids;
        if (prefix.empty()){
            newTids = itemTids; 
        } else {
            auto it1 = prefixTids.begin();
            auto it2 = itemTids.begin();
            
            while (it1 != prefixTids.end() && it2 != itemTids.end()){
                if (*it1 < *it2) {
                    ++it1;
                } else if (*it2 < *it1) {
                    ++it2;
                } else {
                    newTids.insert(*it1);
                    ++it1;
                    ++it2;
                }
            }
        }
        
        int support = newTids.size();
        if (support >= minSup){
            vector<string> newItemset = prefix;
            newItemset.push_back(item);
            
            results.push_back({newItemset, support});
            
            vector<pair<string, set<int>>> newItems;
            for (size_t j = i + 1; j < items.size(); ++j){
                newItems.push_back(items[j]);
            }
            
            if (!newItems.empty()){
                ECLAT(newItems, newTids, newItemset, minSup, results);
            }
        }
    }
}

bool comp(pair<string,set<int>> a, pair<string, set<int>> b){
    return a.second.size() < b.second.size();
}

signed main(){
    vector<vector<string>> transaction ={
        {"f", "a", "c", "d", "g", "i", "m", "p"},
        {"a", "b", "c", "f", "l", "m", "o"},
        {"b", "f", "h", "j", "o"},
        {"b", "c", "k", "s", "p"},
        {"a", "f", "c", "e", "l", "p", "m", "n"}
    };
    
    map<string, set<int>> convert_transaction= convert(transaction);
    // for(pair<string, set<int>> x : convert_transaction){
    //     cout << x.first << ": ";
    //     for(int i:x.second){
    //         cout << i << " ";
    //     }
    //     cout << endl;
    // }
    int minSup=3;

    // loc cac itemset co sup < minSup
    vector<pair<string, set<int>>> itemSet;
    for(pair<string, set<int>> x : convert_transaction){
        if((int)x.second.size() >= minSup){
            itemSet.push_back({x.first, x.second});
        }
    }

    //sap xep cac itemset theo so luong TID trong cac itemset tang dan
    //vd: milk:  1 2 5 4
    //    butter 1 4 3
    //    bread  1 2
    sort(itemSet.begin(), itemSet.end(), comp);

    // luu kq
    vector<pair<vector<string>,int>> ans;
    ECLAT(itemSet, set<int>{}, vector<string>{}, minSup, ans);

    cout << "cac tap pho bien:" << endl;
    for(pair<vector<string>, int> x : ans){
        //item
        for(string y : x.first){
            cout << y << " ";
        }
        //sup
        cout << ": " << x.second << endl;
    }
}