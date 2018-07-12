#include "cielib.h"

int main() {
	int d = podajD();
	int t[d];
	for(int i = 0; i < d; ++i) {
		t[i] = i;
	}

	while(czyCieplo(t)) {
		for(int i = 0; i < d; ++i) {
			t[i] += 1;
		}
	}

	znalazlem(t);
}
