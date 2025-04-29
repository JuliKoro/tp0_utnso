#include "server.h"

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);
	// fd: file descriptor
	int server_fd = iniciar_servidor(); // nos permite distinguir una conexion de otra
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd); // el recibir es bloqueante -> se queda esperando en esa linea
		switch (cod_op) { //con el cod_op elijo que estoy recibiendo?
		case MENSAJE: // recibe los log_info
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE: // recibe los paquete
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}
