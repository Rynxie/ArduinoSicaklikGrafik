const int termistorPin = A0;

void setup() {
  Serial.begin(9600); //seri monitor baslatiliyor
}
unsigned long eskiZaman=0;//zaman takibi icin gerekli degiskenler
unsigned long yeniZaman;
unsigned long saniye = 0;

void loop() {
  yeniZaman = millis(); //program calismaya basladagi andan itibaren sayan milis fonksiyonu
  
  int adcValue = analogRead(termistorPin);
  float gerilim = adcValue * (5.0 / 1023.0); // NTC icin gerekli hesaplamalar
  float direnc = (5.0 * 10000.0 / gerilim) - 10000.0;
  float sicaklik = 1.0 / ((log(direnc / 10000.0) / 3950.0) + (1.0 / 298.15)) - 273.15;

  if(yeniZaman - eskiZaman > 1000){
      saniye++; //her 1000 ms de saniye degeri 1 artıyor
      yeniZaman = eskiZaman;
        Serial.print(saniye); //daha sonra c programinda kullanilmak uzere programda tanimlanan formatta veriler porta yollaniyor
        Serial.print(" ");
        Serial.print(sicaklik);
        Serial.println();
    }
    
 
  

  delay(1000);
}
