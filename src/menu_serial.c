#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "inventaris.h"

char rxPushback = '\0';
unsigned char hasPushback = 0;

void uartAvailable(unsigned char *out)
{
    if (hasPushback != 0) {
        *out = 1;
    } else if ((UCSR0A & (1 << RXC0)) != 0) {
        *out = 1;
    } else {
        *out = 0;
    }
}

void uartReadChar(char *out)
{
    if (hasPushback != 0) {
        *out = rxPushback;
        hasPushback = 0;
        return;
    }

    while ((UCSR0A & (1 << RXC0)) == 0) {
        /* Menunggu input UART */
    }

    *out = (char)UDR0;
}

void uartWriteChar(char c)
{
    while ((UCSR0A & (1 << UDRE0)) == 0) {
        /* Menunggu buffer transmit kosong */
    }

    UDR0 = (unsigned char)c;
}

void flushLineEnding(void)
{
    unsigned char ada;
    char c;

    _delay_ms(2);

    while (1) {
        uartAvailable(&ada);

        if (ada == 0) {
            break;
        }

        uartReadChar(&c);

        if (c != '\n' && c != '\r') {
            rxPushback = c;
            hasPushback = 1;
            break;
        }
    }
}

void initHardware(void)
{
    unsigned int ubrr = 103;

    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;

    UCSR0A = 0;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void serialPrintFlash(const char *p)
{
    char c;

    while (1) {
        c = (char)pgm_read_byte(p);
        if (c == '\0') {
            break;
        }
        uartWriteChar(c);
        p++;
    }
}

void serialPrintStr(const char *s)
{
    while (*s != '\0') {
        uartWriteChar(*s);
        s++;
    }
}

void serialPrintChar(char c)
{
    uartWriteChar(c);
}

void serialPrintInt(int val)
{
    char buf[12];
    itoa(val, buf, 10);
    serialPrintStr(buf);
}

void serialPrintLn(void)
{
    uartWriteChar('\r');
    uartWriteChar('\n');
}

void bacaString(char *buf, int maks)
{
    int i = 0;
    char ch;

    while (1) {
        uartReadChar(&ch);

        if (ch == '\n' || ch == '\r') {
            flushLineEnding();
            break;
        }

        if (i < maks - 1) {
            buf[i] = ch;
            i++;
        }
    }

    buf[i] = '\0';

    serialPrintStr(buf);
    serialPrintLn();
}

void bacaInt(int *out)
{
    char buf[12];
    int val = 0;
    int i = 0;
    int neg = 0;

    bacaString(buf, 12);

    if (buf[0] == '-') {
        neg = 1;
        i = 1;
    }

    for (; buf[i] >= '0' && buf[i] <= '9'; i++) {
        val = val * 10 + (buf[i] - '0');
    }

    if (neg == 1) {
        *out = -val;
    } else {
        *out = val;
    }
}

void tampilMenu(void)
{
    PRINTLN("\n================================================");
    PRINTLN("            MENU INVENTARIS LAB");
    PRINTLN("================================================");
    PRINTLN("  1. Tambah Data Barang");
    PRINTLN("  2. Tampil Semua Data");
    PRINTLN("  3. Cari Data by ID");
    PRINTLN("  4. Hapus Data by ID");
    PRINTLN("  5. Update Stok");
    PRINTLN("  6. Update Status");
    PRINTLN("  7. Ringkasan Inventaris");
    PRINTLN("  0. Reset Sistem");
    PRINTLN("================================================");
    PRINT("Pilihan: ");
}

void prosesMenu(char pilihan)
{
    switch (pilihan) {
        case '1':
            tambahData();
            break;
        case '2':
            tampilSemua();
            break;
        case '3':
            cariDataByID();
            break;
        case '4':
            hapusDataByID();
            break;
        case '5':
            updateStok();
            break;
        case '6':
            updateStatus();
            break;
        case '7':
            tampilRingkasan();
            break;
        case '0':
            PRINTLN("[INFO] Reset sistem.");
            initSistem();
            break;
        default:
            PRINT("[ERROR] Pilihan '");
            serialPrintChar(pilihan);
            PRINTLN("' tidak valid.");
            break;
    }
}

void runProgram(void)
{
    char pilBuf[2];

    while (1) {
        bacaString(pilBuf, 2);

        if (pilBuf[0] != '\0') {
            prosesMenu(pilBuf[0]);
            tampilMenu();
        }
    }
}
