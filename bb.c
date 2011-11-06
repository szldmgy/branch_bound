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

GArray *compute(GArray *tasks) {
    GArray *best = NULL;

    return best;
}

int main(void) {
    GArray *tasks, *best;

    tasks = read_tasks("data/5.txt");
    best = compute(tasks);
    print_tasks(tasks, best);

    if (tasks) g_array_free(tasks, TRUE);
    if (best) g_array_free(best, TRUE);

    return 0;
}

