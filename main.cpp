#include<iostream>
#include"b_plus_tree.h"

int main(){
    // Keep the order even just to avoid the violation of rules of a btree while splitting an internal node.
    BTree<int, 4> t;
    t.showEveryOpn = true;
    t.insert(10);
    t.insert(11);
    t.insert(12);
    t.insert(13);
    t.insert(14);
    t.insert(15);
    t.insert(16);
    t.insert(17);
    t.insert(18);
    t.insert(19);
    t.insert(1);
    t.insert(2);
    t.insert(3);
    t.BFS();
    t.range_traversal();
    bool demonstrate_removal = true;
    if(demonstrate_removal){
        cout << "Removing leaf " << 10 << endl;
        t.remove(10);
        t.range_traversal();
        cout << "Removing internal key " << 17 << " demonstrating removal of leaf 17 and replace internal 17 with successor." << endl;
        t.remove(17);
        t.range_traversal();
        cout << "Removing internal key " << 15 << " demonstrating merge" << endl;
        t.remove(15);
        t.range_traversal();
        cout << "Removing internal key " << 16 << " demonstrating filling of node" << "(borrow from right)" << endl;
        t.remove(16);
        t.range_traversal();
        cout << "Removing internal key " << 11 << " demonstrating filling of a node(borrow from left)" << endl;
        t.remove(11);
        t.range_traversal();
        cout << "Removing Internal key " << 10 << " which doesn't exist but rebalance occurs" << endl;
        t.remove(10);
        t.range_traversal();
    }
    
    t.in_order_traversal();
    cout << "Range based traversal, with limits" << endl;
    t.range_traversal(10, 16);
    cout << "Range based traversal, limits lying outside : " << endl;
    t.range_traversal(9, 118.5);
    cout << "Range based traversal, without specifying limits : " << endl;
    t.range_traversal();
    return 0;
}