#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

#define BAUDRATE CBR_9600
#define SERIAL_PORT "\\\\.\\COM3"  // Arduino'nun bağlı olduğu seri portun adını doğru şekilde ayarlayın

HANDLE openSerialPort() { //Seri Portu açan ve ayarlarini yapan fonksiyonlar
    HANDLE serialPort = CreateFile(SERIAL_PORT, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (serialPort == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Hata: Seri port acilamadi\n");
        exit(1);
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(serialPort, &dcbSerialParams)) {
        fprintf(stderr, "Hata: Seri port ayarlari alinamadi\n");
        CloseHandle(serialPort);
        exit(1);
    }

    dcbSerialParams.BaudRate = BAUDRATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.StopBits = ONESTOPBIT;

    if (!SetCommState(serialPort, &dcbSerialParams)) {
        fprintf(stderr, "Hata: Seri port ayarlari yapilamadi\n");
        CloseHandle(serialPort);
        exit(1);
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;

    if (!SetCommTimeouts(serialPort, &timeouts)) {
        fprintf(stderr, "Hata: Seri port zaman asimi ayarlari yapilamadi\n");
        CloseHandle(serialPort);
        exit(1);
    }

    return serialPort;
}

int main() {
    HANDLE serialPort = openSerialPort(); //Seriportu acan fonksiyonun cagrilmasi
    DWORD bytesRead;
    char buffer[256];
    FILE *data; //gelen verilerin tutlacagi dosya
    FILE *gnuplot; //grafik programina komut gondere dosya
    gnuplot = _popen("gnuplot -persist","w"); 
    data = fopen("data.txt", "w"); //gelen verileri tutan dosyanın yaratilmasi
    if(gnuplot == NULL){
        printf("Hata: GNUPlot'a Ulasilamiyor");
    }else{
        printf("GNUPlot calisiyor...");
    }
    fprintf(gnuplot,"set term qt persist\n"); //terminal kapansa bile gnuplati acik tutan program komutu
    
    while (1) {
        if (ReadFile(serialPort, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {//seri portta veri varsa true donduren if ifadesi
            
            buffer[bytesRead] = '\0';//tum bytlar okunduktan sonra veriyi NULL ile kapatıyoruz
            
            fprintf(data, "%s", buffer);//veriyi kapattıktan sonra dosyaya yazdırıyoruz
            fflush(data); //dosyanın real-time guncellenmesi icin onbellekten temzilyoruz
            fflush(stdout);

            fprintf(gnuplot, "plot 'data.txt' with lines\n"); //grafik programina dosyadaki verileri cizmesi icin komut gonderiyoruz
            
            fflush(gnuplot); //grafik programinin guncel sekilde veri okumasi icin onbellekten siliyoruz

            

            
        }

        Sleep(1);
    }

    CloseHandle(serialPort);//seri portu kapatıyoruz

    return 0;
}
//codespace denemesi