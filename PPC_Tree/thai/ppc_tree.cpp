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
// Cấu trúc Transaction đại diện cho một giao dịch trong tập dữ liệu
struct Transaction {
  vector<int> data;
  Transaction() = default;
  Transaction(vector<int> dt) : data(dt){};
};
// Cấu trúc Data_Reader giúp đọc dữ liệu từ đầu vào, bao gồm ngưỡng hỗ trợ tối
// thiểu (min_sup) , ngưỡng tin cậy tối thiểu (min_conf) và cũng lưu trữ tập dữ
// liệu dưới dạng một vector các Transaction.
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

// Cấu trúc PP_Code đại diện cho một triplet tiền tố, hậu tố và số lần xuất hiện
// khi dfs trong ppc_tree.
struct PP_Code {
  int pre;
  int post;
  int count;
  PP_Code() = default;
  PP_Code(int pr, int po, int co) : pre(pr), post(po), count(co) {}
};
// Cấu trúc NList đại diện cho tập item và các PP_Code tương ứng.
struct NList {
  vector<int> item;
  vector<PP_Code> nl;
  int count = 0;
  NList() = default;
  NList(vector<int> it, vector<PP_Code> nlist, int c)
      : item(it), nl(nlist), count(c) {}
};

// Cấu trúc Node đại diện cho một nút trong cây PPC, bao gồm các thuộc tính như
// độ sâu (lv), id, số lần xuất hiện (count), tiền tố (pre), hậu tố (post) và
// các con nút (child).
struct Node {
  int lv;
  int id;
  int count;
  int pre, post{};
  map<int, Node *> child;
  Node() = default;
  Node(int item) : id(item), count(0), lv(0) {}
};
// Cấu trúc Frequent đại diện cho một tập hợp item thường xuyên, bao gồm một
// vector các item.
struct Frequent {
  vector<int> itemset;
  Frequent() = default;
  Frequent(vector<int> it) : itemset(it) {}
};

// Cấu trúc PPC_Tree thực hiện thuật toán PPC để tìm các tập hợp item thường
// xuyên trong một tập dữ liệu giao dịch. Nó bao gồm các phương thức để xử lý dữ
// liệu, thêm danh sách, chèn dữ liệu, tính toán tiền tố và hậu tố, khởi tạo
// NList, giao nhau NList, khai thác cây PPC và in ra các tập hợp item thường
// xuyên.
struct PPC_Tree {
  vector<Frequent> frequent_item; // Lưu trữ các tập hợp item thường xuyên
  vector<NList> nlist;            // Lưu trữ danh sách NList
  unordered_map<int, int> support_map; // Giữ support_count cho tập 1 phần tử
  vector<Transaction> dataset; // Lưu trữ tập dữ liệu giao dịch đã được lọc và
                               // sort theo support giảm dần
  double min_sup,
      min_conf; // Ngưỡng hỗ trợ tối thiểu và ngưỡng tin cậy tối thiểu
  int min_freq; // Ngưỡng tần suất tối thiểu, được tính bằng cách nhân min_sup
                // với kích thước của tập dữ liệu sau đó làm tròn
  Node *root; // Con trỏ đến nút gốc của cây PPC

  PPC_Tree() = default;
  PPC_Tree(vector<Transaction> &raw_data, double min_sup, double min_conf) {
    this->dataset = raw_data;
    this->min_sup = min_sup;
    this->min_conf = min_conf;
    root = new Node(0);
  }
  // Hàm xử lý dữ liệu giao dịch, lọc các giao dịch dựa trên ngưỡng hỗ trợ tối
  // thiểu và sắp xếp các phần tử trong mỗi giao dịch theo tần suất giảm dần.
  void process_data(vector<Transaction> &raw_data) {
    this->min_freq = ceil(this->min_sup * raw_data.size());
    vector<Transaction> filter_data;
    for (auto &trans : raw_data) {
      for (auto num : trans.data) {
        support_map[num]++;
      }
    }
    for (auto &trans : raw_data) {
      Transaction tr;
      for (auto num : trans.data) {
        if (support_map[num] >= min_freq)
          tr.data.push_back(num);
      }
      filter_data.push_back(tr);
    }
    for (auto &trans : filter_data) {
      sort(all(trans.data), [&](int a, int b) {
        if (support_map[a] != support_map[b])
          return support_map[a] > support_map[b];
        return a < b;
      });
    }
    this->dataset = filter_data;
  }
  // Hàm dfs để thêm list dữ liệu của 1 giao dịch vào cây PPC
  void add_list(Transaction &trans) {
    Node *p = root;
    for (auto num : trans.data) {
      if (p->child.find(num) == p->child.end()) {
        Node *newNode = new Node(num);
        newNode->lv = p->lv + 1;
        p->child[num] = newNode;
      }
      p = p->child[num];

      p->count++;
    }
  }
  // Hàm chạy trên từng giao dịch để đưa list dữ liệu của giao dịch đó vào cây
  void insert_data() {
    for (auto trans : dataset) {
      add_list(trans);
    }
  }
  // Hàm tính toán tiền tố và hậu tố cho từng nút trong cây PPC bằng cách duyệt
  // qua cây theo thứ tự DFS.
  void compute_prepost(Node *cur, int &start, int &end) {
    cur->pre = start++;
    for (auto child : cur->child) {
      compute_prepost(child.second, start, end);
    }
    cur->post = end++;
  }
  // Hàm khởi tạo NList từ cây PPC, nó sẽ duyệt qua từng nút trong cây bằng dfs
  // và thêm các PP_Code tương ứng vào NList.
  void init_nlist(Node *cur, vector<NList> &nlist) {
    if (cur != root) {
      if (nlist[cur->id].count == 0)
        nlist[cur->id].item.push_back(cur->id);
      nlist[cur->id].nl.push_back({cur->pre, cur->post, cur->count});
      nlist[cur->id].count += cur->count;
    }
    for (auto child : cur->child) {
      init_nlist(child.second, nlist);
    }
  }
  // Hàm giao nhau hai NList, nó sẽ tìm các phần tử chung giữa hai NList và trả
  // về một NList mới chứa các phần tử chung đó. Nó sẽ so sánh tiền tố và hậu tố
  // của các phần tử trong hai NList và nếu tiền tố của phần tử a nhỏ hơn tiền
  // tố của phần tử b và hậu tố của phần tử a lớn hơn hậu tố của phần tử b, thì
  // nó sẽ thêm phần tử đó vào NList kết quả. Nếu phần tử đã tồn tại trong NList
  // kết quả, nó sẽ tăng số lần xuất hiện của phần tử đó.
  NList nlist_intersection(NList &a, NList &b) {
    NList res;
    int i = 0, j = 0;
    while (i < a.nl.size() && j < b.nl.size()) {
      if (a.nl[i].pre < b.nl[j].pre) {
        if (a.nl[i].post > b.nl[j].post) {
          // Nếu tiền tố của a nhỏ hơn tiền tố của b và hậu tố của a lớn hơn hậu
          // tố của b thì a là tổ tiên của b
          if (res.nl.size() && res.nl.back().pre == a.nl[i].pre) {
            // Nếu phần tử đã tồn tại trong NList kết quả, tăng số lần xuất hiện
            res.nl.back().count += b.nl[j].count;
          } else {
            // Nếu phần tử chưa tồn tại trong NList kết quả, thêm phần tử mới
            res.nl.push_back({a.nl[i].pre, a.nl[i].post, b.nl[j].count});
          }
          // Tăng số lần xuất hiện của phần tử trong NList kết quả
          res.count += b.nl[j].count;
          j++;

        } else {
          i++;
        }
      } else {
        j++;
      }
    }
    res.item = a.item;
    res.item.push_back(b.item.back());
    return res;
  }
  // Hàm khai thác cây PPC để tìm các tập hợp item thường xuyên.
  // Hàm này chạy dfs trên tùng nhánh ví dụ ta có tập a,b,c,d,e
  // thì nó sẽ chạy: a ,ab,abc,abcd,abcde
  // và tương tự chạy : b,bc,bcd,bcde.

  // Trong quá trình chạy, nếu NList giao nhau có số lần xuất hiện lớn hơn hoặc
  // bằng ngưỡng tần suất tối thiểu, nó sẽ được thêm vào danh sách NList mới.
  // Nếu NList mới chỉ có một phần tử, nó sẽ thêm phần tử đó vào danh sách các
  // tập hợp item thường xuyên (frequent_item) với tiền tố là phần tử đó. Nếu
  // NList mới có nhiều hơn một phần tử, nó sẽ gọi đệ quy để tiếp tục khai thác
  // các tập hợp con của NList mới đó, tăng độ sâu (depth) lên 1. Cuối cùng, nó
  // sẽ in ra các tập hợp item thường xuyên đã tìm được

  void mining(vector<NList> &nlist, int depth) {

    for (int i = nlist.size() - 1; i > 0; i--) {

      vector<NList> new_nlist;
      for (int j = i - 1; j >= 0; j--) {
        NList intersect = (nlist_intersection(nlist[i], nlist[j]));

        if (intersect.count >= min_freq) {
          new_nlist.push_back(intersect);
        }
      }

      if (!new_nlist.empty()) {
        for (int k = 0; k < new_nlist.size(); k++) {
          if (new_nlist[k].nl.size() == 1) {
            vector<int> prefix;
            for (int m = 0; m < new_nlist[k].item.size(); m++) {
              int num = new_nlist[k].item[m];
              prefix.push_back(num);
              if (m + 1 >= depth + 1)
                frequent_item.push_back(Frequent(prefix));
            }

          } else {
            mining(new_nlist, depth + 1);
          }
        }
      }
    }
  }
  void run() {
    // Xử lý dữ liệu đầu vào
    process_data(this->dataset);
    // Tạo cây
    insert_data();
    int start = 0;
    int end = 0;
    // Tính toán prepost
    compute_prepost(root, start, end);
    vector<NList> dummy(support_map.size());
    // Tạo nlist từ cây PPC với 1 dummy
    init_nlist(root, dummy);
    // Lọc nlist theo ngưỡng tần suất tối thiểu
    for (auto &node_list : dummy) {
      if (node_list.count >= min_freq) {
        nlist.push_back(node_list);
      }
    }
    // Sắp xếp nlist theo tần suất giảm dần và sau đó theo giá trị phần tử
    // Sau đó đảo ngược nlist để có thứ tự từ lớn đến nhỏ vì node sẽ được duyệt
    // từ cuối lên đầu nên phần tử support lớn sẽ được duyệt trước vì thứ tự dfs
    // của nó sớm hơn
    sort(all(nlist), [&](NList A, NList B) {
      if (support_map[A.item[0]] != support_map[B.item[0]]) {
        return support_map[A.item[0]] > support_map[B.item[0]];
      }
      return A.item[0] < B.item[0];
    });
    reverse(all(nlist));
    // Thêm tập hợp item thường xuyên với kích thước 1 vào danh sách
    // frequent_item
    for (auto node_list : nlist) {

      frequent_item.push_back(Frequent({node_list.item[0]}));
    }
    // Tìm tập hợp item thường xuyên với kích thước lớn hơn 1 nếu có
    mining(nlist, 1);
    // In ra kết quả là tập các item thường xuyên
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
  void print_tree(Node *cur) {
    el;
    cout << "tree: ", el;
    cout << cur->id << ": " << cur->pre << " " << cur->post
         << ",lv: " << cur->lv,
        el;
    for (auto child : cur->child) {
      print_tree(child.second);
    }
  }
  void print_nlist(vector<NList> nlist) {
    el;
    cout << "nlist: ", el;
    for (auto node_list : nlist) {
      cout << "{ ";
      for (auto it : node_list.item) {
        cout << it << " ";
      }
      cout << "}: { ";
      for (auto nl : node_list.nl) {
        cout << "( " << nl.pre << " " << nl.post << " " << nl.count << " ), ";
      }
      cout << "}";
      el;
    }
  }
  void print_frequent() {
    cout << "\nFrequent itemsets:\n";
    for (auto &f : frequent_item) {
      cout << "{ ";
      for (int x : f.itemset)
        cout << x << ' ';
      cout << "}";
      el;
    }
  }
};
// a b c d e f g h i j k
// 1 2 3 4 5 6 7 8 9 10

int main() {
  IOS();
#ifdef thai
  freopen("input.txt", "r", stdin);
  freopen("output.txt", "w", stdout);
#endif
  Data_Reader data_reader;
  data_reader.ReadInput();
  PPC_Tree tr(data_reader.dataset, data_reader.min_sup, data_reader.min_conf);
  tr.run();
}