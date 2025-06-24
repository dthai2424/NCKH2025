#include <bits/stdc++.h>
using namespace std;
#define int long long
#define el '\n'
int sz = 0;
struct transaction{
    vector<int> itemset;
    transaction(vector<int> itemset): itemset(itemset) {}
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
        for (int i : t.itemset) {
            cout << i << ' ';
        }
        cout << el;
    }
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
    vector<int> items;
    vector<PP_code> Nl;
    int cnt;
};

// hàm khởi tao N-list;
void Nlist_contruction(node* p, vector<Nlist> &Nls) {
    for (pair<int, node*> ch : p->child) {
        Nls[ch.first].Nl.push_back(PP_code(ch.second->pre_order, ch.second->post_order, ch.second->cnt));
        Nls[ch.first].cnt += ch.second->cnt;
        Nlist_contruction(ch.second, Nls);
    }
}

// hàm tìm giao giữa 2 Nlist
Nlist intersect_Nlist(Nlist a, Nlist b) {
    Nlist result;
    int i = 0, j = 0;
    while(i < a.Nl.size() && j < b.Nl.size()) {
        if (a.Nl[i].pre_order < b.Nl[j].pre_order) {
            if (a.Nl[i].post_order > b.Nl[j].post_order) {
                if (result.Nl.size() && result.Nl[result.Nl.size() - 1].pre_order == a.Nl[i].pre_order) {
                    result.Nl[result.Nl.size() - 1].cnt += b.Nl[i].cnt;
                } else {
                    result.Nl.push_back(PP_code(a.Nl[i].pre_order, a.Nl[i].post_order, b.Nl[i].cnt));
                }
                result.cnt += b.Nl[i].cnt;
                j++;
            } else {
                i++;
            }
        } else {
            j++;
        }
    }
    result.items = a.items;
    result.items.push_back(b.items.back());
    return result;
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
        transactions.push_back(transaction(split(line)));
    }
}

int32_t main() {
    ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    freopen("input.txt", "r", stdin);
//    freopen("Eclat\\Nhan_Eclat\\output.txt", "w", stdout);
    vector<transaction> transactions;
    read_data(transactions);
    vector<Nlist> Nls(sz + 1);
    cout << transactions.size() << el;
    PPC_Tree t = PPC_Tree(transactions, 2);
    t.print(t.root);
    for (int i = 0; i <= sz; ++i) {
        Nls[i].items.push_back(i);
    }
    Nlist_contruction(t.root, Nls);
    for (Nlist n : Nls) {
        for (int item : n.items) {
            cout << item << ' ';
        }
        cout << ':';
        for (PP_code ppc : n.Nl) {
            cout << '<' << '(' << ppc.pre_order << ',' << ppc.post_order << ')' << ':' << ppc.cnt << '>' << ' ';
        }
        cout << el;
    }
    Nlist c = intersect_Nlist(Nls[3], Nls[6]);
    for (int item : c.items) {
        cout << item << ' ';
    }
    cout << ':';
    for (PP_code ppc : c.Nl) {
        cout << '<' << '(' << ppc.pre_order << ',' << ppc.post_order << ')' << ':' << ppc.cnt << '>' << ' ';
    }
    cout << el;

    return 0;
}
