#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;


class Manacher {
    public:
        string s;
        vector<int> palindromes;

        
        // prepare the string for manachers
        // add @ to beginning and $ to end
        // add # before all chars and after last char. 2n+1 = odd, so all strings are odd length
        // create arr of size s. Stores the radius of palindrome from that center 
        void init(string inS) {
            s = "@";
            for(char c: inS){
                s += "#";
                s += c;
            }
            s+= "#$";

            palindromes.assign(s.length(), 0);

        }

        
        // run manachers, return longest pallindromic substring, O(n) time
        string runManacher(){
            
            // l and r store the bounds of the right-most extending pallindrome found so far
            int l=0;
            int r=0;

            int n = s.length();
            int longest = -1;

            for(int i=1;i<n-1;i++){

                // mirror of i around center
                int mirror = l+r-i;

                // if i is within the bounds the right-most extending palindrome
                // then initialize palindrome length to min of the mirror and r-i.
                // the min of mirror(you know the mirror is the same) or r-i(goes to the edge of the bound)
                if (i < r){
                    palindromes[i] = min(r - i, palindromes[mirror]);
                }

                // while the pallindrome centered at i with radius, palindromes[i], is valid, increment the radius
                while(s[i+1+palindromes[i]] == s[i-1-palindromes[i]]){
                    palindromes[i] += 1;
                }
                
                // if the right edge of the current pallindrome goes further than the previous right-most edge of any previous pallindrome
                // then update the bounds of the right-most extending pallindrome
                if(i+palindromes[i] > r){
                    l = i - palindromes[i];
                    r = i + palindromes[i];
                }

                // if the current pallindrome is longer than the previous longest pallindrome, then update
                if(longest == -1 || palindromes[i] > palindromes[longest]){
                    longest = i;
                }
            }
            
            // the longest pallindrome center is at "longest" 
            // so it begins at longest - (radius of longest) and it goes until longest + (radius of longest)
            // Then, clean up, remove filler chars
            string toRet = "";
            for(int j=longest - palindromes[longest]; j<longest+palindromes[longest]+1;j++){
                if(s[j] != '#' && s[j] != '@' && s[j] != '$'){
                    toRet+=s[j];
                }
            }
            return toRet;
        }
};

// take string from input return longest pallindromic substring
int main() {
    Manacher m;
    string x;
    cout << "Type a string: ";
    cin >> x;
    m.init(x);
    cout << m.runManacher()<< endl;
    return 0;
}
