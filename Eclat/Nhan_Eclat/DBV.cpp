#include<bits/stdc++.h>
using namespace std;
#define int long long
#define el '\n'
struct transaction {
    vector<int> items;
    transaction(vector<int> items) : items(items) {}
};

struct DBV {
    int pos;
    vector<int> bit_vector;
    DBV() : pos(-1) {}
};

struct node {
    int support;
    vector<int> itemset;
    DBV dbv;
    node() : support(0) {}
};

int sz = 0;
vector<int> count_bit(1 << 16);

// hàm chuẩn bị mảng đếm số bit 1 của các giá trị từ 0 -> 1 << 16
void repare() {
    for (int i = 0; i < (1 << 16); ++i) {
        for (int j = 0; j < 16; ++j) {
            if (i & (1 << j)) {
                count_bit[i]++;
            }
        }
    }
}

// hàm tách chuỗi dùng để đọc dữ liệu
vector<int> split(string str) {
    vector<int> result;
    stringstream ss(str);
    int item;
    while (ss >> item) {
        result.push_back(item);
        sz = max(sz, item);
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

// hàm khởi tạo các 1-itemset và các DBV của nó
void init_node(vector<transaction> transactions, vector<node> &node_list, int minsupp) {
    vector<node> tmp(sz + 1);
    for (int i = 0; i <= sz; ++i) {
        tmp[i].itemset.push_back(i);
        tmp[i].support = 0;
        tmp[i].dbv.pos = -1;
        tmp[i].dbv.bit_vector.resize(transactions.size()/16 + 1);
    }

    // đọc transaction ghi vào bit_vector
    for (int i = 0; i < transactions.size(); ++i) {
        for (int item : transactions[i].items) {
            tmp[item].dbv.bit_vector[i/16] |= (1 << (i % 16));
            tmp[item].support++;
        }
    }
    // lọc các 1-itemset phổ biến
    for (node &n : tmp) {
        if (n.support >= minsupp) {
            //xóa bit 0 ở cuối
            vector<int> new_bit_vector;
            while(!n.dbv.bit_vector.empty() && n.dbv.bit_vector.back() == 0) {
                n.dbv.bit_vector.pop_back();
            }
            int pos = 0;
            while (pos < n.dbv.bit_vector.size() && n.dbv.bit_vector[pos] == 0) {
                pos ++;
            }
            n.dbv.pos = pos;
            // duyệt lại để bỏ bit 0 ở đầu
            for (int i = n.dbv.pos; i < n.dbv.bit_vector.size(); ++i) {
                new_bit_vector.push_back(n.dbv.bit_vector[i]);
            }
            n.dbv.bit_vector = new_bit_vector;
            node_list.push_back(n);
        }
    }
}

// hàm giao giữa 2 dbv
node intersect(node a, node b, int minsupp) {
    node result;
    result.dbv.pos = -1;
    result.support = 0;
    int pos = max(a.dbv.pos, b.dbv.pos);
    int i = (a.dbv.pos < b.dbv.pos) ? b.dbv.pos - a.dbv.pos : 0;
    int j = (b.dbv.pos < a.dbv.pos) ? a.dbv.pos - b.dbv.pos : 0;
    int cnt = (a.dbv.bit_vector.size() - i < b.dbv.bit_vector.size() - j) ? a.dbv.bit_vector.size() - i : b.dbv.bit_vector.size() - j;

    // nếu cnt * 16 < minsupp thì không phổ biến
    if (cnt * 16 < minsupp) return result;
    // tìm vị trí đầu tiên có giá trị and khác 0
    while (cnt && (a.dbv.bit_vector[i] & b.dbv.bit_vector[j]) == 0) {
        i++;
        j++;
        cnt--;
        pos++;
    }
    // tìm vị trí cuối có giá trị and khác 0
    int il = i + cnt - 1, jl = j + cnt - 1;
    if (il >= a.dbv.bit_vector.size() || jl >= b.dbv.bit_vector.size()) return result;
    while (cnt && (a.dbv.bit_vector[il] & b.dbv.bit_vector[jl]) == 0) {
        il--;
        jl--;
        cnt--;
    }
    // nếu cnt * 16 < minsupp thì không phổ biến
    if (cnt * 16 < minsupp) return result;
    result.dbv.pos = pos;
    // lấy những bit giao
    for (int t = 0; t < cnt; ++t) {
        result.dbv.bit_vector.push_back(a.dbv.bit_vector[i] & b.dbv.bit_vector[j]);
        result.support += count_bit[a.dbv.bit_vector[i] & b.dbv.bit_vector[j]];
        i++;
        j++;
    }
    return result;
}

// thuật toán eclat
void eclat(vector<node> node_list, int minsupp, vector<vector<int>> &FI) {
    for (int i = 0; i < node_list.size(); ++i) {
        FI.push_back(node_list[i].itemset);
        vector<node> new_node_list;
        for (int j = i + 1; j < node_list.size(); ++j) {
            node new_node;
            new_node = intersect(node_list[i], node_list[j], minsupp);
            if (new_node.dbv.pos == -1) continue; // không phải itemset phổ biến => bỏ qua
            if (new_node.support >= minsupp) {
                new_node.itemset = node_list[i].itemset;
                new_node.itemset.push_back(node_list[j].itemset.back());
                new_node_list.push_back(new_node);
            }
        }
        if (new_node_list.size()) {
            eclat(new_node_list, minsupp, FI);
        }
    }
}

int32_t main() {
    ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    freopen("Eclat\\Nhan_Eclat\\chess.txt", "r", stdin);
    vector<transaction> transactions;
    vector<node> node_list;
    vector<vector<int>> FI;
    repare();
    read_data(transactions);
    int minsupp = 0.55 * transactions.size();
    init_node(transactions, node_list, minsupp);

    eclat(node_list, minsupp, FI);
    cout << "So tap: " << FI.size() << el;
    cout << "minsupp: " << minsupp << el;
    cout << "So giao dich: " << transactions.size() << el;

    return 0;
}
