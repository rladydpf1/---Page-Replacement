#include <stdio.h>
#include <stdlib.h>

void FIFO(char *string);
void LRU(char *string);
void LFU(char *string);
void Optimal(char *string);
void Second_Chance(char *string);
void PrintFrame(int k);
void Start(char *string);

int fault, f, r;
int *reference, *frame;

int main(int argc, char* argv[]) {
	int algorithm = atoi(argv[1]);
	f = atoi(argv[2]);
	r = argc - 3;
	reference = (int *)malloc(sizeof(int)*r);
	frame = (int *)malloc(sizeof(int)*f);
	int i;
	for (i = 0; i < r; i++) {
		reference[i] = atoi(argv[i + 3]);
	}
	for (i = 0; i < f; i++) {
		frame[i] = -1;
	}
	switch (algorithm) {
	case 0:
		FIFO("FIFO");
		break;
	case 1:
		LRU("LRU");
		break;
	case 2:
		LFU("LFU");
		break;
	case 3:
		Optimal("Optimal");
		break;
	case 4:
		Second_Chance("Second Chance");
	}

	printf("Page Fault = %d\n", fault);
	printf("Terminated \n");
	return 0;
}

void FIFO(char *string) {
	Start(string);
	int i, j, key = 0, counter = 0, victim = 0;
	for (i = 0; i < r; i++) {
		key = 0;
		for (j = 0; j < f; j++) {
			if (frame[j] == reference[i]) {
				key = 1;
				break;
			}
		}
		if (!key) {
			fault++;
			if (counter < f) {
				frame[counter++] = reference[i];
			}
			else {
				frame[victim] = reference[i];
				victim = (victim + 1) % f;
			}
		}
		PrintFrame(i);
	}
}

void LRU(char *string) {
	Start(string);
	int i, j, k, key = 0, counter = 0, victim = 0;
	int top = -1;
	int *pointer, *valid;
	pointer = (int *)malloc(sizeof(int)*r);
	valid = (int *)calloc(r, sizeof(int));
	for (i = 0; i < r; i++) {
		top++;
		key = 0;
		for (j = 0; j < f; j++) {
			if (frame[j] == reference[i]) {
				for (k = top - 1; k >= 0; k--) {
					if (reference[k] == frame[j] && !valid[k]) {
						pointer[top] = pointer[k];
						valid[k] = 1;
						break;
					}
				}
				key = 1;
				break;
			}
		}
		if (!key) {
			fault++;
			if (counter < f) {
				pointer[top] = counter;
				frame[counter++] = reference[i];
			}
			else {
				for (j = 0; j < top; j++) {
					if (!valid[j]) {
						victim = pointer[j];
						valid[j] = 1;
						break;
					}
				}
				pointer[top] = victim;
				frame[victim] = reference[i];
			}
		}
		PrintFrame(i);
	}
}

void LFU(char *string) {
	Start(string);
	int i, j, key = 0, counter = 0, victim = 0;
	int *frequent, *fifo, pointer = 0;
	frequent = (int *)calloc(f, sizeof(int));
	fifo = (int *)calloc(f, sizeof(int));
	for (i = 0; i < r; i++) {
		int least = 999;
		key = 0;
		for (j = 0; j < f; j++) {
			if (frame[j] == reference[i]) {
				key = 1;
				frequent[j]++;
				break;
			}
		}
		if (!key) {
			fault++;
			if (counter < f) {
				frequent[counter]++;
				fifo[counter] = pointer++;
				frame[counter++] = reference[i];
			}
			else {
				for (j = 0; j < f; j++) {
					if (least > frequent[j]) {
						least = frequent[j];
						victim = j;
					}
					else if (least == frequent[j] && fifo[victim] >= fifo[j]) {
						victim = j;
					}
				}
				fifo[victim] = pointer++;
				frequent[victim] = 1;
				frame[victim] = reference[i];
			}

		}
		PrintFrame(i);
	}
}

void Optimal(char *string) {
	Start(string);
	int i, j, k, key = 0, counter = 0, victim = 0;
	int *future;
	future = (int *)calloc(f, sizeof(int));
	for (i = 0; i < r; i++) {
		int max = 0, pointer = 0;
		key = 0;
		for (j = 0; j < f; j++) {
			if (frame[j] == reference[i]) {
				key = 1;
				break;
			}
		}
		if (!key) {
			fault++;
			if (counter < f) {
				frame[counter++] = reference[i];
			}
			else {
				for (j = 0; j < f; j++) {
					if (max < future[j]) {
						max = future[j];
						pointer = j;
					}
				}
				victim = pointer;
				frame[victim] = reference[i];
			}
		}
		for (j = 0; j < counter; j++) {
			for (k = i + 1; k < r; k++) {
				if (frame[j] == reference[k]) {
					future[j] = k;
					break;
				}
			}
			if (k == r) future[j] = r;
			if (f == j) break;
		}
		PrintFrame(i);
	}
}

void Second_Chance(char *string) {
	Start(string);
	int i, j, key = 0, counter = 0, victim = 0;
	int *chance;
	chance = (int *)calloc(f, sizeof(int));
	for (i = 0; i < r; i++) {
		key = 0;
		for (j = 0; j < f; j++) {
			if (frame[j] == reference[i]) {
				chance[j] = 1;
				key = 1;
				break;
			}
		}
		if (!key) {
			fault++;
			if (counter < f) {
				frame[counter++] = reference[i];
			}
			else {
				while (1) {
					if (chance[victim] == 1) {
						chance[victim] = 0;
						victim = (victim + 1) % f;
					}
					else break;
				}
				frame[victim] = reference[i];
				victim = (victim + 1) % f;
			}
		}
		PrintFrame(i);
	}
}

void PrintFrame(int k) {
	printf("Page %d > ", reference[k]);
	int i;
	printf("|");
	for (i = 0; i < f; i++) {
		if (frame[i] == -1) printf(" F |");
		else printf(" %d |", frame[i]);
	}
	printf("\n");
}

void Start(char *string) {
	int i;
	printf("%s, Reference string = ", string);
	for (i = 0; i < r; i++) printf("%d ", reference[i]);
	printf("\n");
}
