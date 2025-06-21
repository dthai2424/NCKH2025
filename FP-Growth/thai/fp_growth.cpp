#include <bits/stdc++.h>
//           /＞　　 フ
// 　　　　　| 　_　_ 𝕝
// 　 　　　／`ミ＿𝕩 ノ
// 　　 　 /　　　 　|
// 　　　 /　 ヽ　　 ﾉ
// 　 　 │　　|　|　|
// 　／￣|　　|　|　|
// 　| (￣ヽ＿_ヽ_ )__)
// 　＼二つ
#define thai
#define ll long long
#define fi first
#define se second
#define all(a) (a).begin(), (a).end()
#define pb(x) push_back(x)
#define f0(i, n) for (ll i = 0; i < n; i++)
#define f1(i, n) for (ll i = 1; i <= n; i++)
#define fa(i, v) for (auto &i : v)
#define IOS() ios::sync_with_stdio(false), cin.tie(0), cout.tie(0)
#define el cout << "\n"
using namespace std;
struct Data {
  double min_sup, min_conf;
  map<string, set<int>> data;
  Data() {}
  Data(map<string, set<int>> data) { this->data = data; }
  void ReadInput() {
    cin >> this->min_sup >> this->min_conf;
    string dummy;
    getline(cin, dummy);
    string line;
    while (getline(cin, line)) {
      line.push_back(',');
      int len = line.length();
      int ptr = 0;
      int curNumber = 0;
      string key = "";
      set<int> list;
      bool is_key = true;
      while (ptr < len) {
        if (line[ptr] == ' ') {
          is_key = false;
          ptr++;
          continue;
        }
        if (is_key) {
          key.push_back(line[ptr]);
        } else {
          if (line[ptr] == ',') {
            list.insert(curNumber);
            curNumber = 0;
          } else {
            curNumber = curNumber * 10 + line[ptr] - '0';
          }
        }
        ptr++;
      }
      this->data.insert({key, list});
    }
  }
};

struct FP_Growth {
  struct Node {
    int item;
    int count;
    map<int, Node *> child;
    Node *parent;
    Node() : item(-1), count(0), parent(nullptr) {}
    Node(int item, int count) : item(item), count(count), parent(nullptr) {}
  };
  struct Tree {
    map<int, vector<Node *>> header_table;
    map<int, vector<vector<Node *>>> conditional_pattern_base;
    map<int, Tree *> conditional_tree;
    map<int, int> support_map;
    Node *root;
    Tree() { root = new Node(-1, 0); }
  };

  Tree *fp_tree;
  map<int, int> support_map;
  map<string, vector<int>> dataset;
  vector<vector<int>> frequent_set;
  double min_supp, min_conf;
  int min_frequency;

  FP_Growth() { fp_tree = new Tree(); }
  FP_Growth(map<string, set<int>> &raw, double min_supp, double min_conf) {
    this->min_supp = min_supp;
    this->min_conf = min_conf;
    fp_tree = new Tree();
    this->min_frequency = ceil(min_supp * raw.size());
    process_dataset(raw, min_frequency);
  }

  void compute_support_map(map<string, set<int>> &raw) {
    for (auto &data : raw)
      for (int num : data.second)
        support_map[num]++;
  }

  void sort_by_support(vector<int> &v, map<int, int> &sup_map) {
    sort(v.begin(), v.end(), [&](int a, int b) {
      if (sup_map[a] != sup_map[b])
        return sup_map[a] > sup_map[b];
      return a < b;
    });
  }

  map<string, vector<int>> filter_dataset(map<string, set<int>> &raw,
                                          int min_freq) {
    map<string, vector<int>> res;
    for (auto &data : raw) {
      vector<int> vec;
      for (int num : data.second)
        if (support_map[num] >= min_freq)
          vec.push_back(num);
      res[data.first] = move(vec);
    }
    return res;
  }

  void process_dataset(map<string, set<int>> &raw, int min_freq) {
    compute_support_map(raw);
    dataset = filter_dataset(raw, min_freq);
    for (auto &kv : dataset)
      sort_by_support(kv.second, support_map);
  }
  void insert_item(vector<int> &list, Tree *tree, map<int, int> &local_sup) {
    Node *p = tree->root;
    for (int num : list) {
      if (!p->child.count(num)) {
        p->child[num] = new Node(num, 0);
        p->child[num]->parent = p;
      }
      tree->header_table[num].push_back(p->child[num]);
      p = p->child[num];
      int inc = local_sup.empty() ? 1 : local_sup[num];
      p->count += inc;
      tree->support_map[num] += inc;
    }
  }

  vector<Node *> &backtrack(Node *cur, vector<Node *> &cpb) {
    if (!cur || !cur->parent)
      return cpb;
    cpb.push_back(cur);
    return backtrack(cur->parent, cpb);
  }

  map<int, int> count_support_prefix(vector<vector<Node *>> &paths) {
    map<int, int> res;
    for (auto &path : paths)
      for (auto *n : path)
        res[n->item] += n->count;
    return res;
  }

  vector<vector<Node *>> filter_prefix_path(Tree *tree,
                                            vector<vector<Node *>> &paths) {
    vector<vector<Node *>> res;
    for (auto &path : paths) {
      vector<Node *> kept;
      for (auto *n : path)
        if (tree->support_map[n->item] >= min_frequency)
          kept.push_back(n);
      if (!kept.empty())
        res.push_back(move(kept));
    }
    return res;
  }

  void sort_conditional_pattern_base(Tree *tree,
                                     vector<vector<Node *>> &paths) {
    for (auto &path : paths)
      sort(path.begin(), path.end(), [&](Node *a, Node *b) {
        if (tree->support_map[a->item] != tree->support_map[b->item])
          return tree->support_map[a->item] > tree->support_map[b->item];
        return a->item < b->item;
      });
  }

  void build_fp_tree() {
    map<int, int> dummy;
    for (auto &kv : dataset)
      insert_item(kv.second, fp_tree, dummy);
  }

  void build_conditional_pattern_base(Tree *tree, map<int, int> support_map) {
    for (auto &kv : tree->header_table) {
      int item = kv.first;
      for (auto *node : kv.second) {
        vector<Node *> path;
        backtrack(node->parent, path);
        if (!path.empty())
          tree->conditional_pattern_base[item].push_back(path);
      }
      auto &cpb = tree->conditional_pattern_base[item];
      sort_conditional_pattern_base(tree, cpb);
    }
  }

  void build_conditional_tree(Tree *tree) {
    for (auto &kv : tree->conditional_pattern_base) {
      int item = kv.first;
      auto paths = kv.second;
      auto sup_count = count_support_prefix(paths);
      auto filtered = filter_prefix_path(tree, paths);
      Tree *ct = new Tree();
      // convert Node* paths to item lists and insert
      for (auto &path : filtered) {
        vector<int> items;
        for (auto *n : path)
          items.push_back(n->item);
        sort_by_support(items, sup_count);
        insert_item(items, ct, sup_count);
      }
      tree->conditional_tree[item] = ct;
    }
  }

  void mine_tree(Tree *tree, vector<int> &prefix_path) {
    bool single_path = true;
    if (tree->root->child.size() > 1) {
      single_path = false;
    }
    if (single_path) {
      vector<int> path(prefix_path);
      Node *p = tree->root;
      while (p->child.size()) {
        p = p->child.begin()->second;

        path.push_back(p->item);
      }
      for (int mask = 1; mask < (1 << path.size()); mask++) {
        vector<int> combination;
        for (int i = 0; i < path.size(); i++) {
          if (mask & (1 << i))
            combination.push_back(chain[i]);
        }
        frequent_set.push_back(combination);
      }
      return;
    }
    for (auto &item : tree->header_table) {
      if (tree->support_map[item.first] < min_frequency)
        continue;
      vector<int> new_prefix = prefix;
      Tree *child_tree = new Tree();
      map<int, int> tree - child_tree =
          build_conditional_pattern_base(child_tree, child);
      new_prefix.push_back(item.first);
    }
  }
  void run() {
    build_fp_tree();
    build_conditional_pattern_base(fp_tree, this->support_map);
    build_conditional_tree(fp_tree);
    // mine_tree();
    // return result;
  }
};

int main() {
  IOS();
#ifdef thai
  freopen("test.inp", "r", stdin);
  freopen("test.out", "w", stdout);
#endif
Data dt;
dt.ReadInput();
FP_Growth tree(dt.data,dt.min_sup,dt.min_conf);
tree.run();
}