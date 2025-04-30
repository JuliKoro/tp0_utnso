// client.h: "header file" (encabezado) del cliente
// Se ubican las declaraciones de funciones, constantes, structs, etc., que luego se usan en client.c.

#ifndef CLIENT_H_
#define CLIENT_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h> // para operar las configs
#include<readline/readline.h> // para leer de la consola

#include "utils.h"

/**
 * @brief Crea un archivo logger, listo para utilizar
 * @return t_log* nuevo_logger: puntero al archivo cliente.log
*/
t_log* iniciar_logger(void);

/**
 * @brief carga un archivo de configuración al inicio del programa, y obtener acceso fácil a sus valores clave.
 * @return t_config* nuevo_config:  puntero a una estructura config, que maneja las claves y valores del archivo
 */
t_config* iniciar_config(void);

/**
 * @brief Lee desde la consola y lo loggea
 * @param logger puntero t_log* al logger creado previamente
 * 
 * Lee líneas de texto que el usuario escribe en la consola (una por una) y las muestra en el log, hasta que el usuario ingresa una línea vacía (solo presiona Enter).
 */
void leer_consola(t_log*);

void paquete(int);
void terminar_programa(int, t_log*, t_config*);

#endif /* CLIENT_H_ */
