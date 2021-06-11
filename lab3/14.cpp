
#include <windows.h> // заголовочный файл для Windows

#include <math.h>    // заголовочный файл для математической бибилиотеки Windows(добавлено)

#include <stdio.h>   // заголовочный файл для стандартного ввода/вывода(добавлено)

#include <stdarg.h>  // заголовочный файл для манипуляций

                     // с переменными аргументами (добавлено)

#include <gl\gl.h>   // заголовочный файл для библиотеки OpenGL32

#include <gl\glu.h>  // заголовочный файл для библиотеки GLu32

#include <gl\glaux.h>// заголовочный файл для библиотеки GLaux

 

HDC       hDC=NULL;  // Частный контекст устройства GDI

HGLRC     hRC=NULL;  // Контекст текущей визуализации

HWND      hWnd=NULL; // Декриптор нашего окна

HINSTANCE hInstance; // Копия нашего приложения

 


 

GLuint base;              // База отображаемого списка для набора символов (добавлено)

GLfloat   rot;               // Используется для вращения текста (добавлено)

bool   keys[256];         // Массив для манипуляций с клавиатурой

bool   active=TRUE;       // Флаг активности окна, по умолчанию=TRUE

bool   fullscreen=TRUE;   // Флаг полноэкранного режима, по умолчанию=TRUE

 

GLYPHMETRICSFLOAT gmf[256] будет содержать информацию о местоположении и ориентации каждого из 256 списков отображения нашего векторного шрифта. Чтобы получить доступ к нужной букве просто напишем gmf[num], где num - это номер списка отображения, соответствующий требуемой букве. Позже в программе я покажу вам, как узнать ширину каждого символа для того, чтобы вы смогли автоматически центрировать текст на экране. Имейте в виду, что каждый символ имеет свою ширину. Метрика шрифта (glyphmetrics) на порядок облегчит вам жизнь.

 

GLYPHMETRICSFLOAT gmf[256];  // Массив с информацией о нашем шрифте

LRESULT CALLBACK WndProc(

HWND, UINT, WPARAM, LPARAM); // Объявление оконной процедуры

 

GLvoid BuildFont(GLvoid)           // Строим растровый шрифт

{

  HFONT  font;                     // Идентификатор шрифта Windows

  base = glGenLists(256);          // массив для 256 букв

 

font = CreateFont(       -12,          // высота шрифта


              0,            // ширина знакомест 

              0,            //Угол перехода

              0,            //Угол направления
 

FW_BOLD,              //Ширина шрифта
 

              FALSE,        // Курсив

              FALSE,        // Подчеркивание

              FALSE,        // Перечеркивание
 

              ANSI_CHARSET,       //Идентификатор кодировки

              OUT_TT_PRECIS,       // Точность вывода

              IP_DEFAULT_PRECIS,       //Точность отсечения

              ANTIALIASED_QUALITY,// Качество вывода


              FF_DONTCARE|DEFAULT_PITCH, // Семейство и Шаг

              "Comic Sans MS");          // Имя шрифта



 

SelectObject(hDC, font);       //Выбрать шрифт, созданный нами

 

wglUseFontOutlines( hDC,         // Выбрать текущий контекст устройства (DC)

               0,            // Стартовый символ

               255,          // Количество создаваемых списков отображения

               base,         // Стартовое значение списка отображения

 
                    0.0f,       //Отклонение от настоящего контура

                    0.2f,       //Толщина шрифта по оси Z

                    WGL_FONT_POLYGONS,       //Использовать полигоны, а не линии

                    gmf),       //буфер адреса для данных списка отображения

                    }



GLvoid KillFont(GLvoid)                   // Удаление шрифта

              {

              glDeleteLists(base, 256); // Удаление всех 256 списков отображения

              }

 

GLvoid glPrint(const char *fmt, ...)     // Функция вывода текста в OpenGL

              {
 

              float         length=0;     // Переменная для нахождения

                                          // физической длины текста

              char          text[256];    // Здесь наша строка

              va_list              ap;    // Указатель на переменный список аргументов
 

              if (fmt == NULL)            // Если нет текста,

                    return;               // ничего не делаем

              va_start(ap, fmt);         // Анализ строки на переменные

              vsprintf(text, fmt, ap);   // И конвертация символов в реальные коды

              va_end(ap);                // Результат сохраняется в text
 

       for (unsigned int loop=0;loop//Цикл поиска размера строки

                    {

                    length+=gmf[text[loop]].gmfCellIncX;

                    // Увеличение размера на ширину символа

                    }

 

glTranslatef(-length/2,0.0f,0.0f);       //Центровка на экране нашей строки


 

       glPushAttrib(GL_LIST_BIT); // Сохраняет в стеке значения битов списка отображения

       glListBase(base);          // Устанавливает базовый символ в 0

 

 

       // Создает списки отображения текста

       glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);

       glPopAttrib(); // Восстанавливает значение Display List Bits

                    }

 

int InitGL(GLvoid)                             // Здесь будут все настройки для OpenGL

{

       glShadeModel(GL_SMOOTH);                // Включить плавное затенение

       glClearColor(0.0f, 0.0f, 0.0f, 0.5f);   // Черный фон

       glClearDepth(1.0f);                     // Настройка буфера глубины

       glEnable(GL_DEPTH_TEST);                // Разрешить проверку глубины

       glDepthFunc(GL_LEQUAL);                 // Тип проверки глубины

       // Действительно хорошие вычисления перспективы

       glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

       glEnable(GL_LIGHT0);           // Включить встроенное освещение (черновое) (новая)

       glEnable(GL_LIGHTING);                  // Разрешить освещение                        (новая)

       glEnable(GL_COLOR_MATERIAL);            // Включить раскраску материалов (новая)

 

       BuildFont();                            // Построить шрифт (добавлена)

 

       return TRUE;                            // Инициализация прошла успешно

}

 


int DrawGLScene(GLvoid)                  // Здесь весь вывод на экран

{

       // Очистка экрана и буфера глубины

       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       glLoadIdentity();                 // Сброс вида

       glTranslatef(0.0f,0.0f,-10.0f);   // Смещение на 10 единиц в экран

 
       glRotatef(rot,1.0f,0.0f,0.0f);          // Поворот по оси X

       glRotatef(rot*1.5f,0.0f,1.0f,0.0f);     // Поворот по оси Y

       glRotatef(rot*1.4f,0.0f,0.0f,1.0f);     // Поворот по оси Z

 

       // Цветовая пульсация основанная на вращении

       glColor3f(1.0f*float(cos(rot/20.0f)),1.0f*float(sin(rot/25.0f)),

              1.0f-0.5f*float(cos(rot/17.0f)));
 

       glPrint("NeHe - %3.2f",rot/50);   // Печать текста на экране

 


       rot+=0.5f;                        // Увеличить переменную вращения

       return TRUE;                      // Все прошло успешно

              }



if (!UnregisterClass("OpenGL",hInstance))// Если класс незарегистрирован

       {

       MessageBox(NULL,"Could Not Unregister Class.",

              "SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);

       hInstance=NULL;                   // Установить копию приложения в ноль

       }

KillFont();                              // Уничтожить шрифт
