#include "inventaris.h"

/* ========================================================================== */
/* DEFINISI VARIABEL GLOBAL                                                   */
/* ========================================================================== */
Node  pool[MAX_NODES];
int   poolUsed = 0;
Node *head = NULL;

/* ========================================================================== */
/* FUNGSI UTAMA ANGGOTA 1                                                     */
/* ========================================================================== */

void initSistem(void) {
    int i;
    poolUsed = 0;
    head = NULL;
    
    /* Membilas seluruh pool memori statis agar bersih dari data sampah */
    for (i = 0; i < MAX_NODES; i++) {
        pool[i].id[0] = '\0';
        pool[i].nama[0] = '\0';
        pool[i].kategori[0] = '\0';
        pool[i].stok = 0;
        pool[i].lokasi[0] = '\0';
        pool[i].status = ST_TERSEDIA;
        pool[i].pemilik[0] = '\0';
        pool[i].pic[0] = '\0';
        pool[i].next = NULL;
    }
    PRINTLN(">>> Sistem Inventaris Berhasil Diinisialisasi <<<");
}

void tambahData(void) {
    char tempID[MAX_ID_LEN];
    Node *curr;
    Node *newNode;
    int inputStatus;

    /* 1. Validasi batas kapasitas memori (Penanganan memori habis) */
    if (poolUsed >= MAX_NODES) {
        PRINTLN("[ERROR] Kapasitas memori penuh! Tidak bisa menambah data.");
        return;
    }

    /* Peringatan jika memori hampir penuh */
    if (poolUsed >= WARN_THRESHOLD) {
        PRINTLN("[PERINGATAN] Kapasitas memori hampir habis!");
    }

    /* 2. Mengambil input ID dan validasi duplikasi ID */
    PRINT("Masukkan ID Barang: ");
    bacaString(tempID, MAX_ID_LEN);
    serialPrintStr(tempID);
    serialPrintLn();

    /* Cari apakah ID sudah ada di linked list */
    curr = head;
    while (curr != NULL) {
        if (strcmp(curr->id, tempID) == 0) {
            PRINTLN("[ERROR] ID Barang sudah terdaftar (Duplikat)!");
            return;
        }
        curr = curr->next;
    }

    /* 3. Ambil slot kosong berikutnya dari pool memori statis */
    newNode = &pool[poolUsed];
    strcpy(newNode->id, tempID);

    /* 4. Mengisi data atribut inventaris */
    PRINT("Masukkan Nama Komponen: ");
    bacaString(newNode->nama, MAX_NAMA_LEN);
    serialPrintStr(newNode->nama);
    serialPrintLn();

    PRINT("Masukkan Kategori: ");
    bacaString(newNode->kategori, MAX_KAT_LEN);
    serialPrintStr(newNode->kategori);
    serialPrintLn();

    PRINT("Masukkan Jumlah Stok: ");
    bacaInt(&(newNode->stok));
    serialPrintInt(newNode->stok);
    serialPrintLn();

    PRINT("Masukkan Lokasi Penyimpanan: ");
    bacaString(newNode->lokasi, MAX_LOK_LEN);
    serialPrintStr(newNode->lokasi);
    serialPrintLn();

    PRINT("Masukkan Status (0:Tersedia, 1:Dipinjam, 2:Rusak, 3:Habis): ");
    bacaInt(&inputStatus);
    serialPrintInt(inputStatus);
    serialPrintLn();
    if (inputStatus >= ST_TERSEDIA && inputStatus <= ST_HABIS) {
        newNode->status = inputStatus;
    } else {
        newNode->status = ST_TERSEDIA;
    }

    PRINT("Masukkan Pemilik: ");
    bacaString(newNode->pemilik, MAX_PEM_LEN);
    serialPrintStr(newNode->pemilik);
    serialPrintLn();

    PRINT("Masukkan PIC: ");
    bacaString(newNode->pic, MAX_PIC_LEN);
    serialPrintStr(newNode->pic);
    serialPrintLn();

    /* 5. Penyisipan node baru ke ujung linked list (Tail) */
    newNode->next = NULL;
    if (head == NULL) {
        head = newNode; /* Penanganan jika linked list awalnya kosong */
    } else {
        curr = head;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = newNode;
    }

    /* Menambah catatan jumlah slot terpakai */
    poolUsed++;
    PRINTLN("[SUKSES] Data berhasil ditambahkan ke inventaris.");
}

void tampilSemua(void) {
    Node *curr = head;

    /* Penanganan jika data linked list masih kosong */
    if (curr == NULL) {
        PRINTLN("[INFO] Data inventaris kosong.");
        return;
    }

    /* Cetak tabel terstruktur */
    cetakHeader();
    while (curr != NULL) {
        cetakBaris(curr);
        curr = curr->next;
    }
    cetakGaris();
}

/* ========================================================================== */
/* FUNGSI HELPER / FORMAT CETAK                                               */
/* ========================================================================== */

void cetakHeader(void) {
    cetakGaris();
    PRINTLN("| ID     | Nama Komponen           | Kategori         | Stok | Lokasi          | Status   | Pemilik          | PIC             |");
    cetakGaris();
}

void cetakBaris(Node *n) {
    PRINT("| "); serialPrintStr(n->id);        PRINT("\t| ");
    serialPrintStr(n->nama);      PRINT("\t| ");
    serialPrintStr(n->kategori);  PRINT("\t| ");
    serialPrintInt(n->stok);      PRINT("\t| ");
    serialPrintStr(n->lokasi);    PRINT("\t| ");
    cetakStatus(n->status);       PRINT("\t| ");
    serialPrintStr(n->pemilik);   PRINT("\t| ");
    serialPrintStr(n->pic);       PRINT("\t|\n");
}

void cetakGaris(void) {
    PRINTLN("---------------------------------------------------------------------------------------------------------------------------------");
}

void cetakStatus(int st) {
    if (st == ST_TERSEDIA)       PRINT("Tersedia");
    else if (st == ST_DIPINJAM)  PRINT("Dipinjam");
    else if (st == ST_RUSAK)     PRINT("Rusak   ");
    else if (st == ST_HABIS)     PRINT("Habis   ");
    else                         PRINT("Unknown ");
}

/* ========================================================================== */
/* MANAGEMENT MEMORI STATIS (DEFRAGMENTASI LINKED LIST)                      */
/* ========================================================================== */

void rebuildLinkedList(void) {
    int i;
    int activeCount = 0;
    Node tempPool[MAX_NODES];

    /* 1. Ambil seluruh node yang masih terhubung di linked list asli, amankan ke buffer sementara */
    Node *curr = head;
    while (curr != NULL && activeCount < MAX_NODES) {
        tempPool[activeCount] = *curr;
        tempPool[activeCount].next = NULL;
        activeCount++;
        curr = curr->next;
    }

    /* 2. Bersihkan dan reset ulang pool utama */
    initSistem();

    /* 3. Kembalikan data dari buffer ke pool utama dan sambungkan kembali urutan pointer-nya */
    if (activeCount > 0) {
        poolUsed = activeCount;
        for (i = 0; i < poolUsed; i++) {
            pool[i] = tempPool[i];
            if (i < poolUsed - 1) {
                pool[i].next = &pool[i + 1];
            } else {
                pool[i].next = NULL;
            }
        }
        head = &pool[0]; /* Head menunjuk kembali ke indeks pertama pool utama */
    }
}