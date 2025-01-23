#include<iostream>
#include"b_plus_tree.h"

int main(){
    // Keep the order even just to avoid the violation of rules of a btree while splitting an internal node.
    BTree<int, 4> t;
    t.showEveryOpn = false;
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
    bool demonstrate_removal = false;
    if(demonstrate_removal){
        cout << "Removing internal key " << 11 << " demonstrating predecessor" << endl;
        t.remove(11);
        cout << "Removing internal key " << 17 << " demonstrating successor" << endl;
        t.remove(17);
        cout << "Removing internal key " << 15 << " demonstrating merge" << endl;
        t.remove(15);
        cout << "Removing internal key " << 16 << " demonstrating filling of node" << 18 << "(borrow from left)" << endl;
        t.remove(16);
        cout << "Removing internal key " << 3 << " demonstrating filling of a node" << 2 << "(borrow from right)" << endl;
        cout << "Here key 2 is filled with 10 and then 3, 10, 12 are merged as merging happens with next node." << endl;
        t.remove(3);
        cout << "Removing Internal key " << 10 << " demonstrating shrinking" << endl;
        cout << "Here, we merge 2, 13, 18 as we want to fill 2 and 2 got no siblings to borrow from." << endl;
        cout << "Now that root(13) has been merged into first child(2)  and no of keys in root is 0, create a new root and assign it to the first child(2)." << endl;
        t.remove(2);
    }
    
    // t.in_order_traversal();
    cout << "Range based traversal, with limits" << endl;
    t.range_traversal(10, 16);
    cout << "Range based traversal, limits lying outside : " << endl;
    t.range_traversal(9, 14.6);
    cout << "Range based traversal, without specifying limits : " << endl;
    t.range_traversal();
    return 0;
}