/*
    РОБОТЫ и НАБОРЫ ПО РОБОТОТЕХНИКЕ на МРобот! mrobot.by
    http://www.mrobot.by

    Кухня Роботов <maxxlife>
    http://www.vk.com/cookrobot
    Copyright (c) Кухня роботов <maxxlife.by> All rights reserved.
    Copyright (c) Макслайф Робот Maxxlife Robot All rights reserved.
    Наши лаборатории по робототехнике:
    Ленинская РОС ДОСААФ, ул. Рокоссовского 63/2, Минск, Беларусь
    Подробнее в нашей группе Вконтакте http://www.vk.com/cookrobot
    И на сайте http://www.maxxlife.by
    ****************************************************
    Мы всегда рады новым членам нашего сообщества Кухня Роботов.
    У нас есть вводные курсы, где мы объясняем
    как работать с нашими образовательными наборами по робототехнике и электронике.
    ****************************************************
    Название набора: РобоРовер М1 2WD
    Программа создана и протестирована разработчиком:
    Имя: Максим Массальский
    Ник: maxxlife
    E-mail: maxxliferobot@gmail.com
*/

/*В роботе используются пины (для Arduino Uno-подобной платы)
 * Мотордрайвер использует пины:
 * D2, D3, D5, D8, D10, D11
 * Сервопривод использует пины:
 * D9
 * Ультразвуковой датчик использует пины:
 * D4, D7
 * Свободные цифровые пины:
 * D0, D1, D6, D12, D13
 * Аналоговые ИК-датчики используют пины:
 * A0, A1
 * Свободные аналоговые пины:
 * A2, A3, A4, A5
 */

//Самодельный класс Servo, чтобы не использовать стандартную библиотеку Servo
//Т.к. она мешает управлению пинами мотордрайвера L298N
class Servo // имя класса
{
  public:
    int write(int data)
    {
      analogWrite(9, data);
    }
};
//Создаем экземпляр класса Servo под именем radar
Servo radar;

//Пины ультразвукового датчика
int trigPin = 7;
int echoPin = 4;

//Константа voltconst -это значение опорного напряжения в милливольтах
//Нужна для того, чтобы переводить значение напряжения с ИК-датчика в расстояние.
double voltconst = 0.0048;

//Определение номеров пинов мотордрайвера
int enA = 5;
int in1 = 2;
int in2 = 8;
// motor two
int enB = 3;
int in3 = 11;
int in4 = 10;

//Переменные по хранению расстояний с переднего левого и правого ИК-датчиков
volatile int rfront = 0;
volatile int lfront = 0;

// Поворотный сервопривод radar
volatile int posr;
//Значение для угла в 90 градусов (от 0 до 255)
int radarfront = 185;
//Значение для левого положения
int radarleft = 120;
//Значение для правого положения
int radarright = 250;

// Ультразвуковой сенсор
//Переменная по хранению расстояния с УЗ-датчика
volatile long USrange = 0;
//Максимальный порог
int UShigh = 100;
//Минимальный порог
int USlow = 40;
volatile long USrangeleft = 0;
volatile long USrangeright = 0;

// Датчики инфракрасные Передние
//double anglefront=cos((30*PI)/180);// Угол 30 градусов
float anglefront = 1;
//Левый ИК-датчик подключен к аналоговому пину 0
int irpinfrontleft = 0;
//Правый ИК-датчик подключен к аналоговому пину 1
int irpinfrontright = 1;

//Переменные по настройке алгоритма передних ИК-датчиков под условия и расстояния
//Максимальный порог
int fronthigh = 40;
//Минимальный порог
int frontlow = 20;

//Класс для измерения расстояния с ИК-датчиков робота
class IR_Sharp_Measure
{
  public:
    int irmeasure(int irpin)
    {
      int distance;
      // Измеряем значение с датчика в милливольтах.
      //Значение, которое было считано с аналогового порта умножается на переменную voltconst
      //И получается значение в вольтах, которое есть на аналоговом пине
      double volts = analogRead(irpin) * voltconst;

      //Serial.println(volts);
      //Экспериментальные значения в вольтах для определенного расстояния
      //Определены экспериментальным путем и с использованием даташита к ИК-датчику Sharp (20-150cm)
      //Диапазон можно расширить до 150 см, т.к. сейчас только до 110 см. Можешь доделать сам:)
      double val[46] = {
        2.53, 2.43, 2.31, 2.17, 2.05, 1.95, 1.84, 1.73, 1.66, 1.57, 1.47, 1.42, 1.34, 1.29, 1.23, 1.18, 1.14, 1.10, 1.05, 1.01, 0.99, 0.96, 0.93, 0.90, 0.87, 0.86, 0.84, 0.80, 0.78, 0.76, 0.74, 0.73, 0.71, 0.70, 0.68, 0.67, 0.65, 0.63, 0.62, 0.61, 0.59, 0.57, 0.56, 0.55, 0.53, 0.52
      };
      int cm[46] = {
        20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110
      };
      //Алгоритм по сопоставлению значения в вольтах расстоянию
      for (int i = 0; i < 46; i++)
      {
        if (val[i] <= volts)
        {
          //Находим расстояние
          //Если датчик находится под углом, то можно узнать расстояние до объекта по теореме Пифагора, если это необходимо
          //Умножив на anglefront
          distance = cm[i] * anglefront;
          break;
        }
        //Если volts меньше определенного порога, то считаем, что расстояние больше 120 см
        if (volts < 0.52)
        {
          distance = 120 * anglefront;
        }
      }
      //Если volts больше определенного порогоа, то считаем, что расстояние меньше 10 см
      if (volts > 2.53)
      {
        distance = 10 * anglefront;
      }
      //Возвращаем значение distance
      return distance;
    }
};
//Создаем экземпляр класса под именем IR
IR_Sharp_Measure IR;

//Функция по измерению расстояний с переднего левого и правого ИК-датчиков
void IRMeasure()
{
  //Меряем расстояние с правого ИК
  rfront = IR.irmeasure(irpinfrontright);
  //Меряем расстояние с левого ИК
  lfront = IR.irmeasure(irpinfrontleft);
}

//Функция по измерению расстояния с УЗ-датчика HCSR-04
void USMeasure () {

  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  USrange = (duration / 2) / 29.1;
  delay(50);
}

//Функция по выезду из тупика при помощи ИК-датчиков и УЗ-датчика
void IrfrontGoBack()
{
  //Если с двух передних ИК-датчиков расстояния одинаковые,
  //То используем ультразвуковой датчик
  if (lfront == rfront)
  {
    //Поворачиваем сначала УЗ-датчик вправо
    radar.write(radarright);
    delay(200);
    //Делаем измерением
    USMeasure();
    //Записываем расстояние справа в отдельную переменную
    USrangeright = USrange;
    //Поворачиваем влево
    radar.write(radarleft);
    delay(200);
    //Делаем измерение
    USMeasure();
    //Записываем расстояние слева в отдельную переменную
    USrangeleft = USrange;
    //Если расстояние слева меньше расстояния справа
    if (USrangeleft < USrangeright)
    {
      //То поворачиваем вправо
      go_right_full();
      delay(250);
      go_stop();
      //Поворачиваем УЗ влево
      radar.write(radarleft);

      //Едем назад до тех пор пока расстояния с ИК-датчиков не будут удовлетворять условиям
      //Можешь настраивать как тебе необходимо
      while ((rfront < frontlow + 15) && (lfront < frontlow + 15))
      {
        IRMeasure();
        go_backward();
      }
      //Едем назад, чтобы и с переднего УЗ-датчика, расстояние было больше необходимого
      USMeasure();
      while ( USrange < 30)
      {
        USMeasure();
        go_backward();
      }
      //Поворачиваем робота вправо
      go_right_full();
      delay(500);
      go_stop();
    }
    //Здесь как и выше описано, только все операции наоборот.
    if (USrangeright < USrangeleft)
    {
      go_left_full();
      delay(250);
      go_stop();
      radar.write(radarright);
      while ((rfront < frontlow + 15) && (lfront < frontlow + 15))
      {
        IRMeasure();
        go_backward();
      }
      USMeasure();
      while (USrange < 30)
      {
        USMeasure();
        go_backward();
      }
      go_left_full();
      delay(500);
      go_stop();
    }
    //Если у нас произошло, что и УЗ-датчика одинаковое расстояние(!), робот просто едет назад
    //До тех пор пока не будут удовлетворены условия
    if (USrangeright == USrangeleft)
    {
      radar.write(radarfront);
      while ((rfront < frontlow + 15) && (lfront < frontlow + 15))
      {
        IRMeasure();
        go_backward();
      }
      USMeasure();
      while ((USrange < 30))
      {
        USMeasure();
        go_backward();
      }
    }
    radar.write(radarfront);
  }
  //Если с правого ИК-датчика расстояние меньше чем с левого ИК-датчика
  if (rfront < lfront)
  {
    //Поворачиваем робота влево
    go_left_full();
    delay(250);
    go_stop();
    //Едем назад до тех пор пока расстояния с ИК-датчиков не будут удовлетворять условиям
    //Можешь настраивать как тебе необходимо.
    while ((rfront < frontlow + 15) && (lfront < frontlow + 15))
    {
      IRMeasure();
      go_backward();
    }
    USMeasure();
    //Едем назад, чтобы и с переднего УЗ-датчика, расстояние было больше необходимого
    while ((USrange < 30))
    {
      USMeasure();
      go_backward();
    }
    //Поворачиваем робота влево
    go_left_full();
    delay(500);
    go_stop();
  }
  //Если с левого ИК-датчика расстояние меньше чем с правого ИК-датчика
  if (lfront < rfront)
  {
    //Поворачиваем робота вправо
    go_right_full();
    delay(250);
    go_stop();
    //Едем назад до тех пор пока расстояния с ИК-датчиков не будут удовлетворять условиям
    //Можешь настраивать как тебе необходимо.
    while ((rfront < frontlow + 15) && (lfront < frontlow + 15))
    {
      go_backward();
      IRMeasure();
    }
    USMeasure();
    //Едем назад, чтобы и с переднего УЗ-датчика, расстояние было больше необходимого
    while ((USrange < 30))
    {
      USMeasure();
      go_backward();
    }
    //Поворачиваем робота вправо
    go_right_full();
    //Задержка обуславливает длительность поворота
    delay(500);
    //Остановка
    go_stop();
  }
}
//Функция по измерению расстояния с УЗ-датчика
//В трех пололожениях. По центру, крайнее правое и крайнее левое.
void USRadar()
{
  radar.write(radarright);
  delay(250);
  USMeasure();
  USrangeright = USrange;
  radar.write(radarleft);
  delay(250);
  USMeasure();
  USrangeleft = USrange;
}

//Функция для мотордрайвера по движению робота прямо
//A-левый
//B-правый
void go_forward()
{
  //Левый мотор
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  //Настройка скорости вращения мотора, используя ШИМ (от 0 до 255)
  analogWrite(enA, 220);

  //Правый мотор
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  //Настройка скорости вращения мотора, используя ШИМ (от 0 до 255)
  analogWrite(enB, 220);
}

//Функция, которая поворачивает влево
//Правые колеса едут на вперед
//Левые колеса едут на назад
void go_left_full()
{
  //Левый мотор
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 100);

  //Правый мотор
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, 220);
}

// Функция легкого поворота влево
//Просто замедляется скорость вращения левых колес
void go_left_half()
{
  //Левый мотор
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 100);

  //Правый мотор
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, 180);
}

//Функция, которая поворачивает вправо
//Левые колеса едут вперед
//Правые колеса едут назад
void go_right_full()
{
  //Левый мотор
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 220);

  //Правый мотор
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, 100);
}

// Функция легкого поворота вправо
//Просто замедляется скорость вращения правых колес
void go_right_half()
{
  //Левый мотор
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, 180);

  //Правый мотор
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, 100);
}

//Функция по движению назад
void go_backward()
{
  //Левый мотор
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, 220);

  //Правый мотор
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, 220);
}
void go_stop()
{
  //Левый мотор
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 200);

  //Правый мотор
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enB, 200);
}

// Функция setup() выполняется каждый раз, когда будет перезапущена подача питания
// или будет произведена перезагрузка платы
void setup() {
  //Можно инициализровать Serial чтобы производить отладку.
  //Serial.begin(9600);

  //Установка сервопривода в центральное положение
  radar.write(radarfront);
  delay(5);
  //Инициализация пинов УЗ-датчика на входи и на выход
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  delay(5);
  //Инициализация пинов мотордрайвера на выход
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  //Для отладки можно еще настроить встроенный светодиод на 13 пине
  //pinMode(13,OUTPUT);
  delay(1000);

}

//Главный бесконечный цикл, в котором находится управляющие операторы
void loop() {
  //Вызов функции по измерению расстояний с ИК-датчиков.
  //Необходимо всегда ее вызывать, чтобы обновлялись значения.
  IRMeasure();
  //Вызов функции по измерению расстояний с УЗ-датчика.
  //Необходимо всегда ее вызывать, чтобы обновлялись значения.
   USMeasure();
  //Принятие решения на основе показаний с ИК-датчиков
  //Если с левого и правого ИК-датчика расстояние больше определенного значения
  //То робот едет прямо
  if ((lfront > 50) && (rfront > 50))
  {
    //  digitalWrite(13,HIGH);
    go_forward();
  }
  //В другом случае
  else
  {
    //Если с левого и правого ИК-датчика расстояние маленькое и с УЗ-датчика маленькое
    if ((lfront <= 40) && (rfront <= 40) || (USrange <= 15))
    {
      //То думаем, что робот рядом со стеной
      //Вызываем функцию по движению робота назад и выезду из тупика
      IrfrontGoBack();
    }
    //В другом случае
    else
    {
      //Если расстояние с правого ИК-датчика меньше чем с левого ИК-датчика
      if ((rfront <= lfront))
      {
        //То робот поворачивает влево
        go_left_full();
      }
      //Если расстояние с левого ИК-датчика меньше чем с правого ИК-датчика
      if (lfront < rfront)
      {
        //То робот поворачивает вправо
        go_right_full();
      }
    }
  }
}

