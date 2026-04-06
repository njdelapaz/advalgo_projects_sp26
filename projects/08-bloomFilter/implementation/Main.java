import java.io.File;
import java.io.FileNotFoundException;
import java.util.HashSet;
import java.util.Locale;
import java.util.Scanner;
import java.util.Set;

public class Main {

    public static void main(String[] args) throws FileNotFoundException {
        Scanner sc;

        if (args.length > 0) {
            sc = new Scanner(new File(args[0]));
        } else {
            sc = new Scanner(System.in);
        }

        int testCases = Integer.parseInt(sc.nextLine().trim());

        for (int t = 0; t < testCases; t++) {
            String line = sc.nextLine().trim();
            while (line.isEmpty()) {
                line = sc.nextLine().trim();
            }

            String[] parts = line.split("\\s+");
            int n = Integer.parseInt(parts[0]); // expected distinct elements for sizing
            double p = Double.parseDouble(parts[1]); // target false positive rate
            int insertCount = Integer.parseInt(parts[2]);
            int q = Integer.parseInt(parts[3]);

            int m = computeM(n, p);
            int k = computeK(n, m);

            BloomFilter<String> bf = new BloomFilter<>(m, k);
            Set<String> inserted = new HashSet<>();

            for (int i = 0; i < insertCount; i++) {
                String word = sc.nextLine().trim();
                bf.insert(word);
                inserted.add(word);
            }

            String separator = sc.nextLine().trim(); // "---"

            System.out.println("m=" + m + " k=" + k + " n=" + n + " p=" +
                    String.format(Locale.US, "%.2f", p));

            for (int i = 0; i < q; i++) {
                String query = sc.nextLine().trim();
                String result;

                if (inserted.contains(query)) {
                    result = "member";
                } else if (bf.query(query)) {
                    result = "false_positive";
                } else {
                    result = "absent";
                }

                System.out.println(query + "\t" + result);
            }

            if (t < testCases - 1) {
                System.out.println();
            }
        }

        sc.close();
    }

    // m = floor( -(n * ln(p)) / (ln(2)^2) )
    private static int computeM(int n, double p) {
        if (n <= 0 || p <= 0 || p >= 1) {
            return 1;
        }

        double value = -(n * Math.log(p)) / (Math.pow(Math.log(2), 2));
        return Math.max(1, (int) value);
    }

    // k = round((m / n) * ln(2))
    private static int computeK(int n, int m) {
        if (n <= 0) {
            return 1;
        }

        int k = (int) ((m / (double) n) * Math.log(2));
        return Math.max(1, k);
    }
}