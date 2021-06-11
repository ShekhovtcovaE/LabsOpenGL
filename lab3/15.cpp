

#include <windows.h>  // Заголовочный файл для Windows

#include <stdio.h>    // Заголовочный файл для стандартной библиотеки ввода/вывода

#include <gl\gl.h>    // Заголовочный файл для библиотеки OpenGL32

#include <gl\glu.h>   // Заголовочный файл для библиотеки GLu32

#include <gl\glaux.h> // Заголовочный файл для библиотеки GLaux

#include <math.h>     // Заголовочный файл для математической библиотеки

 

HDC        hDC=NULL;  // Приватный контекст устройства GDI

HGLRC      hRC=NULL;  // Постоянный контекст рендеринга

HWND       hWnd=NULL; // Сохраняет дескриптор окна

HINSTANCE  hInstance; // Сохраняет экземпляр приложения

 

bool  keys[256];      // Массив для работы с клавиатурой

bool  active=TRUE;    // Флаг активации окна, по умолчанию = TRUE

bool  fullscreen=TRUE;// Флаг полноэкранного режима



GLuint  texture[1]; // Одна текстура ( НОВОЕ )

GLuint  base;       // База списка отображения для фонта

 

GLfloat   rot;      // Используется для вращения текста

 

LRESULT  CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Объявление WndProc

  

GLvoid BuildFont(GLvoid)       // Построение шрифта

{

  GLYPHMETRICSFLOAT  gmf[256]; // Адрес буфера для хранения шрифта

  HFONT  font;                 // ID шрифта в Windows

 

  base = glGenLists(256);      // Храним 256 символов

  font = CreateFont(  -12,     // Высота фонта

        0,        // Ширина фонта

        0,        // Угол отношения

        0,        // Угол наклона

        FW_BOLD,  // Ширина шрифта

        FALSE,    // Курсив

        FALSE,    // Подчеркивание

        FALSE,    // Перечеркивание

 

        SYMBOL_CHARSET,      // Идентификатор набора символов ( Модифицировано )

  Следующие строки не изменились.

 

        OUT_TT_PRECIS,       // Точность вывода

        CLIP_DEFAULT_PRECIS, // Точность отсечения

        ANTIALIASED_QUALITY, // Качество вывода

        FF_DONTCARE|DEFAULT_PITCH, // Семейство и шаг

 

        "Wingdings");       // Имя шрифта ( Модифицировано )

 

  SelectObject(hDC, font);  // Выбрать шрифт, созданный нами

 

  wglUseFontOutlines(  hDC, // Выбрать текущий контекст устройства (DC)

        0,                  // Стартовый символ

        255,                // Количество создаваемых списков отображения

        base,               // Стартовое значение списка отображения

  

        0.1f,        // Отклонение от истинного контура


 

        0.2f,        // Толщина шрифта по оси Z

        WGL_FONT_POLYGONS, // Использовать полигоны, а не линии

        gmf);        // Буфер адреса для данных списка отображения

}


AUX_RGBImageRec *LoadBMP(char *Filename)     // Загрузка картинки

{

 FILE *File=NULL;          // Индекс файла

 

 if (!Filename)            // Проверка имени файла

 {

  return NULL;             // Если нет вернем NULL

 }

 

 File=fopen(Filename,"r"); // Проверим существует ли файл

 

 if (File)                 // Файл существует?

 {

  fclose(File);            // Закрыть файл

  return auxDIBImageLoad(Filename); // Загрузка картинки и вернем на нее указатель

 }

 return NULL;              // Если загрузка не удалась вернем NULL

}

 

int LoadGLTextures()                      // Загрузка картинки и конвертирование в текстуру

{

 int Status=FALSE;                        // Индикатор состояния

 

 AUX_RGBImageRec *TextureImage[1];        // Создать место для текстуры

 

 memset(TextureImage,0,sizeof(void *)*1); // Установить указатель в NULL

 

 // Загрузка картинки, проверка на ошибки, если картинка не найдена - выход

 if (TextureImage[0]=LoadBMP("Data/Lights.bmp"))

 {

  Status=TRUE;       // Установим Status в TRUE

  glGenTextures(1, &texture[0]);     // Создание трех текстур

  // Создание текстуры с мип-мап наложением

  glBindTexture(GL_TEXTURE_2D, texture[0]);

  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY,

   GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);

 }

 
    // Текстуризация контура закрепленного за объектом

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

    // Текстуризация контура закрепленного за объектом

    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

    glEnable(GL_TEXTURE_GEN_S);      // Автоматическая генерация

    glEnable(GL_TEXTURE_GEN_T);      // Автоматическая генерация

  }

 

  if (TextureImage[0])            // Если текстура существует

  {

    if (TextureImage[0]->data)    // Если изображение текстуры существует

    {

      free(TextureImage[0]->data); // Освобождение памяти изображения текстуры

    }

 

    free(TextureImage[0]);         // Освобождение памяти под структуру

  }

 

  return Status;        // Возвращаем статус

}

 
int InitGL(GLvoid)        // Все начальные настройки OpenGL здесь

{

  if (!LoadGLTextures())  // Переход на процедуру загрузки текстуры

  {

    return FALSE;         // Если текстура не загружена возвращаем FALSE

  }

  BuildFont();            // Построить шрифт

 

  glShadeModel(GL_SMOOTH);    // Разрешить плавное затенение

  glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // Черный фон

  glClearDepth(1.0f);         // Установка буфера глубины

  glEnable(GL_DEPTH_TEST);    // Разрешение теста глубины

  glDepthFunc(GL_LEQUAL);     // Тип теста глубины

  glEnable(GL_LIGHT0);        // Быстрое простое освещение

                              // (устанавливает в качестве источника освещения Light0)

  glEnable(GL_LIGHTING);      // Включает освещение

  // Действительно хорошие вычисления перспективы

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  
  glEnable(GL_TEXTURE_2D); // Разрешение наложения текстуры

  glBindTexture(GL_TEXTURE_2D, texture[0]); // Выбор текстуры

  return TRUE; // Инициализация окончена успешно

}

 

int DrawGLScene(GLvoid) // Здесь мы будем рисовать все

{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очистка экран и буфера глубины

  glLoadIdentity(); // Сброс просмотра

 
  // Позиция текста

  glTranslatef(1.1f*float(cos(rot/16.0f)),0.8f*float(sin(rot/20.0f)),-3.0f);

  

  glRotatef(rot,1.0f,0.0f,0.0f);        // Вращение по оси X

  glRotatef(rot*1.2f,0.0f,1.0f,0.0f);   // Вращение по оси Y

  glRotatef(rot*1.4f,0.0f,0.0f,1.0f);   // Вращение по оси Z

 

  glTranslatef(-0.35f,-0.35f,0.1f);      // Центр по осям X, Y, Z


  glPrint("N"); // Нарисуем символ эмблемы смерти

  rot+=0.1f;    // Увеличим переменную вращения

  return TRUE;  // Покидаем эту процедуру

}

  

  if (!UnregisterClass("OpenGL",hInstance))    // Если класс не зарегистрирован

  {

    MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);

    hInstance=NULL;          // Установить копию приложения в ноль

  }

 

  KillFont();            // Уничтожить шрифт

}

