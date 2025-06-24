package NCKH2025.N_List.Phuc;
import java.util.*;
import java.util.stream.Collectors;

class PPCNode{
    String itemName;
    int count;
    List<PPCNode> children;
    PPCNode parent;
    int preOrder;
    int postOrder;

    public PPCNode(String itemName, PPCNode parent){
        this.itemName = itemName;
        this.count = 1;
        this.children = new ArrayList<>();
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
        return codes.stream().mapToInt(c -> c.count).sum();
    }

    @Override
    public String toString() {
        return itemName + ": " + codes;
    }
}


public class Main{
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