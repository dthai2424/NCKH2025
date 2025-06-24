#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;
#define el '\n'
int sz = 0;
struct transaction {
    vector<int> itemset;
    transaction(vector<int> _itemset) {
        itemset = _itemset;
    }
};

struct node {
    int item;
    unordered_map<int, node*> child;
    int pre_order, post_order, cnt;
    node() : pre_order(-1), post_order(-1), cnt(0) {}
    node(int item) :item(item), pre_order(-1), post_order(-1), cnt(0) {}
};

struct PPC_Tree{
    node *root;
    int post = 0, pre = 0;
    PPC_Tree(vector<transaction> transactions, int minsupp) {
        root = new node(-1);
        PPC_construction(transactions, minsupp);
    }

    // them itemset vao cay
    void insert(vector<int> itemset) {
        node* p = root;
        for (int item : itemset) {
            if (p->child.find(item) == p->child.end()) {
                p->child[item] = new node(item);
            }
            p = p->child[item];
            p->cnt++;
        }
    }

    //sắp xếp theo support
    void sort_by_support(vector<int> &v, vector<int> support) {
        sort(v.begin(), v.end(), [&](int a, int b) {
            if (support[a] != support[b])
                return support[a] > support[b];
            return a < b;
        });
    }

    //tạo ordered_frequent_item
    void init_ordered_frequent_item(vector<transaction> &transactions, int minsupp) {
        vector<int> support(sz + 1);
        for (transaction t : transactions) {
            for (int item : t.itemset) {
                support[item] ++;
            }
        }
        for (transaction &t : transactions) {
            vector<int> new_itemset;
            for (int item : t.itemset) {
                if (support[item] < minsupp) continue;
                new_itemset.push_back(item);
            }
            sort_by_support(new_itemset, support);
            t.itemset = new_itemset;
        }
    }

    // hàm tạo pre-order và post-order
    void pre_post_generation(node* p) {
        p->pre_order = pre++;
        for (pair<int, node*> ch : p->child) {
            pre_post_generation(ch.second);
        }
        p->post_order = post++;
    }

    //xay dung cay PPC
    void PPC_construction(vector<transaction> &transactions, int minsupp) {
        init_ordered_frequent_item(transactions, minsupp);
        for (transaction t : transactions) {
            insert(t.itemset);
        }
        pre_post_generation(root);
    }

    // hàm in cây PPC
    void print(node* p) {
        for (auto ch : p->child) {
            cout << p->item << " <" << p->pre_order << ", " << p->post_order << "> " << p->cnt;
            cout << " => " << ch.second->item << " <" << ch.second->pre_order << ", " << ch.second->post_order << "> " << ch.second->cnt << el;
            print(ch.second);
        }
    }
};

struct PP_code {
    int pre_order, post_order, cnt;
    PP_code(int pre_order, int post_order, int cnt) : pre_order(pre_order), post_order(post_order), cnt(cnt) {}
};

struct Nlist {
    vector<int> itemset;
    vector<PP_code> Nl;
    int cnt;
    Nlist() :cnt(0) {}
};

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

// hàm khởi tao N-list;
void Nlist_contruction(node* p, vector<Nlist> &Nls) {
    for (pair<int, node*> ch : p->child) {
        Nls[ch.first].Nl.push_back(PP_code(ch.second->pre_order, ch.second->post_order, ch.second->cnt));
        Nls[ch.first].cnt += ch.second->cnt;
        Nlist_contruction(ch.second, Nls);
    }
}

// hàm tìm tập giao giữa hai tập
Nlist intersect_Nlist(Nlist a, Nlist b, int minsupp) {
    Nlist result;
    int i = 0, j = 0;
    while(i < a.Nl.size() && j < b.Nl.size()) {
        if (a.Nl[i].pre_order < b.Nl[j].pre_order) {
            if (a.Nl[i].post_order > b.Nl[j].post_order) {
                if (result.Nl.size() && result.Nl[result.Nl.size() - 1].pre_order == a.Nl[i].pre_order) {
                    result.Nl[result.Nl.size() - 1].cnt += b.Nl[j].cnt;
                } else {
                    result.Nl.push_back(PP_code(a.Nl[i].pre_order, a.Nl[i].post_order, b.Nl[j].cnt));
                }
                result.cnt += b.Nl[j].cnt;
                j++;
            } else {
                i++;
            }
        } else {
            j++;
        }
    }
    if (result.cnt >= minsupp) {
        result.itemset = b.itemset;
        result.itemset.push_back(a.itemset.back());
    }
    return result;
}

// thuật toán eclat
void eclat(vector<Nlist> node_list, int minsupp, vector<Nlist> &FL) {
    for (Nlist node : node_list) {
        FL.push_back(node);
    }
    for (int i = 0; i < node_list.size() - 1; ++i) {
        vector<Nlist> new_node_list;
        for (int j = i + 1; j < node_list.size(); j++) {
            Nlist new_node;
            new_node = intersect_Nlist(node_list[j], node_list[i], minsupp);
            if (new_node.cnt >= minsupp) {
                new_node_list.push_back(new_node);
            }
        }
        if (new_node_list.size())
            eclat(new_node_list, minsupp, FL);
    }
}

// hàm in các tập phổ bien
void printFL(vector<Nlist> FL) {
    for (Nlist n : FL) {
        cout << '{';
        for (int item : n.itemset) {
            cout << item << ' ';
        }
        cout << '}' << ':' << n.cnt;
        cout << el;
    }
}

int32_t main() {
    ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    freopen("Eclat\\Nhan_Eclat\\mushroom.txt", "r", stdin);
//    freopen("Eclat\\Nhan_Eclat\\output.txt", "w", stdout);
    vector<transaction> transactions;
    vector<Nlist> FL;
    read_data(transactions);
    int minsupp = 0.08 * transactions.size();

    // tạo PPC_Tree
    PPC_Tree t = PPC_Tree(transactions, minsupp);

    //tao Nlist
    vector<Nlist> tmp(sz + 1);
    for (int i = 0; i <= sz; ++i) {
        tmp[i].itemset.push_back(i);
    }
    Nlist_contruction(t.root, tmp);
    vector<Nlist> node_list;
    for (Nlist n : tmp) {
        if (n.cnt >= minsupp) {
            node_list.push_back(n);
        }
    }
    sort(node_list.begin(), node_list.end(), [](Nlist a, Nlist b) {
        if (a.cnt != b.cnt)
            return a.cnt < b.cnt;
        return a.itemset[0] > b.itemset[0];
    });

    // Thuật Eclat
    auto start = high_resolution_clock::now();
    eclat(node_list, minsupp, FL);
    auto stop = high_resolution_clock::now(); // kết thúc đo
    auto duration = duration_cast<milliseconds>(stop - start); // đo bằng mili-giây

    cout << "Thoi gian chay: " << duration.count() << " ms" << el;
    cout << "So tap: " << FL.size() << el;
    cout << "minsupp: " << minsupp << el;
    cout << "So giao dich: " << transactions.size() << el;

    return 0;
}