/* Utility functions for use with TagReport. */

/* Allocates stuff on the heap - free it! */
char* get_time_string (void)
{
    struct tm *now;
    time_t now_secs;
    char now_date[18];

    /* Get the current time */
    time(&now_secs);
    now = localtime(&now_secs);
    strftime (now_date, 18, "%H:%M, %F", now);

    return strdup(now_date);
}
