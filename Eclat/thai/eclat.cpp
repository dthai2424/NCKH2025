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
#define all(a) (a).begin(), (a).end()
#define IOS() ios::sync_with_stdio(false), cin.tie(0), cout.tie(0)
#define el cout << "\n"
using namespace std;
struct Transaction {
  vector<int> data;
  Transaction() = default;
  Transaction(vector<int> dt) : data(dt){};
};
int sz = 0;
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
void read_data(vector<Transaction> &transactions) {
  string line;
  while (getline(cin, line)) {
    transactions.push_back(Transaction(split(line)));
  }
}
struct Node {
  vector<int> item;
  vector<int> tidset;
  Node() = default;
};
struct Frequent {
  vector<int> itemset;
  Frequent() = default;
  Frequent(vector<int> it) : itemset(it) {}
};
struct Eclat {
  vector<Frequent> frequent_itemset;
  vector<Transaction> dataset;
  vector<Node> node_list;
  double min_sup, min_conf;
  int min_freq;
  Eclat(vector<Transaction> &data, double min_s, double min_c)
      : dataset(data), min_sup(min_s), min_conf(min_c) {}
  Eclat() = default;
  void init_tidset() {
    min_freq = min_sup * dataset.size();
    vector<Node> dummy(sz + 5);
    for (int i = 0; i <= sz; i++) {
      dummy[i].item.push_back(i);
    }
    for (int i = 0; i < dataset.size(); i++) {
      for (auto num : dataset[i].data) {
        dummy[num].tidset.push_back(i);
      }
    }

    for (auto &node : dummy) {
      if (node.tidset.size() >= min_freq) {

        node_list.push_back(node);
      }
    }
  }
  vector<int> tidset_intersection(vector<int> &a, vector<int> &b) {
    int i = 0, j = 0;
    vector<int> res;
    while (i < a.size() && j < b.size()) {
      if (a[i] == b[j]) {
        res.push_back(a[i]);
        i++;
        j++;
      } else {
        if (a[i] > b[j]) {
          j++;
        } else {
          i++;
        }
      }
    }
    return res;
  }
  void eclat(vector<Node> &node_list) {
    for (int i = 0; i < node_list.size(); i++) {
      frequent_itemset.push_back(node_list[i].item);
      vector<Node> new_node_list;
      for (int j = i + 1; j < node_list.size(); j++) {
        Node new_node;
        vector<int> intersect_tidset =
            tidset_intersection(node_list[i].tidset, node_list[j].tidset);

        if (intersect_tidset.size() >= min_freq) {
          new_node.tidset = intersect_tidset;
          new_node.item = node_list[i].item;
          new_node.item.push_back(node_list[j].item.back());
          new_node_list.push_back(new_node);
        }
      }
      if (new_node_list.size()) {
        eclat(new_node_list);
      }
    }
  }
  void run() {
    // print_data();
    init_tidset();
    eclat(node_list);
    print_frequent();
  }
  void print_data() {
    el;
    cout << "Data: ", el;
    for (auto trans : dataset) {
      for (auto num : trans.data) {
        cout << num << " ";
      }
      el;
    }
  }
  void print_frequent() {
    cout << "\nFrequent itemsets:\n";
    for (auto &f : frequent_itemset) {
      cout << "{ ";
      for (int x : f.itemset)
        cout << x << ' ';
      cout << "}";
      // el;
    }
  }
};
int main() {
  IOS();
#ifdef thai
  freopen("input.txt", "r", stdin);
  freopen("output.txt", "w", stdout);
#endif
  vector<Transaction> dataset;
  read_data(dataset);
  Eclat eclat(dataset,0.5,0.5);
  eclat.run();
}