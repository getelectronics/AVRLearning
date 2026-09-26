#define F_CPU 3333333UL // Default ATmega4809 clock speed (20MHz / 6)
#include <avr/io.h>
#include <util/delay.h>

// I2C 7-Bit Device Addresses
#define SHT40_I2C_ADDR   0x44  // Fixed 7-bit address for SHT40
#define SSD1306_I2C_ADDR 0x3D  // Standard address for SSD1306 OLED

// SHT40 Commands
#define SHT40_CMD_MEAS_HIGH_PREC 0xFD // High precision measurement trigger

// Onboard LED Definition (PF5 on ATmega4809 Nano)
#define HEARTBEAT_LED_MASK PIN5_bm

// --- Font Table (ASCII 32 to 90: ' ' through 'Z') ---
const uint8_t FONT_5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // ' '
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // '!'
    {0x00, 0x07, 0x00, 0x07, 0x00}, // '"'
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // '#'
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // '$'
    {0x23, 0x13, 0x08, 0x64, 0x62}, // '%'
    {0x36, 0x49, 0x55, 0x22, 0x50}, // '&'
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '\''
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // '('
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // ')'
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // '*'
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // '+'
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ','
    {0x08, 0x08, 0x08, 0x08, 0x08}, // '-'
    {0x00, 0x60, 0x60, 0x00, 0x00}, // '.'
    {0x20, 0x10, 0x08, 0x04, 0x02}, // '/'
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // '0'
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // '1'
    {0x42, 0x61, 0x51, 0x49, 0x46}, // '2'
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // '3'
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // '4'
    {0x27, 0x45, 0x45, 0x45, 0x39}, // '5'
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // '6'
    {0x01, 0x71, 0x09, 0x05, 0x03}, // '7'
    {0x36, 0x49, 0x49, 0x49, 0x36}, // '8'
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // '9'
    {0x00, 0x36, 0x36, 0x00, 0x00}, // ':'
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ';'
    {0x08, 0x14, 0x22, 0x41, 0x00}, // '<'
    {0x14, 0x14, 0x14, 0x14, 0x14}, // '='
    {0x00, 0x41, 0x22, 0x14, 0x08}, // '>'
    {0x02, 0x01, 0x51, 0x09, 0x06}, // '?'
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // '@'
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 'A'
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 'B'
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 'C'
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 'D'
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 'E'
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // 'F'
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // 'G'
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 'H'
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 'I'
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 'J'
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 'K'
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 'L'
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // 'M'
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 'N'
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 'O'
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 'P'
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 'Q'
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 'R'
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 'S'
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 'T'
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 'U'
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 'V'
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // 'W'
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 'X'
    {0x07, 0x08, 0x70, 0x08, 0x07}, // 'Y'
    {0x61, 0x51, 0x49, 0x45, 0x43}  // 'Z'
};

// --- (I2C) ---
void TWI0_Init(void) {
    PORTA.DIRCLR = PIN2_bm | PIN3_bm;
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm; // Enable pull-up on PA2 (SDA)
    PORTA.PIN3CTRL = PORT_PULLUPEN_bm; // Enable pull-up on PA3 (SCL)

    TWI0.MBAUD = 12; // ~100 kHz @ 3.33MHz system clock
    TWI0.MCTRLA = TWI_ENABLE_bm;
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc; // Set bus state to IDLE
}

uint8_t TWI0_Start(uint8_t addr_rw) {
    TWI0.MADDR = addr_rw;
    while (!(TWI0.MSTATUS & (TWI_RIF_bm | TWI_WIF_bm)));

    if (TWI0.MSTATUS & TWI_RXACK_bm) { // Check for NACK
        TWI0.MCTRLB = TWI_MCMD_STOP_gc;
        return 0;
    }
    return 1;
}

uint8_t TWI0_Write(uint8_t data) {
    TWI0.MDATA = data;
    while (!(TWI0.MSTATUS & TWI_WIF_bm));

    if (TWI0.MSTATUS & TWI_RXACK_bm) {
        return 0; // NACK
    }
    return 1; // ACK
}

uint8_t TWI0_Read(uint8_t ack) {
    while (!(TWI0.MSTATUS & TWI_RIF_bm));
    uint8_t data = TWI0.MDATA;

    if (ack) {
        TWI0.MCTRLB = TWI_MCMD_RECVTRANS_gc;
    } else {
        TWI0.MCTRLB = TWI_ACKACT_NACK_gc | TWI_MCMD_STOP_gc;
    }
    return data;
}

void TWI0_Stop(void) {
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}

// --- SHT40 Temperature & Humidity  ---
uint8_t SHT40_ReadData(float *tempC, float *humidityRH) {
    // 1. Send high precision measurement command
    if (!TWI0_Start((SHT40_I2C_ADDR << 1) | 0)) return 0;
    if (!TWI0_Write(SHT40_CMD_MEAS_HIGH_PREC)) return 0;
    TWI0_Stop();

    // 2. Delay for conversion execution (~10ms)
    _delay_ms(10);

    // 3. Read back 6 bytes (Temp MSB, LSB, CRC, RH MSB, LSB, CRC)
    if (!TWI0_Start((SHT40_I2C_ADDR << 1) | 1)) return 0;

    uint8_t rxBuf[6];
    for (uint8_t i = 0; i < 5; i++) {
        rxBuf[i] = TWI0_Read(1); // Read with ACK
    }
    rxBuf[5] = TWI0_Read(0);     // Read final byte with NACK & STOP

    // Construct 16-bit raw values
    uint16_t rawTemp = ((uint16_t)rxBuf[0] << 8) | rxBuf[1];
    uint16_t rawRH   = ((uint16_t)rxBuf[3] << 8) | rxBuf[4];

    // Calculate physical values using SHT40 datasheets formulas
    *tempC = -45.0f + (175.0f * ((float)rawTemp / 65535.0f));
    *humidityRH = -6.0f + (125.0f * ((float)rawRH / 65535.0f));

    // Clamp relative humidity bounds (0% to 100%)
    if (*humidityRH < 0.0f)   *humidityRH = 0.0f;
    if (*humidityRH > 100.0f) *humidityRH = 100.0f;

    return 1;
}

// --- SSD1306 OLED Display  ---
void SSD1306_Command(uint8_t cmd) {
    TWI0_Start((SSD1306_I2C_ADDR << 1) | 0);
    TWI0_Write(0x00);
    TWI0_Write(cmd);
    TWI0_Stop();
}

void SSD1306_Data(uint8_t data) {
    TWI0_Start((SSD1306_I2C_ADDR << 1) | 0);
    TWI0_Write(0x40);
    TWI0_Write(data);
    TWI0_Stop();
}

void SSD1306_Init(void) {
    _delay_ms(100);
    SSD1306_Command(0xAE); 
    SSD1306_Command(0xD5); SSD1306_Command(0x80);
    SSD1306_Command(0xA8); SSD1306_Command(0x3F);
    SSD1306_Command(0xD3); SSD1306_Command(0x00);
    SSD1306_Command(0x40);
    SSD1306_Command(0x8D); SSD1306_Command(0x14); 
    SSD1306_Command(0x20); SSD1306_Command(0x00);
    SSD1306_Command(0xA1);
    SSD1306_Command(0xC8);
    SSD1306_Command(0xDA); SSD1306_Command(0x12);
    SSD1306_Command(0x81); SSD1306_Command(0xCF);
    SSD1306_Command(0xD9); SSD1306_Command(0xF1);
    SSD1306_Command(0xDB); SSD1306_Command(0x40);
    SSD1306_Command(0xA4);
    SSD1306_Command(0xA6);
    SSD1306_Command(0xAF); 
}

void SSD1306_Clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        SSD1306_Command(0xB0 + page);
        SSD1306_Command(0x00);
        SSD1306_Command(0x10);
        for (uint8_t col = 0; col < 128; col++) {
            SSD1306_Data(0x00);
        }
    }
}

void SSD1306_DrawChar(char c, uint8_t page, uint8_t col) {
    if (c >= 'a' && c <= 'z') c -= 32;
    if (c < 32 || c > 90) c = ' ';

    uint8_t font_index = c - 32;

    SSD1306_Command(0xB0 + page);
    SSD1306_Command(0x00 + (col & 0x0F));
    SSD1306_Command(0x10 + ((col >> 4) & 0x0F));

    for (uint8_t i = 0; i < 5; i++) {
        SSD1306_Data(FONT_5x7[font_index][i]);
    }
    SSD1306_Data(0x00);
}

void SSD1306_DrawString(const char *str, uint8_t page, uint8_t col) {
    while (*str && col < 122) {
        SSD1306_DrawChar(*str++, page, col);
        col += 6;
    }
}

// Convert float value to formatted string (e.g. 23.45 -> "23.45")
void Format_Value_String(float val, char *buffer, const char *unit) {
    int whole = (int)val;
    int decimal = (int)((val - (float)whole) * 100.0f);
    if (decimal < 0) decimal = -decimal;

    buffer[0] = (whole / 10) + '0';
    buffer[1] = (whole % 10) + '0';
    buffer[2] = '.';
    buffer[3] = (decimal / 10) + '0';
    buffer[4] = (decimal % 10) + '0';
    buffer[5] = ' ';
    buffer[6] = unit[0];
    buffer[7] = unit[1];
    buffer[8] = '\0';
}

int main(void) {
    // Configure PF5 as Heartbeat LED (Active Low)
    PORTF.DIRSET = HEARTBEAT_LED_MASK;
    PORTF.OUTSET = HEARTBEAT_LED_MASK;

    TWI0_Init();
    SSD1306_Init();
    SSD1306_Clear();

    SSD1306_DrawString("ATMEGA4809 NANO", 0, 16);
    SSD1306_DrawString("SHT40 SENSOR", 2, 28);

    float tempC = 0.0f;
    float humidity = 0.0f;
    char textBuf[10];

    while (1) {
        if (SHT40_ReadData(&tempC, &humidity)) {
            // Render Temperature
            Format_Value_String(tempC, textBuf, "C ");
            SSD1306_DrawString("TEMP: ", 5, 10);
            SSD1306_DrawString(textBuf, 5, 52);

            // Render Humidity
            Format_Value_String(humidity, textBuf, "% ");
            SSD1306_DrawString("HUM:  ", 7, 10);
            SSD1306_DrawString(textBuf, 7, 52);

            PORTF.OUTTGL = HEARTBEAT_LED_MASK; // Toggle onboard LED
        }

        _delay_ms(1000);
    }

    return 0;
}