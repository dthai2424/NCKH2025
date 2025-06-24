import java.util.*;
import java.util.stream.Collectors;

class PPCNode{
    String itemName;
    int count;
    List<PPCNode> child;
    PPCNode parent;
    int preOrder;
    int postOrder;

    public PPCNode(String itemName, PPCNode parent){
        this.itemName = itemName;
        this.count = 1;
        this.child = new ArrayList<>();
        this.parent = parent;
    }
}

class PPCode{
    int preOrder;
    int postOrder;
    int count;

    public PPCode(int preOrder, int postOrder, int count){
        this.preOrder = preOrder;
        this.postOrder = postOrder;
        this.count = count;
    }

    @Override
    public String toString(){
        return preOrder + " " + postOrder + " " + count;
    }
}

class NList{
    List<PPCode> codes = new ArrayList<>();
    String itemName;
    public NList(String itemName){
        this.itemName = itemName;
    }

    public void addCode(PPCode code){
        codes.add(code);
        // sap xep preOrder tang dan
        codes.sort(Comparator.comparingInt(c->c.preOrder));
    }
    // tra ve sup cua 1 itemset
    public int getSup() {
        return codes.stream().mapToInt(c->c.count).sum();
    }

    @Override
    public String toString() {
        return itemName + ": " + codes;
    }
}



public class Main{
    private List<List<String>> transaction;
    private double minSup;
    private int minSupCount; // = minSup * tong so luong giao dich, lam tron len
    private PPCNode root; // = null
    private Map<String, Integer> freq; //tan suat xuat hien cua tung item trong f1

    public Main(List<List<String>> transaction, double minSup){
        this.transaction = transaction;
        this.minSup = minSup;
        this.minSupCount = (int)Math.ceil(minSup * transaction.size());
        this.root = new PPCNode("null", null);
        this.freq = new HashMap<>();
    }

    //buoc 1-3: tim tap f1 va sap xep giam dan theo sup
    public List<String> buildF1(){
        for(List<String> t : transaction){
            for(String item : t){
                freq.put(item, freq.getOrDefault(item,0) + 1);
            }
        }
        
        // loc cac item co sup >= minSupCount
        List<String> f1 = new ArrayList<>();
        for(Map.Entry<String, Integer> e : freq.entrySet()){
            if(e.getValue() >= minSupCount){
                f1.add(e.getKey());
            }
        }
        //sort
        f1.sort((item1, item2) -> freq.get(item2) - freq.get(item1));
        return f1;
    }

    //buoc 4-9: xay cay
    public void buildPPC(List<String> f1){
        for(List<String> t : transaction){
            //danh sach cac item pho bien trong giao dich
            List<String> tran = new ArrayList<>();
            for(String i : f1){
                if(tran.contains(i)){
                    tran.add(i);
                }
            }

            if(!tran.isEmpty()){
                insertTree(tran, root);
            }
        }
    }

    //13-21:chen node vao cay
    private void insertTree(List<String> item, PPCNode node){
        if(item.isEmpty()) return;
        String ItemDauTien = item.get(0);
        PPCNode child = null;

        for(PPCNode c : node.child){
            if(c.itemName.equals(ItemDauTien)){
                child = c;
                break;
            }
        }

        if(child != null){
            child.count++;
        } else{
            child = new PPCNode(ItemDauTien, node);
            node.child.add(child);
        }
        insertTree(item.subList(1, item.size()), child);
    }

    public static void main(String[] args){
        List<List<String>> transaction = new ArrayList<>();
        transaction.add(Arrays.asList("a", "c", "g", "f"));
        transaction.add(Arrays.asList("e", "a", "c", "b"));
        transaction.add(Arrays.asList("e", "c", "b", "i"));
        transaction.add(Arrays.asList("b", "f", "h"));
        transaction.add(Arrays.asList("b", "f", "e", "c", "d"));
        double minSup = 0.4; // 40%

        
    }
}