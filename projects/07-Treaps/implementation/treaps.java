import java.util.Random;

public class treaps {
    /*
    a treap is a binary search tree where each node also has a random priority.
    the tree is heap ordered by priority, while also maintaining the binary search tree property by key.
    insertions and deletions are done by splitting and merging the treap based on the priorities of the nodes.
    this allows for efficient insertions, deletions, and searches while keeping the tree balanced on average.
    */

    // we need a random number generator to assign priorities to the nodes
    private static final Random rand = new Random();

    // we simply define a node to represent the nodes in the treap
    static class Node {
        int key;
        int priority;
        Node left, right;

        Node(int key) {
            this.key = key;
            this.priority = rand.nextInt();
        }
    }

    private Node root;

    /*
    the split function takes a node and a key and will split the treap
    into two: one where all the elements are greater than the key we are at and one where
    the elements are less than or equal to the key we are at.
    the way it works is when we are at a node, if the key is less than or equal to the current nodes value,
    we split the nodes right subtree because there may be larger values that we need to split,
    if the key is greater than the current nodes value, we split the nodes left subtree because there may be smaller values that we need to split.
    so we will create two treaps both priority ordered, split by key, and we will return them as an array of nodes.
    */
    private Node[] split(Node root, int key) {
        if (root == null) {
            return new Node[]{null, null};
        }
        // less than or equal to the key, we split the right subtree
        if (root.key <= key) {
            Node[] res = split(root.right, key);
            // we want to set the root.right to be all the values in the right subtree that are less than or equal to the key we are at
            // this allows us to make sure none of the wrong values are in the wrong treap
            root.right = res[0];
            // we return the root as the left treap and res[1] as the right treap
            // so when we recurse back up, we will have the correct treaps with the correct priorities and keys
            return new Node[]{root, res[1]};
        } else {
            Node[] res = split(root.left, key);
            // similar logic to the above case, just simply reversed because we are splitting the left subtree instead of the right subtree

            root.left = res[1];

            return new Node[]{res[0], root};
        }
    }

    /*
    the point of the merge function is to take two treaps and merge them together
    in a manner that maintains the properties of the treap,
    so we will compare the priorities of the roots of the two treaps and merge the one with the higher priority as the new root,
    and then we will merge the other treap as a child of the new root,
    we will do this recursively until we have merged the two treaps together
    */
    private Node merge(Node left, Node right) {
        if (left == null) return right;
        if (right == null) return left;

        // the left treap has a higher priority than the right so we will call a merge on the left treaps right child
        // since that may have a higher priority than the right treap and we will return the left treap as the new root
        if (left.priority > right.priority) {
            left.right = merge(left.right, right);
            return left;
        // if the right priority is higher, choose the right root
        } else {
            right.left = merge(left, right.left);
            return right;
        }
    }

    /*
    this insert is straightforward, so if we hit the case in our recursion that the node were on's
    priority is less than the inserted node, we will split the current node by the inserted node's key and make the inserted node the new root of the subtree,
    and then we will set the left and right children of the new root to be the two treaps that we got from the split function,
    this allows us to insert our node while maintaining treap property
    */
    // we call insert separately because we need to create a new node

    public void insert(int key) {
        root = insert(root, new Node(key));
    }

    private Node insert(Node root, Node node) {
        if (root == null) return node;
        // check if node priority is > root priority,
        // if it is we call our split function and place node at current root
        if (node.priority > root.priority) {
            Node[] halves = split(root, node.key);

            node.left = halves[0];
            node.right = halves[1];

            return node;
        }
        // traverse the tree to maintain binary search tree order
        if (node.key < root.key) {
            root.left = insert(root.left, node);
        } else if (node.key > root.key) {
            root.right = insert(root.right, node);
        }
        return root;
    }

    /*
    the delete function is easy because all we do is merge
    the left and right subtrees of the node we want to delete,
    this merge will maintain the treap properties and we will return the new root of the merged treap,
    skipping over the node we want to delete, and then return all the way back up with the new treap
    */

    // again call it separately, same reason as insert
    public void delete(int key) {
        root = delete(root, key);
    }

    private Node delete(Node root, int key) {
        if (root == null) return null;

        // if found, remove node by merging its children
        if (root.key == key) {
            return merge(root.left, root.right);
        }

        // traverse to locate node, then refresh pointer
        if (key < root.key) {
            root.left = delete(root.left, key);
        } else {
            root.right = delete(root.right, key);
        }

        return root;
    }

    // search for key in treap (BST traversal)
    public boolean search(int key) {
        return search(root, key);
    }

    private boolean search(Node root, int key) {
        // normal search
        if (root == null) return false;
        if (root.key == key) return true;
        if (key < root.key) {
            return search(root.left, key);
        } else {
            return search(root.right, key);
        }
    }

    // inorder traversal for sorted output (by key)
    public void inorder() {
        inorder(root);
        System.out.println();
    }

    private void inorder(Node root) {
        if (root == null) return;
        inorder(root.left);
        System.out.print(root.key + " ");
        inorder(root.right);
    }

    // this function is just to help us test because 
    // our current inorder function only prints the values but we need a comparison
    private static String inorderString(treaps t) {
        return inorderString(t.root).trim();
    }

    private static String inorderString(Node root) {
        if (root == null) return "";
        String left = inorderString(root.left);
        String node = Integer.toString(root.key);
        String right = inorderString(root.right);

        String result = "";
        if (!left.isEmpty()) {
            result += left + " ";
        }; 
        result += node;
        if (!right.isEmpty()) result += " " + right;
        return result;
    }

    /*
    
    */
    public static void main(String[] args) {
        int passed = 0;
        int total = 4;

        // basic insert and search
        treaps treaps = new treaps();
        treaps.insert(5);
        treaps.insert(3);
        treaps.insert(7);
        treaps.insert(2);
        treaps.insert(4);
        boolean testinsert = treaps.search(5) && treaps.search(2) && !treaps.search(6);
        System.out.println("Test 1: insert/search is a" + (testinsert ? "PASS" : "FAIL"));
        if (testinsert) {
            passed++;
        }

        // make sure inorder is right
        // test inorder should be 2 3 4 5 7 because we inserted those values 
        String expected = "2 3 4 5 7";
        String actual = inorderString(treaps);
        boolean testinorder = expected.equals(actual);
        System.out.println("Test 2: inorder is a " + (testinorder ? "PASS" : "FAIL"));
        if (testinorder) {
            passed++;
        }


        // delete
        // test that we went from 2 3 4 5 7 to 2 4 5 7 and that search for the deleted node returns false
        treaps.delete(3);
        String expecteddel = "2 4 5 7";
        String actualdel = inorderString(treaps);
        boolean testdel = expecteddel.equals(actualdel) && !treaps.search(3);
        System.out.println("Test 3: delete is a " + (testdel ? "PASS" : "FAIL"));
        if (testdel) {
            passed++; 
        } 

        // Test Case 4: if we try to delete something that doesnt exist it wont change 
        treaps.delete(42);
        String expected4 = "2 4 5 7";
        String actual4 = inorderString(treaps);
        boolean test4 = expected4.equals(actual4);
        System.out.println("Test 4: delete something that doesnt exist  " + (test4 ? "PASS" : "FAIL") + " (got: " + actual4 + ")");
        if (test4) {
            passed++;
        }
        // test that all of our cases passed and return to user!
        System.out.println("\nwhen running my treaps you got: " + passed + " / " + total + " tests passed.");
        if (passed != total) {
            System.exit(1);
        }
    }
}
