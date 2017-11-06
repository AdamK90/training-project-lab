#define BUFFER_SIZE  (256 * 1024)  /* 256 KB */

#define URL_FORMAT   "https://api.github.com/repos/%s/%s/commits"
#define URL_SIZE     256

struct write_result
{
    char *data;
    int pos;
};

typedef struct commit_data {
	const char *sha;
	const char *message;
} commit_data;

char * get_result(const char *user, const char *repository);
commit_data get_first(const char* user, const char* repository, int *error);