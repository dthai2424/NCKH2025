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
#define all(a) (a).begin(), (a).end()
#define IOS() ios::sync_with_stdio(false), cin.tie(0), cout.tie(0)
using namespace std;

// Cấu trúc Data giúp đọc input từ người dùng, bao gồm ngưỡng hỗ trợ tối thiểu
// (min_sup) và ngưỡng tin cậy tối thiểu (min_conf).
// Nó cũng lưu trữ dữ liệu dưới dạng một bản đồ, trong đó mỗi khóa là một chuỗi
// (string) và giá trị là một tập hợp các số nguyên (set<int>).
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
// Cấu trúc Rule đại diện cho một quy tắc trong thuật toán Apriori, bao gồm hai
// tập hợp số nguyên (x và y) và độ tin cậy (confidence). Nó cũng định nghĩa
// toán tử so sánh để sắp xếp các quy tắc theo thứ tự nhất định. Toán tử so sánh
// này so sánh trước tiên theo tập hợp x, sau đó theo tập hợp y nếu x bằng nhau.
struct Rule {
  set<int> x, y;
  double confidence;
  Rule();
  Rule(set<int> x, set<int> y) {
    this->x = x;
    this->y = y;
  }
  void set_confidence(double confidence) { this->confidence = confidence; }
  bool operator<(const Rule &other) const {
    if (x != other.x)
      return x < other.x;
    return y < other.y;
  }
};
// Cấu trúc Apriori thực hiện thuật toán Apriori để tìm tập hợp con thường xuyên
// và quy tắc kết hợp. Nó bao gồm các phương thức để khởi tạo tập hợp con thường
// xuyên (l_set), tạo ra các ứng viên (candidates), lọc các ứng viên dựa trên
// tần suất tối thiểu, và tạo ra các quy tắc từ các tập hợp con thường xuyên.
struct Apriori {
  set<set<int>> l_set;
  set<Rule> rule_set;
  map<string, set<int>> dataset;
  double min_sup;
  double min_conf;
  Apriori(map<string, set<int>> dataset, double min_sup, double min_conf) {
    this->dataset = dataset;
    this->min_sup = min_sup;
    this->min_conf = min_conf;
  }
  // Hàm chuyển đổi tập hợp các tập hợp thành vector các tập hợp.
  vector<set<int>> convert_to_vector(set<set<int>> &sets) {
    vector<set<int>> res(all(sets));
    return res;
  }
  // Hàm đếm số lượng hỗ trợ của một tập hợp con trong tập dữ liệu.
  int count_support(map<string, set<int>> &dataset, set<int> &candidates) {

    int count = 0;
    for (auto data : dataset) {
      bool valid = true;
      for (auto item : candidates) {
        if (data.second.find(item) == data.second.end()) {
          valid = false;
          break;
        }
      }
      if (valid)
        count++;
    }
    return count;
  }
  // Hàm kiểm tra xem một cặp tập hợp có phải là ứng viên hợp lệ hay không.
  // Vì trong quá trình generate candidates, chúng ta sẽ tạo ra các tập hợp con
  // có kích thước bằng k hay nói cách khác 2 tập chỉ khác nhau ở 1 phần tử
  // nên chúng ta sẽ kiểm tra xem nếu bỏ đi một phần tử của tập hợp con này và
  // thêm vào cái chưa xuất hiện thì tập hợp con còn lại có nằm trong l_set hay
  // không
  bool is_valid_candidate(set<int> &a_set, set<int> &b_set,
                          set<set<int>> &l_set) {
    int not_exist_b = 0, not_exist_a = 0;
    for (auto num : a_set) {
      if (b_set.find(num) == b_set.end()) {
        not_exist_b = num;
        break;
      }
    }
    for (auto num : b_set) {
      if (a_set.find(num) == a_set.end()) {
        not_exist_a = num;
        break;
      }
    }
    for (auto num : a_set) {
      set<int> check_set(all(a_set));
      check_set.erase(num);
      check_set.insert(not_exist_a);
      if (l_set.find(check_set) == l_set.end())
        return false;
    }
    for (auto num : b_set) {
      set<int> check_set(all(b_set));
      check_set.erase(num);
      check_set.insert(not_exist_b);
      if (l_set.find(check_set) == l_set.end())
        return false;
    }
    return true;
  }
  // Hàm hợp nhất hai tập hợp thành một tập hợp mới.
  set<int> merge_set(set<int> &a_set, set<int> &b_set) {
    set<int> union_set;
    set_union(all(a_set), all(b_set), inserter(union_set, union_set.begin()));
    return union_set;
  }
  // Hàm tạo ra các ứng viên từ tập hợp con thường xuyên trước đó.
  // Nó sẽ tạo ra các tập hợp con có kích thước bằng k từ các tập hợp con có
  // kích thước k-1(lưu ý k-1 là tập ứng viên hợp lệ trước đó). Nếu tập hợp con
  // mới có kích thước bằng k và là ứng viên hợp lệ, nó sẽ được thêm vào tập hợp
  // ứng viên.
  set<set<int>> generate_candidate(set<set<int>> &prev_set, int k) {
    set<set<int>> candidate_set;
    vector<set<int>> prev_vec(all(prev_set));
    for (int i = 0; i < prev_vec.size() - 1; i++) {
      for (int j = i + 1; j < prev_vec.size(); j++) {
        set<int> union_set = merge_set(prev_vec[i], prev_vec[j]);
        if (union_set.size() == k) {
          if (is_valid_candidate(prev_vec[i], prev_vec[j], prev_set))
            candidate_set.insert(union_set);
        }
      }
    }
    return candidate_set;
  }
  // Hàm lọc các ứng viên dựa trên tần suất tối thiểu.
  // Nó sẽ đếm số lượng hỗ trợ của mỗi ứng viên trong tập dữ liệu và thêm vào
  // nếu hợp lệ
  set<set<int>> filter_candidate(map<string, set<int>> &dataset,
                                 set<set<int>> &candidates, int min_frequency) {
    set<set<int>> valid_set;
    for (auto child : candidates) {
      if (count_support(dataset, child) >= min_frequency) {
        valid_set.insert(child);
      }
    }
    return valid_set;
  }
  // Hàm thêm các ứng viên vào tập hợp con thường xuyên.
  void insert_candidate(set<set<int>> &l_set, set<set<int>> &candidates) {
    for (auto child : candidates) {
      l_set.insert(child);
    }
  }
  // Hàm khởi tạo tập hợp con thường xuyên từ tập dữ liệu.
  // Nó sẽ tạo ra các tập hợp con có kích thước bằng 1 từ các phần tử trong tập
  // dữ liệu, sau đó lọc các ứng viên dựa trên tần suất tối thiểu và thêm vào
  // tập hợp con thường xuyên.
  void init_l_set(map<string, set<int>> dataset, set<set<int>> &l_set,
                  int min_frequency) {
    set<set<int>> res;
    for (auto data : dataset) {
      for (auto num : data.second) {
        set<int> st;
        st.insert(num);
        res.insert(st);
      }
    }
    res = filter_candidate(dataset, res, min_frequency);
    insert_candidate(l_set, res);
  }
  // Hàm in ra tập hợp con thường xuyên.
  void print_set() {
    vector<set<int>> l_vec = convert_to_vector(l_set);
    sort(all(l_vec), [](auto const &a, auto const &b) {
      if (a.size() != b.size())
        return a.size() < b.size();
      return a < b;
    });

    cout << "L: ";
    for (auto num_set : l_vec) {
      cout << "{ ";
      for (auto num : num_set) {
        cout << num << " ";
      }
      cout << "}, ";
    }
  }
  // Hàm thực hiện giai đoạn 1 của thuật toán Apriori.
  // Nó sẽ khởi tạo tập hợp con thường xuyên, tạo ra các ứng viên, lọc ứng viên
  // hợp lệ và thêm vào tập thường xuyên
  // Trong khi tạo ứng viên thì giữ lại tập trước đó k-1 để check xem ứng viên
  // mới có hợp lệ hay không và làm đến khi tập ứng viên mới có size=0 Sau đó in
  // ra tập hợp con thường xuyên.
  void phase1() {
    int min_frequency = ceil(this->min_sup * dataset.size());
    init_l_set(this->dataset, this->l_set, min_frequency);
    int k = 1;
    set<set<int>> candidates = l_set;
    set<set<int>> prev_candidates = l_set;
    k++;
    while (candidates.size()) {
      candidates = generate_candidate(prev_candidates, k);
      candidates = filter_candidate(this->dataset, candidates, min_frequency);
      insert_candidate(l_set, candidates);
      prev_candidates = candidates;
      k++;
    }
    print_set();
  }

  // Hàm tạo ra các quy tắc từ tập hợp con thường xuyên.
  // Nó sẽ tạo ra các quy tắc từ các tập hợp con có kích thước lớn hơn hoặc
  // bằng 2. Mỗi quy tắc sẽ có phần antecedent (x) và consequent (y
  set<Rule> generate_rule(set<int> &candidate) {
    set<Rule> res;
    vector<int> candidate_v(all(candidate));
    for (int mask = 1; mask < (1 << candidate.size()) - 1; mask++) {

      set<int> ante, conse;
      for (int i = 0; i < candidate.size(); i++) {

        if ((mask & (1 << i)))
          ante.insert(candidate_v[i]);
        else
          conse.insert(candidate_v[i]);
      }
      if (!ante.empty() && !conse.empty())
        res.insert(Rule(ante, conse));
    }
    return res;
  }
  // Hàm tính độ tin cậy của một quy tắc.
  // Confidence( x => y)= Support(x U y) / Support(x)
  double get_confidence(map<string, set<int>> &dataset, set<int> &xy,
                        set<int> &x) {
    double xy_support = count_support(dataset, xy);
    double x_support = count_support(dataset, x);
    if (x_support == 0.0)
      return 0;
    return xy_support / x_support;
  }
  // Hàm lọc các quy tắc dựa trên độ tin cậy tối thiểu.
  set<Rule> filter_rule(map<string, set<int>> &dataset, set<int> candidate,
                        set<Rule> &rules, double min_conf) {
    set<Rule> res;
    for (auto child : rules) {
      double confidence = get_confidence(dataset, candidate, child.x);
      if (confidence >= min_conf) {
        child.set_confidence(confidence);
        res.insert(child);
      }
    }
    return res;
  }
  // Hàm thêm các quy tắc vào tập hợp quy tắc.
  void insert_rule(set<Rule> &rule_set, set<Rule> &generated_rule) {
    for (auto child : generated_rule) {
      rule_set.insert(child);
    }
  }
  // Hàm in ra các quy tắc đã tìm được.
  void print_rule() {
    cout << "\nRule:\n{\n";
    for (auto r : this->rule_set) {
      cout << "{ ";
      for (auto ante : r.x) {
        cout << ante << " ";
      }
      cout << "} => { ";
      for (auto conse : r.y) {
        cout << conse << " ";
      }
      cout << "} ";
      cout << "confidence: " << r.confidence << " \n";
    }
    cout << "}";
  }
  // Hàm thực hiện giai đoạn 2 của thuật toán Apriori.
  // Nó sẽ tạo ra các quy tắc từ các tập hợp con thường xuyên và lọc các quy tắc dựa trên độ tin cậy tối thiểu.
  void phase2() {
    for (auto numset : l_set) {
      if (numset.size() >= 2) {
        set<Rule> rules = generate_rule(numset);
        rules = filter_rule(this->dataset, numset, rules, this->min_conf);
        insert_rule(this->rule_set, rules);
      }
    }
    print_rule();
  }
  void run() {
    phase1();
    phase2();
  }
};
int main() {
  IOS();
#ifdef thai
  freopen("input.txt", "r", stdin);
  freopen("output.txt", "w", stdout);
#endif
  Data dt;
  dt.ReadInput();
  Apriori ap(dt.data,dt.min_sup,dt.min_conf);
  ap.run();

  return 0;
}
