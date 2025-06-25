import java.io.*;
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



public class PrePost_file{
    private List<List<String>> transaction;
    private double minSup;
    private int minSupCount; // = minSup * tong so luong giao dich, lam tron len
    private PPCNode root; // = null
    private Map<String, Integer> freq; //tan suat xuat hien cua tung item trong f1

    public PrePost_file(List<List<String>> transaction, double minSup){
        this.transaction = transaction;
        this.minSup = minSup;
        this.minSupCount = (int)Math.ceil(minSup * transaction.size());
        this.root = new PPCNode("null", null);
        this.freq = new HashMap<>();
    }

    public static List<List<String>> docFile(String file){
        List<List<String>> gd = new ArrayList<>();
        try(BufferedReader br = new BufferedReader(new FileReader(file))){
            String dong = br.readLine();
            while(dong != null){
                if(!dong.trim().isEmpty()){
                    String[] item = dong.trim().split("\\s+");
                    List<String> t = Arrays.asList(item);
                    gd.add(t);
                }
                dong = br.readLine();
            }
        } catch(IOException e){
            System.err.println("loi " + e.getMessage());
        } 
        return gd;
    }

// build cay PPC
    //buoc 1-3: tim tap f1 va sap xep giam dan theo sup
    public List<String> buildf1(){
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
        //sort theo tan suat giam dan -> l1
        f1.sort((item1, item2) -> freq.get(item2) - freq.get(item1));
        return f1;
    }

    //buoc 4-9: xay cay
    public void buildPPC(List<String> f1){
        for(List<String> t : transaction){
            //danh sach cac item pho bien trong giao dich
            List<String> tran = new ArrayList<>();
            for(String i : f1){
                if(t.contains(i)){
                    tran.add(i);
                }
            }

            if(!tran.isEmpty()){
                insertTree(tran, root);
            }
        }
    }

    //10-11: dfs gan preOrder va postOrder cho tung node
    private int preOrder_count=0;
    private int postOrder_count=0;
    public void assignPre(PPCNode node){
        node.preOrder = preOrder_count++;
        for(PPCNode child : node.child){
            assignPre(child);
        }
        node.postOrder = postOrder_count++;
    }

    //13-21:chen node vao cay
    private void insertTree(List<String> item, PPCNode node){
        if(item.isEmpty()) return;
        String ItemDauTien = item.get(0);
        PPCNode ch = null;

        for(PPCNode c : node.child){
            if(c.itemName.equals(ItemDauTien)){
                ch = c;
                break;
            }
        }

        if(ch != null){
            ch.count++;
        } else{
            ch = new PPCNode(ItemDauTien, node);
            node.child.add(ch);
        }
        insertTree(item.subList(1, item.size()), ch);
    }


//build nlist
    //duyet cay theo pre order 
    public void NListPreOrder(PPCNode node, List<NList> nl1, List<String> l1){
        for(int i=0;i<l1.size();i++){
            if(node.itemName.equals(l1.get(i))){
                PPCode res = new PPCode(node.preOrder, node.postOrder, node.count);
                nl1.get(i).addCode(res);
            }
        }
        
        for(PPCNode c : node.child){
            NListPreOrder(c, nl1, l1);
        }
    }

    public List<NList> buildNList(List<String> l1){
        List<NList> nl1 = new ArrayList<>();
        for(String i : l1){
            NList nlist = new NList(i);
            nl1.add(nlist);
        }
        NListPreOrder(root, nl1, l1);
        return nl1;
    }

    public static void main(String[] args){
        List<List<String>> transaction = docFile("c:\\Users\\DELL\\Desktop\\NCKH\\NCKH2025\\PPC_Tree\\Phuc\\chess.txt");
        double minSup = 0.5;

        PrePost_file ppc = new PrePost_file(transaction, minSup);

        List<String> f1 = ppc.buildf1();

        try (PrintWriter write = new PrintWriter(new FileWriter("c:\\Users\\DELL\\Desktop\\NCKH\\NCKH2025\\PPC_Tree\\Phuc\\NlistChess.txt"))){
            write.println("F1: " + f1);

            ppc.buildPPC(f1);
            ppc.assignPre(ppc.root);
            write.println("Cay PPC:");
            writeTree(ppc.root, " ", write);

            List<NList> nlist = ppc.buildNList(f1);
            write.println("NList:");
            for(NList n : nlist){
                write.println(n);
            }
        } catch (IOException e){
            System.err.println("loi: " + e.getMessage());
        }
    }

    public static void writeTree(PPCNode node, String a, PrintWriter writer){
        if(!node.itemName.equals("null")){
            writer.println(a + node.itemName + " (" + node.count + ") [" + node.preOrder + "," + node.postOrder + "]");
        }
        for(PPCNode c : node.child){
            writeTree(c, a + "      ", writer);
        }
    }
}