import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;
import java.util.BitSet;

public class BloomFilter<T> {
    private final BitSet bits; // stores the bits
    private final int size; // length of the BitSet
    private final int k; // number of rounds of hashing

    // constructor with bounds for validity
    public BloomFilter(int size, int k) {
        if (size > 0) {
            this.size = size;
        } else {
            this.size = 1;
        }

        if (k > 0) {
            this.k = k;
        } else {
            this.k = 1;
        }

        this.bits = new BitSet(this.size);
    }


    // common method of hashing in Bloom Filter is to just simulate k hashes by running this function k times with different i values
    // Instead of using k independent hash functions, we derive them from:
    // h_i(x) = h1(x) + i * h2(x)
    // standard for Bloom filters and avoids the cost of multiple hashes
    private int hash(T item, int i) {
        int h1 = item.hashCode();
        int h2 = Integer.rotateRight(h1, 16); // mix bits for second hash

        long combined = (long) h1 + (long) i * h2;

        // floorMod ensures a non-negative index even if hash is negative
        return (int) Math.floorMod(combined, size);
    }


    // inserts an item into the filter.
    // Sets k different bit positions determined by the hash function.
    public void insert(T item) {
        for (int i = 0; i < k; i++) {
            int index = hash(item, i); // find the hash then set the index accordingly
            bits.set(index);
        }
    }


    // check if an item is either possibly present or definitely not present
    // you can have false positives if other elements coincidentally set all bits for another element
    // but never false negatives
    public boolean query(T item) {
        for (int i = 0; i < k; i++) {
            int index = hash(item, i); // find the hash
            if (!bits.get(index)) {
                return false; // definitely not present
            }
        }
        return true; // possibly present
    }

}
