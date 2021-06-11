
#include <windows.h>  // Заголовочный файл для Windows

#include <math.h>     // Заголовочный файл для математической библиотеки Windows

#include <stdio.h>    // Заголовочный файл для стандартной библиотеки ввода/вывода

#include <gl\gl.h>    // Заголовочный файл для библиотеки OpenGL32

#include <gl\glu.h>   // Заголовочный файл для библиотеки GLu32

#include <gl\glaux.h> // Заголовочный файл для библиотеки Glaux

 

HDC        hDC=NULL;  // Приватный контекст устройства GDI

HGLRC      hRC=NULL;  // Постоянный контекст визуализации

HWND       hWnd=NULL; // Сохраняет дескриптор окна

HINSTANCE  hInstance; // Сохраняет экземпляр приложения

  

bool  keys[256];      // Массив для работы с клавиатурой

bool  active=TRUE;    // Флаг активации окна, по умолчанию = TRUE

bool  fullscreen=TRUE;// Флаг полноэкранного режима

 

bool  masking=TRUE;   // Маскирование Вкл/Выкл

bool  mp;             // M нажата?

bool  sp;             // Пробел нажат?

bool  scene;          // Какая сцена выводиться

 

GLuint  texture[5];   // Память для пяти наших текстур

GLuint  loop;         // Общая переменная цикла

 

GLfloat  roll;        // Катание текстуры

 

LRESULT  CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Объявление WndProc

  
 

int LoadGLTextures()                // Загрузка картинки и конвертирование в текстуру

{

  int Status=FALSE;                 // Индикатор состояния

  AUX_RGBImageRec *TextureImage[5]; // Создать место для текстуры

  memset(TextureImage,0,sizeof(void *)*5); // Установить указатель в NULL

 

  if ((TextureImage[0]=LoadBMP("Data/logo.bmp")) &&   // Текстура эмблемы

      (TextureImage[1]=LoadBMP("Data/mask1.bmp")) &&  // Первая маска

      (TextureImage[2]=LoadBMP("Data/image1.bmp")) && // Первое изображение

      (TextureImage[3]=LoadBMP("Data/mask2.bmp")) &&  // Вторая маска

      (TextureImage[4]=LoadBMP("Data/image2.bmp")))   // Второе изображение

  {

    Status=TRUE;                    // Задать статус в TRUE

    glGenTextures(5, &texture[0]);  // Создать пять текстур

 

    for (loop=0; loop<5; loop++)    // Цикл по всем пяти текстурам

    {

      glBindTexture(GL_TEXTURE_2D, texture[loop]);

      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

      glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop]->sizeX, TextureImage[loop]->sizeY,

        0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[loop]->data);

    }

  }

  for (loop=0; loop<5; loop++)      // Цикл по всем пяти текстурам

  {

    if (TextureImage[loop])         // Если текстура существуют

    {

      if (TextureImage[loop]->data) // Если изображение текстуры существует

      {

        free(TextureImage[loop]->data); // Освободить память изображения

      }

      free(TextureImage[loop]);     // Освободить структуру изображения

    }

  }

  return Status;                    // Возвращаем статус

}

  Код ReSizeGLScene() не изменился, и мы опустим его.



int InitGL(GLvoid)            // Все начальные настройки OpenGL здесь

{

  if (!LoadGLTextures())      // Переход на процедуру загрузки текстуры

  {

    return FALSE;             // Если текстура не загружена возвращаем FALSE

  }

 

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Черный фон

  glClearDepth(1.0);          // Установка буфера глубины

  glEnable(GL_DEPTH_TEST);    // Разрешение теста глубины

  glShadeModel(GL_SMOOTH);    // Разрешить плавное закрашивание

  glEnable(GL_TEXTURE_2D);    // Разрешение наложения текстуры

  return TRUE;                // Инициализация завершена OK

}



int DrawGLScene(GLvoid)          // Здесь мы все рисуем

{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      // Очистка экрана и буфера глубины

  glLoadIdentity();              // Сброс матрицы вида

  glTranslatef(0.0f,0.0f,-2.0f); // Перемещение вглубь экрана на 2 единицы



  glBindTexture(GL_TEXTURE_2D, texture[0]); // Выбор текстуры эмблемы

  glBegin(GL_QUADS);       // Начало рисования текстурного четырехугольника

    glTexCoord2f(0.0f, -roll+0.0f); glVertex3f(-1.1f, -1.1f,  0.0f);  // Лево Низ

    glTexCoord2f(3.0f, -roll+0.0f); glVertex3f( 1.1f, -1.1f,  0.0f);  // Право Низ

    glTexCoord2f(3.0f, -roll+3.0f); glVertex3f( 1.1f,  1.1f,  0.0f);  // Право Верх

    glTexCoord2f(0.0f, -roll+3.0f); glVertex3f(-1.1f,  1.1f,  0.0f);  // Лево Верх

  glEnd();                // Завершения рисования четырехугольника

 

  glEnable(GL_BLEND);       // Разрешение смешивания

  glDisable(GL_DEPTH_TEST); // Запрет теста глубины

  

  if (masking)              // Маскировка разрешена?

  {

  

    glBlendFunc(GL_DST_COLOR,GL_ZERO); // Смешивание цвета экрана с нулем (Черное)

  }

 

  if (scene)                // Рисовать вторую сцену?

  {


    glTranslatef(0.0f,0.0f,-1.0f);      // Перемещение вглубь экрана на одну единицу

    glRotatef(roll*360,0.0f,0.0f,1.0f); // Вращение по оси Z на 360 градусов

 

    if (masking)              // Маскирование включено?

    {


      glBindTexture(GL_TEXTURE_2D, texture[3]); // Выбор второй маски текстуры

      glBegin(GL_QUADS);  // Начало рисования текстурного четырехугольника

        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.1f, -1.1f,  0.0f);  // Низ Лево

        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.1f, -1.1f,  0.0f);  // Низ Право

        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.1f,  1.1f,  0.0f);  // Верх Право

        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.1f,  1.1f,  0.0f);  // верх Лево

      glEnd();            // Конец рисования четырехугольника

    }



    glBlendFunc(GL_ONE, GL_ONE);  // Копирование цветного изображения 2 на экран

    glBindTexture(GL_TEXTURE_2D, texture[4]); // Выбор второго изображения текстуры

    glBegin(GL_QUADS);            // Начало рисования текстурного четырехугольника

      glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.1f, -1.1f,  0.0f);  // Низ Лево

      glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.1f, -1.1f,  0.0f);  // Низ Право

      glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.1f,  1.1f,  0.0f);  // Верх Право

      glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.1f,  1.1f,  0.0f);  // Верх Лево

    glEnd();                      // Завершение рисования четырехугольника

  }


  else                  // Иначе

  {

 

    if (masking)              // Вкл. маскировка?

    {

 

      glBindTexture(GL_TEXTURE_2D, texture[1]); // Выбор первой маски текстуры

      glBegin(GL_QUADS);  // Начало рисования текстурного четырехугольника

        glTexCoord2f(roll+0.0f, 0.0f); glVertex3f(-1.1f, -1.1f,  0.0f);  // Низ Лево

        glTexCoord2f(roll+4.0f, 0.0f); glVertex3f( 1.1f, -1.1f,  0.0f);  // Низ Право

        glTexCoord2f(roll+4.0f, 4.0f); glVertex3f( 1.1f,  1.1f,  0.0f);  // Верх Право

        glTexCoord2f(roll+0.0f, 4.0f); glVertex3f(-1.1f,  1.1f,  0.0f);  // Верх Лево

      glEnd();            // Конец рисования четырехугольника

    }

    glBlendFunc(GL_ONE, GL_ONE); // Копирование цветного изображения 1 на экран

    glBindTexture(GL_TEXTURE_2D, texture[2]); // Выбор первого изображения текстуры

    glBegin(GL_QUADS);    // Начало рисования текстурного четырехугольника

      glTexCoord2f(roll+0.0f, 0.0f); glVertex3f(-1.1f, -1.1f,  0.0f);  // Низ Лево

      glTexCoord2f(roll+4.0f, 0.0f); glVertex3f( 1.1f, -1.1f,  0.0f);  // Низ Право

      glTexCoord2f(roll+4.0f, 4.0f); glVertex3f( 1.1f,  1.1f,  0.0f);  // Верх Право

      glTexCoord2f(roll+0.0f, 4.0f); glVertex3f(-1.1f,  1.1f,  0.0f);  // Верх Лево

    glEnd();              // Конец рисования четырехугольника

  }


  glEnable(GL_DEPTH_TEST); // Разрешение теста глубины

  glDisable(GL_BLEND);     // Запрещение смешивания

 

  roll+=0.002f;            // Увеличим прокрутку нашей текстуры

  if (roll>1.0f)           // Roll больше чем

  {

    roll-=1.0f;            // Вычтем 1 из Roll

  }

 

  return TRUE;             // Все OK

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

  if (!CreateGLWindow("NeHe's Masking Tutorial",640,480,16,fullscreen))

  {

    return 0;                  // Выходим если окно не было создано

  }

 

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

      // Рисуем сцену. Ожидаем нажатия кнопки ESC и сообщения о выходе от DrawGLScene()

      // Активно?  Было получено сообщение о выходе?

      if ((active && !DrawGLScene()) || keys[VK_ESCAPE])

      {

        done=TRUE; // ESC или DrawGLScene просигналили "выход"

      }

      else // Не время выходить, обновляем экран

      {

        SwapBuffers(hDC); // Переключаем буферы (Двойная буферизация)


        if (keys[' '] && !sp) // Пробел нажат?

        {

          sp=TRUE;            // Сообщим программе, что пробел нажат

          scene=!scene;       // Переключение сцен

        }



        if (!keys[' '])    // Пробел отжат?

        {

          sp=FALSE;        // Сообщим программе, что пробел отжат

        }


        if (keys['M'] && !mp) // M нажата?

        {

          mp=TRUE;            // Сообщим программе, что M нажата

          masking=!masking;   // Переключение режима маскирования Выкл/Вкл

        }


        if (!keys['M']) // M отжата?

        {

          mp=FALSE;     // Сообщим программе, что M отжата

        }

  
        if (keys[VK_F1])             // Была нажата кнопка F1?

        {

          keys[VK_F1]=FALSE;         // Если так - установим значение FALSE

          KillGLWindow();            // Закроем текущее окно OpenGL 

          fullscreen=!fullscreen;    // Переключим режим "Полный экран"/"Оконный"

          // Заново создадим наше окно OpenGL

          if (!CreateGLWindow("NeHe's Masking Tutorial",640,480,16,fullscreen))

          {

            return 0;                // Выйти, если окно не было создано

 

          }

        }

      }

    }

  }

 

  // Сброс

  KillGLWindow();                    // Закроем окно

  return (msg.wParam);               // Выйдем из программы

}

