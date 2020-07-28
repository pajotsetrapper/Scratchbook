#define PIN_R 2 //Resistor 220 ohm (206 ideal)
#define PIN_G 3 //Resistor 180 ohm
#define PIN_B 4 //Resistor 127 ohm (120 ideal)

void setup()
{
pinMode(PIN_R, OUTPUT);
pinMode(PIN_R, OUTPUT);
pinMode(PIN_B, OUTPUT);
}

void loop() {
  int Red = random(120)+134;
  int Yellow = random(20)+20;
  analogWrite(PIN_R, Red);
  analogWrite(PIN_G, Yellow);
  analogWrite(PIN_B, Yellow);
  delay(random(200));
}
