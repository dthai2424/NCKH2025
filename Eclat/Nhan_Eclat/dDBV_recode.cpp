    #include <bits/stdc++.h>
using namespace std;
#define el '\n'

struct transaction {
    vector<int> items;
    transaction(vector<int> items) : items(items) {}
};

struct DBV {
    int pos, diffsize;
    vector<int> bit_vector;
    DBV() : pos(-1), diffsize(0) {}
};

struct node {
    int support;
    vector<int> itemset;
    DBV dbv;
    node() : support(0), dbv() {}
};

int sz;
vector<int> cnt_bit(1 << 16, 0);

// hàm đếm trước bit 1 của các số => tính diffset nhanh
void repare_cnt_bit() {
    for (int mask = 0; mask < (1 << 16); mask++) {
        cnt_bit[mask] = __builtin_popcount(mask);
    }
}

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
        transactions.emplace_back(split(line));
    }
}

// hàm khởi tạo cái tập phổ biến có 1 phần tử
void init_node(vector<transaction> &transactions, vector<node> &node_list, int minsupp) {
    vector<node> tmp(sz + 1);
    vector<int> pos(sz + 1, 0);
    for (int i = 0; i <= sz; ++i) {
        tmp[i].itemset.push_back(i);
        tmp[i].dbv.bit_vector.resize(transactions.size()/16 + 1);
    }
    for (int i = 0; i < transactions.size(); i++) {
        for (int item : transactions[i].items) {
            tmp[item].support++;
            if (i > pos[item]) { // nếu i > pos[item] có nghĩa là transaction từ pos[item] đến i-1 không có item => đưa vào diffset
                for (int j = pos[item]; j < i; ++j) {
                    tmp[item].dbv.diffsize++; // tăng độ lớn của diffset
                    tmp[item].dbv.bit_vector[j/16] |= (1 << (j % 16)); // thêm vào diffset
                }
            }
            pos[item] = i + 1;
        }
    }
    for (int item = 0; item <= sz; ++item) {
        if (pos[item] < transactions.size()) {
            for (int i = pos[item]; i < transactions.size(); ++i) {
                tmp[item].dbv.diffsize++;
                tmp[item].dbv.bit_vector[i/16] |= (1 << (i % 16));
            }
        }
    }

    // lọc ra các tập hợp phổ biến, đồng thời bỏ các bit 0 ở đầu và cuối các bit_vector
    for (node &n : tmp) {
        if (n.support >= minsupp) {
            vector<int> new_bit_vector;
            int st = 0;
            int ed = (int)n.dbv.bit_vector.size() - 1;
            while(st < n.dbv.bit_vector.size() && n.dbv.bit_vector[st] == 0) {
                st ++;
            }
            while (ed >= st && n.dbv.bit_vector[ed] == 0) {
                ed --;
            }
            if (ed - st + 1 > 0) {
                n.dbv.pos = st;
                for (int i = st; i <= ed; ++i) {
                    new_bit_vector.push_back(n.dbv.bit_vector[i]);
                }
            } else {
                n.dbv.pos = -1;
            }
            n.dbv.bit_vector = new_bit_vector;
            node_list.emplace_back(n);
        }
    }
}

// hàm tìm hiệu của 2 diffset
DBV subtract(DBV a, DBV b) {
    if (a.pos == -1 || b.pos == -1) { // pos = -1 => tập rổng, a - b (nếu a rổng) = rổng, a - b (nếu b rổng) = a
        return a;
    }
    DBV result;
    int pos = a.pos;
    int i = (a.pos < b.pos) ? (b.pos - a.pos) : 0; // chỉ số bắt đầu trong dbv a
    int j = (b.pos < a.pos) ? (a.pos - b.pos) : 0; // chỉ số bắt đầu trong dbv b
    if ((int)a.bit_vector.size() - i <= 0 || (int)b.bit_vector.size() - j <= 0) { // nếu 2 dbv không giao nhau thì a - b = a
        return a;
    }

    // trừ các bit mà 2 dbv giao nhau
    // ta có: a - b = (a ^ b) & a
    vector<int> sub_bit_vector;
    // nếu i > 0 => phần tử 0 cho đến i - 1 a không giao với b nên a - b = a
    for (int t = 0; t < i; ++t) {
        sub_bit_vector.push_back(a.bit_vector[t]);
    }
    // tính các phần giao nhau theo công thức
    while (i < a.bit_vector.size() && j < b.bit_vector.size()) {
        sub_bit_vector.push_back((a.bit_vector[i] ^ b.bit_vector[j]) & a.bit_vector[i]);
        i++;
        j++;
    }
    // nếu j đã chạy hết b mà i chưa chạy hết a => từ i + 1 đến cuối a không giao b => a - b = a
    while (i < a.bit_vector.size()) {
        sub_bit_vector.push_back(a.bit_vector[i]);
        i++;
    }

    // bỏ các bit 0 ở đầu và ở cuối
    int st = 0, ed = (int)sub_bit_vector.size() - 1;
    while (st < sub_bit_vector.size() && sub_bit_vector[st] == 0) {
        st ++;
        pos ++;
    }
    while (ed > st && sub_bit_vector[ed] == 0) {
        ed --;
    }
    if (ed - st + 1 > 0) {
        result.pos = pos;
        for (int t = st; t <= ed; ++t) {
            result.bit_vector.push_back(sub_bit_vector[t]);
            result.diffsize += cnt_bit[sub_bit_vector[t]];
        }
    }
    return result;
}


// thuật eclat
void eclat(vector<node> node_list, int minsupp, vector<vector<int>> &FI) {
    for (int i = 0; i < node_list.size(); ++i) {
        FI.push_back(node_list[i].itemset);
        vector<node> new_node_list;
        for (int j = i + 1; j < node_list.size(); ++j) {
            node new_node;
            new_node.dbv = subtract(node_list[j].dbv, node_list[i].dbv);
            if (node_list[i].support - new_node.dbv.diffsize >= minsupp) {
                new_node.support = node_list[i].support - new_node.dbv.diffsize;
                new_node.itemset = node_list[i].itemset;
                new_node.itemset.push_back(node_list[j].itemset.back());
                new_node_list.push_back(new_node);
            }
        }
        if (!new_node_list.empty()) {
            eclat(new_node_list, minsupp, FI);
        }
    }
}

int32_t main() {
    ios_base:: sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    freopen("Eclat\\Nhan_Eclat\\mushroom.txt", "r", stdin);
    repare_cnt_bit();
    vector<transaction> transactions;
    vector<node> node_list;
    vector<vector<int>> FI;
    read_data(transactions);
    int minsupp = 0.08 * transactions.size();
    init_node(transactions, node_list, minsupp);
    eclat(node_list, minsupp, FI);
    cout << "So tap: " << FI.size() << el;
    cout << "minsupp: " << minsupp << el;
    cout << "So giao dich: " << transactions.size() << el;

    return 0;
}
