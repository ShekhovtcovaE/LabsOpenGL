
#include <windows.h>   // заголовочный файл для Windows

#include <math.h>      // заголовочный файл для математической библиотеки Windows(добавлено)

#include <stdio.h>     // заголовочный файл для стандартного ввода/вывода(добавлено)

#include <stdarg.h>    // заголовочный файл для манипуляций с переменными аргументами(добавлено)

#include <gl\gl.h>     // заголовочный файл для библиотеки OpenGL32

#include <gl\glu.h>    // заголовочный файл для библиотеки GLu32

#include <gl\glaux.h>  // заголовочный файл для библиотеки GLaux

 

HDC    hDC=NULL;     // Частный контекст устройства GDI

HGLRC    hRC=NULL;   // Контекст текущей визуализации

HWND    hWnd=NULL;   // Дескриптор нашего окна

HINSTANCE hInstance; // Копия нашего приложения

 

bool           keys[256];        // Массив для работы с клавиатурой

bool           active=TRUE;      // Флаг активности окна, TRUE по умолчанию

bool           fullscreen=TRUE;  // Флаг полноэкранного режима, TRUE по умолчанию

 

//Параметры освещения

static Glfloat LightAmb[]={0.7f, 0.7f, 0.7f};  //Окружающий свет

static Glfloat LightDif[]={1.0f, 1.0f, 1.0f};  //Рассеянный свет

//Позиция источника освещения

static Glfloat LightPos[]={4.0f, 4.0f, 6.0f, 1.0f};



GLUquadricObj  *q;     // Квадратичный объект для рисования сферы мяча

GLfloat xrot = 0.0f;   // Вращение по Х

GLfloat yrot = 0.0f;   // Вращение по Y

GLfloat xrotspeed = 0.0f;// Скорость вращения по X

GLfloat yrotspeed = 0.0f;// Скорость вращения по Y

GLfloat zoom = -7.0f;  // Глубина сцены в экране

GLfloat height = 2.0f; // Высота мяча над полом

GLuint  texture[3];    // 3 Текстуры

      // Объявление WndProc

LRESULT  CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);     

// Функция изменения размера и инициализации OpenGL-окна

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)

// Функция загрузки растрового рисунка

AUX_RGBImageRec *LoadBMP(char *Filename)


int LoadGLTextures()    // Загрузка рисунков и создание текстур

{

    int Status=FALSE;                        // Индикатор статуса

    AUX_RGBImageRec *TextureImage[3];        // массив для текстур

    memset(TextureImage,0,sizeof(void *)*3); // Обнуление

    if ((TextureImage[0]=LoadBMP("Data/EnvWall.bmp")) && // Текстура пола             

    (TextureImage[1]=LoadBMP("Data/Ball.bmp")) &&        // Текстура света

    (TextureImage[2]=LoadBMP("Data/EnvRoll.bmp")))       // Текстура стены

    {  

      Status=TRUE;                      // Статус положительный

      glGenTextures(3, &texture[0]);    // Создание текстуры

      for (int loop=0; loop<3; loop++)  // Цикл для 3 текстур

      {

        glBindTexture(GL_TEXTURE_2D, texture[loop]);

        glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop]->sizeX,

          TextureImage[loop]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,

          TextureImage[loop]->data);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

      }

      for (loop=0; loop<3; loop++)      // Цикл для 3 рисунков

      {

        if (TextureImage[loop])         // Если текстура существует

        {

          if (TextureImage[loop]->data) // Если рисунок есть

          {

            free(TextureImage[loop]->data);

          }

          free(TextureImage[loop]);     // Очистить память из-под него

        }

      }

    }

    return Status;                      // Вернуть статус

}


int InitGL(GLvoid)        // Инициализация OpenGL

{

  if (!LoadGLTextures())  // Если текстуры не загружены, выход

  {

    return FALSE;           

  }

  glShadeModel(GL_SMOOTH);//Включаем плавное закрашивание

  glClearColor(0.2f, 0.5f, 1.0f, 1.0f);// Фон

  glClearDepth(1.0f);    // Значение для буфера глубины

  glClearStencil(0);     // Очистка буфера шаблона 0

  glEnable(GL_DEPTH_TEST);//Включить проверку глубины

  glDepthFunc(GL_LEQUAL); // Тип проверки глубины

  // Наилучшая коррекция перспективы

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glEnable(GL_TEXTURE_2D);//Включить рисование 2D-текстур

 

  // Фоновое освещение для источника LIGHT0

  glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmb);

  // Рассеянное освещение для источника LIGHT0

  glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDif);

  // Положение источника LIGHT0

  glLightfv(GL_LIGHT0, GL_POSITION, LightPos);     

  // Включить Light0

  glEnable(GL_LIGHT0);           

  // Включить освещение

  glEnable(GL_LIGHTING);           

  q = gluNewQuadric();  // Создать квадратичный объект

  // тип генерируемых нормалей для него – «сглаженные»

  gluQuadricNormals(q, GL_SMOOTH);

  // Включить текстурные координаты для объекта

  gluQuadricTexture(q, GL_TRUE);

  // Настройка сферического наложения

  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

  // Настройка отображения сферы

  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

  return TRUE;  // Инициализация прошла успешно

}

 

void DrawObject()  // Рисование мяча

{

  glColor3f(1.0f, 1.0f, 1.0f);// Цвет - белый

  glBindTexture(GL_TEXTURE_2D, texture[1]);// Выбор текстуры 2 (1)

  gluSphere(q, 0.35f, 32, 16);// Рисование первого мяча

  

  glBindTexture(GL_TEXTURE_2D, texture[2]);// Выбор текстуры 3 (2)

  glColor4f(1.0f, 1.0f, 1.0f, 0.4f);// Белый цвет с 40%-й прозрачностью

  glEnable(GL_BLEND);               // Включить смешивание

  // Режим смешивания

  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  // Разрешить сферическое наложение

  glEnable(GL_TEXTURE_GEN_S);          

  // Разрешить сферическое наложение

  glEnable(GL_TEXTURE_GEN_T);     

  // Нарисовать новую сферу при помощи новой текстуры

  gluSphere(q, 0.35f, 32, 16);

  // Текстура будет смешена с созданной для эффекта мультитекстурирования (Отражение)

  glDisable(GL_TEXTURE_GEN_S);      // Запретить сферическое наложение

  glDisable(GL_TEXTURE_GEN_T);      // Запретить сферическое наложение

  glDisable(GL_BLEND);              // Запретить смешивание

}

 
void DrawFloor()    // Рисование пола

{

  glBindTexture(GL_TEXTURE_2D, texture[0]);// текстура 1 (0)

  glBegin(GL_QUADS);           // Начало рисования

    glNormal3f(0.0, 1.0, 0.0); // «Верхняя» нормаль

    glTexCoord2f(0.0f, 1.0f);  // Нижняя левая сторона текстуры

    glVertex3f(-2.0, 0.0, 2.0);//Нижний левый угол пола

 

    glTexCoord2f(0.0f, 0.0f);  //Верхняя левая сторона текстуры

    glVertex3f(-2.0, 0.0,-2.0);//Верхний левый угол пола

     

    glTexCoord2f(1.0f, 0.0f);  //Верхняя правая сторона текстуры

    glVertex3f( 2.0, 0.0,-2.0);//Верхний правый угол пола

     

    glTexCoord2f(1.0f, 1.0f);  //Нижняя правая сторона текстуры

    glVertex3f( 2.0, 0.0, 2.0);//Нижний правый угол пола

  glEnd();                     // конец рисования

}



int DrawGLScene(GLvoid)// Рисование сцены

{

  // Очистка экрана, буфера глубины и буфера шаблона

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Уравнение плоскости отсечения для отсечения отраженных объектов

  double eqr[] = {0.0f,-1.0f, 0.0f, 0.0f};     


  glLoadIdentity();// Сброс матрицы модели

  // Отдаление и подъем камеры над полом (на 0.6 единиц)

  glTranslatef(0.0f, -0.6f, zoom);       



  glColorMask(0,0,0,0);    // Установить маску цвета

 

        // Использовать буфер шаблона для «пометки» пола

  glEnable(GL_STENCIL_TEST);

  // Всегда проходит, 1 битовая плоскость, маска = 1

  glStencilFunc(GL_ALWAYS, 1, 1); // 1, где рисуется хоть какой-нибудь полигон

  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  glDisable(GL_DEPTH_TEST);// Отключить проверку глубины

  DrawFloor();// Рисование пола (только в буфере шаблона)

  
  glEnable(GL_DEPTH_TEST); // Включить проверку глубины

  glColorMask(1,1,1,1); // Маска цвета = TRUE, TRUE, TRUE, TRUE

  glStencilFunc(GL_EQUAL, 1, 1); // Рисуем по шаблону (где шаблон=1)

  // (то есть в том месте, где был нарисован пол)

  // Не изменять буфер шаблона

  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  

  glEnable(GL_CLIP_PLANE0);// Включить плоскость отсечения для удаления

  // артефактов(когда объект пересекает пол)

  glClipPlane(GL_CLIP_PLANE0, eqr);// Уравнение для отраженных объектов

  glPushMatrix();  // Сохранить матрицу в стеке

  glScalef(1.0f, -1.0f, 1.0f);  // Перевернуть ось Y

 

    // Настройка источника света Light0

    glLightfv(GL_LIGHT0, GL_POSITION, LightPos); 

    glTranslatef(0.0f, height, 0.0f);// Перемещение объекта

    // Вращение локальной координатной системы по X-оси

    glRotatef(xrot, 1.0f, 0.0f, 0.0f);

    // Вращение локальной координатной системы по Y-оси

    glRotatef(yrot, 0.0f, 1.0f, 0.0f);   

    DrawObject();// Рисование мяча (для отражения)

  glPopMatrix();    // Восстановить матрицу

  glDisable(GL_CLIP_PLANE0);// Отключить плоскость отсечения

  // Отключение проверки шаблона

  glDisable(GL_STENCIL_TEST);

  glLightfv(GL_LIGHT0, GL_POSITION, LightPos);// Положение источника

  // Включить смешивание (иначе не отразится мяч)

  glEnable(GL_BLEND);

  // В течение использования смешивания отключаем освещение

  glDisable(GL_LIGHTING);

  // Цвет белый, 80% прозрачности

  glColor4f(1.0f, 1.0f, 1.0f, 0.8f);

  // Смешивание, основанное на «Source Alpha And 1 Minus Dest Alpha»

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  DrawFloor();// Нарисовать пол


  glEnable(GL_LIGHTING);// Включить освещение

  glDisable(GL_BLEND);  // Отключить смешивание

  glTranslatef(0.0f, height, 0.0f);// Перемещение мяча

  glRotatef(xrot, 1.0f, 0.0f, 0.0f);// Поворот по оси X

  glRotatef(yrot, 0.0f, 1.0f, 0.0f);// Поворот по оси Y

  DrawObject();         // Рисование объекта

  
  xrot += xrotspeed; // Обновить угол вращения по X

  yrot += yrotspeed; // Обновить угол вращения по Y

  glFlush();   // Сброс конвейера OpenGL

  return TRUE; // Нормальное завершение

}


void ProcessKeyboard()  // Обработка клавиатуры

{

  if (keys[VK_RIGHT])  yrotspeed += 0.08f;// Вправо

  if (keys[VK_LEFT])  yrotspeed -= 0.08f; // Влево

  if (keys[VK_DOWN])  xrotspeed += 0.08f; // Вверх

  if (keys[VK_UP])  xrotspeed -= 0.08f;   // Вниз

 

  if (keys['A'])    zoom +=0.05f; // Приближение

  if (keys['Z'])    zoom -=0.05f; // Отдаление

 

  if (keys[VK_PRIOR])  height +=0.03f; // Подъем

  if (keys[VK_NEXT])  height -=0.03f;  // Спуск

}


GLvoid KillGLWindow(GLvoid)// Удаление окна

 

  static  PIXELFORMATDESCRIPTOR pfd=

  // pfd говорит Windows о наших запросах для формата пикселя

  {

    sizeof(PIXELFORMATDESCRIPTOR),  // Размер структуры

    1,                    // Номер версии

    PFD_DRAW_TO_WINDOW |  // Формат должен поддерживать Window

    PFD_SUPPORT_OPENGL |  // Формат должен поддерживать OpenGL

    PFD_DOUBLEBUFFER,     // Нужна двойная буферизация

    PFD_TYPE_RGBA,        // Формат данных- RGBA

    bits,                 // Глубина цвета

    0, 0, 0, 0, 0, 0,     // Игнорируются биты цвета

    0,                    // Нет альфа-буфера

    0,                    // Игнорируется смещение бит

    0,                    // Нет аккумулирующего буфера

    0, 0, 0, 0,           // Игнорируются биты аккумуляции

    16,                   // 16-битный Z-буфер (глубины)


    1,                    // Использовать буфер шаблона (* ВАЖНО *)

    0,                    // Нет вспомогательного буфера

    PFD_MAIN_PLANE,       // Основной уровень рисования

    0,                    // Не используются

    0, 0, 0  ,            // Нет маски уровня

  };

 


  // Создание окна в Windows

  if (!CreateGLWindow("Banu Octavian & NeHe's Stencil & Reflection Tutorial",

       resx, resy, resbpp, fullscreen))

  {

    return 0;// Выход, если не создалось

  }

 

  while(!done)// Цикл, пока done=FALSE

  {

    // Выборка сообщений

    if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))

    {

      if (msg.message==WM_QUIT) // Выход?

      {

        done=TRUE;// Если да

      }

      else  // Если нет, обработка сообщения

      {

        TranslateMessage(&msg); // Транслировать сообщение

        DispatchMessage(&msg);

      }

    }

    else  // Если нет сообщений

    {  // Отрисовка сцены.  Ожидание клавиши ESC или

      // сообщения о выходе от DrawGLScene()

      if (active)  // Программа активна?

      {

        if (keys[VK_ESCAPE])// ESC нажата?

        {

          done=TRUE;// Если да, выход

        }

        else// Иначе - рисование

        {

          DrawGLScene();// Рисование сцены

          SwapBuffers(hDC);//Переключить буфера

  
          ProcessKeyboard();// Обработка нажатий клавиш

        }

      }

    }

  }    // Конец работы

  KillGLWindow();  // Удалить окно

  return (msg.wParam);// Выход из программы

}
