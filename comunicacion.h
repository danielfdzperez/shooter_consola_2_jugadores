#ifndef __COMUNICACION_H__
#define __COMUNICACION_H__
#ifdef __cplusplus
extern "C"{
#endif
int recibir(int fd, char *buffer);
int enviar(int fd, const char *mssg);
#ifdef __cplusplus
}
#endif
#endif
