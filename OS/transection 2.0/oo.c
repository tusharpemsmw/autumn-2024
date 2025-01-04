#include <stdio.h>
#include <stdbool.h>

bool isSafeState(int P, int R, int Allocation[P][R], int Max[P][R], int Available[R]) {
    int Work[R];
    bool Finish[P];
    int SafeSequence[P];

    for (int i = 0; i < R; i++)
        Work[i] = Available[i];
    for (int i = 0; i < P; i++)
        Finish[i] = false;

    int count = 0;
    while (count < P) {
        bool found = false;
        for (int i = 0; i < P; i++) {
            if (!Finish[i]) {
                bool canAllocate = true;
                for (int j = 0; j < R; j++) {
                    if (Max[i][j] - Allocation[i][j] > Work[j]) {
                        canAllocate = false;
                        break;
                    }
                }
                if (canAllocate) {
                    for (int j = 0; j < R; j++)
                        Work[j] += Allocation[i][j];
                    SafeSequence[count++] = i;
                    Finish[i] = true;
                    found = true;
                }
            }
        }
        if (!found) return false;
    }

    printf("System is in a safe state. Safe sequence is: ");
    for (int i = 0; i < P; i++)
        printf("P%d%s", SafeSequence[i], i == P - 1 ? "\n" : " -> ");
    return true;
}

int main() {
    int P, R;
    printf("Enter the number of processes (P): ");
    scanf("%d", &P);
    printf("Enter the number of resources (R): ");
    scanf("%d", &R);

    int Allocation[P][R];
    int Max[P][R];
    int Available[R];

    printf("Enter Allocation matrix (%dx%d):\n", P, R);
    for (int i = 0; i < P; i++)
        for (int j = 0; j < R; j++)
            scanf("%d", &Allocation[i][j]);

    printf("Enter Max matrix (%dx%d):\n", P, R);
    for (int i = 0; i < P; i++)
        for (int j = 0; j < R; j++)
            scanf("%d", &Max[i][j]);

    printf("Enter Available vector (size %d):\n", R);
    for (int j = 0; j < R; j++)
        scanf("%d", &Available[j]);

    if (!isSafeState(P, R, Allocation, Max, Available))
        printf("System is not in a safe state.\n");

    return 0;
}
