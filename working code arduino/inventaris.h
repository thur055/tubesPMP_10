#ifndef INVENTARIS_H
#define INVENTARIS_H

#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NODES       15
#define MAX_ID_LEN       7
#define MAX_NAMA_LEN    25
#define MAX_KAT_LEN     18
#define MAX_LOK_LEN     15
#define MAX_PEM_LEN     18
#define MAX_PIC_LEN     15
#define WARN_THRESHOLD  12

#define ST_TERSEDIA  0
#define ST_DIPINJAM  1
#define ST_RUSAK     2
#define ST_HABIS     3

typedef struct Node {
    char        id[MAX_ID_LEN];
    char        nama[MAX_NAMA_LEN];
    char        kategori[MAX_KAT_LEN];
    int         stok;
    char        lokasi[MAX_LOK_LEN];
    int         status;
    char        pemilik[MAX_PEM_LEN];
    char        pic[MAX_PIC_LEN];
    struct Node *next;
} Node;

extern Node  pool[MAX_NODES];
extern int   poolUsed;
extern Node *head;

#define PRINT(teks)    serialPrintFlash(PSTR(teks))
#define PRINTLN(teks)  do { serialPrintFlash(PSTR(teks)); serialPrintLn(); } while (0)

/* Hardware dan serial */
void initHardware(void);
void serialPrintFlash(const char *p);
void serialPrintStr(const char *s);
void serialPrintChar(char c);
void serialPrintInt(int val);
void serialPrintLn(void);
void bacaString(char *buf, int maks);
void bacaInt(int *out);

/* Anggota 1 */
void initSistem(void);
void tambahData(void);
void tampilSemua(void);
void cetakHeader(void);
void cetakBaris(Node *n);
void cetakGaris(void);
void cetakStatus(int st);
void rebuildLinkedList(void);

/* Anggota 2 */
void cariNodeByID(const char *id, Node **out);
void cariDataByID(void);
void hapusNodeLL(const char *id);
void hapusDataByID(void);
void terapkanStok(Node *n, int stokBaru);
void updateStok(void);
void updateStatus(void);
void tampilRingkasan(void);
void cekMemori(void);
void cetakDetail(Node *n);

/* Anggota 3 */
void tampilMenu(void);
void prosesMenu(char pilihan);
void runProgram(void);
#ifdef __cplusplus
}
#endif

#endif
