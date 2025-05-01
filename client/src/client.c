/**
 * @file client.c
 * @author sisoputnfrba
 * @author JuliKoro
 * @date 21 Mar 2019
 * @brief Codigo fuente del cliente
 *
 * Tiene la implementación real de funciones, la lógica del programa.
 * @see https://docs.utnso.com.ar/primeros-pasos/tp0
 * @see https://docs.utnso.com.ar/guias/linux/sockets
 */

#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */
	/* Un logger (o sistema de logging) es una herramienta que registra 
	mensajes en tiempo de ejecución para ayudar a los desarrolladores a 
	entender qué está haciendo el programa, detectar errores, y hacer 
	seguimiento de eventos importantes.
	Permite registrar información importante en archivos (cliente.log, por ejemplo).
	*/

	logger = iniciar_logger();

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"
	//printf("Hola! Soy un log");
	log_info(logger, "Hola! Soy un log"); // Todo lo que imprima, se registra en el archivo
	
	// RECOMENDABLE: En cuanto detecta q algo salio mal, termina el programa
	// Asi no sigue loggeando msjs, y en la consola puedo ver exactamente donde se rompio el programa.
	if(logger == NULL){
		perror("Algo raro paso con el log. No se pudo crear o enontrar el archivo.");
		exit(EXIT_FAILURE);
	}

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */
	// Los archivos .config funcionan como un diccionario donde hay:
	// CLAVE=valor

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	ip = config_get_string_value(config, "IP"); // (t_config *config, "KEY")
	puerto = config_get_string_value(config, "PUERTO");
	valor = config_get_string_value(config, "CLAVE");

	// Loggeamos el valor de config

	log_info(logger, "VALOR leido de la config: %s", valor);


	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje

	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
	printf("\nCLIENTE CERRADO!");
}

t_log* iniciar_logger(void)
{
	/* Crea una instancia de logger:
	- cliente.log: archivo en donde escribe los logs
	- CL_LOG: nombre con el que se muestra
	- 1: Para que tambien se muestre por consola
	- LOG_LEVEL_INFO: Loguea solo mensajes de log_info en adelante.
	*/
	t_log* nuevo_logger = log_create("cliente.log", "CL_LOG", 1, LOG_LEVEL_INFO);

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create("cliente.config");
	//abre y parsea el archivo de configuración que debe existir en el mismo directorio donde se ejecuta el binario.
	if(nuevo_config == NULL){
		//no puede abrir o leer el archivo (i.e: si no existe)
		perror("Error al intentar cargar el nuevo config."); //msj de error
		exit(EXIT_FAILURE); // Terminemos el programa
	}

	return nuevo_config;
	//puntero t_config* con la configuración cargada para que lo uses en el resto del programa
}

void leer_consola(t_log* logger)
{
	char* leido; // variable donde va a guardar la línea que se lea de la consola.

	// La primera te la dejo de yapa
	// muestra el prompt ">" y espera a que el usuario escriba algo
	leido = readline("> "); // leo la consola y almaceno 
	log_info(logger, ">> %s", leido); // logeo en el cliente el contenido leido

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while (strcmp(leido, "") != 0) // mientras no sea una linea vacia
	{
		free(leido); // libero lo leido
		leido = readline("> "); //vuelvo a leer
		log_info(logger, ">> %s", leido); // vuelvo a logear
	} // sale cuando vea una linea vacia

	// ¡No te olvides de liberar las lineas antes de regresar!
	free(leido);
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete;

	// Leemos y esta vez agregamos las lineas al paquete


	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */

	  log_destroy(logger); // Cierra el logger
	  config_destroy(config); // Cierra el .config
}
