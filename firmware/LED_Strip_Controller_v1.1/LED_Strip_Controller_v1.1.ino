#pragma region Info_Project
/*
  Скетч к проекту "Светодиодная лента WS2812 на Arduino"
  Автор: BouRHooD, 2023
*/

/*
   Версия 1.1
*/
#pragma endregion

#pragma region Settings_Arduino
// ***************************** НАСТРОЙКИ *****************************

// ----- настройка ИК пульта
#define REMOTE_TYPE 1           // 1 - кастомный пульт (пульт только с цветами (16 цветов))
// система может работать С ЛЮБЫМ ИК ПУЛЬТОМ (практически). Коды пультов определяются скетчем IRtest_2.0, читай инструкцию

// ----- настройки ленты
#define NUM_LEDS 60             // количество светодиодов (данная версия поддерживает до 410 штук)
#define CURRENT_LIMIT 3000      // лимит по току в МИЛЛИАМПЕРАХ, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит
#define DEF_BRIGHTNESS 200      // яркость по умолчанию (0 - 255)

// Пины для других плат Arduino (по умолчанию)
#define MLED_PIN 13             // пин светодиода режимов
#define MLED_ON HIGH
#define LED_PIN 12              // пин DI светодиодной ленты
#define IR_PIN 2                // пин ИК приёмника

// ----- настройки радуги
float RAINBOW_STEP = 5.00;        // шаг изменения цвета радуги
byte EMPTY_BRIGHT = 30;           // яркость "не горящих" светодиодов (0 - 255)
#define EMPTY_COLOR HUE_PURPLE    // цвет "не горящих" светодиодов. Будет чёрный, если яркость 0

int hue;

// ----- отрисовка
#define MODE 0                   // режим при запуске
#define MAIN_LOOP 5              // период основного цикла отрисовки (по умолчанию 5)

// ----- КНОПКИ СВОЕГО ПУЛЬТА (пульт только с цветами (16 цветов)) -----
#if REMOTE_TYPE == 1
#define BUTT_UP_Brightness     3745467309 
#define BUTT_DOWN_Brightness   644329901 
#define BUTT_OFF               3121868717 
#define BUTT_ON                354211501  
#define BUTT_Red               4035585709  
#define BUTT_Green             2218157229  
#define BUTT_Blue              2621819053 
#define BUTT_White             3525530541 
#define BUTT_FLASH             2728395181
#define BUTT_STROBE            2383393453  
#define BUTT_FADE              2269850029  
#define BUTT_SMOOTH            405904301  
#define BUTT_Color_1           1708102061  
#define BUTT_Color_2           856083373  
#define BUTT_Color_3           1759794861  
#define BUTT_Color_4           462609837  
#define BUTT_Color_5           2780087981  
#define BUTT_Color_6           3287104941  
#define BUTT_Color_7           1979731629  
#define BUTT_Color_8           3173561517  
#define BUTT_Color_9           2883443117  
#define BUTT_Color_10          696022701   
#define BUTT_Color_11          3577223341  
#define BUTT_Color_12          2831750317  
#endif 

// ------------------------------ ДЛЯ РАЗРАБОТЧИКОВ --------------------------------
#define MODE_AMOUNT 3      // Количество режимов (0, 1 ...)

// Библиотека для работы с светодиодной лентой
#define FASTLED_ALLOW_INTERRUPTS 1
#include "FastLED.h"
CRGB leds[NUM_LEDS];

// Библиотека для ИК пульта
#include "IRLremote.h"
CHashIR IRLremote;
uint32_t IRdata;

// Переменные для прерывания выполнения кода
unsigned long main_timer, rainbow_timer; 

// Системные переменные
byte this_mode = MODE;       // Режим работы ленты
volatile boolean ir_flag;    // Флаги программы

byte CURRENT_BRIGHTNESS = DEF_BRIGHTNESS;
// ------------------------------ ДЛЯ РАЗРАБОТЧИКОВ --------------------------------
#pragma endregion

#pragma region Setup_Arduino
void setup() {
  // Инициализируем последовательный порт
  Serial.begin(9600);

  // Инициализируем светодиодную ленту
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  FastLED.setBrightness(CURRENT_BRIGHTNESS);

  pinMode(MLED_PIN, OUTPUT);        //Режим пина для светодиода режима на выход
  digitalWrite(MLED_PIN, !MLED_ON); //Выключение светодиода режима

  // Инициализируем ИК пульт
  IRLremote.begin(IR_PIN);
}
#pragma endregion

#pragma region Loop_Arduino
void loop() {
  remoteTick();     // Опрос ИК пульта
  mainLoop();       // Главный цикл обработки и отрисовки
}
#pragma endregion

#pragma region Methods_Arduino
void mainLoop() {
  if (millis() - main_timer > MAIN_LOOP) {
      switch (this_mode) {
        case 0:
          break;
        case 1:
          byte count = 0; int Llenght = 30; int Rlenght = 30;
          int MAX_CH = NUM_LEDS / 2;
          float index = (float)255 / MAX_CH; // коэффициент перевода для палитры
          
          if (millis() - rainbow_timer > 30) {
            rainbow_timer = millis();
            hue = floor((float)hue + RAINBOW_STEP);
          }

          count = 0;
          for (int i = (MAX_CH - 1); i > ((MAX_CH - 1) - Rlenght); i--) {
            leds[i] = ColorFromPalette(RainbowColors_p, (count * index) / 2 - hue);  // заливка по палитре радуга
            count++;
          }
          count = 0;
          for (int i = (MAX_CH); i < (MAX_CH + Llenght); i++ ) {
            leds[i] = ColorFromPalette(RainbowColors_p, (count * index) / 2 - hue); // заливка по палитре радуга
            count++;
          }

          // Если на ИК приёмник не приходит сигнал, то отправляем значение на ленту (без этого НЕ РАБОТАЕТ!)
          if (!IRLremote.receiving()) { FastLED.show(); }
          FastLED.clear();          // очистить массив пикселей
          break;
        case 2:
          break;
      }
      // Сбрасываем значение таймера
      main_timer = millis();
    }
}

#if REMOTE_TYPE != 0
void remoteTick() {
  // Проверка на вхождение данных
  if (IRLremote.available())  {
    auto data = IRLremote.read();
    IRdata = data.command;

    if (data.command != 292984781) {
      ir_flag = true;
    }
  }

  // Если данные пришли
  if (ir_flag) {
    switch (IRdata) {
      // Режимы работы ленты
      case BUTT_Red: 
        SetCHSVOnFullLedStrip(HUE_RED, 255, 255);
        break;
      case BUTT_Green: 
        SetCHSVOnFullLedStrip(HUE_GREEN, 255, 255);
        break;
      case BUTT_Blue: 
        SetCHSVOnFullLedStrip(HUE_BLUE, 255, 255);
        break;
      case BUTT_White: 
        SetCHSVOnFullLedStrip(100, 0, 255);
        break;
      case BUTT_UP_Brightness:
        // В 1 режиме изменяется глобальня яркость
        if (this_mode == 0){ 
          CURRENT_BRIGHTNESS = SmartIncr(CURRENT_BRIGHTNESS, 20, 0, 255);
          Serial.print(F("CURRENT_BRIGHTNESS = ")); Serial.println(CURRENT_BRIGHTNESS);
          FastLED.setBrightness(CURRENT_BRIGHTNESS);
        }
        
        // В других режимах изменяется глобальня скорость
        if (this_mode != 1){ 
          RAINBOW_STEP = SmartIncrFloat(RAINBOW_STEP, 0.5, 0, 20);
          Serial.print(F("RAINBOW_STEP = ")); Serial.println(RAINBOW_STEP);
        }

        if (!IRLremote.receiving()) { FastLED.show(); } 
        break;
      case BUTT_DOWN_Brightness:
        // В 1 режиме изменяется глобальня яркость
        if (this_mode == 0){  
          CURRENT_BRIGHTNESS = SmartIncr(CURRENT_BRIGHTNESS, -20, 0, 255);
          Serial.print(F("CURRENT_BRIGHTNESS = ")); Serial.println(CURRENT_BRIGHTNESS);
          FastLED.setBrightness(CURRENT_BRIGHTNESS);
        }

        // В других режимах изменяется глобальня скорость
        if (this_mode != 1){ 
          RAINBOW_STEP = SmartIncrFloat(RAINBOW_STEP, -0.5, 0, 20);
          Serial.print(F("RAINBOW_STEP = ")); Serial.println(RAINBOW_STEP);
        }

        if (!IRLremote.receiving()) { FastLED.show(); }   
        break;
      case BUTT_OFF: 
        FastLED.setBrightness(0);
        if (!IRLremote.receiving()) { FastLED.show(); }   
        break;
      case BUTT_ON: 
        FastLED.setBrightness(DEF_BRIGHTNESS);
        if (!IRLremote.receiving()) { FastLED.show(); }   
        break;
      case BUTT_Color_1:
        SetCHSVOnFullLedStrip(map(6, 0, 360, 0, 255), map(65, 0, 100, 0, 255), 255);
        break;
      case BUTT_Color_2:
        SetCHSVOnFullLedStrip(map(157, 0, 360, 0, 255), map(99, 0, 100, 0, 255), 255);
        break;
      case BUTT_Color_3:
        SetCHSVOnFullLedStrip(map(205, 0, 360, 0, 255), map(92, 0, 100, 0, 255), 255);
        break;
      case BUTT_Color_4:
        SetCHSVOnFullLedStrip(map(14, 0, 360, 0, 255), map(69, 0, 100, 0, 255), 255);
        break;
      case BUTT_Color_5:
        SetCHSVOnFullLedStrip(map(185, 0, 360, 0, 255), map(100, 0, 100, 0, 255), 255);
        break;
      case BUTT_Color_6:
        SetCHSVOnFullLedStrip(map(310, 0, 360, 0, 255), map(46, 0, 100, 0, 255), 255);
        break;
      case BUTT_Color_7:
        SetCHSVOnFullLedStrip(map(24, 0, 360, 0, 255), map(78, 0, 100, 0, 255), 255);
        break;
      case BUTT_Color_8:
        SetCHSVOnFullLedStrip(map(181, 0, 360, 0, 255), map(100, 0, 100, 0, 255), 255);
        break;
      case BUTT_Color_9:
        SetCHSVOnFullLedStrip(map(292, 0, 360, 0, 255), map(48, 0, 100, 0, 255), 255);
        break;
      case BUTT_Color_10:
        SetCHSVOnFullLedStrip(map(52, 0, 360, 0, 255), map(82, 0, 100, 0, 255), 255);
        break;
      case BUTT_Color_11:
        SetCHSVOnFullLedStrip(map(180, 0, 360, 0, 255), map(86, 0, 100, 0, 255), 255);
        break;
      case BUTT_Color_12:
        SetCHSVOnFullLedStrip(map(336, 0, 360, 0, 255), map(74, 0, 100, 0, 255), 255);
        break;
      case BUTT_FLASH:
        if (++this_mode >= MODE_AMOUNT) this_mode = 0;   // Изменить режим на +1
        break;
    }
    Serial.print(F("IRdata = ")); Serial.print(IRdata); Serial.print(F(" this_mode = ")); Serial.println(this_mode);
    ir_flag = false;
  }
}
#endif

// Устанавливаем значение на всю ленту
void SetCHSVOnFullLedStrip(byte hue, byte saturation, byte brightness) {
  // Устанавливаем режим однотонного цвета
  this_mode = 0;

  // Меняем цвет
  for (int i = 0; i < NUM_LEDS; i++) {
    auto setColor = CHSV(hue, saturation, brightness);
    if (leds[i] != setColor){ leds[i] = setColor; }
  }

  // Если на ИК приёмник не приходит сигнал, то отправляем значение на ленту (без этого НЕ РАБОТАЕТ!)
  if (!IRLremote.receiving()) { FastLED.show(); }         
}

// Вспомогательная функция, изменяет величину value на шаг incr в пределах minimum ... maximum
int SmartIncr(int value, int incr_step, int mininmum, int maximum) {
  int val_buf = value + incr_step;
  val_buf = constrain(val_buf, mininmum, maximum);
  return val_buf;
}

float SmartIncrFloat(float value, float incr_step, float mininmum, float maximum) {
  float val_buf = value + incr_step;
  val_buf = constrain(val_buf, mininmum, maximum);
  return val_buf;
}
#pragma endregion
