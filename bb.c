#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX(a, b) ((a > b)? (a): (b))

typedef struct _task_t {
    int id;
    int length;
    int due;
    int weight;
} task_t;

int read_tasks(task_t **tasks_ptr) {
    task_t *tasks, t;
    int i;

    tasks = NULL;
    for (i = 1; scanf("%d %d %d", &t.length, &t.due, &t.weight) != EOF; i++) {
        tasks = realloc(tasks, i * sizeof(task_t));
        t.id = i;
        tasks[i-1] = t;
    }

    *tasks_ptr = tasks;
    return i - 1;
}

void print_tasks(int n, task_t *tasks, task_t *best) {
    int i;
    task_t t;

    printf(" Nr | p_j | d_j | w_j \n");
    printf("----+-----+-----+-----\n");
    for (i = 0; i < n; i++) {
        t = tasks[i];
        printf("%2d. |%4d |%4d |%4d \n", t.id, t.length, t.due, t.weight);
    }

    printf("\nBest solution is: ");
    for (i = 0; i < n; i++) printf("%d ", best[i].id);
    printf("\n");
}

/* TODO Add caching */
int target(task_t *solution, int n) {
    register int sum, i, time, tmp;

    for (i = 0, time = 0, sum = 0; i < n; i++) {
        time += solution[i].length;
        /* Evaluating expression once */
        tmp = time - solution[i].due;
        sum += solution[i].weight * MAX(0, tmp);
    }

    return sum;
}

char swap_skip(task_t *array, int n, int original) {
    task_t *copy, tmp;
    char result;
    int i;

    /* Creating defensive copy */
    copy = malloc(n * sizeof(task_t));
    memcpy(copy, array, n*sizeof(task_t));

    result = 0;
    for (i = n - 1; i > 0; i--) {
        /* Swapping values */
        tmp = copy[i];
        copy[i] = copy[i - 1];
        copy[i - 1] = tmp;

        if (target(copy, n) < original) {
            result = 1;
            break;
        }
    }

    free(copy);
    return result;
}

task_t *generate_initial(int n, task_t *tasks) {
    task_t *initial, *best, *ptr;
    int *fill, i, r, iterations, best_result;
    char *used;

    initial = malloc(n * sizeof(task_t));
    fill = malloc(n * sizeof(int));
    used = malloc(n * sizeof(char));
    iterations = n*n;
    best = NULL;

    srand(time(NULL));
    while (iterations--) {
        memset(used, 0, n*sizeof(char));
        for (i = 0; i < n; i++) {
            r = rand() % n;
            if (used[r] == 0) {
                fill[i] = r;
                used[r] = 1;
            } else {
                /* Value used, generating again */
                i--;
            }
        }

        initial = malloc(n * sizeof(task_t));
        for (i = 0; i < n; i++) initial[i] = tasks[fill[i]];

        if (best == NULL || best_result > target(initial, n)) {
            ptr = best;
            best = initial;
            initial = ptr;
            best_result = target(best, n);
        }
        free(initial);
    }

    free(used);
    free(fill);
    return best;
}

void permute(int n, task_t *tasks, int *fill, int index, char *used, task_t **best, int *best_result) {
    int i, j, array_result;
    char cont;
    task_t *array, *ptr;

    for (i = 0; i < n; i++) {
        if (used[i] == 0) {
            fill[index] = i;

            /* Rebuilding task array */
            array = malloc((index + 1) * sizeof(task_t));
            for (j = 0; j < index + 1; j++) array[j] = tasks[fill[j]];
            if (index < n-1) {
                /* Override point for elimination */
                cont = 1;

                /* Counting this value once, it will be used several times */
                array_result = target(array, index + 1);

                /* Skip if incomplete permutation is worse than the best */
                if (array_result > *best_result) cont = 0;

                /* Skip if there is a better solution in a swapped subset */
                if (cont && swap_skip(array, index + 1, array_result)) cont = 0;

                /* if (cont && blah blah) cont = 0; */

                if (cont) {
                    used[i] = 1;
                    permute(n, tasks, fill, index + 1, used, best, best_result);
                    used[i] = 0;
                }
            } else {
                /* We have a complete permutation, calling target function */
                if (target(array, n) < *best_result) {
                    ptr = *best;
                    *best = array;
                    array = ptr;

                    /* Caching target function for best result */
                    *best_result = target(*best, n);
                }
            }
            free(array);
        }
    }
}

task_t *compute(int n, task_t *tasks) {
    task_t *best;
    int best_result, *fill;
    char *used;

    /* Creating necessary helper tables */
    used = malloc(n * sizeof(char));
    memset(used, 0, n * sizeof(char));
    fill = malloc(n * sizeof(int));

    /* Generating initial result */
    best = generate_initial(n, tasks);
    best_result = target(best, n);

    permute(n, tasks, fill, 0, used, &best, &best_result);

    /* freeing temporary buffers */
    free(fill);
    free(used);

    return best;
}

int main(void) {
    task_t *tasks, *best;
    int n;

    n = read_tasks(&tasks);
    best = compute(n, tasks);
    print_tasks(n, tasks, best);

    free(tasks);
    free(best);

    return 0;
}

