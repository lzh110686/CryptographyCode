#include <stdio.h>
void encrypt(unsigned int* v, unsigned int* key) {
	unsigned int l = v[0], r = v[1], sum = 0, delta = 0x9e3779b9;
	for (size_t i = 0; i < 32; i++) {
		l += (((r << 4) ^ (r >> 5)) + r) ^ (sum + key[sum & 3]);
		sum += delta;
		r += (((l << 4) ^ (l >> 5)) + l) ^ (sum + key[(sum >> 11) & 3]);
	}
	v[0] = l;
	v[1] = r;
}

void decrypt(unsigned int* v, unsigned int* key) {
	unsigned int l = v[0], r = v[1], sum = 0, delta = 0x9e3779b9;
	sum = delta * 32;
	for (size_t i = 0; i < 32; i++) {
		r -= (((l << 4) ^ (l >> 5)) + l) ^ (sum + key[(sum >> 11) & 3]);
		sum -= delta;
		l -= (((r << 4) ^ (r >> 5)) + r) ^ (sum + key[sum & 3]);
	}
	v[0] = l;
	v[1] = r;
}

int main(int argc, char const *argv[])
{
	//test
	unsigned int v[2] = { 1,2 }, key[4] = { 1,2,3,4 };
	printf("%u,%u\n", v[0], v[1]);
	encrypt(v, key);
	printf("%u,%u\n", v[0], v[1]);
	decrypt(v, key);
	printf("%u,%u\n", v[0], v[1]);
	return 0;
}
