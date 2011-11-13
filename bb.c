/* Copyright (c) 2011 Mateusz Lenik <mt.lenik@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define MAX(a, b) ((a > b)? (a): (b))

typedef struct _task_t {
    int id;
    int length;
    int due;
    int weight;
} task_t;

int read_tasks(task_t **tasks_ptr) {
    task_t *tasks, t;
    int i, limit;

    tasks = NULL;
    limit = INT_MAX;
    i = 0;
    while (limit-- && scanf("%d %d %d", &t.length, &t.due, &t.weight) != EOF) {
        tasks = realloc(tasks, ++i * sizeof(task_t));
        t.id = i;
        tasks[i-1] = t;
    }

    *tasks_ptr = tasks;
    return i;
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

char length_skip(task_t *array, int n, int original) {
    task_t *copy, tmp;
    char result;
    int i;

    /* Creating defensive copy */
    copy = malloc(n * sizeof(task_t));
    memcpy(copy, array, n * sizeof(task_t));

    result = 0;
    for (i = 0; i < n; i++) {
        if (array[i].length > array[n-1].length) {
            tmp = copy[i];
            copy[i] = copy[n-1];
            copy[n-1] = tmp;

            if (target(copy, n) < original) {
                result = 1;
                break;
            }
        }
    }

    free(copy);
    return result;
}

task_t *generate_initial(int n, task_t *tasks) {
    task_t *initial, *best, *ptr;
    int *fill, i, r, iterations, best_result;
    char *used;

    fill = malloc(n * sizeof(int));
    used = malloc(n * sizeof(char));
    iterations = n*n;
    best = NULL;
    initial = NULL;

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

                /* Computing this value once, it will be used several times */
                array_result = target(array, index + 1);

                /* Skip if incomplete permutation is worse than the best */
                if (array_result > *best_result) cont = 0;

                /* Skip if there is a better solution in a swapped subset */
                if (cont && swap_skip(array, index + 1, array_result)) cont = 0;

                /* Skip if the */
                if (cont && length_skip(array, index + 1, array_result)) cont = 0;

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

    /* Creating necessary helper buffers */
    used = malloc(n * sizeof(char));
    memset(used, 0, n * sizeof(char));
    fill = malloc(n * sizeof(int));

    /* Generating initial result */
    best = generate_initial(n, tasks);
    best_result = target(best, n);

    /* Computing happens here */
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

