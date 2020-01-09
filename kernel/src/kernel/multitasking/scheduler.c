#include <kpanic.h>
#include <kprint.h>
#include <string.h>
#include <mm/kmalloc.h>
#include <multitasking/process.h>
#include <multitasking/scheduler.h>

static struct scheduler_priority_bucket priority_buckets[PRIORITY_NUMBER_OF_PRIORITIES];
static struct scheduler_process processes[MAX_PROCESS_PID];
static struct scheduler_process *currently_running_process;

static struct scheduler_process *find_next_process();

/**
 * @brief      Initialize the process scheduler
 */
void scheduler_init()
{
	// Clear out everything
	memset(priority_buckets, 0, sizeof(priority_buckets));
	memset(processes, 0, sizeof(processes));

	currently_running_process = NULL;
}

/**
 * @brief      Add a process to the scheduler
 *
 * @param[in]  process  The process to add to the scheduler
 */
void scheduler_add_process(process_t process)
{
	struct scheduler_priority_bucket *bucket;
	pid_t pid;

	pid = process->pid;

	// Check valid priority
	if(process->priority < PRIORITY_CRITICAL || process->priority >= PRIORITY_NUMBER_OF_PRIORITIES)
		goto failed;

	// Check if PID valid and not being used
	if(pid >= MAX_PROCESS_PID || processes[pid].process)
		goto failed;

	// Add to process list
	processes[pid].process = process;
	processes[pid].next    = NULL;

	// Get the bucket for the priority to be added to
	bucket = &(priority_buckets[process->priority]);

	// Add to head of list
	if(bucket->head == NULL || bucket->head->process == NULL) {
		bucket->head = &(processes[pid]);
		goto process_added;
	}

	// Add to head of list
	processes[pid].next = bucket->head;
	bucket->head = &(processes[pid]);

process_added:
	return;

failed:
	kprintf("Trying to add process:\n");
	dump_process(process);
	kpanic("Failed to add process to task scheduler!");
}

/**
 * @brief      Removes a process from the scheduler
 *
 * @param[in]  process  The process to remove from the scheduler
 */
void scheduler_remove_process(process_t process)
{
	// TODO: Remove process from scheduler list
}

/**
 * @brief      Gets a process by a given pid.
 *
 * @param[in]  pid   The pid of the process
 *
 * @return     The process control block (PCB) for a given PID, or NULL if not found
 */
process_t get_process_by_pid(pid_t pid)
{
	// Should never reach out of bounds as the size of the processes array
	// Is the same size as the max value of pid
	// pid_t is also unsigned, so value cannot be negative
	return processes[pid].process;
}

/**
 * @brief      Get the next process to run
 *
 * @return     The process control block (PCB) for the next process to run
 */
process_t scheduler_get_next()
{
	if(currently_running_process && currently_running_process->next) {
		currently_running_process = currently_running_process->next;
	} else {
		// Need to cycle and find a new full priority list
		currently_running_process = find_next_process();
	}

	if(currently_running_process == NULL)
		kpanic("Could not find any process for scheduler");

	return currently_running_process->process;
}

/**
 * @brief      Finds the next process to run. Looks for the first non-empty bucket of the higher priority.
 *
 * @return     The head of the first non-empty priority bucket, or NULL if no processes are found
 */
static struct scheduler_process *find_next_process()
{
	for(priority_t p = PRIORITY_CRITICAL; p < PRIORITY_NUMBER_OF_PRIORITIES; p++) {
		if(priority_buckets[p].head)
			return priority_buckets[p].head;
	}

	return NULL;
}