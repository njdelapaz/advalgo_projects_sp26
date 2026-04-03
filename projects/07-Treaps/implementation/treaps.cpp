#include <iostream>
#include <vector>
#include <random>
#include <limits>
using namespace std;

/*
    Treap = Tree + Heap

    A treap is a randomized binary search tree.

    It maintains TWO properties at the same time:

    1. BST property by key:
       - all keys in the left subtree are smaller
       - all keys in the right subtree are larger

    2. Heap property by priority (max-heap here):
       - each node's priority is >= its children's priorities

    The key determines where a node belongs in sorted order.
    The priority determines how high the node rises in the tree.

    Random priorities make the tree balanced in expectation,
    so insert/search/erase are expected O(log n).
*/

class Treap {
private:
    /*
        Each node stores:
        - key: the BST ordering value
        - priority: the heap ordering value
        - left/right child pointers
    */
    struct Node {
        int key;
        int priority;
        Node* left;
        Node* right;

        Node(int k, int p) : key(k), priority(p), left(nullptr), right(nullptr) {}
    };

    Node* root;

    /*
        Random number generator for priorities.

        We use a large integer range so equal priorities are unlikely.
    */
    mt19937 rng;
    uniform_int_distribution<int> dist;

private:
    /*
        split(node, key)

        Splits a treap into two treaps:
        - left treap contains all keys <= key
        - right treap contains all keys > key

        This function is one of the most important treap tools!

        Why it works:
        - Because the input is already a valid treap,
          we only need to recursively split one subtree at a time.
        - The BST property tells us which side keys belong to.
        - The heap property is preserved automatically by reconnecting subtrees carefully.

        Returns:
            pair<left_treap_root, right_treap_root>
    */
    pair<Node*, Node*> split(Node* node, int key) {
        if (node == nullptr) {
            return {nullptr, nullptr};
        }

        if (node->key <= key) {
            /*
                Current node belongs in the LEFT result.

                But some nodes in node->right might still be <= key,
                so we recursively split the right subtree.
            */
            auto [middle, rightPart] = split(node->right, key);
            node->right = middle;
            return {node, rightPart};
        } else {
            /*
                Current node belongs in the RIGHT result.

                But some nodes in node->left might still be <= key,
                so we recursively split the left subtree.
            */
            auto [leftPart, middle] = split(node->left, key);
            node->left = middle;
            return {leftPart, node};
        }
    }

    /*
        merge(left, right): Merges two treaps into one treap.

        Note: Every key in 'left' must be <= every key in 'right'.

        If that is true, then we can merge based on priority:
        - whichever root has higher priority becomes the new root
        - recursively merge the remaining subtree

        Why it works:
        - BST property holds because all left keys stay before all right keys
        - heap property holds because higher priority root stays on top
    */
    Node* merge(Node* left, Node* right) {
        if (left == nullptr) return right;
        if (right == nullptr) return left;

        if (left->priority > right->priority) {
            /*
                Left root has higher priority, so it becomes the new root.
                Its right child should be the merge of:
                - left's original right subtree
                - the entire right treap
            */
            left->right = merge(left->right, right);
            return left;
        } else {
            /*
                Right root has higher priority, so it becomes the new root.
                Its left child should be the merge of:
                - the entire left treap
                - right's original left subtree
            */
            right->left = merge(left, right->left);
            return right;
        }
    }

    /*
        search(node, key)

        Standard BST lookup:
        - if key is smaller, go left
        - if key is larger, go right
        - if equal, found it

        Priority does NOT matter for searching.
        Priority only affects shape, not lookup direction.
    */
    bool search(Node* node, int key) const {
        if (node == nullptr) return false;

        if (key == node->key) return true;
        if (key < node->key) return search(node->left, key);
        return search(node->right, key);
    }

    /*
        erase(node, key)

        Deletes one key from the treap.

        We first search like in a BST.
        Once we find the node:
        - remove it
        - replace it with merge(left_subtree, right_subtree)

        This works because:
        - every key in left subtree < node->key
        - every key in right subtree > node->key
        So merging those two subtrees preserves BST order.
    */
    Node* erase(Node* node, int key) {
        if (node == nullptr) return nullptr;

        if (key < node->key) {
            node->left = erase(node->left, key);
        } else if (key > node->key) {
            node->right = erase(node->right, key);
        } else {
            /*
                Found the node to delete.

                Merge its two children to reconnect the tree.
            */
            Node* merged = merge(node->left, node->right);
            delete node;
            return merged;
        }

        return node;
    }

    /*
        unite(a, b): Combines two arbitrary treaps into one.

        Correct idea:
        - choose the root with higher priority
        - split the other treap by that root's key
        - recursively unite matching left parts and right parts

        This preserves:
        - BST property
        - heap property
    */
    Node* unite(Node* a, Node* b) {
        if (a == nullptr) return b;
        if (b == nullptr) return a;

        /*
            Ensure 'a' has the higher-priority root.
        */
        if (a->priority < b->priority) {
            swap(a, b);
        }

        /*
            Split b around a->key:
            - leftB contains keys <= a->key
            - rightB contains keys > a->key

            Since this implementation uses UNIQUE keys only,
            we want:
            - strictly smaller keys on the left
            - strictly larger keys on the right

            Because split() puts <= key on the left,
            duplicates (if any) would land in leftB.
            But our public insert() prevents duplicates,
            so this is okay.
        */
        auto [leftB, rightB] = split(b, a->key);

        a->left = unite(a->left, leftB);
        a->right = unite(a->right, rightB);

        return a;
    }

    /*
        inorder(node, result)

        Standard inorder traversal:
        left -> current -> right

        In a BST, inorder traversal gives keys in sorted order.
    */
    void inorder(Node* node, vector<int>& result) const {
        if (node == nullptr) return;
        inorder(node->left, result);
        result.push_back(node->key);
        inorder(node->right, result);
    }

    /*
        validate(node, low, high)

        Checks both treap invariants:

        1. BST property:
           low < node->key < high

        2. Heap property:
           child's priority must not exceed parent's priority

        This is useful for debugging and testing.
    */
    bool validate(Node* node, long long low, long long high) const {
        if (node == nullptr) return true;

        if (!(low < node->key && node->key < high)) {
            return false;
        }

        if (node->left != nullptr && node->left->priority > node->priority) {
            return false;
        }
        if (node->right != nullptr && node->right->priority > node->priority) {
            return false;
        }

        return validate(node->left, low, node->key) &&
               validate(node->right, node->key, high);
    }

    /*
        clear(node)

        Recursively deletes all nodes to avoid memory leaks.
    */
    void clear(Node* node) {
        if (node == nullptr) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    /*
        Constructor:
        - empty treap
        - initialize RNG
    */
    Treap() : root(nullptr), rng(random_device{}()), dist(1, 1000000000) {}

    /*
        Destructor:
        frees all allocated nodes
    */
    ~Treap() {
        clear(root);
    }

    /*
        Public insert(key)

        This version enforces UNIQUE keys.
        If key already exists, do nothing.

        Algorithm:
        1. split the treap by key
        2. create new node
        3. merge left + new node
        4. merge result + right
    */
    void insert(int key) {
        if (search(key)) {
            return;  // prevent duplicates
        }

        int priority = dist(rng);
        Node* newNode = new Node(key, priority);

        auto [leftPart, rightPart] = split(root, key);
        Node* temp = merge(leftPart, newNode);
        root = merge(temp, rightPart);
    }

    /*
        Public erase(key)
    */
    void erase(int key) {
        root = erase(root, key);
    }

    /*
        Public search(key)
    */
    bool search(int key) const {
        return search(root, key);
    }

    /*
        Public unite(other)

        Combines 'other' into this treap.

        After union, we set other's root to nullptr so that
        its destructor does not delete nodes now owned by this treap.
    */
    void unite(Treap& other) {
        root = unite(root, other.root);
        other.root = nullptr;
    }

    /*
        Public inorder()

        Returns all keys in sorted order.
    */
    vector<int> inorder() const {
        vector<int> result;
        inorder(root, result);
        return result;
    }

    /*
        Public validate()

        Returns true if both BST and heap properties hold.
    */
    bool validate() const {
        return validate(root,
                        numeric_limits<long long>::lowest(),
                        numeric_limits<long long>::max());
    }
};

/*
    Helper function to print a vector nicely.
*/
void printVector(const vector<int>& v) {
    for (int x : v) {
        cout << x << " ";
    }
    cout << "\n";
}

/*
    Example usage / basic test
*/
int main() {
    int passed = 0;
    int total = 4;

    // Test 1
    Treap t1;
    t1.insert(5);
    t1.insert(3);
    t1.insert(7);
    t1.insert(2);
    t1.insert(4);

    bool test1 = t1.search(5) && t1.search(2) && !t1.search(6) && t1.validate();
    cout << "Test 1: basic insert/search "
         << (test1 ? "PASS" : "FAIL") << "\n";
    if (test1) passed++;

    // Test 2
    vector<int> expected2 = {2, 3, 4, 5, 7};
    vector<int> actual2 = t1.inorder();

    bool test2 = (actual2 == expected2);
    cout << "Test 2: inorder traversal "
         << (test2 ? "PASS" : "FAIL") << "\n";

    if (!test2) {
        cout << "Expected: ";
        printVector(expected2);
        cout << "Got:      ";
        printVector(actual2);
    }
    if (test2) passed++;

    // Test 3
    t1.erase(3);
    t1.erase(42);

    vector<int> expected3 = {2, 4, 5, 7};
    vector<int> actual3 = t1.inorder();

    bool test3 = (actual3 == expected3) && !t1.search(3) && t1.validate();
    cout << "Test 3: erase "
         << (test3 ? "PASS" : "FAIL") << "\n";

    if (!test3) {
        cout << "Expected: ";
        printVector(expected3);
        cout << "Got:      ";
        printVector(actual3);
    }
    if (test3) passed++;

    // Test 4
    Treap a;
    Treap b;

    a.insert(10);
    a.insert(20);
    a.insert(30);

    b.insert(5);
    b.insert(15);
    b.insert(25);

    a.unite(b);

    vector<int> expected4 = {5, 10, 15, 20, 25, 30};
    vector<int> actual4 = a.inorder();

    bool test4 = (actual4 == expected4) && a.validate();
    cout << "Test 4: unite "
         << (test4 ? "PASS" : "FAIL") << "\n";

    if (!test4) {
        cout << "Expected: ";
        printVector(expected4);
        cout << "Got:      ";
        printVector(actual4);
    }
    if (test4) passed++;

    cout << "\nFinal: " << passed << " / " << total << " tests passed.\n";
    return (passed == total) ? 0 : 1;
}