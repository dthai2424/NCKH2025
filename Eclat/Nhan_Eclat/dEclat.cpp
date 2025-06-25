#include <bits/stdc++.h>
using namespace std;
#define el '\n'

struct transaction {
    vector<int> items;
    transaction(vector<int> items) : items(items) {}
};

struct node {
    int support;
    vector<int> itemset;
    vector<int> diffset;
};

int sz = 0;

// hàm tách chuỗi dùng để đọc dữ liệu
vector<int> split(string str) {
    vector<int> result;
    stringstream ss(str);
    int item;
    while (ss >> item) {
        result.push_back(item);
        sz = max(sz, result.back());
    }
    return result;
}

// hàm đọc dữ liệu
void read_data(vector<transaction> &transactions) {
    string line;
    while (getline(cin, line)) {
        transactions.push_back(transaction(split(line)));
    }
}

// hàm khởi tạo diffset
void init_diffset(vector<transaction> transactions, vector<node> &node_list, int minsupp) {
    vector<node> tmp(sz + 1);
    int pos[sz + 1];
    for (int i = 0; i <= sz; ++i) {
        tmp[i].itemset.push_back(i);
        pos[i] = 0;
    }

    // khởi tạo diffset
    for (int i = 0; i < transactions.size(); ++i) {
        for (int item : transactions[i].items) {
            if (i == pos[item]) {
                pos[item] ++;
            } else {
                for (int j = pos[item]; j < i; ++j) {
                    tmp[item].diffset.push_back(j);
                }
                pos[item] = i + 1;
            }
        }
    }
    for (int item = 0; item <= sz; ++item) {
        if (pos[item] <= transactions.size()) {
            for (int j = pos[item]; j < transactions.size(); ++j) {
                tmp[item].diffset.push_back(j);
            }
        }
    }

    // lọc itemset support >= minsupp
    for (node n : tmp) {
        if (transactions.size() - n.diffset.size() >= minsupp) {
            n.support = transactions.size() - n.diffset.size();
            node_list.push_back(n);
        }
    }
}

// hàm tìm tập hiêu giữa hai tập
vector<int> subtracting(vector<int> diffseta, vector<int> diffsetb) {
    vector<int> result;
    int i = 0, j = 0;
    while (i < diffseta.size() && j < diffsetb.size()) {
        if (diffseta[i] == diffsetb[j]) {
            i++;
            j++;
        } else {
            if (diffseta[i] > diffsetb[j]) {
                j++;
            } else {
                result.push_back(diffseta[i]);
                i++;
            }
        }
    }
    while (i < diffseta.size()) {
        result.push_back(diffseta[i]);
        i++;
    }
    return result;
}

// thuật eclat
void eclat(vector<node> node_list, int minsupp, vector<vector<int>> &FL) {
    for (int i = 0; i < node_list.size(); ++i) {
        FL.push_back(node_list[i].itemset);
        vector<node> new_node_list;
        for (int j = i + 1; j < node_list.size(); ++j) {
            node new_node;
            new_node.diffset = subtracting(node_list[j].diffset, node_list[i].diffset);
            if (node_list[i].support - new_node.diffset.size() >= minsupp) {
                new_node.itemset = node_list[i].itemset;
                new_node.itemset.push_back(node_list[j].itemset.back());
                new_node.support = node_list[i].support - new_node.diffset.size();
                new_node_list.push_back(new_node);
            }
        }
        if (new_node_list.size()) {
            eclat(new_node_list, minsupp, FL);
        }
    }
}

int32_t main() {
    ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    freopen("Eclat\\Nhan_Eclat\\chess.txt", "r", stdin);
//    freopen("Eclat\\Nhan_Eclat\\output.txt", "w", stdout);
    vector<transaction> transactions;
    read_data(transactions);
    vector<node> node_list;
    vector<vector<int>> FL;
    int minsupp = 0.6 * transactions.size();
    init_diffset(transactions, node_list, minsupp);
    eclat(node_list, minsupp, FL);
    cout << "So tap: " << FL.size() << el;
    cout << "minsupp: " << minsupp << el;
    cout << "So giao dich: " << transactions.size() << el;

    return 0;
}
