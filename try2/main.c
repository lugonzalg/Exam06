#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

# define RAW_SIZE 		1000
# define BAD_ARGUMENTS 	"Error: Bad Arguments"
# define FATAL_ERROR 	"Fatal error"

static void send_error(char *msg, int exit_code) {
	write(2, msg, strlen(msg));

	if (exit_code)
		exit(exit_code);
}

void *get_in_addr(struct sockaddr *sa) {

	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

typedef struct s_client {

	int id;
	int fd;
	struct s_client *next;
	struct s_client *prev;

}	t_client;

typedef struct s_exam {

	fd_set 	master;
	fd_set 	reads;
	int 	serverfd;
	int 	server_port;
	int		maxfd;
	int		newfd;
	char 	raw[RAW_SIZE];
	char 	*buffer;

	t_client *head;
	t_client *tail;
	t_client *node;

} t_exam;

static t_client *create_node(int fd) {

	t_client 	*node;
	static int 	id;

	node = (t_client *)malloc(sizeof(t_client));
	if (node == NULL)
		send_error(FATAL_ERROR, 5);

	node->id = id + 1;
	node->fd = fd;
	node->next = NULL;
	node->prev = NULL;
	return node;
}

static void add_client(t_exam *exam) {

	if (exam->tail == NULL || exam->head == NULL) {

		exam->tail = exam->node;
		exam->head = exam->node;

	} else {

		exam->tail->next = exam->node;
		exam->node->prev = exam->tail;
		exam->tail = exam->node;

	}

}

static void delete_client(int fd, t_exam *exam) {

	t_client *tmp;

	if (exam->head->fd == fd) {

		tmp = exam->head;
		exam->head = exam->head->next;
		free(tmp);
		return ;

	}

	if (exam->tail->fd == fd) {

		tmp = exam->tail;
		exam->tail = exam->tail->prev;
		free(tmp);
		return ;

	}

	tmp = exam->head->next;
	for (; tmp != exam->tail; tmp = tmp->next) {

		if (tmp->fd == fd) {

			tmp->prev->next = tmp->next;
			tmp->next->prev = tmp->prev;
			free(tmp);
			break ;

		}
	}

}

static void broadcast(int fd, t_exam *exam) {

	t_client *tmp;

	tmp = exam->head;
	for (; tmp != NULL; tmp = tmp->next) {

		if (fd != tmp->fd)
			;
	}

}

static void welcome_client(t_client *client) {

	char buffer[1000];
	int  retval;

	retval = sprintf(buffer, "client(%d): Welcome!", client->id);
	buffer[retval] = 0;

	retval = send(client->fd, buffer, strlen(buffer), 0);
}

int main(int argc, char *argv[]) {

	t_client			*head;
	t_client			*tail;
	t_exam				exam;
	struct sockaddr_in 	address;
	int					retval;

	if (argc != 2)
		send_error(BAD_ARGUMENTS, 1);

	memset(&exam, 0, sizeof(t_exam));
	FD_ZERO(&exam.master);
	FD_ZERO(&exam.reads);

	exam.server_port = atoi(argv[1]);

	/* PARTE PROPORCIONADA EN EL EXAMEN, NO HAY QUE APRENDERLO */

	exam.serverfd = socket(AF_INET , SOCK_STREAM , 0);
	if (exam.serverfd < 0)
		send_error(FATAL_ERROR, 2);

	exam.maxfd = exam.serverfd + 1;
	FD_SET(exam.serverfd, &exam.master);
	address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(exam.server_port);

	if (bind(exam.serverfd, (struct sockaddr *)&address, sizeof(address)) < 0)
		send_error(FATAL_ERROR, 3);

	if (listen(exam.serverfd, 10) == -1)
		send_error(FATAL_ERROR, 4);

	/*PARTE DEL EXAMEN A DESARROLLAR */
	while (1) {
		exam.reads = exam.master;
		select(exam.maxfd, &exam.reads, NULL, NULL, NULL);

		for (size_t fd = 0; fd < exam.maxfd; fd++) {

			if (FD_ISSET(fd, &exam.reads)) {

				if (fd == exam.serverfd) {

					exam.newfd = accept(exam.serverfd, (struct sockaddr *)&address, (socklen_t*)&address);
					if (exam.newfd != -1) {
						FD_SET(exam.newfd, &exam.master);
						exam.node = create_node(exam.newfd);
						add_client(&exam);
						welcome_client(exam.node);
					}

				} else {

					retval = recv(fd, exam.raw, RAW_SIZE, 0);
					if (retval <= 0) {
						delete_client(fd, &exam);
						FD_CLR(fd, &exam.master);
						continue ;
					}

					broadcast(fd, &exam);
				}

			}
		}
	}

	return 0;
}
