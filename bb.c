#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct _task_t {
    gint id;
    gint length;
    gint due;
    gint weight;
} task_t;

GArray *read_tasks(gchar *filename) {
    GArray *tasks;
    task_t t;
    gint i;
    FILE *file;

    if (filename != NULL) {
        file = fopen(filename, "r");
    } else {
        file = stdin;
    }
    g_assert(file != NULL);

    i = 1;
    tasks = g_array_new(FALSE, FALSE, sizeof(task_t));
    while (fscanf(file, "%d %d %d", &t.length, &t.due, &t.weight) != EOF) {
        t.id = i++;
        g_array_append_val(tasks, t);
    }

    if (filename != NULL) fclose(file);

    return tasks;
}

void print_tasks(GArray *tasks, GArray *best) {
    gint i;
    task_t t;

    g_printf(" Nr | p_j | d_j | w_j \n");
    g_printf("----+-----+-----+-----\n");
    for (i = 0; i < (gint)tasks->len; i++) {
        t = g_array_index(tasks, task_t, i);
        g_printf("%2d. |%4d |%4d |%4d \n", t.id, t.length, t.due, t.weight);
    }

    if (best != NULL) {
        g_printf("Best solution is: ");
        for (i = 0; i < (gint)best->len; i++) {
            t = g_array_index(best, task_t, i);
            g_printf("%d ", t.id);
        }
        g_printf("\n");
    }
}

gint target(GArray *solution) {
    return 42;
}

/* TODO rewrite to pass best as a task_t array instead of GArray */
GArray *permute(gint n, task_t *tasks, gint *fill, gint index, gboolean *used, GArray *best) {
    gint i, j;
    GArray *array, *ptr;

    for (i = 0; i < n; i++) {
        if (used[i] == FALSE) {
            fill[index] = i;
            if (index < n-1) {
                /* Override point for elimination */

                used[i] = TRUE;
                permute(n, tasks, fill, index + 1, used, best);
                used[i] = FALSE;
            } else {
                /* We have a complete permutation, rebuilding task array */
                array = g_array_sized_new(FALSE, FALSE, sizeof(task_t), n);
                for (j = 0; j < n; j++)
                    g_array_append_val(array, tasks[fill[j]]);

                /* Calling target function */
                if (target(array) < target(best)) {
                    ptr = best;
                    best = array;
                    array = ptr;
                }

                g_array_free(array, TRUE);
            }
        }
    }

    return best;
}

GArray *compute(GArray *tasks) {
    GArray *best;
    gint n, i, *fill;
    gboolean *used;

    /* Creating necessary helper tables */
    n = (gint)tasks->len;
    used = g_new0(gboolean, n);
    fill = g_new(gint, n);

    /* TODO memcpy tasks->data instead */
    best = g_array_sized_new(FALSE, FALSE, sizeof(task_t), n);
    for (i = 0; i < n; i++) {
        g_array_append_val(best, g_array_index(tasks, task_t, i));
    }

    best = permute(n, (task_t *)tasks->data, fill, 0, used, best);

    g_free(fill);
    g_free(used);

    return best;
}

int main(void) {
    GArray *tasks, *best;

    tasks = read_tasks("data/11.txt");
    best = compute(tasks);
    print_tasks(tasks, best);

    if (tasks) g_array_free(tasks, TRUE);
    if (best) g_array_free(best, TRUE);

    return 0;
}

