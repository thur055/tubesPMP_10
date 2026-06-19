#include "inventaris.h"

void cariNodeByID(const char *id, Node **out)
{
    Node *curr = head;
    *out = NULL;

    while (curr != NULL) {
        if (strncmp(curr->id, id, MAX_ID_LEN) == 0) {
            *out = curr;
            return;
        }
        curr = curr->next;
    }
}

void cetakDetail(Node *n)
{
    PRINTLN("  +----------------------------------+");
    PRINT("  | ID       : ");
    serialPrintStr(n->id);
    serialPrintLn();

    PRINT("  | Nama     : ");
    serialPrintStr(n->nama);
    serialPrintLn();

    PRINT("  | Kategori : ");
    serialPrintStr(n->kategori);
    serialPrintLn();

    PRINT("  | Stok     : ");
    serialPrintInt(n->stok);
    serialPrintLn();

    PRINT("  | Lokasi   : ");
    serialPrintStr(n->lokasi);
    serialPrintLn();

    PRINT("  | Status   : ");
    cetakStatus(n->status);
    serialPrintLn();

    PRINT("  | Pemilik  : ");
    serialPrintStr(n->pemilik);
    serialPrintLn();

    PRINT("  | PIC      : ");
    serialPrintStr(n->pic);
    serialPrintLn();
    PRINTLN("  +----------------------------------+");
}

void cariDataByID(void)
{
    char idCari[MAX_ID_LEN];
    Node *hasil;

    PRINTLN("\n[ CARI DATA ]");

    if (head == NULL) {
        PRINTLN("[ERROR] Inventaris kosong.");
        return;
    }

    PRINT("Masukkan ID: ");
    bacaString(idCari, MAX_ID_LEN);

    cariNodeByID(idCari, &hasil);

    if (hasil == NULL) {
        PRINT("[ERROR] ID '");
        serialPrintStr(idCari);
        PRINTLN("' tidak ditemukan.");
        return;
    }

    PRINTLN("Barang ditemukan:");
    cetakDetail(hasil);
}

void hapusNodeLL(const char *id)
{
    Node *curr = head;
    int idx = -1;
    int lastIndex;

    while (curr != NULL) {
        if (strncmp(curr->id, id, MAX_ID_LEN) == 0) {
            idx = (int)(curr - pool);

            if (curr == head) {
                PRINTLN(" Node pertama dihapus.");
            } else if (curr->next == NULL) {
                PRINTLN(" Node terakhir dihapus.");
            } else {
                PRINTLN(" Node tengah dihapus.");
            }

            break;
        }
        curr = curr->next;
    }

    if (idx < 0 || idx >= poolUsed) {
        PRINTLN("[ERROR] Node gagal dihapus.");
        return;
    }

    lastIndex = poolUsed - 1;

    if (idx != lastIndex) {
        memcpy(&pool[idx], &pool[lastIndex], sizeof(Node));
    }

    poolUsed--;
    memset(&pool[poolUsed], 0, sizeof(Node));
    rebuildLinkedList();
}

void hapusDataByID(void)
{
    char idHapus[MAX_ID_LEN];
    char konfirm[2];
    Node *target;

    PRINTLN("\n[ HAPUS DATA ]");

    if (head == NULL) {
        PRINTLN("[ERROR] Inventaris kosong.");
        return;
    }

    PRINT("Masukkan ID barang: ");
    bacaString(idHapus, MAX_ID_LEN);

    cariNodeByID(idHapus, &target);

    if (target == NULL) {
        PRINT("[ERROR] ID '");
        serialPrintStr(idHapus);
        PRINTLN("' tidak ditemukan.");
        return;
    }

    PRINTLN("Data yang akan dihapus:");
    cetakDetail(target);

    PRINT("Konfirmasi hapus? y/n: ");
    bacaString(konfirm, 2);

    if (konfirm[0] != 'y' && konfirm[0] != 'Y') {
        PRINTLN("Penghapusan dibatalkan.");
        return;
    }

    hapusNodeLL(idHapus);

    PRINT("ID '");
    serialPrintStr(idHapus);
    PRINT("' berhasil dihapus. Slot terpakai: ");
    serialPrintInt(poolUsed);
    PRINT("/");
    serialPrintInt(MAX_NODES);
    serialPrintLn();
}

void terapkanStok(Node *n, int stokBaru)
{
    if (stokBaru < 0) {
        PRINTLN("[ERROR] Stok tidak boleh negatif.");
        return;
    }

    n->stok = stokBaru;

    if (stokBaru == 0) {
        n->status = ST_HABIS;
        PRINTLN("[WARN] Stok nol, status otomatis menjadi Habis.");
    } else if (n->status == ST_HABIS && stokBaru > 0) {
        n->status = ST_TERSEDIA;
        PRINTLN("Stok tersedia kembali, status menjadi Tersedia.");
    }

    PRINT("Stok '");
    serialPrintStr(n->nama);
    PRINT("' sekarang = ");
    serialPrintInt(n->stok);
    serialPrintLn();
}

void updateStok(void)
{
    char idUp[MAX_ID_LEN];
    char op[2];
    int jml;
    long stokHitung;
    Node *target;

    PRINTLN("\n[ UPDATE STOK ]");

    if (head == NULL) {
        PRINTLN("[ERROR] Inventaris kosong.");
        return;
    }

    PRINT("ID barang: ");
    bacaString(idUp, MAX_ID_LEN);

    cariNodeByID(idUp, &target);

    if (target == NULL) {
        PRINT("[ERROR] ID '");
        serialPrintStr(idUp);
        PRINTLN("' tidak ditemukan.");
        return;
    }

    PRINT("Nama barang  : ");
    serialPrintStr(target->nama);
    serialPrintLn();

    PRINT("Stok saat ini: ");
    serialPrintInt(target->stok);
    serialPrintLn();

    PRINT("Operasi + atau -: ");
    bacaString(op, 2);

    if (op[0] != '+' && op[0] != '-') {
        PRINTLN("[ERROR] Operator tidak valid.");
        return;
    }

    PRINT("Jumlah: ");
    bacaInt(&jml);

    if (jml < 0) {
        PRINTLN("[ERROR] Jumlah tidak boleh negatif.");
        return;
    }

    if (op[0] == '+') {
        stokHitung = (long)target->stok + jml;
    } else {
        stokHitung = (long)target->stok - jml;
    }

    if (stokHitung < 0) {
        PRINT("[ERROR] Stok tidak mencukupi. Tersedia: ");
        serialPrintInt(target->stok);
        PRINT(", diminta: ");
        serialPrintInt(jml);
        serialPrintLn();
        return;
    }

    if (stokHitung > 32767L) {
        PRINTLN("[ERROR] Nilai stok terlalu besar untuk Arduino Uno.");
        return;
    }

    terapkanStok(target, (int)stokHitung);
}

void updateStatus(void)
{
    char idUp[MAX_ID_LEN];
    int stBaru;
    Node *target;

    PRINTLN("\n[ UPDATE STATUS ]");

    if (head == NULL) {
        PRINTLN("[ERROR] Inventaris kosong.");
        return;
    }

    PRINT("ID barang: ");
    bacaString(idUp, MAX_ID_LEN);

    cariNodeByID(idUp, &target);

    if (target == NULL) {
        PRINT("[ERROR] ID '");
        serialPrintStr(idUp);
        PRINTLN("' tidak ditemukan.");
        return;
    }

    PRINT("Nama barang    : ");
    serialPrintStr(target->nama);
    serialPrintLn();

    PRINT("Status saat ini: ");
    cetakStatus(target->status);
    serialPrintLn();

    PRINTLN("0=Tersedia  1=Dipinjam  2=Rusak  3=Habis");
    PRINT("Status baru: ");
    bacaInt(&stBaru);

    if (stBaru < 0 || stBaru > 3) {
        PRINTLN("[ERROR] Status tidak valid. Masukkan angka 0-3.");
        return;
    }

    target->status = stBaru;

    if (stBaru == ST_HABIS && target->stok > 0) {
        target->stok = 0;
        PRINTLN("[WARN] Status Habis, stok otomatis diset menjadi 0.");
    }

    PRINT("Status '");
    serialPrintStr(target->nama);
    PRINT("' berhasil diubah menjadi ");
    cetakStatus(target->status);
    serialPrintLn();
}

void tampilRingkasan(void)
{
    Node *curr = head;
    int total = 0;
    int totalStok = 0;
    int cSedia = 0;
    int cPinjam = 0;
    int cRusak = 0;
    int cHabis = 0;
    int minStok = 32767;
    char minNama[MAX_NAMA_LEN];
    int persen;

    minNama[0] = '-';
    minNama[1] = '\0';

    PRINTLN("\n================================================");
    PRINTLN("       RINGKASAN INVENTARIS LABORATORIUM");
    PRINTLN("================================================");

    if (head == NULL) {
        PRINTLN(" Inventaris masih kosong.");
        PRINTLN("================================================");
        return;
    }

    while (curr != NULL) {
        total++;
        totalStok += curr->stok;

        switch (curr->status) {
            case ST_TERSEDIA:
                cSedia++;
                break;
            case ST_DIPINJAM:
                cPinjam++;
                break;
            case ST_RUSAK:
                cRusak++;
                break;
            case ST_HABIS:
                cHabis++;
                break;
            default:
                break;
        }

        if (curr->stok < minStok) {
            minStok = curr->stok;
            strncpy(minNama, curr->nama, MAX_NAMA_LEN - 1);
            minNama[MAX_NAMA_LEN - 1] = '\0';
        }

        curr = curr->next;
    }

    persen = (poolUsed * 100) / MAX_NODES;

    PRINT("  Jumlah item    : ");
    serialPrintInt(total);
    PRINT("/");
    serialPrintInt(MAX_NODES);
    serialPrintLn();

    PRINT("  Total stok     : ");
    serialPrintInt(totalStok);
    serialPrintLn();

    PRINTLN("------------------------------------------------");
    PRINTLN("  Status barang:");
    PRINT("    Tersedia : ");
    serialPrintInt(cSedia);
    serialPrintLn();
    PRINT("    Dipinjam : ");
    serialPrintInt(cPinjam);
    serialPrintLn();
    PRINT("    Rusak    : ");
    serialPrintInt(cRusak);
    serialPrintLn();
    PRINT("    Habis    : ");
    serialPrintInt(cHabis);
    serialPrintLn();

    PRINTLN("------------------------------------------------");
    PRINT("  Penggunaan memori: ");
    serialPrintInt(persen);
    PRINT("% (");
    serialPrintInt(poolUsed);
    PRINT("/");
    serialPrintInt(MAX_NODES);
    PRINTLN(" slot)");

    if (poolUsed >= WARN_THRESHOLD) {
        PRINTLN("  [WARN] Memori hampir penuh.");
    }

    PRINTLN("------------------------------------------------");
    PRINT("  Stok terkecil: ");
    serialPrintStr(minNama);
    PRINT(" (");
    serialPrintInt(minStok);
    PRINTLN(" unit)");

    if (minStok == 0) {
        PRINTLN("  [WARN] Ada barang dengan stok 0, segera restock.");
    }

    PRINTLN("================================================");
}

void cekMemori(void)
{
    if (poolUsed >= WARN_THRESHOLD) {
        PRINT("[WARN] Memori hampir penuh: ");
        serialPrintInt(poolUsed);
        PRINT("/");
        serialPrintInt(MAX_NODES);
        PRINTLN(" slot.");
    }
}
