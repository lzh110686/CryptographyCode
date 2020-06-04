#include <stdbool.h>
#include <stdio.h>
#define MX \
  ((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (k[p & 3 ^ e] ^ z))
bool btea(unsigned int* v, int n, unsigned int* k) {
  unsigned int z = v[n - 1], y = v[0], sum = 0, e, DELTA = 0x9e3779b9;
  unsigned int p, q;
  if (n > 1) { /* Coding Part */
    q = 6 + 52 / n;
    while (q-- > 0) {
      sum += DELTA;
      e = (sum >> 2) & 3;
      for (p = 0; p < n - 1; p++)
        y = v[p + 1], z = v[p] += MX;
      y = v[0];
      z = v[n - 1] += MX;
    }
    return 0;
  } else if (n < -1) { /* Decoding Part */
    n = -n;
    q = 6 + 52 / n;
    sum = q * DELTA;
    while (sum != 0) {
      e = (sum >> 2) & 3;
      for (p = n - 1; p > 0; p--)
        z = v[p - 1], y = v[p] -= MX;
      z = v[n - 1];
      y = v[0] -= MX;
      sum -= DELTA;
    }
    return 0;
  }
  return 1;
}
 
int main(int argc, char const* argv[]) {
  // test
  unsigned int v[2] = {1, 2}, key[4] = {1, 2, 3, 4};
  printf("%u,%u\n", v[0], v[1]);
  btea(v, 2, key);
  printf("%u,%u\n", v[0], v[1]);
  btea(v, -2, key);
  printf("%u,%u\n", v[0], v[1]);
  return 0;
}