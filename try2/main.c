# define BAD_ARGUMENTS "Error: Bad Arguments"

static void send_error(char *msg, int exit_code) {
	write(2. msg, strlen(msg));

	if (exit_code)
		exit(exit_code);
}

int main(int argc, char *argv[]) {

	int sockfd;

	if (argc != 2)
		send_error(BAD_ARGUMENTS, sizeof(BAD_ARGUMENTS));

	sockfd = socket(
	return 0;
}
