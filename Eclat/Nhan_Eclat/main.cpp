#include <bits/stdc++.h>
using namespace std;
struct transaction {
  int id;
  vector<string> items;
  transaction(int _id, vector<string> _items) {
    id = _id;
    items = _items;
  }
};
map<set<string>, unordered_set<int>> tidsets; // tidset của các item
vector<transaction> transactions; // tập các transaction
map<set<string>, unordered_set<int>> L; // tập các itemsets phổ biến
double minsupp = 2;

vector<string> split(string str) {
  vector<string> result;
  stringstream ss(str);
  string item;
  while (ss >> item) {
    result.push_back(item);
  }
  return result;
}

void read_data() {
  string line;
  int id = 1;
  while (getline(cin, line)) {
    transactions.push_back(transaction(id, split(line)));
    id ++;
  }
}

void init_tidsets() {
  for (transaction t : transactions) {
    for (string item : t.items) {
      set<string> s;
      s.insert(item);
      tidsets[s].insert(t.id);
    }
  }
  for (auto it = tidsets.begin(); it != tidsets.end(); ) {
    if (it->second.size() < minsupp)
      it = tidsets.erase(it);
    else
      ++it;
  }
}

unordered_set<int> intersect(unordered_set<int> a, unordered_set<int> b) {
  unordered_set<int> result;
  unordered_set<int> small = (a.size() <= b.size()) ? a : b;
  unordered_set<int> large = (a.size() > b.size()) ? a : b;
  for (int tid : small) {
    if (large.count(tid)) {
      result.insert(tid);
    }
  }
  return result;
}

void eclat(set<string> prefix, map<set<string>, unordered_set<int>> tidsets) {
  vector<pair<set<string>, unordered_set<int>>> items(tidsets.begin(), tidsets.end());
  for (int i = 0; i < items.size(); ++i) {
    set<string> new_prefix = prefix;
    new_prefix.insert(items[i].first.begin(), items[i].first.end());
    map<set<string>, unordered_set<int>> new_tidsets;
    for (int j = i + 1; j < items.size(); ++j) {
      set<string> new_itemset = items[i].first;
      new_itemset.insert(items[j].first.begin(), items[j].first.end());
      unordered_set<int> new_tidset = intersect(items[i].second, items[j].second);
      if (new_tidset.size() >= minsupp) {
        new_tidsets[new_itemset] = new_tidset;
        L[new_itemset] = new_tidset;
      }
    }
    if (new_tidsets.size()) {
      eclat(new_prefix, new_tidsets);
    }
  }
}

void print_result() {
  for (auto itemset : L) {
    for (auto item : itemset.first) {
      cout << item << ' ';
    }
    cout << ':' << itemset.second.size() << '\n';
  }
}


int32_t main() {
  ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
  freopen("Eclat\\Nhan_Eclat\\input.txt", "r", stdin);
  freopen("Eclat\\Nhan_Eclat\\output.txt", "w", stdout);
  read_data();
  init_tidsets();
  eclat({}, tidsets);
  print_result();

  return 0;
}