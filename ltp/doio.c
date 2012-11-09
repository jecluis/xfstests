#define O_SSD 0                /* so code compiles on a CRAY2 */
int
do_listio(req)
struct io_req	*req;
{
	return -1;
}

/* ---------------------------------------------------------------------------
 *
 * A new paradigm of doing the r/w system call where there is a "stub"
 * function that builds the info for the system call, then does the system
 * call; this is called by code that is common to all system calls and does
 * the syscall return checking, async I/O wait, iosw check, etc.
 *
 * Flags:
 *	WRITE, ASYNC, SSD/SDS,
 *	FILE_LOCK, WRITE_LOG, VERIFY_DATA,
struct	status {
	int	rval;		/* syscall return */
	int	err;		/* errno */
	int	*aioid;		/* list of async I/O structures */
};

struct syscall_info {
	char		*sy_name;
	int		sy_type;
	struct status	*(*sy_syscall)();
	int		(*sy_buffer)();
	char		*(*sy_format)();
	int		sy_flags;
	int		sy_bits;
};

#define	SY_WRITE		00001
#define	SY_ASYNC		00010
#define	SY_IOSW			00020
#define	SY_SDS			00100

fmt_ioreq(struct io_req *ioreq, struct syscall_info *sy, int fd)
	static char		*errbuf=NULL;
	char			*cp;
	struct rw_req		*io;
	struct smap		*aname;

	io = &ioreq->r_data.io;

	/*
	 * Look up async I/O completion strategy
	 */
	for(aname=aionames;
	    aname->value != -1 && aname->value != io->r_aio_strat;
	    aname++)
		;
			      (io->r_pattern == '\0') ? '?' : io->r_pattern,
	if ((rval = alloc_mem(mem_needed + wtob(1) * 2 + fdc->c_memalign)) < 0) {
		if( !(sy->sy_flags & SY_IOSW) ) {
