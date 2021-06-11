
#include <windows.h>       // Заголовочный файл для Windows

#include <math.h>          // Заголовочный файл для математической библиотеки

#include <stdio.h>         // Заголовочный файл для стандартного ввода/вывода

#include <gl\gl.h>         // Заголовочный файл для библиотеки OpenGL32

#include <gl\glu.h>        // Заголовочный файл для библиотеки GLu32

 

HDC       hDC=NULL;        // Контекст устройства

HGLRC     hRC=NULL;        // Контекст рендеринга

HWND      hWnd=NULL;       // Дескриптор окна

HINSTANCE hInstance;       // Экземпляр приложения

 

bool      keys[256];       // Массив для работы с клавиатурой

bool      active=TRUE;     // Флаг активности приложения

bool      fullscreen=TRUE; // Флаг полноэкранного режима

 

Lfloat xrot, yrot, zrot,       // углы вращения по X, Y и Z

       xspeed, yspeed, zspeed, // скорость вращения по X, Y и Z

       cx, cy, cz=-15;         // положение на X, Y и Z

int    key=1;                  // Используется для проверки морфинга

int    step=0, steps=200;      // Счетчик шага и максимальное число шагов

bool   morph=FALSE;            // По умолчанию morph равен False (морфинг выключен)

  Здесь мы создаем структуру для хранения вершин. В ней будут храниться координаты x, y и z любой точки на экране. Переменные x, y и z – вещественные, поэтому мы можем позиционировать точку в любом месте экрана с большой точностью.         

 

typedef struct                 // Структура для вершины

{

        float x, y, z;         // X, Y и Z координаты

} VERTEX;                      // Назовем ее VERTEX

 

typedef struct         // Структура для объекта

{

   int     verts;      // Количество вершин в объекте

   VERTEX* points;     // Одна вершина 

} OBJECT;              // Назовем ее OBJECT



int    maxver;                      // Хранит максимум числа вершин объектов

OBJECT morph1,morph2,morph3,morph4, // Наши объекты для морфинга (morph1, 2, 3 и 4)

       helper,*sour,*dest;          // Вспомогательный объект, Источник и Назначение

 
LRESULTCALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // Объявление WndProc

void objallocate(OBJECT *k, int n) // Выделение памяти для каждого объекта

{                                  // И определение точек

     k->points=(VERTEX*)malloc(sizeof(VERTEX)*n); // points = размер(VERTEX)* число вершин

}                                  // (3 точки для каждой вершины)


void objfree(OBJECT *k)                   // Удаление объекта (Освобождение памяти)

{

     free(k->points);                     // Освобождаем память, занимаемую points

}


void readstr(FILE *f, char *string)  // Считывает строку из файла (f)

{

     do                              // Повторять

     {

          fgets(string, 255, f);     // Считывание 255 символов из файла f в переменную string

     } while ((string[0] == '/') || (string[0] == '\n')); // Пока строка пуста

     return;                         // Возврат

}

 

void objload(char *name, OBJECT *k)  // Загружает объект из файла (name)

{

     int       ver;                  // Будет хранить количество вершин

     float    rx, ry, rz;            // Будут хранить x, y и z координаты вершины

 

 

     FILE  *filein;                  // Файл для работы

     char    oneline[255];           // Хранит одну строку текста (до 255 символов)

    

     filein = fopen(name, "rt");     // Открываем файл на чтение (CTRL-Z означает конец файла)

    

     readstr(filein, oneline);       // Считываем одну строку текста из файла

                                     // Сканируем текст на "Vertices: ".

                                     // Число вершин сохраняем в ver

     sscanf(oneline, "Vertices: %d\n", &ver);

     k->verts=ver;                   // Устанавливаем переменные verts объектов

                                     // равными значению ver

     objallocate(k, ver);            // Выделяем память для хранения объектов


for (int i=0; i<ver; i++)        // Цикл по всем вершинам

{

     readstr(filein, oneline);   // Считывание следующей строки

                                 // Поиск 3 вещественных чисел и сохранение их в rx, ry и rz

     sscanf(oneline, "%f %f %f", &rx, &ry, &rz);


          k->points[i].x = rx;   // Устанавливаем значение points.x объекта (k) равным rx

          k->points[i].y = ry;   // Устанавливаем значение points.y объекта (k) равным ry

          k->points[i].z = rz;   // Устанавливаем значение points.z объекта (k) равным rz

     }

     fclose(filein);             // Закрываем файл

 

     if(ver > maxver) maxver=ver;// Если ver больше чем maxver, устанавливаем maxver равным ver

}                                // Следим за максимальным числом используемых вершин



VERTEX calculate(int i)    // Вычисление перемещения точек в процессе морфинга

{

     VERTEX a;             // Временная вершина a

                           // a.x равно x Источника - x Назначения делить на Steps

     a.x=(sour->points[i].x-dest->points[i].x)/steps;

                           // a.y равно y Источника - y Назначения делить на Steps

     a.y=(sour->points[i].y-dest->points[i].y)/steps;

                           // a.z равно z Источника - z Назначения делить на Steps

     a.z=(sour->points[i].z-dest->points[i].z)/steps;

     return a;             // Возвращаем результат

}                          // Возвращаем вычисленные точки



int InitGL(GLvoid)                         // Здесь задаются все установки для OpenGL

{

     glBlendFunc(GL_SRC_ALPHA, GL_ONE);    // Устанавливаем функцию смешивания

     glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Очищаем фон в черный цвет

     glClearDepth(1.0);                    // Очищаем буфер глубины

     glDepthFunc(GL_LESS);                 // Устанавливаем тип теста глубины

     glEnable(GL_DEPTH_TEST);              // Разрешаем тест глубины

     glShadeModel(GL_SMOOTH);              // Разрешаем плавное цветовое сглаживание

     glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Улучшенные вычисления перспективы

 

     maxver=0;                            // Устанавливаем максимум вершин в 0 по умолчанию

     objload("data/sphere.txt", &morph1); // Загружаем первый объект в morph1 из файла sphere.txt

     objload("data/torus.txt", &morph2);  // Загружаем второй объект в morph2 из файла torus.txt

     objload("data/tube.txt", &morph3);   // Загружаем третий объект в morph3 из файла tube.txt


objallocate(&morph4, 486);     // Резервируем память для 486 вершин четвертого объекта (morph4)

for(int i=0; i<486; i++)       // Цикл по всем 486 вершинам

{

     // Точка x объекта morph4 принимает случайное значение от -7 до 7

     morph4.points[i].x=((float)(rand()%14000)/1000)-7;

     // Точка y объекта morph4 принимает случайное значение от -7 до 7

     morph4.points[i].y=((float)(rand()%14000)/1000)-7;

     // Точка z объекта morph4 принимает случайное значение от -7 до 7

     morph4.points[i].z=((float)(rand()%14000)/1000)-7;

}



     // Загружаем sphere.txt в helper (используется как отправная точка)

     objload("data/sphere.txt",&helper);

     sour=dest=&morph1;  // Источник и Направление приравниваются к первому объекту (morph1)

    

     return TRUE;        // Инициализация прошла успешно

}

 

void DrawGLScene(GLvoid)          // Здесь происходит все рисование

{

     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очищаем экран и буфер глубины

     glLoadIdentity();            // Сбрасываем просмотр

     glTranslatef(cx, cy, cz);    // Сдвигаем текущую позицию рисования

     glRotatef(xrot, 1, 0, 0);    // Вращаем по оси X на xrot

     glRotatef(yrot, 0, 1, 0);    // Вращаем по оси Y на yrot

     glRotatef(zrot, 0, 0, 1);    // Вращаем по оси Z на zrot

    

     // Увеличиваем xrot, yrot и zrot на xspeed, yspeed и zspeed

     xrot+=xspeed; yrot+=yspeed; zrot+=zspeed;

    

     GLfloat tx, ty, tz;          // Временные переменные X, Y и Z

     VERTEX q;                    // Хранит вычисленные значения для одной вершины


     glBegin(GL_POINTS);  // Начало рисования точек

          // Цикл по всем вершинам объекта morph1 (все объекты состоят из

          for(int i=0; i<morph1.verts; i++)

          {    // одинакового количества вершин, также можно использовать maxver)

               // Если morph равно True, вычисляем перемещение, иначе перемещение = 0

               if(morph) q=calculate(i); else q.x=q.y=q.z=0;

               // Вычитание q.x единиц из helper.points[i].x (перемещение по оси X)

               helper.points[i].x-=q.x;

               // Вычитание q.y единиц из helper.points[i].y (перемещение по оси Y)

               helper.points[i].y-=q.y;

               // Вычитание q.z единиц из helper.points[i].z (перемещение по оси Z)

               helper.points[i].z-=q.z;

               // Делаем временную переменную X равной вспомогательной X

               tx=helper.points[i].x;

               // Делаем временную переменную Y равной вспомогательной Y

               ty=helper.points[i].y;

               // Делаем временную переменную Z равной вспомогательной Z

               tz=helper.points[i].z;

 

glEnd() говорит OpenGL о том, что мы закончили рисовать точки.

 

               glColor3f(0, 1, 1);                // Установить цвет в ярко голубой

               glVertex3f(tx, ty, tz);            // Нарисовать точку

               glColor3f(0, 0.5f, 1);             // Темный цвет

               tx-=2*q.x; ty-=2*q.y; ty-=2*q.y;   // Вычислить на две позиции вперед

               glVertex3f(tx, ty, tz);            // Нарисовать вторую точку

               glColor3f(0, 0, 1);                // Очень темный цвет

               tx-=2*q.x; ty-=2*q.y; ty-=2*q.y;   // Вычислить еще на две позиции вперед

               glVertex3f(tx, ty, tz);            // Нарисовать третью точку

          }                              // Это создает призрачный хвост, когда точки двигаются

glEnd();                                          // Закончим рисовать точки

  

     // Если делаем морфинг и не прошли все 200 шагов, то увеличим счетчик

     // Иначе сделаем морфинг ложью, присвоим источник назначению и счетчик обратно в ноль

     if(morph && step<=steps) step++; else { morph=FALSE; sour=dest; step=0; }

}

 
Lvoid KillGLWindow(GLvoid) // Правильное завершение работы окна

{

     objfree(&morph1);     // Освободим память

     objfree(&morph2);

     objfree(&morph3);

     objfree(&morph4);

     objfree(&helper);

     if (fullscreen)       // Полноэкранный режим?

     {

          ChangeDisplaySettings(NULL, 0);  // Перейти обратно в режим рабочего стола

          ShowCursor(TRUE);                // Показать указатель мыши

     }

     if (hRC)                              // Есть контекст визуализации?

     {

          if (!wglMakeCurrent(NULL, NULL)) // Можем освободим контексты DC и RC?

          {

               MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK |                 

                                     MB_ICONINFORMATION);

          }

          if (!wglDeleteContext(hRC))

          {

               MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK |            

                                     MB_ICONINFORMATION);

          }

          hRC=NULL;                        // Установить RC в NULL

     }

     if (hDC && !ReleaseDC(hWnd, hDC))

     {

          MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK |

                                MB_ICONINFORMATION);

          hDC=NULL;

     }

     if (hWnd && !DestroyWindow(hWnd))     // Можем удалить окно?

     {

          MessageBox(NULL, "Could Not Release hWnd.", "SHUTDOWN ERROR", MB_OK |

                                MB_ICONINFORMATION);

          hWnd=NULL;                       // Установить hWnd в NULL

     }

     if (!UnregisterClass("OpenGL", hInstance))

     {

          MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK |                 

                                MB_ICONINFORMATION);

          hInstance=NULL;

     }

}

BOOL CreateGLWindow()                      // Создает GL окно

LRESULT CALLBACK WndProc()                 // Дескриптор этого окна

  

int WINAPI WinMain( HINSTANCE  hInstance,     // Экземпляр

                    HINSTANCE  hPrevInstance, // Предыдущий экземпляр

                    LPSTR      lpCmdLine,     // Параметры командной строки

                    int        nCmdShow)      // Состояние отображения окна

{

     MSG   msg;                               // Структура сообщений Windows

     BOOL done=FALSE;                         // Переменная для выхода из цикла

 

     // Спросить пользователя какой он предпочитает режим

     if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start  

     FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)

     {

          fullscreen=FALSE;                   // Оконный режим

     }

 

     // Create Our OpenGL Window

     if (!CreateGLWindow(

          "Piotr Cieslak & NeHe's Morphing Points tutorial",640,480,16,fullscreen))

     {

          return 0;                           // Выходим если окно не было создано

     }

     while(!done)                             // Цикл, который продолжается пока done=FALSE

     {

          if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) // Есть ожидаемое сообщение?

          {

               if (msg.message==WM_QUIT)      // Мы получили сообщение о выходе?

               {

                    done=TRUE;                // Если так done=TRUE

               }

               else                           // Если нет, продолжаем работать с сообщениями окна

               {

                    TranslateMessage(&msg);   // Переводим сообщение

                    DispatchMessage(&msg);    // Отсылаем сообщение

               }

          }

          else                                // Если сообщений нет

          {

              // Рисуем сцену. Ожидаем нажатия кнопки ESC

              // Активно?  Было получено сообщение о выходе?

               if (active && keys[VK_ESCAPE])

               {

                    done=TRUE;                // ESC просигналил "выход"

               }

               else                           // Не время выходить, обновляем экран

               {

 

                    // Нарисовать сцену (Не рисовать, когда неактивно 1% использования CPU)

                    DrawGLScene();

                    SwapBuffers(hDC);         // Переключаем буферы (Двойная буферизация)

 
       if (keys[VK_PRIOR]) // Page Up нажата?

       zspeed+=0.01f;      // Увеличиваем zspeed

 

       if (keys[VK_NEXT])  // Page Down нажата?

       zspeed-=0.01f;      // Уменьшаем zspeed

 

       if (keys[VK_DOWN])  // Стрелка Вниз нажата?

       xspeed+=0.01f;      // Увеличиваем xspeed

 

       if (keys[VK_UP])    // Стрелка Вверх нажата?

       xspeed-=0.01f;      // Уменьшаем xspeed

 

       if (keys[VK_RIGHT]) // Стрелка Вправо нажата?

       yspeed+=0.01f ;     // Увеличиваем yspeed

 

       if (keys[VK_LEFT])  // Стрелка Влево нажата?

       yspeed-=0.01f;      // Уменьшаем yspeed

 

       if (keys['Q'])      // Клавиша Q нажата и удерживается?

       cz-=0.01f;          // Перемещение объекта прочь от зрителя

 

       if (keys['Z'])      // Клавиша Z нажата и удерживается?

       cz+=0.01f;          // Перемещение объекта к зрителю

 

       if (keys['W'])      // Клавиша W нажата и удерживается?

       cy+=0.01f;          // Перемещение объекта вверх

 

       if (keys['S'])      // Клавиша S нажата и удерживается?

       cy-=0.01f;          // Перемещение объекта вниз

 

       if (keys['D'])      // Клавиша D нажата и удерживается?

       cx+=0.01f;          // Перемещение объекта вправо

 

       if (keys['A'])      // Клавиша A нажата и удерживается?

       cx-=0.01f;          // Перемещение объекта влево

  

       if (keys['1'] && (key!=1) && !morph) // Если нажата 1, key не равно 1 и morph равен False

       {

              key=1; // Устанавливаем key в 1 (для предотвращения нажатия 1 два раза подряд)

              morph=TRUE; // Устанавливаем morph в True (Начинаем процесс морфинга)

              dest=&morph1; // Устанавливаем объект-назначение в morph1

       }

       if (keys['2'] && (key!=2) && !morph) // Если нажата 2, key не равно 2 и morph равен False

       {

              key=2; // Устанавливаем key в 2 (для предотвращения нажатия 2 два раза подряд)

              morph=TRUE; // Устанавливаем morph в True (Начинаем процесс морфинга)

              dest=&morph2; // Устанавливаем объект-назначение в morph2

       }

       if (keys['3'] && (key!=3) && !morph) // Если нажата 3, key не равно 3 и morph равен False

       {

              key=3; // Устанавливаем key в 3 (для предотвращения нажатия 3 два раза подряд)

              morph=TRUE; // Устанавливаем morph в True (Начинаем процесс морфинга)

              dest=&morph3; // Устанавливаем объект-назначение в morph3

       }

       if (keys['4'] && (key!=4) && !morph) // Если нажата 4, key не равно 4 и morph равен False

       {

              key=4; // Устанавливаем key в 4 (для предотвращения нажатия 4 два раза подряд)

              morph=TRUE; // Устанавливаем morph в True (Начинаем процесс морфинга)

              dest=&morph4; // Устанавливаем объект-назначение в morph4

       }


            if (keys[VK_F1])       // Нажата клавиша F1?

            {

               keys[VK_F1]=FALSE;  // Если да, то устанавливаем ее в FALSE

               KillGLWindow();     // Убиваем наше текущее окно

               fullscreen=!fullscreen; // Переключаемся в Полноэкранный/Оконный режим

               // Регенерируем наше OpenGL окно

               if (!CreateGLWindow("Piotr Cieslak & NeHe's Morphing Points Tutorial",

                    640, 480, 16, fullscreen))

               {

                  return 0;        // Выход если окно не было создано

               }

            }

         }

      }

   }

   // Завершение

   KillGLWindow();                 // Убиваем окно

   return (msg.wParam);            // Выходим из программы

}

