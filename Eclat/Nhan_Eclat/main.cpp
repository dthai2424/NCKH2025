#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;
#define int long long
#define el '\n'
struct transaction {
    vector<int> items;
    transaction(vector<int> _items) {
        items = _items;
    }
};

struct node {
    vector<int> items;
    vector<int> tidset;
};

vector<transaction> transactions;
vector<node> nodes;
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
void read_data() {
    string line;
    while (getline(cin, line)) {
        transactions.push_back(transaction(split(line)));
    }
}

//hàm tạo các tidset, chỉ giữ lại những tidset thỏa điều kiện minsupp
void init(int minsupp) {
    vector<node> node_list(sz + 1);
    for (int i = 0; i <= sz; i++) {
        node_list[i].items.push_back(i);
    }
    for (int i = 0; i < transactions.size(); i++) {
        for (int item : transactions[i].items) {
            node_list[item].tidset.push_back(i);
        }
    }
    for (node n : node_list) {
        if (n.tidset.size() < minsupp) continue;
        nodes.push_back(n);
    }
}

// hàm tìm tập giao giữa hai tập
vector<int> intersect(vector<int> tidseta, vector<int> tidsetb) {
    vector<int> result;
    int i = 0, j = 0;
    while (i < tidseta.size() && j < tidsetb.size()) {
        if (tidseta[i] == tidsetb[j]) {
            result.push_back(tidseta[i]);
            i++;
            j++;
        } else {
            if (tidseta[i] > tidsetb[j]) {
                j++;
            } else {
                i++;
            }
        }
    }
    return result;
}

// thuật toán eclat
void eclat(vector<node> node_list, int minsupp, vector<vector<int>> &FL) {
    for (int i = 0; i < node_list.size(); ++i) {
        FL.push_back(node_list[i].items);
        vector<node> new_node_list;
        for (int j = i + 1; j < node_list.size(); j++) {
            node new_node;
            new_node.tidset = intersect(node_list[i].tidset, node_list[j].tidset);

            if (new_node.tidset.size() >= minsupp) {
                new_node.items = node_list[i].items;
                new_node.items.push_back(node_list[j].items.back());
                new_node_list.push_back(new_node);
            }
        }
        if (new_node_list.size())
            eclat(new_node_list, minsupp, FL);
    }
}

// hàm in các tập phổ bien
void printFL(vector<vector<int>> FL) {
    for (auto n : FL) {
        cout << '{';
        for (int item : n) {
            cout << item << ' ';
        }
        cout << '}' << ':';
        cout << el;
    }
}

int32_t main() {
    ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    freopen("Eclat\\Nhan_Eclat\\input.txt", "r", stdin);
//    freopen("Eclat\\Nhan_Eclat\\output.txt", "w", stdout);
    read_data();
    int minsupp = 0.4 * transactions.size();
    init(minsupp);
    vector<vector<int>> FL;
    auto start = high_resolution_clock::now();
    eclat(nodes, minsupp, FL);
    auto stop = high_resolution_clock::now(); // kết thúc đo
    auto duration = duration_cast<milliseconds>(stop - start); // đo bằng mili-giây

    cout << "Thoi gian chay: " << duration.count() << " ms" << el;
    cout << "So tap: " << FL.size() << el;
    cout << "minsupp: " << minsupp << el;
    cout << "So giao dich: " << transactions.size() << el;

    return 0;
}
