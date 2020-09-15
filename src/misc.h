#ifndef MISC_H
#define MISC_H

bool    cli(int, char *[]);
void    update_stat(uint32_t &counter, uint32_t &maxer, const uint32_t v);
inline bool job_mode(void) {return(!OPTS.cachedir.empty());}
long memused(void);

#endif // MISC_H
