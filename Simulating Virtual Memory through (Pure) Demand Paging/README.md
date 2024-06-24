Instructions for running the files:
    make all - creates all necessary object files Master, MMU, process, sched
    make result - creates the result.txt file which is the concatenation of 4 files namely
                  1.frequency.txt - shoes the number of pagefaults for each process
                  2.pageFaults.txt - has the list of all pagefaults
                  3.globalOrdering.txt -  shows how pages are accessed in global ordering
                  4.invalidPageReference.txt - shows the list of  invalid page references

    ./Master  <number_of_processes> <max_number_of_pages_per_process> <total_number_of_process>