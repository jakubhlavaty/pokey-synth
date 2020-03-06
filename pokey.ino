// addresses of registers (AUDF) that set channel frequencies (pitch)
const byte REGISTER_FREQ_01 = 0;
const byte REGISTER_FREQ_02 = 2;
const byte REGISTER_FREQ_03 = 4;
const byte REGISTER_FREQ_04 = 6;
const byte REGISTERS_FREQ[4] = { REGISTER_FREQ_01, REGISTER_FREQ_02, REGISTER_FREQ_03, REGISTER_FREQ_04 };

// addresses of registers (AUDC) that set channel sound
const byte REGISTER_SND_01 = 1;
const byte REGISTER_SND_02 = 3;
const byte REGISTER_SND_03 = 5;
const byte REGISTER_SND_04 = 7;
const byte REGISTERS_SND[4] = { REGISTER_SND_01, REGISTER_SND_02, REGISTER_SND_03, REGISTER_SND_04 };

// address of the AUDCTL register
const byte REGISTER_AUDCTL = 0x08;

// address of the SKCTL register
const byte REGISTER_SKCTL = 0x0F;

const byte PORTB_MASK = B00011110;

// chip select pin number
const byte CS_PIN = 13;

// wait time between pulses
const byte CS_WAIT_TIME = 1;

const byte SOUND_NOISE_01 = 0;
const byte SOUND_NOISE_02 = 1;
const byte SOUND_NOISE_03 = 2;
const byte SOUND_NOISE_04 = 4;
const byte SOUND_NOISE_05 = 6;
const byte SOUND_CLEAN = 5;
const byte SOUNDS[6] = { SOUND_CLEAN, SOUND_NOISE_05, SOUND_NOISE_04, SOUND_NOISE_03, SOUND_NOISE_02, SOUND_NOISE_01 };


void setup() {
  // set pin directions
  // POKEY D0-D7 (Arduino Nano PD0-PD7/D0-D7)
//  DDRD = B11111111;
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(A6, OUTPUT);
  pinMode(A7, OUTPUT);

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  
  // POKEY A0-A3 (Arduino Nano PB1-PB4/D9-D12)
  //DDRB = DDRB | B00011110;

  // set CS pin mode (inverted logic for this one)
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  // put POKEY in and out of reset state
  writeDataToRegister(REGISTER_SKCTL, 0);
  writeDataToRegister(REGISTER_SKCTL, 3);
  
  // init AUDCTL
  writeDataToRegister(REGISTER_AUDCTL, 0);
  
  muteAllChannels();
}

void loop() {
  for (int s=0; s<6; s++) {
    playSound(1, 0x1D, SOUNDS[S], 8);
    delay(1000);
    playSound(2, 0x2F, SOUNDS[S], 8);
    delay(1000);
    playSound(3, 0x51, SOUNDS[S], 8);
    delay(1000);
    playSound(4, 0x79, SOUNDS[S], 8);
    delay(1000);
    muteAllChannels();
  }
  
}

void writeDataToRegister(byte registerAddress, byte data) {
  setPOKEYAddress(registerAddress);
  setPOKEYData(data);
  writeToPOKEY();
}

void setPOKEYAddress(byte registerAddress) {
  //PORTB = PORTB | (PORTB_MASK & (registerAddress << 1));
  for (int i = 9, j = 0; i <= 12, j <=3; i++, j++) {
    digitalWrite(i, pinOutputValue(j, registerAddress));
  }
  
}

int pinOutputValue(byte pinNumber, byte data) {
  return (((0x01 << pinNumber) & data) >> pinNumber) ? HIGH : LOW;
}

void setPOKEYData(byte data) {
//  PORTD = data;
  for (int i = 0; i < 8; i++) {
    digitalWrite(A0 | i, pinOutputValue(i, data));
  }
}

void writeToPOKEY() {
    digitalWrite(CS_PIN, LOW);
    delayMicroseconds(CS_WAIT_TIME);
    digitalWrite(CS_PIN, HIGH);
}

void playSound(byte channel, byte frequency, byte soundTimbre, byte soundVolume) {
  // first set frequency
  writeDataToRegister(REGISTERS_FREQ[channel - 1], frequency);
  
  // than the rest (TTTxVVVV)
  writeDataToRegister(REGISTERS_SND[channel - 1], (soundTimbre << 5) | (B00001111 & soundVolume));
}

void mute_channel(byte channel) {
  writeDataToRegister(REGISTERS_SND[channel - 1], 0);
}  

void muteAllChannels() {
  for (int i=1; i<=4; i++) {
    muteChannel(i);
  }
}
