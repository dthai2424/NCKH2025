import java.util.*;
import java.util.stream.Collectors;

// Lớp đại diện cho một nút trong PPC-tree
class PPCNode {
    String itemName;
    int count;
    List<PPCNode> children;
    PPCNode parent;
    int preOrder;
    int postOrder;

    public PPCNode(String itemName, PPCNode parent) {
        this.itemName = itemName;
        this.count = 1;
        this.children = new ArrayList<>();
        this.parent = parent;
    }
}

// Lớp đại diện cho mã PP-code (pre-order, post-order và count)
class PPCode {
    int preOrder;
    int postOrder;
    int count;

    public PPCode(int preOrder, int postOrder, int count) {
        this.preOrder = preOrder;
        this.postOrder = postOrder;
        this.count = count;
    }

    @Override
    public String toString() {
        return "⟨(" + preOrder + "," + postOrder + "):" + count + "⟩";
    }
}

// Lớp đại diện cho N-list (danh sách các PP-code)
class NList {
    List<PPCode> codes = new ArrayList<>();
    String itemName;

    public NList(String itemName) {
        this.itemName = itemName;
    }

    public void addCode(PPCode code) {
        codes.add(code);
        // Sắp xếp theo pre-order tăng dần
        codes.sort(Comparator.comparingInt(c -> c.preOrder));
    }

    public int getSupport() {
        return codes.stream().mapToInt(c -> c.count).sum();
    }

    @Override
    public String toString() {
        return itemName + ": " + codes;
    }
}

// Lớp chính triển khai thuật toán PrePost
public class PrePostAlgorithm {
    private List<List<String>> transactions;
    private double minSupport;
    private int minSupportCount;
    private PPCNode root;
    private int preOrderCounter = 1;
    private int postOrderCounter = 1;
    private Map<String, NList> nLists = new HashMap<>();
    private Map<String, Integer> frequentItems = new LinkedHashMap<>();

    public PrePostAlgorithm(List<List<String>> transactions, double minSupport) {
        this.transactions = transactions;
        this.minSupport = minSupport;
        this.minSupportCount = (int) Math.ceil(minSupport * transactions.size());
        this.root = new PPCNode("null", null);
    }

    // Bước 1: Xây dựng PPC-tree
    public void buildPPCTree() {
        // Tính toán các mục phổ biến 1-itemset
        Map<String, Integer> itemCounts = new HashMap<>();
        for (List<String> transaction : transactions) {
            for (String item : transaction) {
                itemCounts.put(item, itemCounts.getOrDefault(item, 0) + 1);
            }
        }

        // Lọc ra các mục phổ biến
        for (Map.Entry<String, Integer> entry : itemCounts.entrySet()) {
            if (entry.getValue() >= minSupportCount) {
                frequentItems.put(entry.getKey(), entry.getValue());
            }
        }

        // Sắp xếp các mục phổ biến theo thứ tự giảm dần của độ hỗ trợ
        frequentItems = frequentItems.entrySet().stream()
                .sorted(Map.Entry.<String, Integer>comparingByValue().reversed())
                .collect(Collectors.toMap(
                        Map.Entry::getKey,
                        Map.Entry::getValue,
                        (e1, e2) -> e1,
                        LinkedHashMap::new));

        // Xây dựng PPC-tree
        for (List<String> transaction : transactions) {
            // Lọc và sắp xếp các mục trong giao dịch
            List<String> filteredSorted = transaction.stream()
                    .filter(frequentItems::containsKey)
                    .sorted(Comparator.comparingInt(item -> -frequentItems.get(item)))
                    .collect(Collectors.toList());

            insertTransaction(filteredSorted, root);
        }

        // Gán mã pre-order và post-order
        assignPrePostOrder(root);
    }

    private void insertTransaction(List<String> items, PPCNode node) {
        if (items.isEmpty()) return;

        String firstItem = items.get(0);
        List<String> remainingItems = items.subList(1, items.size());

        // Tìm nút con có cùng tên mục
        PPCNode child = null;
        for (PPCNode c : node.children) {
            if (c.itemName.equals(firstItem)) {
                child = c;
                break;
            }
        }

        // Nếu không tìm thấy, tạo nút mới
        if (child == null) {
            child = new PPCNode(firstItem, node);
            node.children.add(child);
        } else {
            child.count++;
        }

        // Chèn các mục còn lại
        insertTransaction(remainingItems, child);
    }

    private void assignPrePostOrder(PPCNode node) {
        if (node == null) return;
        
        // Gán pre-order
        node.preOrder = preOrderCounter++;
        
        // Duyệt các nút con
        for (PPCNode child : node.children) {
            assignPrePostOrder(child);
        }
        
        // Gán post-order
        node.postOrder = postOrderCounter++;
    }

    // Bước 2: Xây dựng N-list cho các mục phổ biến
    public void buildNLists() {
        traverseAndCollect(root);
    }

    private void traverseAndCollect(PPCNode node) {
        if (node == null || node.itemName.equals("null")) {
            for (PPCNode child : node.children) {
                traverseAndCollect(child);
            }
            return;
        }

        // Tạo PP-code cho nút hiện tại
        PPCode code = new PPCode(node.preOrder, node.postOrder, node.count);
        
        // Thêm vào N-list tương ứng
        nLists.computeIfAbsent(node.itemName, k -> new NList(k))
              .addCode(code);

        // Duyệt các nút con
        for (PPCNode child : node.children) {
            traverseAndCollect(child);
        }
    }

    // Bước 3: Khai thác itemset thường xuyên sử dụng N-list
    public Set<String> mineFrequentItemsets() {
        Set<String> frequentItemsets = new HashSet<>();

        // Thêm các 1-itemset
        for (String item : frequentItems.keySet()) {
            frequentItemsets.add(item);
        }

        // Khai thác 2-itemset
        Map<String, NList> frequent2Itemsets = new HashMap<>();
        List<String> frequentItemsList = new ArrayList<>(frequentItems.keySet());
        
        for (int i = 0; i < frequentItemsList.size(); i++) {
            for (int j = i + 1; j < frequentItemsList.size(); j++) {
                String item1 = frequentItemsList.get(i);
                String item2 = frequentItemsList.get(j);
                String itemset = item1 + "," + item2;
                
                NList nlist = intersectNLists(nLists.get(item1), nLists.get(item2));
                if (nlist.getSupport() >= minSupportCount) {
                    frequentItemsets.add(itemset);
                    frequent2Itemsets.put(itemset, nlist);
                }
            }
        }

        // Khai thác các itemset lớn hơn (sử dụng phương pháp đệ quy)
        for (Map.Entry<String, NList> entry : frequent2Itemsets.entrySet()) {
            mineKLists(entry.getKey(), entry.getValue(), frequentItemsets);
        }

        return frequentItemsets;
    }

    private void mineKLists(String prefix, NList nlist, Set<String> results) {
        // Logic khai thác itemset kích thước lớn hơn ở đây
        // (Triển khai đầy đủ cần xử lý đệ quy và giao N-list)
    }

    // Thuật toán giao N-list với độ phức tạp O(m+n)
    private NList intersectNLists(NList nlist1, NList nlist2) {
        NList result = new NList(nlist1.itemName + "," + nlist2.itemName);
        List<PPCode> list1 = nlist1.codes;
        List<PPCode> list2 = nlist2.codes;
        
        int i = 0, j = 0;
        while (i < list1.size() && j < list2.size()) {
            PPCode code1 = list1.get(i);
            PPCode code2 = list2.get(j);
            
            // Kiểm tra quan hệ tổ tiên - hậu duệ
            if (code1.preOrder < code2.preOrder && code1.postOrder > code2.postOrder) {
                result.addCode(new PPCode(code1.preOrder, code1.postOrder, code2.count));
                j++;
            } else if (code1.preOrder > code2.preOrder) {
                j++;
            } else {
                i++;
            }
        }
        
        return result;
    }

    // Hàm main để kiểm thử
    public static void main(String[] args) {
        // Tạo dữ liệu giao dịch mẫu
        List<List<String>> transactions = new ArrayList<>();
        transactions.add(Arrays.asList("a", "c", "g", "f"));
        transactions.add(Arrays.asList("e", "a", "c", "b"));
        transactions.add(Arrays.asList("e", "c", "b", "i"));
        transactions.add(Arrays.asList("b", "f", "h"));
        transactions.add(Arrays.asList("b", "f", "e", "c", "d"));

        double minSupport = 0.4; // 40%

        PrePostAlgorithm algorithm = new PrePostAlgorithm(transactions, minSupport);
        
        // Bước 1: Xây dựng PPC-tree
        algorithm.buildPPCTree();
        
        // Bước 2: Xây dựng N-list
        algorithm.buildNLists();
        
        // In N-list
        System.out.println("N-lists:");
        algorithm.nLists.forEach((k, v) -> System.out.println(v));
        
        // Bước 3: Khai thác itemset thường xuyên
        Set<String> frequentItemsets = algorithm.mineFrequentItemsets();
        System.out.println("\nFrequent Itemsets:");
        frequentItemsets.forEach(System.out::println);
    }
}