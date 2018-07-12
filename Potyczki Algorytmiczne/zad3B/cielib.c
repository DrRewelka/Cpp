#ifndef CIELIB_C
#define CIELIB_C

#include <stdio.h>
#include <stdlib.h>

int d_v, k_v, r_v;

void init(void);

int podajD(void) {
	init();

	return d_v;
}

int podajK(void) {
	init();

	return k_v;
}

int podajR(void) {
	init();

	return r_v;
}

int pos[500];

void init(void) {
	static int initialized = -1;
	if(initialized != -1) {
		return;
	}
	initialized = 1;

	printf("Podaj d: ");
	scanf("%d", &d_v);
	printf("Podaj k: ");
	scanf("%d", &k_v);
	printf("Podaj r: ");
	scanf("%d", &r_v);

	printf("Podaj pozycje Krotki:\n");
	for(int i = 0; i < podajD(); ++i) {
		scanf("%d", &pos[i]);
	}
}

int call = 0;

int czyCieplo(int answer[]) {
	init();

	call += 1;
	if(call > podajK()) {
		printf("Przekroczono limit k = %d.", k_v);
		exit(1);
	}

	static int ldiff = -1;
	int diff = 0;

	for(int d, i = 0; i < podajD(); ++i) {
		d = pos[i] - answer[i];
		d = (d<0) ? -d : d;

		diff = (d > diff) ? d : diff;
	}

	const int result = (diff < ldiff) ? 1 : 0;
	ldiff = diff;

	return result;
}

void znalazlem(int answer[]) {
	init();

	printf("Podano:\n");
	for(int i = 0; i < podajD(); ++i) {
		printf("%d%c", pos[i], (i + 1 == podajD()) ? '\n' : ' ');
	}

	int k = 1;
	for(int i = 0; i < podajD(); ++i) {
		if(answer[i] != pos[i]) {
			printf("NIE %d na %d!\n", answer[i], i);
			k = -1;
		}
	}

	if(k == 1) {
		printf("OK --- %d zapytan\n", call);
	}

	exit(0);
}

#endif
