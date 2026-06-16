extern "C" {
    void initHardware(void);
    void initSistem(void);
    void tampilMenu(void);
    void runProgram(void);
}

void setup() {
    initHardware();
    initSistem();
    tampilMenu();
}

void loop() {
    runProgram();
}