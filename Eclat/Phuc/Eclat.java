package Eclat.Phuc;

import java.io.*;
import java.util.*;

public class Eclat {

    static final int MAX_TRANS = 4000; // đủ cho chess.txt

    // Đọc transactions từ file
    public static List<List<String>> readTransactions(String filename) throws IOException {
        List<List<String>> transactions = new ArrayList<>();
        BufferedReader br = new BufferedReader(new FileReader(filename));
        String line;
        while ((line = br.readLine()) != null) {
            String[] tokens = line.trim().split("\\s+");
            transactions.add(Arrays.asList(tokens));
        }
        br.close();
        return transactions;
    }

    // Chuyển transactions sang map<item, BitSet>
    public static Map<String, BitSet> convert(List<List<String>> transactions) {
        Map<String, BitSet> res = new HashMap<>();
        for (int i = 0; i < transactions.size(); i++) {
            for (String item : transactions.get(i)) {
                res.computeIfAbsent(item, k -> new BitSet(MAX_TRANS)).set(i);
            }
        }
        return res;
    }

    // Eclat với BitSet
    public static void eclat(List<Map.Entry<String, BitSet>> items, BitSet prefixTids, List<String> prefix, int minSup,
                             List<Pair<List<String>, Integer>> results, int nTrans) {
        for (int i = 0; i < items.size(); i++) {
            String item = items.get(i).getKey();
            BitSet itemTids = (BitSet) items.get(i).getValue().clone();

            BitSet newTids;
            if (prefix.isEmpty()) {
                newTids = itemTids;
            } else {
                newTids = (BitSet) prefixTids.clone();
                newTids.and(itemTids);
            }

            int support = newTids.cardinality();
            if (support >= minSup) {
                prefix.add(item);
                results.add(new Pair<>(new ArrayList<>(prefix), support));

                // Chuẩn bị các item phía sau
                List<Map.Entry<String, BitSet>> newItems = new ArrayList<>();
                for (int j = i + 1; j < items.size(); j++) {
                    newItems.add(items.get(j));
                }
                if (!newItems.isEmpty()) {
                    eclat(newItems, newTids, prefix, minSup, results, nTrans);
                }
                prefix.remove(prefix.size() - 1);
            }
        }
    }

    // Sắp xếp tăng dần theo support
    public static Comparator<Map.Entry<String, BitSet>> supportComparator() {
        return Comparator.comparingInt(e -> e.getValue().cardinality());
    }

    // Định nghĩa Pair đơn giản
    public static class Pair<A, B> {
        public A first;
        public B second;
        public Pair(A a, B b) { first = a; second = b; }
    }

    public static void main(String[] args) throws IOException {
        String filename = "chess.txt"; // Đặt đúng đường dẫn file
        int minSup = 1500; // Chỉnh theo nhu cầu

        List<List<String>> transactions = readTransactions(filename);
        int nTrans = transactions.size();

        Map<String, BitSet> itemTid = convert(transactions);

        // Lọc các item đủ minSup
        List<Map.Entry<String, BitSet>> itemSet = new ArrayList<>();
        for (Map.Entry<String, BitSet> x : itemTid.entrySet()) {
            if (x.getValue().cardinality() >= minSup) {
                itemSet.add(x);
            }
        }
        itemSet.sort(supportComparator());

        List<Pair<List<String>, Integer>> ans = new ArrayList<>();
        eclat(itemSet, new BitSet(MAX_TRANS), new ArrayList<>(), minSup, ans, nTrans);

        System.out.println("Các tập phổ biến:");
        for (Pair<List<String>, Integer> x : ans) {
            for (String y : x.first) System.out.print(y + " ");
            System.out.println(": " + x.second);
        }
    }
}
