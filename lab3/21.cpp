
#include <windows.h> // заголовочный файл для Windows

#include <stdio.h>   // заголовочный файл для стандартного ввода/вывода

#include <stdarg.h>  // заголовочный файл для манипуляций с переменными аргументами

#include <gl\gl.h>   // заголовочный файл для библиотеки OpenGL32

#include <gl\glu.h>  // заголовочный файл для библиотеки GLu32

#include <gl\glaux.h>// заголовочный файл для библиотеки GLaux

 

HDC       hDC=NULL;  // Частный контекст устройства GDI

HGLRC     hRC=NULL;  // Контекст текущей визуализации

HWND      hWnd=NULL; // Декриптор нашего окна

HINSTANCE hInstance; // Копия нашего приложения



bool    vline[11][10];     // Отслеживает вертикальные линии

bool    hline[10][11];     // Отслеживает горизонтальные линии

bool    ap;                // Клавиша 'A' нажата?

bool    filled;            // Сетка закрашена?

bool    gameover;          // Игра окончена?

bool    anti=TRUE;         // Сглаживание?

bool    keys[256];         // Массив для манипуляций с клавиатурой

bool    active=TRUE;       // Флаг активности окна, по умолчанию=TRUE

bool    fullscreen=TRUE;   // Флаг полноэкранного режима, по умолчанию=TRUE


int    loop1;              // Общая переменная 1

int    loop2;              // Общая переменная 2

int    delay;              // Задержка для Противника

int    adjust=3;           // Настройка скорости для медленных видеокарт

int    lives=5;            // Жизни для игрока

int    level=1;            // Внутренний уровень игры

int    level2=level;       // Уровень игры для отображения

int    stage=1;            // Стадия игры



struct    object           // Структура для игрока

{

  int  fx, fy;             // Точная позиция для передвижения

  int  x, y;               // Текущая позиция игрока

  float  spin;             // Направление вращения

};



struct  object  player;                // Информация о игроке

struct  object  enemy[9];              // Информация о противнике

struct  object  hourglass;             // Информация о песочных часах


struct                    // Создание структуры для информации о таймере

{

  __int64       frequency;                 // Частота таймера

  float         resolution;                // Точность таймера

  unsigned long mm_timer_start;            // Стартовое значение мультимедийного таймера

  unsigned long mm_timer_elapsed;          // Прошедшее время мультимедийного таймера

  bool          performance_timer;         // Использовать высокоточный таймер?

  __int64       performance_timer_start;   // Стартовое значение высокоточного таймера

  __int64       performance_timer_elapsed; // Прошедшее время высокоточного таймера

} timer;                  // Структура по имени таймер

 

int    steps[6]={ 1, 2, 4, 5, 10, 20 }; // Значения шагов для работы

                                        // на медленных видеокартах

GLuint    texture[2];          // Память для текстур

GLuint    base;                // База для списка отображения шрифта

 

LRESULT  CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Объявление для WndProc

 

void TimerInit(void) // Инициализация нашего таймера (Начали)

{

  memset(&timer, 0, sizeof(timer)); // Очистка нашей структуры

 

  // Проверим доступность высокоточного таймера

  // Если доступен, то частота таймера будет задана

  if (!QueryPerformanceFrequency((LARGE_INTEGER *) &timer.frequency))

  {

    // Нет высокоточного таймера

    timer.performance_timer  = FALSE;       // Установим флаг высокоточного таймера в ЛОЖЬ

    timer.mm_timer_start  = timeGetTime();  // Текущее время из timeGetTime()

    timer.resolution  = 1.0f/1000.0f;       // Точность равна 0.001f

    timer.frequency    = 1000;              // Частота равна 1000

    timer.mm_timer_elapsed  = timer.mm_timer_start; // Прошедшее время равно текущему

  }



  else

  {

    // Высокоточный таймер доступен, используем его вместо мультимедийного таймера

    // Взять текущее время и сохранить его в performance_timer_start

    QueryPerformanceCounter((LARGE_INTEGER *) &timer.performance_timer_start);

    timer.performance_timer    = TRUE;        // Установить флаг наличия таймера в TRUE

    // Вычислить точность таймера, используя частоту

    timer.resolution    = (float) (((double)1.0f)/((double)timer.frequency));

    // Присвоить прошедшему времени текущее время

    timer.performance_timer_elapsed  = timer.performance_timer_start;

  }

}



float TimerGetTime()           // Взять время в миллисекундах

{

  __int64 time;                // time содержит 64 бита

 

  if (timer.performance_timer) // Есть высокоточный таймер?

  {

    // Захват текущего значения высокоточного таймера

    QueryPerformanceCounter((LARGE_INTEGER *) &time);

    // Вернем текущее время минус начальное время, умноженное на точность и 1000 (для миллисекунд)

    return ( (float) ( time - timer.performance_timer_start) * timer.resolution)*1000.0f;

  }

  else

  {

    // Вернем текущее время минус начальное время, умноженное на точность и 1000 (для миллисекунд)

    return( (float) ( timeGetTime() - timer.mm_timer_start) * timer.resolution)*1000.0f;

  }

}


void ResetObjects(void)      // Сброс Игрока и Противников

{

  player.x=0;                // Сброс позиции игрока X на левый край экрана

  player.y=0;                // Сброс позиции игрока Y на верх экрана

  player.fx=0;               // Установим точную позиции X

  player.fy=0;               // Установим точную позиции Y

 

  for (loop1=0; loop1<(stage*level); loop1++) // Цикл по всем противникам

  {

    enemy[loop1].x=5+rand()%6;                // Выбор случайной позиции X

    enemy[loop1].y=rand()%11;                 // Выбор случайной позиции Y

    enemy[loop1].fx=enemy[loop1].x*60;        // Установка точной X

    enemy[loop1].fy=enemy[loop1].y*40;        // Установка точной Y

  }

}

 
int LoadGLTextures()                       // Загрузка растра и конвертирование его в текстуры

{

  int Status=FALSE;                        // Индикатор статуса

  AUX_RGBImageRec *TextureImage[2];        // Память для текстур

  memset(TextureImage,0,sizeof(void *)*2); // Указатель в NULL

 

  if   ((TextureImage[0]=LoadBMP("Data/Font.bmp")) &&  // Загрузка фонта

     (TextureImage[1]=LoadBMP("Data/Image.bmp")))      // Загрузка фона

  {

    Status=TRUE;                           // Установка статуса в TRUE

 

    glGenTextures(2, &texture[0]);         // Создание текстуры

 

    for (loop1=0; loop1<2; loop1++)        // Цикл из 2 текстур

    {

      glBindTexture(GL_TEXTURE_2D, texture[loop1]);

      glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop1]->sizeX, TextureImage[loop1]->sizeY,

        0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop1]->data);

      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    }

 

    for (loop1=0; loop1<2; loop1++)        // Цикл из 2 текстур

    {

      if (TextureImage[loop1])             // Если текстура существует

      {

        if (TextureImage[loop1]->data)     // Если изображение текстуры существует

        {

          free(TextureImage[loop1]->data); // Освободить память текстуры

        }

        free(TextureImage[loop1]);         // Освободить структуру изображения

      }

    }

  }

  return Status;                           // Возврат статуса

}

 
GLvoid BuildFont(GLvoid)                     // Создаем список отображения нашего шрифта

{

  base=glGenLists(256);                      // Создаем списки

  glBindTexture(GL_TEXTURE_2D, texture[0]);  // Выбираем текстуру шрифта

  for (loop1=0; loop1<256; loop1++)          // Цикл по всем 256 спискам

  {

    float cx=float(loop1%16)/16.0f;          // X координата текущего символа

    float cy=float(loop1/16)/16.0f;          // Y координата текущего символа

 

    glNewList(base+loop1,GL_COMPILE);              // Начинаем делать список

      glBegin(GL_QUADS);         // Используем четырехугольник, для каждого символа

        glTexCoord2f(cx,1.0f-cy-0.0625f);          // Точка в текстуре (Левая нижняя)

        glVertex2d(0,16);        // Координаты вершины (Левая нижняя)

        glTexCoord2f(cx+0.0625f,1.0f-cy-0.0625f);  // Точка на текстуре (Правая нижняя)

        glVertex2i(16,16);       // Координаты вершины (Правая нижняя)

        glTexCoord2f(cx+0.0625f,1.0f-cy);          // Точка текстуры (Верхняя правая)

        glVertex2i(16,0);        // Координаты вершины (Верхняя правая)

        glTexCoord2f(cx,1.0f-cy);                  // Точка текстуры (Верхняя левая)

        glVertex2i(0,0);         // Координаты вершины (Верхняя левая)

      glEnd();                   // Конец построения четырехугольника (Символа)

      glTranslated(15,0,0);      // Двигаемся вправо от символа

    glEndList();                 // Заканчиваем создавать список отображения

  }                              // Цикл для создания всех 256 символов

}

 
GLvoid KillFont(GLvoid)               // Удаляем шрифт из памяти

{

  glDeleteLists(base,256);            // Удаляем все 256 списков отображения

}



GLvoid glPrint(GLint x, GLint y, int set, const char *fmt, ...) // Печать

{

  char    text[256];          // Место для строки

  va_list    ap;              // Ссылка на список аргументов

 

  if (fmt == NULL)            // Если нет текста

    return;                   // то выходим

 

  va_start(ap, fmt);          // Разбор строки из значений

      vsprintf(text, fmt, ap);// и конвертирование символов в фактические числа

  va_end(ap);                 // Результат в текст

 

  if (set>1)                  // Если выбран не верный набор символов?

  {

    set=1;                    // Если так, то выбрать набор 1 (Курсив)

  }

  glEnable(GL_TEXTURE_2D);    // Разрешить наложение текстуры

  glLoadIdentity();           // Сбросить матрицу просмотра вида

  glTranslated(x,y,0);        // Позиция текста (0,0 – Низ Лево)

  glListBase(base-32+(128*set)); // Выбор набора символов (0 или 1)

 

  if (set==0)                 // Если 0 используем укрупненный фонт

  {

    glScalef(1.5f,2.0f,1.0f); // Ширина и Высота укрупненного шрифта

  }

 

  glCallLists(strlen(text),GL_UNSIGNED_BYTE, text); // Вывод текста на экран

  glDisable(GL_TEXTURE_2D);   // Запрет наложения текстуры

}

  
GLvoid ReSizeGLScene(GLsizei width, GLsizei height) // Масштабирование и инициализация окна GL

{

  if (height==0)                 // Предотвращение деления на ноль

  {

    height=1;                    // Сделать высоту равной 1

  }

 

  glViewport(0,0,width,height);  // Сброс текущей области просмотра

 

  glMatrixMode(GL_PROJECTION);   // Выбор матрицы проектирования

  glLoadIdentity();              // Сброс матрицы проектирования

 

  glOrtho(0.0f,width,height,0.0f,-1.0f,1.0f); // Создание ортог. вида 640x480 (0,0 – верх лево)

 

  glMatrixMode(GL_MODELVIEW);    // Выбор матрицы просмотра вида

  glLoadIdentity();              // Сброс матрицы просмотра вида

}

 

int InitGL(GLvoid)            // Все настройки для OpenGL делаются здесь

{

  if (!LoadGLTextures())      // Переход на процедуру загрузки текстур

  {

    return FALSE;             // Если текстура не загружена, вернем ЛОЖЬ

  }

 

  BuildFont();                // Построение шрифта

 

  glShadeModel(GL_SMOOTH);    // Разрешить плавное сглаживание

  glClearColor(0.0f, 0.0f, 0.0f, 0.5f);   // Черный фон

  glClearDepth(1.0f);         // Настройка буфера глубины

  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Сглаживание линий

  glEnable(GL_BLEND);         // Разрешить смешивание

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Тип смешивания

  return TRUE;                // Инициализация окончена успешна

}

  

int DrawGLScene(GLvoid)                 //Здесь мы будем рисовать

{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очистка экрана и буфера глубины

  glBindTexture(GL_TEXTURE_2D, texture[0]); // Выбор текстуры нашего шрифта

  glColor3f(1.0f,0.5f,1.0f);            // Установить фиолетовый цвет

  glPrint(207,24,0,"GRID CRAZY");       // Написать GRID CRAZY на экране

  glColor3f(1.0f,1.0f,0.0f);            // Установить желтый цвет

  glPrint(20,20,1,"Level:%2i",level2);  // Вывод состояние текущего уровня

  glPrint(20,40,1,"Stage:%2i",stage);   // Вывод состояние стадии



  if (gameover)                      // Игра окончена?

  {

    glColor3ub(rand()%255,rand()%255,rand()%255); // Выбор случайного цвета

    glPrint(472,20,1,"GAME OVER");   // Вывод GAME OVER на экран

    glPrint(456,40,1,"PRESS SPACE"); // Вывод PRESS SPACE на экран

  }

  
  for (loop1=0; loop1<lives-1; loop1++)          // Цикл по всем жизням минус текущая жизнь

  {

    glLoadIdentity();             // Сброс вида

    glTranslatef(490+(loop1*40.0f),40.0f,0.0f);  // Перенос вправо от нашего заголовка

    glRotatef(-player.spin,0.0f,0.0f,1.0f);      // Вращение против часовой стрелки

    glColor3f(0.0f,1.0f,0.0f);    // Цвет игрока зеленный

    glBegin(GL_LINES);            // Рисуем игрока с помощью линий

      glVertex2d(-5,-5);          // Лево верх игрока

      glVertex2d( 5, 5);          // Низ право

      glVertex2d( 5,-5);          // Верх право

      glVertex2d(-5, 5);          // Низ лево

    glEnd();                      // Закончили рисовать игрока

    glRotatef(-player.spin*0.5f,0.0f,0.0f,1.0f); // Вращение против часовой стрелки

    glColor3f(0.0f,0.75f,0.0f);   // Установка темно-зеленного

    glBegin(GL_LINES);            // Рисуем игрока с помощью линий

      glVertex2d(-7, 0);          // Влево от центра игрока

      glVertex2d( 7, 0);          // Вправо от центра

      glVertex2d( 0,-7);          // Вверх от центра

      glVertex2d( 0, 7);          // Вниз от центра

    glEnd();                      // Закончили рисовать игрока

  }

  
  filled=TRUE;                       // Задать True до начала тестирования

  glLineWidth(2.0f);                 // Задать ширину линий для ячеек 2.0f

  glDisable(GL_LINE_SMOOTH);         // Запретить сглаживание

  glLoadIdentity();                  // Сброс текущей матрицы вида и модели

  for (loop1=0; loop1<11; loop1++)   // Цикл слева направо

  {

    for (loop2=0; loop2<11; loop2++) // Цикл сверху вниз

    {

      glColor3f(0.0f,0.5f,1.0f);     // Задать синий цвет линии

      if (hline[loop1][loop2])       // Прошли горизонтальную линию?

      {

        glColor3f(1.0f,1.0f,1.0f);   // Если да, цвет линии белый

      }

      if (loop1<10)                  // Не рисовать на правом краю

      {

        if (!hline[loop1][loop2])    // Если горизонтальную линию не прошли

        {

          filled=FALSE;              // filled равно False

        }

        glBegin(GL_LINES);           // Начало рисования горизонтального бордюра ячейки

          glVertex2d(20+(loop1*60),70+(loop2*40)); // Левая сторона горизонтальной линии

          glVertex2d(80+(loop1*60),70+(loop2*40)); // Правая сторона горизонтальной линии

        glEnd();                     // Конец рисования горизонтального бордюра ячейки

      }

 
      glColor3f(0.0f,0.5f,1.0f);     // Задать синий цвет линии

      if (vline[loop1][loop2])       // Прошли вертикальную линию?

      {

        glColor3f(1.0f,1.0f,1.0f);   // Если да, цвет линии белый

      }

      if (loop2<10)                  // Не рисовать на нижнем краю

      {

        if (!vline[loop1][loop2])    // Если вертикальную линию не прошли

        {

          filled=FALSE;              // filled равно False

        }

        glBegin(GL_LINES);           // Начало рисования вертикального бордюра ячейки

          glVertex2d(20+(loop1*60),70+(loop2*40));  // Верхняя сторона вертикальной линии

          glVertex2d(20+(loop1*60),110+(loop2*40)); // Нижняя сторона вертикальной линии

        glEnd();                     // Конец рисования вертикального бордюра ячейки

      }



      glEnable(GL_TEXTURE_2D);      // Разрешение наложение текстуры

      glColor3f(1.0f,1.0f,1.0f);    // Ярко белый свет

      glBindTexture(GL_TEXTURE_2D, texture[1]); // Выбор мозаичного изображения

      if ((loop1<10) && (loop2<10)) // Если в диапазоне, заполнить пройденные ячейки

      {

        // Все ли стороны ячейки пройдены?

        if (hline[loop1][loop2] && hline[loop1][loop2+1] &&

            vline[loop1][loop2] && vline[loop1+1][loop2])

        {

          glBegin(GL_QUADS);       // Нарисовать текстурированный четырехугольник

            glTexCoord2f(float(loop1/10.0f)+0.1f,1.0f-(float(loop2/10.0f)));

            glVertex2d(20+(loop1*60)+59,(70+loop2*40+1));  // Право верх

            glTexCoord2f(float(loop1/10.0f),1.0f-(float(loop2/10.0f)));

            glVertex2d(20+(loop1*60)+1,(70+loop2*40+1));   // Лево верх

            glTexCoord2f(float(loop1/10.0f),1.0f-(float(loop2/10.0f)+0.1f));

            glVertex2d(20+(loop1*60)+1,(70+loop2*40)+39);  // Лево низ

            glTexCoord2f(float(loop1/10.0f)+0.1f,1.0f-(float(loop2/10.0f)+0.1f));

            glVertex2d(20+(loop1*60)+59,(70+loop2*40)+39); // Право низ

          glEnd();                 // Закончить текстурирование ячейки

        }

      }

      glDisable(GL_TEXTURE_2D);    // Запрет наложения текстуры

    }

  }

  glLineWidth(1.0f);               // Ширина линий 1.0f

 
  if (anti)                   // Anti TRUE?

  {

    glEnable(GL_LINE_SMOOTH); // Если так, то разрешить сглаживание

  }


  if (hourglass.fx==1)              // Если fx=1 нарисовать песочные часы

  {

    glLoadIdentity();               // Сброс матрицы вида модели

    glTranslatef(20.0f+(hourglass.x*60),70.0f+(hourglass.y*40),0.0f); // Поместим часы

    glRotatef(hourglass.spin,0.0f,0.0f,1.0f);      // Вращаем по часовой стрелке

    glColor3ub(rand()%255,rand()%255,rand()%255);  // Зададим случайный цвет часов

 
    glBegin(GL_LINES);            // Начало рисования наших песочных часов линиями

      glVertex2d(-5,-5);          // Лево Верх песочных часов

      glVertex2d( 5, 5);          // Право Низ песочных часов

      glVertex2d( 5,-5);          // Право Верх песочных часов

      glVertex2d(-5, 5);          // Лево Низ песочных часов

      glVertex2d(-5, 5);          // Лево Низ песочных часов

      glVertex2d( 5, 5);          // Право Низ песочных часов

      glVertex2d(-5,-5);          // Лево Верх песочных часов

      glVertex2d( 5,-5);          // Право Верх песочных часов

    glEnd();                      // Конец рисования песочных часов

  }

  
  glLoadIdentity();              // Сброс матрицы вида модели

  glTranslatef(player.fx+20.0f,player.fy+70.0f,0.0f); // Перемещение игрока в точную позицию

  glRotatef(player.spin,0.0f,0.0f,1.0f);              // Вращение по часовой стрелки

  glColor3f(0.0f,1.0f,0.0f);      // Установить светло-зеленный цвет

  glBegin(GL_LINES);              // Начать рисование нашего игрока из линий

    glVertex2d(-5,-5);            // Лево Верх игрока

    glVertex2d( 5, 5);            // Право Низ игрока

    glVertex2d( 5,-5);            // Право Верх игрока

    glVertex2d(-5, 5);            // Лево Низ игрока

  glEnd();                        // Конец рисования игрока

  

  glRotatef(player.spin*0.5f,0.0f,0.0f,1.0f); // Вращаем по часовой

  glColor3f(0.0f,0.75f,0.0f);     // Задаем цвет игрока темно-зеленный

  glBegin(GL_LINES);              // Начало рисования нашего игрока используя линии

    glVertex2d(-7, 0);            // Влево от центра игрока

    glVertex2d( 7, 0);            // Вправо от центра игрока

    glVertex2d( 0,-7);            // Вверх от центра игрока

    glVertex2d( 0, 7);            // Вниз от центра игрока

  glEnd();                        // Конец рисования игрока

  

  for (loop1=0; loop1<(stage*level); loop1++) // Цикл рисования противников

  {

    glLoadIdentity();             // Сброс матрицы просмотра вида

    glTranslatef(enemy[loop1].fx+20.0f,enemy[loop1].fy+70.0f,0.0f);

    glColor3f(1.0f,0.5f,0.5f);    // Сделать тело противника розовым

    glBegin(GL_LINES);            // Начало рисования противника

      glVertex2d( 0,-7);          // Верхняя точка тела

      glVertex2d(-7, 0);          // Левая точка тела

      glVertex2d(-7, 0);          // Левая точка тела

      glVertex2d( 0, 7);          // Нижняя точка тела

      glVertex2d( 0, 7);          // Нижняя точка тела

      glVertex2d( 7, 0);          // Правая точка тела

      glVertex2d( 7, 0);          // Правая точка тела

      glVertex2d( 0,-7);          // Верхняя точка тела

    glEnd();                      // Конец рисования противника


    glRotatef(enemy[loop1].spin,0.0f,0.0f,1.0f); // Вращение клинка противника

    glColor3f(1.0f,0.0f,0.0f);    // Сделаем клинок противника красным

    glBegin(GL_LINES);            // Начало рисования клинка противника

      glVertex2d(-7,-7);          // Лево верх противника

      glVertex2d( 7, 7);          // Право низ противника

      glVertex2d(-7, 7);          // Лево низ противника

      glVertex2d( 7,-7);          // Право верх противника

    glEnd();                      // Конец рисования противника

  }

  return TRUE;                    // Все OK

}

 
GLvoid KillGLWindow(GLvoid) // Корректное удаление окна

{

  if (fullscreen)           // Мы в полноэкранном режиме?

  {

    ChangeDisplaySettings(NULL,0);  // Если это так, то переключиться на рабочий стол

    ShowCursor(TRUE);       // Показать курсор мыши

  }

 

  if (hRC)                  // У нас есть контекст визуализации?

  {

    if (!wglMakeCurrent(NULL,NULL)) // Мы можем освободить контексты DC и RC?

    {

      MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",

                 MB_OK | MB_ICONINFORMATION);

    }

 

    if (!wglDeleteContext(hRC))     // Мы можем удалить RC?

    {

      MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",

                 MB_OK | MB_ICONINFORMATION);

    }

    hRC=NULL;               // Задать RC в NULL

  }

 

  if (hDC && !ReleaseDC(hWnd,hDC))  // Мы можем освободить DC?

  {

    MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",

               MB_OK | MB_ICONINFORMATION);

    hDC=NULL;               // Задать DC в NULL

  }

 

  if (hWnd && !DestroyWindow(hWnd)) // Мы можем уничтожить окно?

  {

    MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",

               MB_OK | MB_ICONINFORMATION);

    hWnd=NULL;              // Задать hWnd в NULL

  }

 

  if (!UnregisterClass("OpenGL",hInstance)) // Мы можем удалить регистрацию класса?

  {

    MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",

               MB_OK | MB_ICONINFORMATION);

    hInstance=NULL;         // Задать hInstance в NULL

  }

 

  KillFont();               // Уничтожить фонт, который мы сделали

}


int WINAPI WinMain(

          HINSTANCE hInstance,     // Экземпляр

          HINSTANCE hPrevInstance, // Предыдущий экземпляр

          LPSTR     lpCmdLine,     // Параметры командной строки

          int       nCmdShow)      // Показать состояние окна

{

  MSG  msg;        // Структура сообщения окна

  BOOL done=FALSE; // Булевская переменная выхода из цикла

 

  // Запросим пользователя какой режим отображения он предпочитает

  if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?",

      "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)

  {

    fullscreen=FALSE;              // Оконный режим

  }

 

 

  // Создадим наше окно OpenGL

  if (!CreateGLWindow("NeHe's Line Tutorial",640,480,16,fullscreen))

  {

    return 0;              // Выходим если окно не было создано

  }

 

  ResetObjects();          // Установка стартовых позиций Игрока / Противников

  TimerInit();             // Инициализация таймера

 

  while (!done) // Цикл, который продолжается пока done=FALSE

  {

    if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) // Есть ожидаемое сообщение?

    {

      if (msg.message==WM_QUIT) // Мы получили сообщение о выходе?

      {

        done=TRUE; // Если так done=TRUE

      }

      else // Если нет, продолжаем работать с сообщениями окна

      {

        TranslateMessage(&msg); // Переводим сообщение

        DispatchMessage(&msg);  // Отсылаем сообщение

      }

    }

    else // Если сообщений нет

    {

  

      float start=TimerGetTime(); // Захват времени до начала рисования

 

      // Нарисовать сцену. Отследить нажатие на клавишу ESC и

      // приход сообщения о выходе из DrawGLScene()

      if ((active && !DrawGLScene()) || keys[VK_ESCAPE])  // Активно?  Выход принят?

      {

        done=TRUE;                // ESC или DrawGLScene сигнализирует о выходе

      }

      else                        // Не время выходить, надо обновить сцену

      {

        SwapBuffers(hDC);         // Переключить буфера (Двойная Буферизация)

      }

 

      // Отбросим циклы на быстрой системе

      while(TimerGetTime()<start+float(steps[adjust]*2.0f)) {}

  
      if (keys[VK_F1])          // Была нажата кнопка F1?

      {

        keys[VK_F1]=FALSE;      // Если так - установим значение FALSE

        KillGLWindow();         // Закроем текущее окно OpenGL 

        fullscreen=!fullscreen; // Переключим режим "Полный экран"/"Оконный"

        // Заново создадим наше окно OpenGL

        if (!CreateGLWindow("NeHe's Line Tutorial",640,480,16,fullscreen))

        {

          return 0;             // Выйти, если окно не было создано

        }

      }

  

      if (keys['A'] && !ap) // Если клавиша 'A' нажата и не удерживается

      {

        ap=TRUE;            // ap равно TRUE

        anti=!anti;         // Переключим сглаживание

      }

      if (!keys['A'])       // Если клавиша 'A' отпущена

      {

        ap=FALSE;           // ap равно FALSE

      }

  

      if (!gameover && active) // Если игра не окончена и программа активна – передвинуть объекты

      {

        for (loop1=0; loop1<(stage*level); loop1++)  // Цикл по противникам

        {



          if ((enemy[loop1].x<player.x) && (enemy[loop1].fy==enemy[loop1].y*40))

          {

            enemy[loop1].x++;    // Сдвиг противника вправо

          }

 

          if ((enemy[loop1].x>player.x) && (enemy[loop1].fy==enemy[loop1].y*40))

          {

            enemy[loop1].x--;    // Сдвиг противника влево

          }

 

          if ((enemy[loop1].y<player.y) && (enemy[loop1].fx==enemy[loop1].x*60))

          {

            enemy[loop1].y++;    // Сдвиг противника вниз

          }

 

          if ((enemy[loop1].y>player.y) && (enemy[loop1].fx==enemy[loop1].x*60))

          {

            enemy[loop1].y--;    // Сдвиг противника вверх

          }



          // Если наша задержка истекла, и игрок не коснулся песочных часов

          if (delay>(3-level) && (hourglass.fx!=2))

          {

            delay=0;          // Сброс задержки

            for (loop2=0; loop2<(stage*level); loop2++)  // Цикл по всем противникам

            {

 

              // Точная позиция по оси X меньше чем назначенная позиция?

              if (enemy[loop2].fx<enemy[loop2].x*60)

              {

                enemy[loop2].fx+=steps[adjust];    // Увеличим точную позицию по оси X

                enemy[loop2].spin+=steps[adjust];  // Вращаем по часовой

              }

              // Точная позиция по оси X больше чем назначенная позиция?

              if (enemy[loop2].fx>enemy[loop2].x*60)

              {

                enemy[loop2].fx-=steps[adjust];    // Уменьшим точную позицию по оси X

                enemy[loop2].spin-=steps[adjust];  // Вращаем против часовой

              }

              // Точная позиция по оси Y меньше чем назначенная позиция?

              if (enemy[loop2].fy<enemy[loop2].y*40)

              {

                enemy[loop2].fy+=steps[adjust];    // Увеличим точную позицию по оси Y

                enemy[loop2].spin+=steps[adjust];  // Вращаем по часовой

              }

              // Точная позиция по оси Y больше чем назначенная позиция?

              if (enemy[loop2].fy>enemy[loop2].y*40)

              {

                enemy[loop2].fy-=steps[adjust];    // Уменьшим точную позицию по оси Y

                enemy[loop2].spin-=steps[adjust];  // Вращаем против часовой

              }

            }

          }


          // Кто-нибудь из противников сверху игрока?

          if ((enemy[loop1].fx==player.fx) && (enemy[loop1].fy==player.fy))

          {

            lives--;            // Уменьшим жизни

 

            if (lives==0)       // Нет больше жизней?

            {

              gameover=TRUE;    // gameover равно TRUE

            }

 

            ResetObjects();      // Сброс позиций игрока / противников

            PlaySound("Data/Die.wav", NULL, SND_SYNC);  // Играем звук смерти

          }

        }

 

        if (keys[VK_RIGHT] && (player.x<10) && (player.fx==player.x*60) && (player.fy==player.y*40))

        {

          // Пометить текущую горизонтальную границу как пройденную

          hline[player.x][player.y]=TRUE;

          player.x++;        // Переместить игрока вправо

        }

        if (keys[VK_LEFT] && (player.x>0) && (player.fx==player.x*60) && (player.fy==player.y*40))

        {

          player.x--;        // Переместить игрока влево

          // Пометить текущую горизонтальную границу как пройденную

          hline[player.x][player.y]=TRUE;

        }

        if (keys[VK_DOWN] && (player.y<10) && (player.fx==player.x*60) && (player.fy==player.y*40))

        {

          // Пометить текущую вертикальную границу как пройденную

          vline[player.x][player.y]=TRUE;

          player.y++;        // Переместить игрока вниз

        }

        if (keys[VK_UP] && (player.y>0) && (player.fx==player.x*60) && (player.fy==player.y*40))

        {

          // Пометить текущую вертикальную границу как пройденную

          player.y--;        // Переместить игрока вверх

          vline[player.x][player.y]=TRUE;

        }

 
        if (player.fx<player.x*60)  // Точная позиция по оси X меньше чем назначенная позиция?

        {

          player.fx+=steps[adjust]; // Увеличим точную позицию X

        }

        if (player.fx>player.x*60)  // Точная позиция по оси X больше чем назначенная позиция?

        {

          player.fx-=steps[adjust]; // Уменьшим точную позицию X

        }

        if (player.fy<player.y*40)  // Точная позиция по оси Y меньше чем назначенная позиция?

        {

          player.fy+=steps[adjust]; // Увеличим точную позицию Y

        }

        if (player.fy>player.y*40)  // Точная позиция по оси Y больше чем назначенная позиция?

        {

          player.fy-=steps[adjust]; // Уменьшим точную позицию X

        }

      }

 
      else                // Иначе

      {

        if (keys[' '])    // Если пробел нажат

        {

          gameover=FALSE; // gameover равно FALSE

          filled=TRUE;    // filled равно TRUE

          level=1;        // Стартовый уровень установим обратно в один

          level2=1;       // Отображаемый уровень также установим в один

          stage=0;        // Стадию игры установим в ноль

          lives=5;        // Количество жизней равно пяти

        }

      }



      if (filled)         // Если сетка заполнена?

      {

        PlaySound("Data/Complete.wav", NULL, SND_SYNC);  // Играем звук завершения уровня

        stage++;          // Увеличиваем Stage

        if (stage>3)      // Если Stage больше чем 3?

        {

          stage=1;        // Тогда Stage равно 1

          level++;        // Увеличим уровень

          level2++;       // Увеличим отображаемый уровень

          if (level>3)    // Если уровень больше чем 3?

          {

            level=3;      // Тогда Level равно 3

            lives++;      // Добавим игроку лишнюю жизнь

            if (lives>5)  // Если число жизней больше чем 5?

            {

              lives=5;    // Тогда установим Lives равной 5

            }

          }

        }



        ResetObjects();                    // Сброс позиции Игрока / Противника

 

        for (loop1=0; loop1<11; loop1++)   // Цикл по X координатам сетки

        {

          for (loop2=0; loop2<11; loop2++) // Цикл по Y координатам сетки

          {

            if (loop1<10)                  // Если X координата меньше чем 10

            {

              hline[loop1][loop2]=FALSE;   // Задаем текущее горизонтальное значение в FALSE

            }

            if (loop2<10)                  // Если Y координата меньше чем 10

            {

              vline[loop1][loop2]=FALSE;   // Задаем текущее вертикальное значение в FALSE

            }

          }

        }

      }

 

      // Если игрок попал в песочные часы и они на экране

      if ((player.fx==hourglass.x*60) && (player.fy==hourglass.y*40) && (hourglass.fx==1))

      {

        // Играть звук замораживания

        PlaySound("Data/freeze.wav", NULL, SND_ASYNC | SND_LOOP);

        hourglass.fx=2;          // Задать hourglass fx значение 2

        hourglass.fy=0;          // Задать hourglass fy значение 0

      }


      player.spin+=0.5f*steps[adjust]; // Вращение игрока по часовой

      if (player.spin>360.0f)          // Значение spin больше чем 360?

      {

        player.spin-=360;              // Тогда вычтем 360

      }

 

      hourglass.spin-=0.25f*steps[adjust]; // Вращение часов против часовой

      if (hourglass.spin<0.0f)             // spin меньше чем 0?

      {

        hourglass.spin+=360.0f;            // Тогда добавим 360

      }

 

      hourglass.fy+=steps[adjust];    // Увеличим hourglass fy

      // Если hourglass fx равно 0 и fy больше чем 6000 деленное на текущий уровень?

      if ((hourglass.fx==0) && (hourglass.fy>6000/level))

      {

        // Тогда играем звук песочных часов

        PlaySound("Data/hourglass.wav", NULL, SND_ASYNC);

        hourglass.x=rand()%10+1;      // Случайная позиция часов по X

        hourglass.y=rand()%11;        // Случайная позиция часов по Y

        hourglass.fx=1;               // Задать hourglass fx значение 1 (стадия часов)

        hourglass.fy=0;               // Задать hourglass fy значение 0 (счетчик)

      }


      // Если hourglass fx равно 1 и fy больше чем 6000 деленное на текущий уровень?

      if ((hourglass.fx==1) && (hourglass.fy>6000/level))

      {

        hourglass.fx=0;          // Тогда зададим fx равным 0 (Обратим часы в ноль)

        hourglass.fy=0;          // Задать fy равным 0 (Сброс счетчика)

      }



      // Переменная песочных часов fx равно 2 и переменная fy

      // больше чем 500 плюс 500 умноженное на текущий уровень?

      if ((hourglass.fx==2) && (hourglass.fy>500+(500*level)))

      {

        PlaySound(NULL, NULL, 0);// Тогда прерываем звук заморозки

        hourglass.fx=0;          // Все в ноль

        hourglass.fy=0;

      }


      delay++;         // Увеличение счетчика задержки противника

    }

  }

 

  // Shutdown

  KillGLWindow();      // Уничтожить окно

  return (msg.wParam); // Выход из программы

}
