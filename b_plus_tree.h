#include <iostream>
using namespace std;
#include <bits/stdc++.h>
template <typename T, int ORDER> // to keep the b tree generic. Order is the max no of children in a node.
class BTreeNode {
public:
    T keys[ORDER - 1];
    BTreeNode* children[ORDER];
    int n; //current no of keys
    bool leaf;

    BTreeNode(bool isLeaf = true) : n(0), leaf(isLeaf) {
        for(int i = 0; i < ORDER; i++){
            children[i] = nullptr;
        }
    }
};

template <typename T, int ORDER>
class BTree {
private:
    BTreeNode<T, ORDER>* root;
    void splitChild(BTreeNode<T, ORDER>* x, int i){
        BTreeNode<T, ORDER>* y = x -> children[i];
        BTreeNode<T, ORDER>* z = new BTreeNode<T, ORDER>(y -> leaf);
        // In case of odd order like 5, the no of keys will be 4. {10, 20, 30, 40} will split into {10} and {30, 40}.
        // In case of even order like 6, the no of keys will be 5. {10, 20, 30, 40, 50} will split into {10, 20} and {40, 50}.
        if(ORDER%2){
            z -> n = ORDER/2;
        }
        else {
            z -> n = ORDER/2 - 1;
        }
        y -> n = ORDER/2 - 1; // just change the no of keys instead of removing them.
        // To implement a b+ tree, we need to add the parent promoted key to the next child to ensure that the range queries happen. In order to support this, copy the middle key of y to the first child of z.
        // If we are splitting a leaf node, we start from middle element. In case of internal node, we start from middle element + 1. so in case of leaf node, ORDER/2 - 1 which is the middle element.
        int middleStart = 0;
        if(y -> leaf){
            z -> n++;
            middleStart = -1;
            z -> children[ORDER - 1] = y -> children[ORDER - 1];
            y -> children[ORDER - 1] = z;
        }
        // If the node we are splitting is an internal node, there is no change, we just copy the last half elements.
        for(int j = 0; j < z -> n; j++){ // In case of odd no of keys, we start at the middle element. right half in case of even elements.
            z -> keys[j] = y -> keys[middleStart + j + ORDER/2];
        }
        if(!y -> leaf){ // In case y is an internal node, transfer the children of y to z.
            for(int j = 0; j < z -> n + 1; j++){
                z -> children[j] = y -> children[j + ORDER/2];
            }
        }
        for(int j = x -> n; j >= i + 1; j--){ // Here, we are shifting the childrens by 1 towards right side.
            x -> children[j + 1] = x -> children[j];
        }
        x -> children[i + 1] = z; // y is at i and z will be at i + 1.
        for(int j = x -> n - 1; j >= i; j--){
            x -> keys[j + 1] = x -> keys[j];
        }
        x -> keys[i] = y -> keys[ORDER/2 - 1];
        x -> n = x -> n + 1;
    }

    void insertNonFull(BTreeNode<T, ORDER>* x, T k){
        int i = x -> n - 1;
        if(x -> leaf){ // If we are at leaf, just insert the key.
            while(i >= 0 && k < x -> keys[i]){
                x -> keys[i + 1] = x -> keys[i];
                i--;
            }
            x -> keys[i + 1] = k;
            x -> n = x -> n + 1;
        }
        else {
            // Iterate to the correct child and recursively insert the key k.
            while(i >= 0 && k < x -> keys[i]){
                i--;
            }
            i++; // We are at an index where the key corresponding to that index is >= our current key. This key should be put in x->children[i].
            if(x -> children[i] -> n == ORDER - 1){ // In case of when the subtree root node that contains the child is full, split it and push to the right node.We do this to ensure not going back upwards when the split happens at the leaf.
                splitChild(x, i);
                if(k > x -> keys[i]){
                    i++;
                }
            }
            insertNonFull(x -> children[i], k);
        }
    }
    void traverse_in_order(BTreeNode<T, ORDER>* x){ // InOrder traversal of b tree
        // cout << "no of keys in x : " << x -> n << endl;
        int i;
        for(i = 0; i < x -> n; i++){
            if(!x -> leaf){
                // cout << "Traversing to child " << x -> keys[i] << endl;
                traverse_in_order(x -> children[i]);
            }
            cout << x -> keys[i] << " ";
        }
        if(!x -> leaf){ // Traversing the last sub tree.
            traverse_in_order(x -> children[i]);
        }
        // cout << endl;
        // cout << "completed one level" << endl;
    }
    void traverse_start_end(int start, int end){
        // get the node that contains the start element and then linear traverse from there.
        BTreeNode<T, ORDER>* x = search(root, start);
        while(x){
            for(int i = 0; i < x -> n; i++){
                if(x -> keys[i] >= start && x -> keys[i] <= end){
                    cout << x -> keys[i] << " ";
                }
            }
            if(x -> children[ORDER - 1] != nullptr){
                x = x -> children[ORDER - 1];
            }
            else {
                break;
            }
        }
    }
    BTreeNode<T, ORDER>* search(BTreeNode<T, ORDER>* x, T k){
        int i = 0;
        while(i < x -> n && k > x -> keys[i]){
            i++;
        }
        // we are now at a point where keys[i] might be equal to or greater than k.
        // if equal, return current node. else if current node is leaf, return null else search in the possible node.
        if(i < x -> n && k == x -> keys[i]){
            return x;
        }
        if(x -> leaf){
            // cout << "Node not found, could exist in the returned node " << endl; 
            return x;
        }
        return search(x -> children[i], k);
    }

    T getPredecessor(BTreeNode<T, ORDER>* node, int ind){
        BTreeNode<T, ORDER>* currentChild = node -> children[ind]; // left child of the key present at ind.
        while(!currentChild -> leaf){
            currentChild = currentChild->children[currentChild -> n]; // go to the last element of the current child
        }
        return currentChild -> keys[currentChild -> n - 1];
    }

    T getSuccesor(BTreeNode<T, ORDER>* node, int ind){
        BTreeNode<T, ORDER>* currentChild = node -> children[ind + 1];
        while(!currentChild -> leaf){
            currentChild = currentChild -> children[0];
        }
        return currentChild -> keys[0];
    }

    // Siblings have an extra key => borrow one key from them.
    // Siblings don't have any extra key => merge with a sibling 
    void fill(BTreeNode<T, ORDER>* node, int ind){
        if(ind != 0 && (node -> children[ind - 1]->n) >= ORDER/2){
            borrowFromPrev(node, ind);
        }
        else if(ind != node -> n && (node -> children[ind + 1]->n) >= ORDER/2){
            borrowFromNext(node, ind);
        }
        else {// Merging always happens with the next node.
            if(ind != node -> n){
                merge(node, ind);
            }
            else {
                merge(node, ind - 1);
            }
        }
    }
    // Shift the current child's keys and children to the right side by one. Now push the parent to the child. Also, assign the last child of the sibling as the first child of the current child. Adjust the no of children.
    void borrowFromPrev(BTreeNode<T, ORDER>* node, int ind){
        BTreeNode<T, ORDER>* child = node -> children[ind];
        BTreeNode<T, ORDER>* sibling = node -> children[ind - 1];
        for(int i = child -> n - 1; i >= 0; i--){
            child -> keys[i + 1] = child -> keys[i];
        }
        if(!child -> leaf){
            for(int i = child -> n; i >= 0; i--){
                child -> children[i + 1] = child -> children[i];
            }
        }
        child -> keys[0] = node -> keys[ind - 1];
        if(!child -> leaf){
            child -> children[0] = sibling -> children[sibling -> n];
        }
        node -> keys[ind - 1] = sibling -> keys[sibling -> n - 1];
        if(child -> leaf){
            child -> keys[0] = node -> keys[ind - 1];
        }
        sibling -> n -= 1;
        child -> n += 1;
    }
    // Push the parent to the current child first and then Shift the current sibling's keys and children to the left side by one.
    void borrowFromNext(BTreeNode<T, ORDER>* node, int ind){
        BTreeNode<T, ORDER>* child = node -> children[ind];
        BTreeNode<T, ORDER>* sibling = node -> children[ind + 1];
        child -> keys[child -> n] = node -> keys[ind];
        if(!child -> leaf){
            child -> children[child -> n + 1] = sibling -> children[0];
        }
        node -> keys[ind] = sibling -> keys[0];
        for(int i = 1; i < sibling -> n; i++){
            sibling -> keys[i - 1] = sibling -> keys[i];
        }
        if(!sibling -> leaf){
            for(int i = 1; i <= sibling -> n; i++){
                sibling -> children[i - 1] = sibling -> children[i];
            }
        }
        if(child -> leaf){
            node -> keys[ind] = sibling -> keys[0];
        }
        sibling -> n -= 1;
        child -> n += 1;
    }
    // Merge the current child with it's next sibling
    // First transfer the parent's key to the current child and then Transfer the siblings keys and children to the current child and then shift the parent's keys and children by one to the left.
    void merge(BTreeNode<T, ORDER>* node, int ind){
        BTreeNode<T, ORDER>* child = node -> children[ind];
        BTreeNode<T, ORDER>* sibling = node -> children[ind + 1];

        int n = child -> n;
        int flag = 1;
        if(child -> leaf){
            flag = 0;
        }
        if(flag){
            child -> keys[n] = node -> keys[ind];
        }
        for(int i = 0; i < sibling -> n; i++){
            child -> keys[n + i + flag] = sibling -> keys[i];
        }
        if(!child -> leaf){
            for(int i = 0; i <= sibling -> n; i++){
                child -> children[n + i + flag] = sibling -> children[i];
            }
        }
        // For keys, i starts from ind + 1, meaning that, in the parent if we merge child 0 with child 1, key 1 will be shifted left and key 0 will be lost in the parent and it is already backed up by storing in the left child as shown above.
        for(int i = ind + 1; i < node -> n; i++){
            node -> keys[i - 1] = node -> keys[i];
        }
        // But in the case of children, we start at ind + 2. because if we merge child 0 with 1, we don't want to lose the child 0 as it holds the merged data of parent and child 1. We will instead lose child 1 as it is already backed
        for(int i = ind + 2; i <= node -> n; i++){
            node -> children[i - 1] = node -> children[i];
        }
        child -> n += sibling -> n + flag; // because we merged both children and we also pushed the parent key.
        if(child -> leaf){
            child -> children[ORDER - 1] = sibling -> children[ORDER - 1];
            // remove(root, node -> keys[ind]);
        }
        node -> n--;
        delete sibling;
    }

    void removeFromNonLeaf(BTreeNode<T, ORDER>* node, int ind){
        // T k = node -> keys[ind];
        // if(node -> children[ind]->n >= ORDER/2){ // We make this check as to ensure that the subtree can handle the deletion of the predecessor.
        //     T pred = getPredecessor(node, ind);
        //     node -> keys[ind] = pred;
        //     remove(node -> children[ind], pred);
        // }
        // else if(node -> children[ind + 1] -> n >= ORDER/2){
        //     T succ = getSuccesor(node, ind);
        //     node -> keys[ind] = succ;
        //     remove(node -> children[ind + 1], succ);
        // }
        // else {
        //     merge(node, ind);
        //     remove(node -> children[ind], k);
        // }
        T k = node -> keys[ind];
        if(node -> children[ind + 1] -> leaf && node -> children[ind + 1] -> n < ORDER/2){
            cout << "filling for a leaf node " << node -> keys[0] << endl;
            fill(node, ind + 1); 
            cout << "After filling the leaf nodes : " << endl;
            BFS();
        }
        // Now remove the key k from the leaf.
        remove(node -> children[ind + 1], k);
        // cout << "hai"  << k << endl;
        // BFS();
        // Replace the occurence of k with it's successor.
        BTreeNode<T, ORDER>* iNode = search(root, k);
        // cout << iNode -> keys[0] << endl;
        // In case k has gone to leaf node for merging purpose, remove k as a leaf node key.
        if(iNode -> leaf){
            remove(iNode, k);
            return;
        }
        int i = 0;
        while(i < iNode -> n && iNode -> keys[i] < k){
            i++;
        }
        // Even though we get the key from current node, balancing might happen due to filling. so we find the right node again.
        T succ = getSuccesor(iNode, i);
        iNode -> keys[i] = succ;
    }
    //Removing a key from a node.
    void removeFromLeaf(BTreeNode<T, ORDER>* node, int ind){
        for(int i = ind + 1; i < node -> n; i++){
            node -> keys[i - 1] = node -> keys[i];
        }
        node -> n--;
    }

    void remove(BTreeNode<T, ORDER>* node, int k){
        int ind = 0;
        while(ind < node -> n && k > node -> keys[ind]){ //Go to the node that might contain the key.
            ind++;
        }
        if(ind < node -> n && node -> keys[ind] == k){ // If key is present in current node, delete it.
            if(node -> leaf){
                removeFromLeaf(node, ind);
            }
            else {
                removeFromNonLeaf(node, ind);
            }
        }
        else {                                          // If key is not present in current node, go to that node. and before going, ensure the child node has enough no of children.
            if(node -> leaf){
                cout << "The key hasn't been found" << endl;
                return;
            }
            bool flag = false;
            if(node -> n == ind){
                flag = true;
            }
            if(node -> children[ind] -> n < ORDER/2){
                fill(node, ind);
            }
            if(flag && ind > node -> n){
                remove(node -> children[ind - 1], k);
            }
            else {
                remove(node -> children[ind], k);
            }
        }
    }
public:
    bool showEveryOpn = false;
    BTree() {
        root = new BTreeNode<T, ORDER> (true);
    }
    void insert(T k){
        // cout << "current children in root :  " << root -> keys[0] << endl;
        if(root -> n == ORDER - 1){
            BTreeNode<T, ORDER>* s = new BTreeNode<T, ORDER>(false);
            s -> children[0] = root;
            root = s;
            splitChild(s, 0);
            insertNonFull(s, k);
        }
        else {
            insertNonFull(root, k);
        }
        if(showEveryOpn){
            BFS();
        }
    }
    void in_order_traversal(){
        cout << "In Order Traversal : " << endl;
        // cout << "root " << root -> keys[0] << endl;
        if(root){
            traverse_in_order(root);
        }
        cout << endl;
    }
    void range_traversal(int start = INT_MIN, int end = INT_MAX){
        cout << "Range Traversal" << endl;
        if(root){
            traverse_start_end(start, end);
        }
        cout << endl;
    }
    void search(T k){
        BTreeNode<T, ORDER>* nodeFound = search(root, k);
        if(search(root, k)){
            cout << "We have found the value at " << nodeFound << endl;
            cout << "The no of elements is : " << nodeFound -> n << endl;
        }
        else {
            cout << "Couldn't find the value." << endl;
        }
    }
    void remove(T k){
        if(!root){
            cout << "Cannot remove from an empty tree" << endl;
            return;
        }
        remove(root, k);
        // What if root -> n == 0???
        if(root -> n == 0){
            BTreeNode<T, ORDER>* temp = root;
            if(root -> leaf){
                root = nullptr;
            }
            else {
                root = root -> children[0];
            }
            delete temp;
        }
        if(showEveryOpn){
            BFS();
        }
    }
    void BFS() {
        if(!root){
            cout << "Empty tree" << endl;
            return;
        }
        queue<BTreeNode<T, ORDER>*> que;
        que.push(root);
        while(!que.empty()){
            int noOfNodes = que.size();
            for(int i = 0; i < noOfNodes; i++){
                BTreeNode<T, ORDER>* tempNode = que.front();
                que.pop();
                int noOfKeys = tempNode -> n;
                for(int i = 0; i < noOfKeys; i++){
                    cout << tempNode -> keys[i] << " ";
                }
                for(int i = 0; i < noOfKeys + 1; i++){
                    if(!tempNode -> leaf && tempNode -> children[i]){
                        que.push(tempNode -> children[i]);
                    }
                }
                cout << "\t|\t";
            }
            cout << endl << endl;
        }
        for(int i = 0;i < 40; i++){
            cout << "-";
        }
        cout << endl;
    }
};