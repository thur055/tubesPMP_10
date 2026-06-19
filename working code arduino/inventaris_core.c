#include "inventaris.h"

Node  pool[MAX_NODES];
int   poolUsed = 0;
Node *head     = NULL;

void rebuildLinkedList(void)
{
    int i;

    if (poolUsed <= 0) {
        head = NULL;
        return;
    }

    head = &pool[0];

    for (i = 0; i < poolUsed; i++) {
        if (i == poolUsed - 1) {
            pool[i].next = NULL;
        } else {
            pool[i].next = &pool[i + 1];
        }
    }
}

void initSistem(void)
{
    int i;

    poolUsed = 0;
    head = NULL;

    for (i = 0; i < MAX_NODES; i++) {
        memset(&pool[i], 0, sizeof(Node));
    }

    PRINTLN("================================================");
    PRINTLN(" SISTEM INVENTARISASI LABORATORIUM EL2008");
    PRINTLN("================================================");
    PRINT(" Kapasitas maksimum : ");
    serialPrintInt(MAX_NODES);
    serialPrintLn();
    PRINT(" Peringatan memori  : >= ");
    serialPrintInt(WARN_THRESHOLD);
    serialPrintLn();
    PRINTLN("================================================");
}

void cetakStatus(int st)
{
    switch (st) {
        case ST_TERSEDIA: PRINT("Tersedia"); break;
        case ST_DIPINJAM: PRINT("Dipinjam"); break;
        case ST_RUSAK:    PRINT("Rusak   "); break;
        case ST_HABIS:    PRINT("Habis   "); break;
        default:          PRINT("Unknown "); break;
    }
}

void cetakGaris(void)
{
    PRINTLN("+------+---------------------+--------------+----+------------+----------+");
}

void cetakHeader(void)
{
    cetakGaris();
    PRINTLN("| ID   | Nama                | Kategori     |Stk | Lokasi     | Status   |");
    cetakGaris();
}

void cetakBaris(Node *n)
{
    char buf[8];
    int pad;

    PRINT("| ");
    serialPrintStr(n->id);
    pad = 5 - (int)strlen(n->id);
    while (pad-- > 0) serialPrintChar(' ');
    PRINT(" | ");

    serialPrintStr(n->nama);
    pad = 21 - (int)strlen(n->nama);
    while (pad-- > 0) serialPrintChar(' ');
    PRINT(" | ");

    serialPrintStr(n->kategori);
    pad = 12 - (int)strlen(n->kategori);
    while (pad-- > 0) serialPrintChar(' ');
    PRINT(" | ");

    itoa(n->stok, buf, 10);
    pad = 3 - (int)strlen(buf);
    while (pad-- > 0) serialPrintChar(' ');
    serialPrintStr(buf);
    PRINT(" | ");

    serialPrintStr(n->lokasi);
    pad = 10 - (int)strlen(n->lokasi);
    while (pad-- > 0) serialPrintChar(' ');
    PRINT(" | ");

    cetakStatus(n->status);
    PRINTLN(" |");
}

void tampilSemua(void)
{
    Node *curr = head;
    int jumlah = 0;

    PRINTLN("\n[ DAFTAR SELURUH INVENTARIS ]");

    if (curr == NULL) {
        PRINTLN("[INFO] Inventaris masih kosong.");
        return;
    }

    cetakHeader();

    while (curr != NULL) {
        cetakBaris(curr);
        curr = curr->next;
        jumlah++;
    }

    cetakGaris();

    PRINT("Total item: ");
    serialPrintInt(jumlah);
    PRINT(" | Slot terpakai: ");
    serialPrintInt(poolUsed);
    PRINT("/");
    serialPrintInt(MAX_NODES);
    serialPrintLn();

    PRINTLN("\n--- Pemilik dan PIC ---");

    curr = head;
    while (curr != NULL) {
        serialPrintStr(curr->id);
        PRINT(" | Pemilik: ");
        serialPrintStr(curr->pemilik);
        PRINT(" | PIC: ");
        serialPrintStr(curr->pic);
        serialPrintLn();
        curr = curr->next;
    }
}

void tambahData(void)
{
    char  idBaru[MAX_ID_LEN];
    Node *cekID;
    Node *newNode;

    PRINTLN("\n[ TAMBAH DATA ]");

    if (poolUsed >= MAX_NODES) {
        PRINT("Kapasitas penuh: ");
        serialPrintInt(poolUsed);
        PRINT("/");
        serialPrintInt(MAX_NODES);
        serialPrintLn();
        return;
    }

    PRINT("ID barang maks 6 karakter: ");
    bacaString(idBaru, MAX_ID_LEN);

    cariNodeByID(idBaru, &cekID);

    if (cekID != NULL) {
        PRINT("ID '");
        serialPrintStr(idBaru);
        PRINTLN("' sudah ada. Gunakan ID unik.");
        return;
    }

    newNode = &pool[poolUsed];
    poolUsed++;

    memset(newNode, 0, sizeof(Node));

    strncpy(newNode->id, idBaru, MAX_ID_LEN - 1);
    newNode->id[MAX_ID_LEN - 1] = '\0';

    PRINT("Nama komponen      : ");
    bacaString(newNode->nama, MAX_NAMA_LEN);

    PRINT("Kategori           : ");
    bacaString(newNode->kategori, MAX_KAT_LEN);

    PRINT("Jumlah stok        : ");
    bacaInt(&newNode->stok);

    if (newNode->stok < 0) {
        PRINTLN("Stok negatif tidak valid, stok diset 0.");
        newNode->stok = 0;
    }

    PRINT("Lokasi penyimpanan : ");
    bacaString(newNode->lokasi, MAX_LOK_LEN);

    PRINTLN("Status 0=Tersedia, 1=Dipinjam, 2=Rusak, 3=Habis");
    PRINT("Status             : ");
    bacaInt(&newNode->status);

    if (newNode->status < 0 || newNode->status > 3) {
        PRINTLN("Status tidak valid, status diset Tersedia.");
        newNode->status = ST_TERSEDIA;
    }

    if (newNode->stok == 0) {
        newNode->status = ST_HABIS;
    }

    PRINT("Pemilik            : ");
    bacaString(newNode->pemilik, MAX_PEM_LEN);

    PRINT("PIC                : ");
    bacaString(newNode->pic, MAX_PIC_LEN);

    rebuildLinkedList();

    PRINT("Data '");
    serialPrintStr(newNode->nama);
    PRINTLN("' berhasil ditambahkan.");

    cekMemori();
}
