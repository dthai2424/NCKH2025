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
struct Data_Reader {
  double min_sup, min_conf;
  vector<Transaction> dataset;
  Data_Reader() {}
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
      Transaction tr;
      bool is_key = true;
      while (ptr < len) {
        if (line[ptr] == ' ') {
          is_key = false;
          ptr++;
          continue;
        }
        if (is_key) {

        } else {
          if (line[ptr] == ',') {
            tr.data.push_back(curNumber);
            curNumber = 0;
          } else {
            curNumber = curNumber * 10 + line[ptr] - '0';
          }
        }
        ptr++;
      }
      dataset.push_back(tr);
    }
  }
};
struct Frequent {
  vector<int> itemset;
  Frequent() = default;
  Frequent(vector<int> it) : itemset(it) {}
};

struct DBV {
  vector<int> item;
  int pos;
  vector<uint8_t> bit;
  DBV() = default;
  DBV(int p, vector<uint8_t> b) : pos(p), bit(b) {}
};
int sz = 0;
struct DBV_Eclat {
  double min_sup, min_conf;
  int min_freq;
  vector<Frequent> frequent_itemset;
  vector<DBV> tidset;
  vector<Transaction> dataset;
  DBV_Eclat(vector<Transaction> &data, double min_s, double min_c)
      : dataset(data), min_sup(min_s), min_conf(min_c) {}
  void init_dbv() {
    int n = dataset.size();
    int byte_len = (n + 7) / 8;
    this->min_freq = this->min_sup * dataset.size();
    vector<DBV> dummy(sz + 1, DBV());
    for (int i = 0; i <= sz; i++) {
      dummy[i].item.push_back(i);
      dummy[i].bit.resize(byte_len, 0);
      dummy[i].pos = 0;
    }
    for (int tid = 0; tid < dataset.size(); tid++) {
      for (auto num : dataset[tid].data) {
        dummy[num].bit[tid / 8] |= (1 << (7 - (tid % 8)));
      }
    }
    for (int i = 0; i <= sz; i++) {
      int start = 0, end = dummy[i].bit.size() - 1;
      while (start <= end && dummy[i].bit[start] == 0)
        start++;
      while (end >= 0 && dummy[i].bit[end] == 0)
        end--;
      if (start > end) {
        dummy[i].pos = 0;
        dummy[i].bit.clear();

      } else {
        dummy[i].pos = 0;
        dummy[i].bit = vector<uint8_t>(dummy[i].bit.begin() + start,
                                       dummy[i].bit.begin() + end + 1);
      }
      int sup = 0;
      for (auto num : dummy[i].bit) {
        sup += __builtin_popcount(num);
      }
      if (sup >= min_freq) {
        tidset.push_back(dummy[i]);
      }
    }
  }
  DBV dbv_intersection(DBV &a, DBV &b) {
    DBV res;
    int pos = max(a.pos, b.pos);
    int i = a.pos < b.pos ? b.pos - a.pos : 0;
    int j = a.pos < b.pos ? 0 : a.pos - b.pos;
    int count = a.bit.size() - i < b.bit.size() - j ? a.bit.size() - i
                                                    : b.bit.size() - j;
    // cout << endl;
    while (count > 0 && (a.bit[i] & b.bit[j]) == 0) {
      i++;
      j++;
      pos++;
      count--;
    }
    int i1 = i + count - 1, j1 = j + count - 1;
    while (count > 0 && ((a.bit[i1] & b.bit[j1]) == 0)) {
      i1--;
      j1--;
      count--;
    }
    if (count <= 0) {
      res.pos = 0;
      res.bit.clear();
      return res;
    }
    res.pos = pos;
    res.bit.resize(count);
    for (int k = 0; k < count; k++) {
      res.bit[k] = a.bit[i + k] & b.bit[j + k];
    }
    res.item = a.item;
    res.item.push_back(b.item.front());
    return res;
  }
  void eclat(vector<DBV> &tidset) {
    for (int i = 0; i < tidset.size(); i++) {
      frequent_itemset.push_back(tidset[i].item);
      // print_frequent();
      vector<DBV> new_tidset;
      for (int j = i + 1; j < tidset.size(); j++) {
        DBV intersect_tid = dbv_intersection(tidset[i], tidset[j]);
        int sup = 0;
        for (auto num : intersect_tid.bit) {
          sup += __builtin_popcount(num);
        }
        if (sup >= min_freq) {
          new_tidset.push_back(intersect_tid);
        }
      }
      if (new_tidset.size()) {
        eclat(new_tidset);
      }
    }
  }
  void print_dbv() {
    for (auto i : tidset) {
      cout << "{ ";
      for (auto num : i.item) {
        cout << num << " ";
      }
      cout << "}";
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
  void run() {
    init_dbv();
    // print_dbv();
    // print_data();
    eclat(tidset);
    // frequent_itemset.size();
    // print_frequent();
    cout << "Số tập: " << frequent_itemset.size();
  }
};

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

int main() {
  IOS();
#ifdef thai
  freopen("input.txt", "r", stdin);
  freopen("output.txt", "w", stdout);
#endif
  Data_Reader data_reader;
  read_data(data_reader.dataset);
  DBV_Eclat eclat(data_reader.dataset,0.5,0.6);
  eclat.run();
}