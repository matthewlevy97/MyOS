#pragma once

/**
 * @brief      Linked-list entry used by the process scheduler
 */
struct scheduler_process {
	process_t process;
	struct scheduler_process *next;
};

/**
 * @brief      Bucket for a given priority
 */
struct scheduler_priority_bucket {
	struct scheduler_process *head;
};

void scheduler_init();

void scheduler_add_process(process_t process);
void scheduler_remove_process(process_t process);

process_t scheduler_get_next();