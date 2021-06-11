
#include <windows.h>        // Заголовочный файл для Windows

#include <math.h>           // Заголовочный файл для математической

                            // библиотеки Windows  (Добавлено)

#include <stdio.h>          // Заголовочный файл для стандартной библиотеки

                            //ввода/вывода (Добавлено)

#include <gl\gl.h>          // Заголовочный файл для библиотеки OpenGL32

#include <gl\glu.h>         // Заголовочный файл для библиотеки GLu32

#include <gl\glaux.h>       // Заголовочный файл для библиотеки GLaux

HDC             hDC=NULL;   // Приватный контекст устройства GDI

HGLRC           hRC=NULL;   // Постоянный контекст визуализации

HWND            hWnd=NULL;  // Сохраняет дескриптор окна

HINSTANCE       hInstance;  // Сохраняет экземпляр приложения

bool    keys[256];          // Массив для работы с клавиатурой

bool    active=TRUE;        // Флаг активации окна, по умолчанию = TRUE

bool    fullscreen=TRUE;    // Флаг полноэкранного вывода

  

GLuint  base;       // Основной список отображения для шрифта

GLuint  texture[2]; // Место для текстуры нашего шрифта

GLuint  loop;       // Общая переменная для циклов

GLfloat cnt1;       // Первый счетчик для движения и раскрашивания текста

GLfloat cnt2;       // Второй счетчик для движения и раскрашивания текста

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);   // Объявление WndProc

 
AUX_RGBImageRec *LoadBMP(char *Filename) // Загрузка изображения

{

       FILE *File=NULL;                  // Дескриптор файла

       if (!Filename)                    // Удостоверимся, что имя файла передано

       {

              return NULL;               // Если нет, возвратим NULL

       }

       File=fopen(Filename,"r");         // Проверка, существует ли файл

       if (File)                         // Существует?

       {

              fclose(File);              // Закрываем файл

              // Загружаем изображение и возвращаем указатель

             return auxDIBImageLoad(Filename);

       }

       return NULL;                      // Если загрузка не удалась, возвращаем NULL

}

  

int LoadGLTextures()                           // Загрузка и преобразование текстур

{

       int Status=FALSE;                       // Индикатор статуса

       AUX_RGBImageRec *TextureImage[2];       // Место хранения для текстур

  

memset(TextureImage,0,sizeof(void *)*2);          // Устанавливаем указатель в NULL

 

if ((TextureImage[0]=LoadBMP("Data/Font.bmp")) &&// Загружаем изображение шрифта (TextureImage[1]=LoadBMP("Data/Bumps.bmp")))     // Загружаем текстуру

       {

              Status=TRUE;                        // Устанавливаем статус в TRUE



glGenTextures(2, &texture[0]);            // Создание 2-х текстур

 

for (loop=0; loop<2; loop++)              // Цикл для всех текстур

{

            // Создание всех текстур

            glBindTexture(GL_TEXTURE_2D, texture[loop]);

            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, 3,

TextureImage[loop]->sizeX, TextureImage[loop]->sizeY, 0,

GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop]->data);

}

}

 



for (loop=0; loop<2; loop++)

       {

       if (TextureImage[loop])                // Если текстура существует

              {

              if (TextureImage[loop]->data)   // Если изображение текстуры существует

              {

                    // Освобождаем память от изображения текстуры

                    free(TextureImage[loop]->data);

              }

              free(TextureImage[loop]);   // Освобождаем память от структуры изображения

              }

       }

return Status;                                // Возвращаем статус

}

 

GLvoid BuildFont(GLvoid)                   // Создаем список отображения нашего шрифта

{



       float       cx;                           // Содержит X координату символа

       float       cy;                           // Содержит Y координату символа

 



       base=glGenLists(256);                    // Создаем списки

       glBindTexture(GL_TEXTURE_2D, texture[0]);// Выбираем текстуру шрифта

 

       for (loop=0; loop<256; loop++)           // Цикл по всем 256 спискам

       {

 

       cx=float(loop%16)/16.0f;        // X координата текущего символа

       cy=float(loop/16)/16.0f;        // Y координата текущего символа

  

       glNewList(base+loop,GL_COMPILE); // Начинаем делать список

 


       glBegin(GL_QUADS);              // Используем четырехугольник, для каждого символа

 



                    glTexCoord2f(cx,1-cy-0.0625f);  // Точка в текстуре (Левая нижняя)

                    glVertex2i(0,0);                // Координаты вершины (Левая нижняя)

  

                    // Точка на текстуре (Правая нижняя)

                    glTexCoord2f(cx+0.0625f,1-cy-0.0625f);

                    glVertex2i(16,0); // Координаты вершины (Правая нижняя)



                    glTexCoord2f(cx+0.0625f,1-cy);  // Точка текстуры (Верхняя правая)

                    glVertex2i(16,16);              // Координаты вершины (Верхняя правая)

 
                    glTexCoord2f(cx,1-cy);  // Точка текстуры (Верхняя левая)

                    glVertex2i(0,16);       // Координаты вершины (Верхняя левая)

              glEnd();                      // Конец построения четырехугольника (Символа)

  

       glTranslated(10,0,0);           // Двигаемся вправо от символа

       glEndList();                    // Заканчиваем создавать список отображения

}                                      // Цикл для создания всех 256 символов

}

 

GLvoid KillFont(GLvoid)                // Удаляем шрифт из памяти

{

       glDeleteLists(base,256);        // Удаляем все 256 списков отображения

}

 


GLvoid glPrint(GLint x, GLint y, char *string, int set) // Где печатать

{


       if (set>1)                     // Больше единицы?

       {

              set=1;                  // Сделаем Set равное единице

       }

 
       glBindTexture(GL_TEXTURE_2D, texture[0]);       // Выбираем нашу текстуру шрифта

 


       glDisable(GL_DEPTH_TEST);               // Отмена проверки глубины

 

       glMatrixMode(GL_PROJECTION);            // Выбираем матрицу проекции

       glPushMatrix();                         // Сохраняем матрицу проекции

       glLoadIdentity();                       // Сбрасываем матрицу проекции

       glOrtho(0,640,0,480,-1,1);              // Устанавливаем плоский экран

  
       glMatrixMode(GL_MODELVIEW);             // Выбираем матрицу модели просмотра

       glPushMatrix();                         // Сохраняем матрицу модели просмотра

       glLoadIdentity();                       // Сбрасываем матрицу модели просмотра

 

       glTranslated(x,y,0);                    // Позиция текста (0,0 - Нижняя левая)



       glListBase(base-32+(128*set));         // Выбираем набор символов (0 или 1)

 
       glCallLists(strlen(string),GL_BYTE,string); // Рисуем текст на экране



       glMatrixMode(GL_PROJECTION);            // Выбираем матрицу проекции

       glPopMatrix();                          // Восстанавливаем старую матрицу проекции

       glMatrixMode(GL_MODELVIEW);             // Выбираем матрицу просмотра модели

       glPopMatrix();                          // Восстанавливаем старую матрицу проекции


       glEnable(GL_DEPTH_TEST);                // Разрешаем тест глубины

}

 

int InitGL(GLvoid)                             // Все установки для OpenGL здесь

{



       if (!LoadGLTextures()) // Переходим к загрузке текстуры

       {

              return FALSE;   // Если текстура не загрузилась - возвращаем FALSE

       }

 



       BuildFont();  // Создаем шрифт



       glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Очищаем фон черным цветом

       glClearDepth(1.0);                    // Очистка и сброс буфера глубины

       glDepthFunc(GL_LEQUAL);               // Тип теста глубины

       glBlendFunc(GL_SRC_ALPHA,GL_ONE);     // Выбор типа смешивания

 

       glShadeModel(GL_SMOOTH);              // Сглаженное заполнение

       glEnable(GL_TEXTURE_2D);              // 2-мерное текстурирование

 

       return TRUE;                          // Инициализация прошла успешно

}

 



int DrawGLScene(GLvoid)                             // Здесь мы рисуем все объекты

{

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очистка экрана и буфера глубины

glLoadIdentity();                                   // Сброс матрицы просмотра модели



glBindTexture(GL_TEXTURE_2D, texture[1]);// Выбираем вторую текстуру

glTranslatef(0.0f,0.0f,-5.0f);           // Сдвигаемся на 5 единиц вглубь экрана

 

       glRotatef(45.0f,0.0f,0.0f,1.0f); // Поворачиваем на 45 градусов (по часовой стрелке)


       glRotatef(cnt1*30.0f,1.0f,1.0f,0.0f); // Вращение по X & Y на cnt1 (слева направо)



       glDisable(GL_BLEND);            // Отменяем смешивание перед рисованием 3D

       glColor3f(1.0f,1.0f,1.0f);      // Ярко белый

       glBegin(GL_QUADS);              // Рисуем первый текстурированный прямоугольник

       glTexCoord2d(0.0f,0.0f);        // Первая точка на текстуре

 

              glVertex2f(-1.0f, 1.0f); // Первая вершина

              glTexCoord2d(1.0f,0.0f); // Вторая точка на текстуре

              glVertex2f( 1.0f, 1.0f); // Вторая вершина

              glTexCoord2d(1.0f,1.0f); // Третья точка на текстуре

              glVertex2f( 1.0f,-1.0f); // Третья вершина

              glTexCoord2d(0.0f,1.0f); // Четвертая точка на текстуре

              glVertex2f(-1.0f,-1.0f); // Четвертая вершина

 

       glEnd();                        // Заканчиваем рисование четырехугольника

 

// Поворачиваемся по X и Y на 90 градусов (слева на право)

glRotatef(90.0f,1.0f,1.0f,0.0f);

glBegin(GL_QUADS);                 // Рисуем второй текстурированный четырехугольник

              glTexCoord2d(0.0f,0.0f); // Первая точка на текстуре

              glVertex2f(-1.0f, 1.0f); // Первая вершина

              glTexCoord2d(1.0f,0.0f); // Вторая точка на текстуре

              glVertex2f( 1.0f, 1.0f); // Вторая вершина

              glTexCoord2d(1.0f,1.0f); // Третья точка на текстуре

              glVertex2f( 1.0f,-1.0f); // Третья вершина

              glTexCoord2d(0.0f,1.0f); // Четвертая точка на текстуре

              glVertex2f(-1.0f,-1.0f); // Четвертая вершина

       glEnd();                        // Заканчиваем рисовать четырехугольник

 


       glEnable(GL_BLEND);         // Разрешаем смешивание

       glLoadIdentity();           // Сбрасываем просмотр

 
       // Изменение цвета основывается на положении текста

       glColor3f(1.0f*float(cos(cnt1)),1.0f*float(sin(cnt2)),

              1.0f-0.5f*float(cos(cnt1+cnt2)));



       // Печатаем GL текст на экране

       glPrint(int((280+250*cos(cnt1))),int(235+200*sin(cnt2)),"NeHe",0);

       glColor3f(1.0f*float(sin(cnt2)),

              1.0f-0.5f*float(cos(cnt1+cnt2)),1.0f*float(cos(cnt1)));

       // Печатаем GL текст на экране

       glPrint(int((280+230*cos(cnt2))),int(235+200*sin(cnt1)),"OpenGL",1);



       glColor3f(0.0f,0.0f,1.0f);        // Устанавливаем синий цвет

       glPrint(int(240+200*cos((cnt2+cnt1)/5)),

              2,"Giuseppe D'Agata",0);   // Рисуем текст на экране

       glColor3f(1.0f,1.0f,1.0f);        // Устанавливаем белый цвет

       glPrint(int(242+200*cos((cnt2+cnt1)/5)),

              2,"Giuseppe D'Agata",0);   // Рисуем смещенный текст

 
       cnt1+=0.01f;               // Увеличим первый счетчик

       cnt2+=0.0081f;             // Увеличим второй счетчик

       return TRUE;               // Все прошло успешно

}

 
int WINAPI WinMain( HINSTANCE       hInstance,          // Экземпляр

                    HINSTANCE       hPrevInstance,      // Предыдущий экземпляр

                    LPSTR           lpCmdLine,          // Параметры командной строки

                    int             nCmdShow)           // Стиль вывода окна

{

       MSG           msg;          // Структура сообщения

       BOOL       done=FALSE;      // Переменная для выхода из цикла

 

       // Спрашиваем у пользователя, какой режим он предпочитает

       if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?",

              "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO) {

              fullscreen=FALSE;       // Режим окна

       }



       // Создаем окно OpenGL

       if (!CreateGLWindow(

              "NeHe & Giuseppe D'Agata's 2D Font Tutorial",640,480,16,fullscreen))

       {

              return 0;           // Окно не создалось - выходим

       }

       while(!done)               // Цикл пока done=FALSE

       {

              if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) // Пришло сообщение?

              {

                    if (msg.message==WM_QUIT)           // Это сообщение о выходе?

                    {

                           done=TRUE; // Если да, то done=TRUE

                    }

                    else              // Если нет, то обрабатываем сообщение

                    {

                           TranslateMessage(&msg);// Переводим сообщение

                           DispatchMessage(&msg); // Отсылаем сообщение

                    }

              }

              else                    // Нет сообщений

              {

     // Рисуем сцену.  Ждем клавишу ESC или сообщение о выходе из DrawGLScene()

              // Активно?  Было сообщение о выходе?

                    if ((active && !DrawGLScene()) || keys[VK_ESCAPE])

                    {

                           done=TRUE; // ESC или DrawGLScene сообщает о выходе

                    }

                    else              // Не время выходить, обновляем экран

                    {

                           SwapBuffers(hDC); // Меняем экраны (Двойная буферизация)

                    }

              }

       }

       // Закрываем приложение

 
       if (!UnregisterClass("OpenGL",hInstance)) // Можем удалить регистрацию класса

       {

             MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",

                  MB_OK | MB_ICONINFORMATION);

             hInstance=NULL;       // Устанавливаем hInstance в NULL

       }

       KillFont();                 // Уничтожаем шрифт

}
    
