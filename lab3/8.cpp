#include <windows.h>  // Заголовочный файл для Windows

#include <stdio.h>    // Заголовочный файл для стандартного ввода/вывода (ДОБАВИЛИ)

#include <gl\gl.h>    // Заголовочный файл для библиотеки OpenGL32

#include <gl\glu.h>   // Заголовочный файл для для библиотеки GLu32

#include <gl\glaux.h> // Заголовочный файл для библиотеки GLaux

 

HDC    hDC=NULL;      // Служебный контекст GDI устройства

HGLRC  hRC=NULL;      // Постоянный контекст для визуализации

HWND   hWnd=NULL;     // Содержит дискриптор для окна

HINSTANCE hInstance;  // Содержит данные для нашей программы

 

bool keys[256];       // Массив, использующийся для сохранения состояния клавиатуры

bool active=TRUE;     // Флаг состояния активности приложения (по умолчанию: TRUE)

bool fullscreen=TRUE; // Флаг полноэкранного режима (по умолчанию: полноэкранное)

  Строки ниже новые. Мы собираемся добавлять три логических переменных. Тип BOOL означает, что переменная может только быть ИСТИННА (TRUE) или ЛОЖЬ (FALSE). Мы создаем переменную называемую light, чтобы отслеживать, действительно ли освещение включено или выключено. Переменные lp и fp используются, для отслеживания нажатия клавиш 'L' и 'F'. Я объясню, почему нам нужны эти переменные позже. Пока, запомните, что они необходимы.

 

bool  blend; // Смешивание НЕТ/ДА? (НОВОЕ)

bool  light; // Освещение Вкл./Выкл.

bool  lp;    // L Нажата?

bool  fp;    // F Нажата?

bool  bp;    // B Нажата? ( Новое )

 

GLfloat xrot;     // Вращение вдоль оси X

GLfloat yrot;     // Вращение вдоль оси Y

GLfloat xspeed;   // Скорость вращения вдоль оси X

GLfloat yspeed;   // Скорость вращения вдоль оси X

GLfloat z=-5.0f;  // Глубина в экран.  
 

// Задаем параметры освещения

GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };

GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };

GLfloat LightPosition[]={ 0.0f, 0.0f, 2.0f, 1.0f };

 


 

GLuint filter;         // Какой фильтр использовать

GLuint texture[3];     // Место для хранения 3 текстур

 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);    // Декларация WndProc

  texture1 = auxDIBImageLoad("Data/glass.bmp");
 


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

 if (TextureImage[0]=LoadBMP("Data/Crate.bmp"))

 {

  Status=TRUE;       // Установим Status в TRUE

 



glGenTextures(3, &texture[0]);     // Создание трех текстур

 



  // Создание текстуры с фильтром по соседним пикселям

  glBindTexture(GL_TEXTURE_2D, texture[0]);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // ( НОВОЕ )

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // ( НОВОЕ )

  glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY,

   0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);

 



  // Создание текстуры с линейной фильтрацией

  glBindTexture(GL_TEXTURE_2D, texture[1]);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY,

   0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);

 



  // Создание Текстуры с Мип-Наложением

  glBindTexture(GL_TEXTURE_2D, texture[2]);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // ( НОВОЕ )

 



  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY,

   GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data); // ( НОВОЕ )

 }

  

 if (TextureImage[0])           // Если текстура существует

 {

  if (TextureImage[0]->data)    // Если изображение текстуры существует

  {

   free(TextureImage[0]->data); // Освобождение памяти изображения текстуры

  }

 

  free(TextureImage[0]);        // Освобождение памяти под структуру

 }

  

 return Status;        // Возвращаем статус

}

 

int InitGL(GLvoid)             // Все настройки для OpenGL делаются здесь

{
  glColor4f(1.0f,1.0f,1.0f,0.5f);   // Полная яркость, 50% альфа (НОВОЕ)

  glBlendFunc(GL_SRC_ALPHA,GL_ONE); // Функция смешивания для непрозрачности,

                                  // базирующаяся на значении альфы(НОВОЕ)

 if (!LoadGLTextures())        // Переход на процедуру загрузки текстуры

 {

  return FALSE;                // Если текстура не загружена возвращаем FALSE

 }

 

 glEnable(GL_TEXTURE_2D);      // Разрешить наложение текстуры

 glShadeModel(GL_SMOOTH);      // Разрешение сглаженного закрашивания

 glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // Черный фон

 glClearDepth(1.0f);           // Установка буфера глубины

 glEnable(GL_DEPTH_TEST);      // Разрешить тест глубины

 glDepthFunc(GL_LEQUAL);       // Тип теста глубины

 glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Улучшенные вычисления перспективы

 



glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);    // Установка Фонового Света

 



glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);    // Установка Диффузного Света

 



 glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);   // Позиция света

 



 glEnable(GL_LIGHT1); // Разрешение источника света номер один

 return TRUE;         // Инициализация прошла OK

}

 



int DrawGLScene(GLvoid)        // Здесь мы делаем все рисование

{

 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   // Очистка Экрана и Буфера Глубины

 glLoadIdentity();       // Сброс Просмотра

 



 glTranslatef(0.0f,0.0f,z);      // Перенос В/Вне экрана по z

 glRotatef(xrot,1.0f,0.0f,0.0f); // Вращение по оси X на xrot

 glRotatef(yrot,0.0f,1.0f,0.0f); // Вращение по оси Y по yrot

 



 glBindTexture(GL_TEXTURE_2D, texture[filter]);    // Выбор текстуры основываясь на filter

 

 glBegin(GL_QUADS);       // Начало рисования четырехугольников

 



  // Передняя грань

  glNormal3f( 0.0f, 0.0f, 1.0f);     // Нормаль указывает на наблюдателя

  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f); // Точка 1 (Перед)

  glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f); // Точка 2 (Перед)

  glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f); // Точка 3 (Перед)

  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f); // Точка 4 (Перед)

  // Задняя грань

  glNormal3f( 0.0f, 0.0f,-1.0f);     // Нормаль указывает от наблюдателя

  glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // Точка 1 (Зад)

  glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f); // Точка 2 (Зад)

  glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f); // Точка 3 (Зад)

  glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f); // Точка 4 (Зад)

  // Верхняя грань

  glNormal3f( 0.0f, 1.0f, 0.0f);     // Нормаль указывает вверх

  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f); // Точка 1 (Верх)

  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f); // Точка 2 (Верх)

  glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f); // Точка 3 (Верх)

  glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f); // Точка 4 (Верх)

  // Нижняя грань

  glNormal3f( 0.0f,-1.0f, 0.0f);     // Нормаль указывает вниз

  glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // Точка 1 (Низ)

  glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f); // Точка 2 (Низ)

  glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f); // Точка 3 (Низ)

  glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f); // Точка 4 (Низ)

  // Правая грань

  glNormal3f( 1.0f, 0.0f, 0.0f);     // Нормаль указывает вправо

  glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f); // Точка 1 (Право)

  glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f); // Точка 2 (Право)

  glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f); // Точка 3 (Право)

  glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f); // Точка 4 (Право)

  // Левая грань

  glNormal3f(-1.0f, 0.0f, 0.0f);     // Нормаль указывает влево

  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f); // Точка 1 (Лево)

  glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f); // Точка 2 (Лево)

  glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f); // Точка 3 (Лево)

  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f); // Точка 4 (Лево)

 glEnd();        // Кончили рисовать четырехугольник

  

 xrot+=xspeed;        // Добавить в xspeed значение xrot

 yrot+=yspeed;        // Добавить в yspeed значение yrot

 return TRUE;         // Выйти

}

 



    SwapBuffers(hDC);     // Переключение буферов (Двойная буферизация)

    if (keys['L'] && !lp) // Клавиша 'L' нажата и не удерживается?

    {

 



     lp=TRUE;      // lp присвоили TRUE

     light=!light; // Переключение света TRUE/FALSE

  

     if (!light)               // Если не свет

     {

      glDisable(GL_LIGHTING);  // Запрет освещения

     }

     else                      // В противном случае

     {

      glEnable(GL_LIGHTING);   // Разрешить освещение

     }

    }

 



    if (!keys['L']) // Клавиша 'L' Отжата?

    {

     lp=FALSE;      // Если так, то lp равно FALSE

    }

 



    if (keys['F'] && !fp) // Клавиша 'F' нажата?

    {

     fp=TRUE;             // fp равно TRUE

     filter+=1;           // значение filter увеличивается на один

     if (filter>2)        // Значение больше чем 2?

     {

      filter=0;           // Если так, то установим filter в 0

     }

    }

    if (!keys['F'])       // Клавиша 'F' отжата?

    {

     fp=FALSE;            // Если так, то fp равно FALSE

    }

 



    if (keys[VK_PRIOR])   // Клавиша 'Page Up' нажата?

    {

     z-=0.02f;            // Если так, то сдвинем вглубь экрана

    }

 



    if (keys[VK_NEXT])    // Клавиша 'Page Down' нажата?

    {

     z+=0.02f;            // Если так, то придвинем к наблюдателю

    }

  

    if (keys[VK_UP])     // Клавиша стрелка вверх нажата?

    {

     xspeed-=0.01f;      // Если так, то уменьшим xspeed

    }

    if (keys[VK_DOWN])   // Клавиша стрелка вниз нажата?

    {

     xspeed+=0.01f;      // Если так, то увеличим xspeed

    }

    if (keys[VK_RIGHT])  // Клавиша стрелка вправо нажата?

    {

     yspeed+=0.01f;      // Если так, то увеличим yspeed

    }

    if (keys[VK_LEFT])   // Клавиша стрелка влево нажата?

    {

     yspeed-=0.01f;      // Если так, то уменьшим yspeed

    }
    
   if (keys['B'] && !bp)                     

       {

              bp=TRUE;                        

              blend = !blend;                  // Инвертируем blend 

              if(blend)                        // blend TRUE?

              {

                    glEnable(GL_BLEND);        // Включаем смешивание

                    glDisable(GL_DEPTH_TEST);  // Выключаем тест глубины

              }

              else                            

              {

                    glDisable(GL_BLEND);        // Выключаем смешивание

                    glEnable(GL_DEPTH_TEST);    // Включаем тест глубины

              }

       }

       if (!keys['B'])                    // ’B’ отжата?

             {

              bp=FALSE;                         // тогда bp возвращает ложь

             }    

 

    if (keys[VK_F1])          // Клавиша 'F1' нажата?

    {

     keys[VK_F1]=FALSE;       // Если так, то сделаем Key FALSE

     KillGLWindow();          // Уничтожим наше текущее окно

     fullscreen=!fullscreen;  // Переключение между режимами Полноэкранный/Оконный

                              // Повторное создание нашего окна OpenGL

     if (!CreateGLWindow("Урок NeHe Текстуры, Свет & Обработка Клавиатуры",640,480,16,fullscreen))

     {

      return 0;               // Выход, если окно не создано

     }

    }

   }

  }

 }

 

 // Сброс

 KillGLWindow();              // Уничтожение окна

 return (msg.wParam);         // Выход из программы

}
