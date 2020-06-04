#include <stdio.h>
void encrypt(unsigned int* v, unsigned int* key) {
	unsigned int l = v[0], r = v[1], sum = 0, delta = 0x9e3779b9;
	for (size_t i = 0; i < 32; i++) {
		sum += delta;
		l += ((r << 4) + key[0]) ^ (r + sum) ^ ((r >> 5) + key[1]);
		r += ((l << 4) + key[2]) ^ (l + sum) ^ ((l >> 5) + key[3]);
	}
	v[0] = l;
	v[1] = r;
}

void decrypt(unsigned int* v, unsigned int* key) {
	unsigned int l = v[0], r = v[1], sum = 0, delta = 0x9e3779b9;
	sum = delta * 32;
	for (size_t i = 0; i < 32; i++) {
		r -= ((l << 4) + key[2]) ^ (l + sum) ^ ((l >> 5) + key[3]);
		l -= ((r << 4) + key[0]) ^ (r + sum) ^ ((r >> 5) + key[1]);
		sum -= delta;
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