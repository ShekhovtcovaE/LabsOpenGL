
#include <windows.h>       // Заголовочный файл для Windows

#include <stdio.h>         // Заголовочный файл для стандартного ввода/вывода

#include <stdarg.h>        // Заголовочный файл для переменного числа параметров   

#include <string.h>        // Заголовочный файл для работы с типом String

#include <gl\gl.h>         // Заголовочный файл для библиотеки OpenGL32

#include <gl\glu.h>        // Заголовочный файл для библиотеки GLu32

 

HDC       hDC=NULL;        // Частный контекст устройства

HGLRC     hRC=NULL;        // Постоянный контекст рендеринга

HWND      hWnd=NULL;       // Содержит дескриптор окна

HINSTANCE hInstance;       // Содержит экземпляр приложения

 

bool      keys[256];       // Массив для работы с клавиатурой

bool      active=TRUE;     // Флаг активности приложения

bool      fullscreen=TRUE; // Флаг полноэкранного режима


int       scroll;         // Используется для прокручивания экрана

int       maxtokens;      // Количество поддерживаемых расширений

int       swidth;         // Ширина вырезки

int       sheight;        // Высота вырезки

 

Gluint    base;           // База списков отображения для шрифта



typedef   struct                      // Создать структуру

{

Glubyte  *imageData;                  // Данные изображения (до 32 бит)

Gluint   bpp;                         // Глубина цвета в битах на пиксель

Gluint   width;                       // Ширина изображения

Gluint   height;                      // Высота изображения

Gluint   texID;                       // texID используется для выбора

                                      // текстуры

} TextureImage;                       // Имя структуры

 

TextureImage   textures[1];           // Память для хранения

                                      // одной текстуры

 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);   // Объявление WndProc



bool LoadTGA(TextureImage *texture, char *filename)

     // Загружаем TGA файл в память

{   

Glubyte  TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0}; // Заголовок несжатого TGA файла

Glubyte  TGAcompare[12]; // Используется для сравнения заголовка TGA файла

Glubyte  header[6];      // Первые 6 полезных байт заголовка

Gluint   bytesPerPixel;  // Количество байтов на пиксель используемое в TGA файле

Gluint   imageSize;      // Количество байтов, необходимое для хранения изображения в памяти

Gluint   temp;           // Временная переменная

Gluint   type=GL_RGBA;   // Установим по умолчанию режим RBGA (32 BPP)

  FILE *file = fopen(filename, "rb");         // Открытие TGA файла

  if(file==NULL ||                            // Существует ли файл

     fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||

     // Если прочитаны 12 байтов заголовка

     memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0 || // Если заголовок правильный

     fread(header,1,sizeof(header),file)!=sizeof(header)) // Если прочитаны следующие 6 байтов

  {

         if (file == NULL)                      // Если ошибка

                return false;                   // Возвращаем false

         else

         {

                fclose(file);                   // Если ошибка, закрываем файл

                return false;                   // Возвращаем false

         }

  }



  // Определяем ширину TGA (старший байт*256+младший байт)

  texture->width  = header[1] * 256 + header[0];

  // Определяем высоту TGA (старший байт*256+младший байт)

  texture->height = header[3] * 256 + header[2];

 

  if(texture->width  <=0 ||            // Если ширина меньше или равна нулю

   texture->height <=0 ||            // Если высота меньше или равна нулю   

  (header[4]!=24 && header[4]!=32))  // Является ли TGA 24 или 32 битным?

  {

    fclose(file);                    // Если где-то ошибка, закрываем файл

    return false;                    // Возвращаем false

  }


  texture->bpp  = header[4];        // Получаем TGA бит/пиксель (24 or 32)

  bytesPerPixel = texture->bpp/8;   // Делим на 8 для получения байт/пиксель 

  // Подсчитываем размер памяти для данных TGA

  imageSize = texture->width*texture->height*bytesPerPixel;

  texture->imageData=(GLubyte *)malloc(imageSize); // Резервируем память для хранения данных TGA

  if(texture->imageData==NULL ||           // Удалось ли выделить память?

   fread(texture->imageData, 1, imageSize, file)!=imageSize)

  // Размер выделенной памяти равен  imageSize?

  {

    if(texture->imageData!=NULL)       // Были ли загружены данные?

      free(texture->imageData);        // Если да, то освобождаем память

 

    fclose(file);                      // Закрываем файл

    return false;                      // Возвращаем false

  }   

  for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)  // Цикл по данным, описывающим изображение

  {                                                    // Обмена 1го и 3го байтов ('R'ed и 'B'lue)

    temp=texture->imageData[i];                        // Временно сохраняем значение imageData[i]

    texture->imageData[i] = texture->imageData[i + 2]; // Устанавливаем 1й байт в значение 3го байта

    texture->imageData[i + 2] = temp;                  // Устанавливаем 3й байт в значение,

                                                       // хранящееся в temp (значение 1го байта)

  }

  fclose (file);                                       // Закрываем файл

  
  // Строим текстуру из данных

  glGenTextures(1, &texture[0].texID);  // Сгенерировать OpenGL текстуру IDs

  glBindTexture(GL_TEXTURE_2D, texture[0].texID); // Привязать нашу текстуру

  // Линейная фильтрация

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // Линейная фильтрация

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  


  if (texture[0].bpp==24)                      // Если TGA 24 битный

  {

     type=GL_RGB;                            // Установим 'type' в GL_RGB

  }

  

  glTexImage2D(GL_TEXTURE_2D, 0, type, texture[0].width, texture[0].height,                  

                              0, type, GL_UNSIGNED_BYTE, texture[0].imageData);

 

  return true;              // Построение текстуры прошло Ok, возвратим true

}

  

GLvoid BuildFont(Glvoid)                 // Построение нашего шрифта

{

  base=glGenLists(256);                  // Создадим 256 списков отображения

                                         // Выбираем нашу текстуру шрифта

  glBindTexture(GL_TEXTURE_2D, textures[0].texID);

  for (int loop1=0; loop1<256; loop1++)  // Цикл по всем 256 спискам

  {

    float cx=float(loop1%16)/16.0f;      // X позиция текущего символа

    float cy=float(loop1/16)/16.0f;      // Y позиция текущего символа

    glNewList(base+loop1,GL_COMPILE);    // Начало построение списка

    glBegin(GL_QUADS);   // Используем квадрат для каждого символа

      glTexCoord2f(cx,1.0f-cy-0.0625f);  // Коорд. текстуры (Низ Лево)

      glVertex2d(0,16);                  // Коорд. вершины  (Низ Лево)

      glTexCoord2f(cx+0.0625f,1.0f-cy-0.0625f);  // Коорд. текстуры (Низ Право)

      glVertex2i(16,16);                 // Коорд. вершины  (Низ Право)

      glTexCoord2f(cx+0.0625f,1.0f-cy-0.001f); // Коорд. текстуры (Верх Право)

      glVertex2i(16,0);                  // Коорд. вершины  (Верх Право)

      glTexCoord2f(cx,1.0f-cy-0.001f);   // Коорд. текстуры (Верх Лево)

      glVertex2i(0,0);                   // Коорд. вершины  (Верх Лево)

    glEnd();                // Конец построения квадрата (символа)

    glTranslated(14,0,0);   // Смещаемся в право от символа

    glEndList();            // Конец построения списка

  }                         // Цикл пока не будут построены все 256 списков

}


GLvoid KillFont(GLvoid)                          // Удаляем шрифт из памяти

{

      glDeleteLists(base,256);                   // Удаляем все 256 списков

}


GLvoid glPrint(GLint x, GLint y, int set, const char *fmt, ...)  // Здесь происходит печать

{

  char    text[1024];           // Содержит нашу строку

  va_list ap;                   // Указатель на список аргументов

 

  if (fmt == NULL)              // Если текста нет

    return;                     // Ничего не делаем

 

  va_start(ap, fmt);            // Разбор строки переменных

  vsprintf(text, fmt, ap);      // И конвертирование символов в реальные коды

  va_end(ap);                   // Результат помещаем в строку

 

  if (set>1)                    // Если выбран неправильный набор символов

  {

    set=1;                      // Если да, выбираем набор 1 (Italic)

  }

  glEnable(GL_TEXTURE_2D);      // Разрешаем двумерное текстурирование

  glLoadIdentity();             // Сбрасываем матрицу просмотра модели

  glTranslated(x,y,0);          // Позиционируем текст (0,0 - Верх Лево)

  glListBase(base-32+(128*set));// Выбираем набор шрифта (0 или 1)

 

  glScalef(1.0f,2.0f,1.0f);     // Делаем текст в 2 раза выше

 

  glCallLists(strlen(text),GL_UNSIGNED_BYTE, text);// Выводим текст на экран

  glDisable(GL_TEXTURE_2D);     // Запрещаем двумерное текстурирование

}


GLvoid ReSizeGLScene(GLsizei width, GLsizei height) // Изменение размеров и инициализация GL окна

{     

  swidth=width;                // Устанавливаем ширину вырезки в ширину окна

  sheight=height;              // Устанавливаем высоту вырезки в высоту окна

  if (height==0)               // Предотвращаем деление на нуль

  {

    height=1;                  // Делаем высоту равной 1

  }

  glViewport(0,0,width,height);       // Сбрасываем область просмотра

  glMatrixMode(GL_PROJECTION);        // Выбираем матрицу проекции

  glLoadIdentity();                   // Сбрасываем матрицу проекции

  // Устанавливаем ортографическую проекцию 640x480  (0,0 - Верх Лево)

  glOrtho(0.0f,640,480,0.0f,-1.0f,1.0f);   

  glMatrixMode(GL_MODELVIEW);         // Выбираем матрицу просмотра модели

  glLoadIdentity();                   // Сбрасываем матрицу просмотра модели

}

 

int InitGL(Glvoid)                           // Все настройки для OpenGL

{

  if (!LoadTGA(&textures[0],"Data/Font.TGA"))// Загружаем текстуру шрифта

  {

    return false;                            // Если ошибка, возвращаем false

  }

 

  BuildFont();                              // Строим шрифт

 

  glShadeModel(GL_SMOOTH);                  // Разрешаем плавное сглаживание

  glClearColor(0.0f, 0.0f, 0.0f, 0.5f);     // Черный фон

  glClearDepth(1.0f);                       // Устанавливаем буфер глубины

  glBindTexture(GL_TEXTURE_2D, textures[0].texID); // Выбираем нашу текстуру шрифта

 

  return TRUE;                              // Инициализация прошла OK

}

 
int DrawGLScene(GLvoid)                 // Здесь происходит все рисование

{

  char *token;                          // Место для хранения расширений

  int  cnt=0;                           // Локальная переменная цикла

 

  // Очищаем экран и буфер глубины

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glColor3f(1.0f,0.5f,0.5f);            // Устанавливаем цвет в ярко-красный

  glPrint(50,16,1,"Renderer");          // Выводим имя производителя

  glPrint(80,48,1,"Vendor");            // Выводим имя поставщика

  glPrint(66,80,1,"Version");           // Выводим версию карты

 

  glColor3f(1.0f,0.7f,0.4f);               // Устанавливаем цвет в оранжевый

  glPrint(200,16,1,(char *)glGetString(GL_RENDERER));// Выводим имя производителя

  glPrint(200,48,1,(char *)glGetString(GL_VENDOR));  // Выводим имя поставщика

  glPrint(200,80,1,(char *)glGetString(GL_VERSION)); // Выводим версию

 

  glColor3f(0.5f,0.5f,1.0f);             // Устанавливаем цвет в ярко-голубой

  glPrint(192,432,1,"NeHe Productions"); // Печатаем NeHe Productions в низу экрана

  

  glLoadIdentity();                // Сбрасываем матрицу просмотра модели

  glColor3f(1.0f,1.0f,1.0f);       // Устанавливаем цвет в белый

  glBegin(GL_LINE_STRIP);          // Начало рисования ломаной линии

    glVertex2d(639,417);           // Верх Право нижней рамки

    glVertex2d(  0,417);           // Верх Лево нижней рамки

    glVertex2d(  0,480);           // Низ Лево нижней рамки

    glVertex2d(639,480);           // Низ Право нижней рамки

    glVertex2d(639,128);           // Вверх к Низу Права верхней рамки

  glEnd();                         // Конец первой ломаной линии

  glBegin(GL_LINE_STRIP);          // Начало рисования другой ломаной линии

    glVertex2d(  0,128);           // Низ Лево верхней рамки

    glVertex2d(639,128);           // Низ Право верхней рамки

    glVertex2d(639,  1);           // Верх Право верхней рамки

    glVertex2d(  0,  1);           // Верх Лево верхней рамки

  glVertex2d(  0,417);             // Вниз к Верху Лева нижней рамки

  glEnd();                         // Конец второй ломаной линии

 

  // Определяем область вырезки

  glScissor(1,int(0.135416f*sheight),swidth-2,int(0.597916f*sheight)); 

  glEnable(GL_SCISSOR_TEST);        // Разрешаем вырезку

 

  // Выделяем память для строки расширений

  char* text=(char*)malloc(strlen((char *)glGetString(GL_EXTENSIONS))+1);

  // Получаем список расширений и сохраняем его в text

  strcpy (text,(char *)glGetString(GL_EXTENSIONS));


  // Разбиваем 'text' на слова, разделенные " " (пробелом)

  token=strtok(text," ");

 

  while(token!=NULL)              // Пока token не NULL

  {

    cnt++;                // Увеличиваем счетчик

    if (cnt>maxtokens)        // Если 'maxtokens' меньше или равно 'cnt'

    {

        maxtokens=cnt; // Если да, то 'maxtokens' приравниваем к 'cnt'

    }


 

    glColor3f(0.5f,1.0f,0.5f);      // Устанавливаем цвет в ярко-зеленый

    glPrint(0,96+(cnt*32)-scroll,0,"%i",cnt); // Печатаем текущий номер расширения

    glColor3f(1.0f,1.0f,0.5f);            // Устанавливаем цвет в желтый

    glPrint(50,96+(cnt*32)-scroll,0,token);     // Печатаем текущее расширение

    token=strtok(NULL," ");               // Поиск следующего расширения

  }

  
  glDisable(GL_SCISSOR_TEST);               // Запрещаем вырезку

 

  free (text);                              // Освобождаем выделенную память



  glFlush();                                   // Сброс конвейера рендеринга

  return TRUE:                                 // Все прошло ok

}

GLvoid KillGLWindow(GLvoid)               // Правильное уничтожение окна

{

    if (fullscreen)                       // Полноэкранный режим?

    {

        ChangeDisplaySettings(NULL,0);    // Переход в режим разрешения рабочего стола

        ShowCursor(TRUE);// Показать указатель мыши

    }

 

    if (hRC)                              // Существует контекст рендеринга?

    {

        if (!wglMakeCurrent(NULL,NULL))   // Можно ли освободить DC и RC контексты?

        {

            MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",

                                                MB_OK | MB_ICONINFORMATION);

        }

        if (!wglDeleteContext(hRC))         // Можно ли уничтожить RC?

        {

            MessageBox(NULL,"Release Rendering Context Failed.",

                   "SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);

        }

        hRC=NULL;                               // Установим RC в NULL

    }

 

    if (hDC && !ReleaseDC(hWnd,hDC))      // Можно ли уничтожить DC?

    {

        MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",

                                              MB_OK | MB_ICONINFORMATION);

        hDC=NULL;                             // Установим DC в NULL

    }

    if (hWnd && !DestroyWindow(hWnd))     // Можно ли уничтожить окно?

    {

        MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK |

                                                      MB_ICONINFORMATION);

        hWnd=NULL;                            // Уствновим hWnd в NULL

    }

    if (!UnregisterClass("OpenGL",hInstance)) // Можно ли уничтожить класс?

    {

        MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | 

                                                          MB_ICONINFORMATION);

        hInstance=NULL;                       // Устанавливаем hInstance в NULL

    }

    KillFont();                       // Уничтожаем шрифт

}

 

CreateGLWindow(), и WndProc() - те же.

 
int WINAPI WinMain(HINSTANCE hInstance,      // Экземпляр

                   HINSTANCE hPrevInstance,  // Предыдущий экземпляр

                   LPSTR     lpCmdLine,      // Параметры командной строки

                   Int       nCmdShow)       // Состояние окна

{

  MSG msg;                        // Структура сообщения Windows

  BOOL done=FALSE;                // Логическая переменная выхода из цикла

 

  // Спрашиваем у юзера какой режим он предпочитает

  if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?",

         "Start FullScreen?", MB_YESNO | MB_ICONQUESTION)==IDNO)

  {

    fullscreen=FALSE;             // Оконный режим

  }

 

  // Создание OpenGL окна

  if (!CreateGLWindow("NeHe's Token, Extensions, Scissoring & TGA Loading                     

                                            Tutorial",640,480,16,fullscreen))

  {

    return 0;                     // Выход, если окно не было создано

  }

  while(!done)                    // Цикл выполняется пока done=FALSE

  {

    if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) // Есть сообщение?

    {

      if (msg.message==WM_QUIT)   // Получили сообщение Quit?

      {

        done=TRUE;                // Если да, то done=TRUE

      }

      else // Если нет, то обрабатываем оконные сообщения

      {

        DispatchMessage(&msg);    // Отправляем сообщение

      }

    }

    else                          // Если нет сообщений

    {

        // Рисуем сцену. Проверяем клавишу ESC и сообщение QUIT из DrawGLScene()

        // Активно?  Получили Quit сообщение?

        if ((active && !DrawGLScene()) || keys[VK_ESCAPE])

        {

          done=TRUE; // ESC или DrawGLScene сигнализирует о выходе

        }

        else                      // Не время выходить, обновляем экран

        {

          SwapBuffers(hDC);       // Меняем буфера (двойная буферизация)

          if (keys[VK_F1])        // Нажата клавиша F1?

          {

            keys[VK_F1]=FALSE;    // Если да, то установим в FALSE

            KillGLWindow();       // Уничтожаем наше текущее окно

            fullscreen=!fullscreen; // Полноэкран./окон. режим

            // Создаем наше OpenGL окно

            if (!CreateGLWindow("NeHe's Token, Extensions,

            Scissoring & TGA Loading Tutorial", 640,480,16,fullscreen))

            {

              return 0; // Выход если окно не было создано

            }

          }



        if (keys[VK_UP] && (scroll>0))  // Нажата стрелка вверх?

        {

          scroll-=2; // Если да, то уменьшаем 'scroll', двигая экран вниз

        }


        if (keys[VK_DOWN] && (scroll<32*(maxtokens-9))) // Нажата стрелка вниз?

        {

           scroll+=2; // Если да, то увеличиваем 'scroll', двигая экран вверх

        }

      }

    }

  }

 

  // Завершение

  KillGLWindow(); // Уничтожаем окно

  return (msg.wParam); // Выходим из программы

}
