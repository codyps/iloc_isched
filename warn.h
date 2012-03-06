#ifndef WARN_H_
#define WARN_H_

#define DEBUG_PR(...) do {				\
	fflush(stdout);					\
	fprintf(stderr, "%s (%s:%d): \t", __func__, __FILE__, __LINE__);	\
	fprintf(stderr, __VA_ARGS__);			\
	fputc('\n', stderr);				\
} while(0)

#define WARN(...) do {			\
	fflush(stdout);			\
	fprintf(stderr, __VA_ARGS__);	\
	fputc('\n', stderr);		\
} while(0)

#define WARN_AT_POS(file, lineno, charno, ...) do {		\
	fflush(stdout);						\
	fprintf(stderr, "%s:%d:%d: ", file, lineno, charno);	\
	fprintf(stderr, __VA_ARGS__);				\
	fputc('\n', stderr);					\
} while(0)

#endif
