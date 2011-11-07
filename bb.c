#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct _task_t {
    gint id;
    gint length;
    gint due;
    gint weight;
} task_t;

GArray *read_tasks(void) {
    GArray *tasks;
    task_t t;
    gint i;

    i = 1;
    tasks = g_array_new(FALSE, FALSE, sizeof(task_t));
    while (scanf("%d %d %d", &t.length, &t.due, &t.weight) != EOF) {
        t.id = i++;
        g_array_append_val(tasks, t);
    }

    return tasks;
}

void print_tasks(GArray *tasks, GArray *best) {
    gint i;
    task_t t;

    printf(" Nr | p_j | d_j | w_j \n");
    printf("----+-----+-----+-----\n");
    for (i = 0; i < (gint)tasks->len; i++) {
        t = g_array_index(tasks, task_t, i);
        printf("%2d. |%4d |%4d |%4d \n", t.id, t.length, t.due, t.weight);
    }

    if (best != NULL) {
        printf("\nBest solution is: ");
        for (i = 0; i < (gint)best->len; i++) {
            t = g_array_index(best, task_t, i);
            printf("%d ", t.id);
        }
        printf("\n");
    }
}

gint target(task_t *solution, gint n) {
    gint sum, i, time;

    for (i = 0, time = 0, sum = 0; i < n; i++) {
        time += solution[i].length;
        sum += solution[i].weight * MAX(0, time - solution[i].due);
    }

    return sum;
}

gboolean swap_skip(task_t *array, gint n, gint original) {
    task_t *copy, tmp;
    gboolean result;
    gint i;

    /* Creating defensive copy */
    copy = g_new(task_t, n);
    memcpy(copy, array, n*sizeof(task_t));

    result = FALSE;
    for (i = n - 1; i > 0; i--) {
        /* Swapping values */
        tmp = copy[i];
        copy[i] = copy[i - 1];
        copy[i - 1] = tmp;

        if (target(copy, n) < original) {
            result = TRUE;
            break;
        }
    }

    g_free(copy);
    return result;
}

void permute(gint n, task_t *tasks, gint *fill, gint index, gboolean *used, task_t **best, gint *best_result) {
    gint i, j, array_result;
    gboolean skip;
    task_t *array, *ptr;

    for (i = 0; i < n; i++) {
        if (used[i] == FALSE) {
            fill[index] = i;

            if (index < n-1) {
                /* Rebuilding task array */
                array = g_new(task_t, index + 1);
                for (j = 0; j < index + 1; j++) array[j] = tasks[fill[j]];

                /* Override point for elimination */
                skip = FALSE;

                /* Counting this value once, it will be used several times */
                array_result = target(array, index + 1);
                /* Skip if incomplete permutation is worse than the best */
                if (array_result > *best_result) skip = TRUE;

                /* Skip if there is a better solution in a rotated subset */
                if (swap_skip(array, index + 1, array_result)) skip = TRUE;

                if (!skip) {
                    used[i] = TRUE;
                    permute(n, tasks, fill, index + 1, used, best, best_result);
                    used[i] = FALSE;
                }

                g_free(array);
            } else {
                /* We have a complete permutation, rebuilding task array */
                array = g_new(task_t, n);
                for (j = 0; j < n; j++) array[j] = tasks[fill[j]];
                /* Calling target function */
                if (target(array, n) < *best_result) {
                    ptr = *best;
                    *best = array;
                    array = ptr;

                    /* Caching target function for best result */
                    *best_result = target(*best, n);
                }

                g_free(array);
            }
        }
    }
}

GArray *compute(GArray *tasks) {
    task_t *best;
    gint n, i, best_result, *fill;
    gboolean *used;
    GArray *result;

    /* Creating necessary helper tables */
    n = (gint)tasks->len;
    used = g_new0(gboolean, n);
    fill = g_new(gint, n);
    best = g_new(task_t, n);
    memcpy(best, tasks->data, n*sizeof(task_t));

    best_result = target((task_t *)tasks->data, n);
    permute(n, (task_t *)tasks->data, fill, 0, used, &best, &best_result);

    /* rebuilding g_array */
    result = g_array_sized_new(FALSE, FALSE, sizeof(task_t), n);
    for (i = 0; i < n; i++) g_array_append_val(result, best[i]);

    /* freeing temporary buffers */
    g_free(best);
    g_free(fill);
    g_free(used);

    return result;
}

int main(void) {
    GArray *tasks, *best;

    tasks = read_tasks();
    best = compute(tasks);
    print_tasks(tasks, best);

    if (tasks) g_array_free(tasks, TRUE);
    if (best) g_array_free(best, TRUE);

    return 0;
}

