#include <algorithm>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

string burrowsWheelerTransform(string s){
    s += '$';

    int n = s.size();
    const int alphabet = 256;
    vector<int> p(n), pn(n), c(n), cn(n), cnt(alphabet, 0);
//count each character
    for (int i = 0; i <n; i++){
        cnt[(unsigned char)s[i]] += 1;
    }
//prefix sum the counts
    for (int i=1; i<alphabet; i++){
        cnt[i] += cnt[i-1];
    }
//assign indexes for p (ordering of indices in alphabetical order) based on prefix sums of counts
    for (int i=n-1; i>=0; i--){
        p[--cnt[(unsigned char)s[i]]] = i;
    }
//assign ranks in c based on equivalence classes
    int cat = 0;
    c[p[0]] = cat;
    for (int i=1; i<n; i++){
        if (s[p[i]] != s[p[i-1]]){
            cat++;
        }
        c[p[i]] = cat;
    }

    cnt.resize(n);
    for (int k = 0; (1<<k) < n; k++){
        for (int i=0; i<n; i++){
            pn[i] = p[i]-(1<<k);
            if (pn[i] < 0){
                pn[i] += n;
            }
        }

        fill(cnt.begin(), cnt.begin() + cat + 1, 0);
        for (int i = 0; i <n; i++){
            cnt[c[pn[i]]] += 1;
        }
        for (int i=1; i<cat+1; i++){
            cnt[i] += cnt[i-1];
        }
        for (int i=n-1; i>=0; i--){
            p[--cnt[c[pn[i]]]] = pn[i];
        }
        cn[p[0]] = 0;
        cat = 0;
        for (int i = 1; i <n; i++){
            if (c[p[i]] != c[p[i-1]] || c[(p[i]+(1<<k))%n] != c[(p[i-1]+(1<<k))%n]){
                cat++;
            }
            cn[p[i]] = cat;
        }
        c.swap(cn);


    }
    string result;
    for (int i = 0; i <n; i++){
        result += s[(p[i] + n -1)%n];
    }
    return result;
}




int main(){
    string s;
    cin >> s;
    cout << burrowsWheelerTransform(s);
}