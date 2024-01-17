Banco de pruebas para el último examen, utilizando como base "Beej’s Guide to Network Programming"

7.3 select()—Synchronous I/O Multiplexing, Old School

```c
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
	int select(int numfds, fd_set *readfds, fd_set *writefds,
		fd_set *exceptfds, struct timeval *timeout);
```

Lógica de sockets para la comunicación Cliente <----> Servidor

1. Parametrizar el servidor para escuchar el puerto.

	* Inicializar los fd_socket
		* master
		* read_fds
	* socket
	* bind
	* listen

2. Gestionar servidor y clientes

	* Copiar en read_fds, master
	* Hacer select de read_fds
	* Iterar sobre read_fds desde 0 hasta maxfd y comprobar si el fd está presente
	* Si el fd está presente:

		1. FD Servidor:
			* Aceptar conexión.
			* Añade el nuevo fd a master

		2. FD Cliente:
			* Leer el contenido.
			* Mandar el contenido a todos, menos el servidor y el cliente.

Select utiliza como estructura de datos 'fd_set'. El parámetro numfds = maxfd + 1. Donde maxfd es el mas grande recibido.
