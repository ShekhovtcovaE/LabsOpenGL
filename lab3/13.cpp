
#include <windows.h>  // Заголовочный файл для Windows

#include <stdio.h>    // Заголовочный файл для стандартной библиотеки ввода/вывода

#include <gl\gl.h>    // Заголовочный файл для библиотеки OpenGL32

#include <gl\glu.h>   // Заголовочный файл для библиотеки GLu32

#include <gl\glaux.h> // Заголовочный файл для библиотеки GLaux

#include <math.h>     // Заголовочный файл для математической библиотеки ( НОВОЕ )

#include <stdarg.h>   // Заголовочный файл для функций для работы с переменным

                      //  количеством аргументов ( НОВОЕ )

 

HDC        hDC=NULL;  // Приватный контекст устройства GDI

HGLRC      hRC=NULL;  // Постоянный контекст рендеринга

HWND       hWnd=NULL; // Сохраняет дескриптор окна

HINSTANCE  hInstance; // Сохраняет экземпляр приложения

 
GLuint  base;      // База списка отображения для фонта

GLfloat  cnt1;     // Первый счетчик для передвижения и закрашивания текста

GLfloat  cnt2;     // Второй счетчик для передвижения и закрашивания текста

 

bool  keys[256];      // Массив для работы с клавиатурой

bool  active=TRUE;    // Флаг активации окна, по умолчанию = TRUE

bool  fullscreen=TRUE;// Флаг полноэкранного режима

 

LRESULT  CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // Объявление WndProc

 

GLvoid BuildFont(GLvoid)  // Построение нашего растрового шрифта

{

  HFONT  font;            // Идентификатор фонта

 

  base = glGenLists(96);  // Выделим место для 96 символов ( НОВОЕ )

  

font = CreateFont(  -24,        // Высота фонта ( НОВОЕ )

  

        0,        // Ширина фонта

  

        0,        // Угол отношения

        0,        // Угол наклона

 

        FW_BOLD,      // Ширина шрифта

  
 

        FALSE,        // Курсив

        FALSE,        // Подчеркивание

        FALSE,        // Перечеркивание

  

        ANSI_CHARSET,      // Идентификатор набора символов

  

        OUT_TT_PRECIS,      // Точность вывода

  
 

        CLIP_DEFAULT_PRECIS,    // Точность отсечения

  
 

        ANTIALIASED_QUALITY,    // Качество вывода

 

        FF_DONTCARE|DEFAULT_PITCH,  // Семейство и шаг

  

        "Courier New");      // Имя шрифта

  
 

  SelectObject(hDC, font);        // Выбрать шрифт, созданный нами ( НОВОЕ )

 

  wglUseFontBitmaps(hDC, 32, 96, base); // Построить 96 символов начиная с пробела ( НОВОЕ )

}

  
 

GLvoid KillFont(GLvoid)            // Удаление шрифта

{

   glDeleteLists(base, 96);        // Удаление всех 96 списков отображения ( НОВОЕ )

}

  
 

GLvoid glPrint(const char *fmt, ...)        // Заказная функция «Печати» GL

{

 
 

  char    text[256];      // Место для нашей строки

  va_list    ap;          // Указатель на список аргументов

  
 

  if (fmt == NULL)     // Если нет текста

    return;            // Ничего не делать

 

  va_start(ap, fmt);           // Разбор строки переменных

      vsprintf(text, fmt, ap); // И конвертирование символов в реальные коды

  va_end(ap);                  // Результат помещается в строку

  

  glPushAttrib(GL_LIST_BIT);      // Протолкнуть биты списка отображения ( НОВОЕ )

  glListBase(base - 32);          // Задать базу символа в 32 ( НОВОЕ )

  

 glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);// Текст списками отображения(НОВОЕ)

 glPopAttrib(); // Возврат битов списка отображения ( НОВОЕ )

}

  

int InitGL(GLvoid)            // Все начальные настройки OpenGL здесь

{

  glShadeModel(GL_SMOOTH);    // Разрешить плавное затенение

  glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // Черный фон

  glClearDepth(1.0f);         // Установка буфера глубины

  glEnable(GL_DEPTH_TEST);    // Разрешение теста глубины

  glDepthFunc(GL_LEQUAL);     // Тип теста глубины

  // Действительно хорошие вычисления перспективы

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  BuildFont();            // Построить шрифт

  return TRUE;            // Инициализация окончена

}

  
 

int DrawGLScene(GLvoid) // Здесь мы будем рисовать все

{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очистка экран и буфера глубины

  glLoadIdentity(); // Сброс просмотра

  glTranslatef(0.0f,0.0f,-1.0f); // Передвижение на одну единицу вглубь

  
 

  // Цветовая пульсация, основанная на положении текста

  glColor3f(1.0f*float(cos(cnt1)),1.0f*float(sin(cnt2)),1.0f-0.5f*float(cos(cnt1+cnt2)));

  
 

  // Позиционирование текста на экране

  glRasterPos2f(-0.45f+0.05f*float(cos(cnt1)), 0.35f*float(sin(cnt2)));

  
 

  glPrint("Active OpenGL Text With NeHe - %7.2f", cnt1);  // Печать текста GL на экран

  

  cnt1+=0.051f;  // Увеличение первого счетчика

  cnt2+=0.005f;  // Увеличение второго счетчика

  return TRUE;   // Все отлично

}

  

  if (!UnregisterClass("OpenGL",hInstance))    // Если класс не зарегистрирован

  {

    MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",
MB_OK | MB_ICONINFORMATION);

    hInstance=NULL;          // Установить копию приложения в ноль

  }

  KillFont();            // Уничтожить шрифт

}
