

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

 

BOOL light;      // Свет ВКЛ / ВЫКЛ

BOOL lp;         // L нажата?

BOOL fp;         // F нажата?

BOOL   sp;                   // Пробел нажат? ( НОВОЕ ) 

int    part1;                // Начало диска ( НОВОЕ )

int    part2;                // Конец диска  ( НОВОЕ )

int    p1=0;                 // Приращение 1 ( НОВОЕ )

int    p2=1;                 // Приращение 2 ( НОВОЕ )
 

GLfloat xrot;         // X вращение

GLfloat yrot;         // Y вращение

GLfloat xspeed;       // X скорость вращения

GLfloat yspeed;       // Y скорость вращения

 

GLfloat z=-5.0f;      // Сдвиг вглубь экрана

  
 

GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f }; // Значения фонового света 


 

GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f }; // Значения диффузного света 

 



 

GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };     // Позиция света 

 


 

GLuint filter;         // Какой фильтр использовать

GLuint texture[6];     // Место для хранения 6 текстур

GLuint object=0;                               // Какой объект рисовать ( НОВОЕ )


 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);    // Декларация WndProc

 


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

 


int LoadGLTextures()                // Загрузить картинки и создать текстуры

{

  int Status=FALSE;                 // Индикатор статуса

 

  AUX_RGBImageRec *TextureImage[2]; // Выделим место для хранения текстур

 

  memset(TextureImage,0,sizeof(void *)*2);         // Сбросим эти указатели

 

  // Загрузим картинку, проверим на ошибки, если картинка не найдена - выйдем

  if ((TextureImage[0]=LoadBMP("Data/BG.bmp")) &&  // Фоновая текстура

    (TextureImage[1]=LoadBMP("Data/Reflect.bmp"))) // Текстура отражения

                                                   // (сферическая карта)

  {

    Status=TRUE;                    // Установить индикатор в TRUE

 

    glGenTextures(6, &texture[0]);  // Создадим три текстуры

 

    for (int loop=0; loop<=1; loop++)

    {

      // Создадим текстуры без фильтрации

      glBindTexture(GL_TEXTURE_2D, texture[loop]); // Текстуры 0 и 1

      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop]->sizeX,

                   TextureImage[loop]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,

                   TextureImage[loop]->data);

 

      // Создадим линейно фильтрованные текстуры

      glBindTexture(GL_TEXTURE_2D, texture[loop+2]);    // Текстуры 2, 3 и 4

      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

      glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[loop]->sizeX,

                   TextureImage[loop]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,

                   TextureImage[loop]->data);

 

      // мип-мап текстуры

      glBindTexture(GL_TEXTURE_2D, texture[loop+4]);    // Текстуры 4 и 5

      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);

      gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[loop]->sizeX,

                        TextureImage[loop]->sizeY, GL_RGB, GL_UNSIGNED_BYTE,

                        TextureImage[loop]->data);

    }

    for (loop=0; loop<=1; loop++)

    {

          if (TextureImage[loop])                  // Если текстура существует

        {

              if (TextureImage[loop]->data)        // Если существует изображение текстуры

            {

                  free(TextureImage[loop]->data);  // Освободим память изображения текстуры

          }

          free(TextureImage[loop]);                // Освободим память

                                                   // структуры изображения

      }

    }

  }

 

  return Status; // Вернем статус

}

 

int InitGL(GLvoid)             // Все настройки для OpenGL делаются здесь
      // Изменить для S режим генерации текстур на "сферическое наложение" ( Новое )

  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

  // Изменить для T режим генерации текстур на "сферическое наложение" ( Новое )

  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  
     quadratic=gluNewQuadric();     // Создаем указатель на квадратичный объект ( НОВОЕ )

       gluQuadricNormals(quadratic, GLU_SMOOTH); // Создаем плавные нормали ( НОВОЕ )

       gluQuadricTexture(quadratic, GL_TRUE);    // Создаем координаты текстуры ( НОВОЕ )

{

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

GLvoid glDrawCube()

{

    glBegin(GL_QUADS);

    // Передняя грань

    glNormal3f( 0.0f, 0.0f, 0.5f);          ( Изменено )

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);

    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);

    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);

    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);

    // Задняя грань

    glNormal3f( 0.0f, 0.0f,-0.5f);          ( Изменено )

    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);

    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);

    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);

    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);

    // Верхняя грань

    glNormal3f( 0.0f, 0.5f, 0.0f);          ( Изменено )

    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);

    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);

    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);

    // Нижняя грань

    glNormal3f( 0.0f,-0.5f, 0.0f);          ( Изменено )

    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);

    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);

    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);

    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);

    // Правая грань

    glNormal3f( 0.5f, 0.0f, 0.0f);          ( Изменено )

    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);

    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);

    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);

    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);

    // Левая грань

    glNormal3f(-0.5f, 0.0f, 0.0f);          ( Изменено )

    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);

    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);

    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);

    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);

  glEnd();

}


int DrawGLScene(GLvoid)          // Здесь происходит все рисование

{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очистим экран и буфер глубины

  glLoadIdentity();              // Сбросим вид

 

 

  glTranslatef(0.0f,0.0f,z);

 

  glEnable(GL_TEXTURE_GEN_S);    // Включим генерацию координат текстуры для S ( НОВОЕ )

  glEnable(GL_TEXTURE_GEN_T);    // Включим генерацию координат текстуры для T ( НОВОЕ )

 

  // Выберем сферическое текстурирование ( ИЗМЕНЕНО )

  glBindTexture(GL_TEXTURE_2D, texture[filter+(filter+1)]);

  glPushMatrix();

  glRotatef(xrot,1.0f,0.0f,0.0f);

  glRotatef(yrot,0.0f,1.0f,0.0f);

  switch(object)

  {

  case 0:

    glDrawCube();

    break;

  case 1:

    glTranslatef(0.0f,0.0f,-1.5f);               // Отцентруем цилиндр

    gluCylinder(quadratic,1.0f,1.0f,3.0f,32,32); // Цилиндр радиусом 0.5 и высотой 2

    break;

  case 2:

    // Сфера радиусом 1, состоящая из 16 сегментов по долготе/широте

    gluSphere(quadratic,1.3f,32,32);

    break;

  case 3:

    glTranslatef(0.0f,0.0f,-1.5f);                // Отцентруем конус

    // Конус с радиусом основания 0.5 и высотой 2

    gluCylinder(quadratic,1.0f,0.0f,3.0f,32,32);

    break;

  };

 

  glPopMatrix();

  glDisable(GL_TEXTURE_GEN_S);        // Отключим генерацию текстурных координат ( НОВОЕ )

  glDisable(GL_TEXTURE_GEN_T);        // Отключим генерацию текстурных координат ( НОВОЕ )

 

  glBindTexture(GL_TEXTURE_2D, texture[filter*2]); // Выберем фоновую текстуру ( НОВОЕ )

  glPushMatrix();

    glTranslatef(0.0f, 0.0f, -24.0f);

    glBegin(GL_QUADS);

      glNormal3f( 0.0f, 0.0f, 1.0f);

      glTexCoord2f(0.0f, 0.0f); glVertex3f(-13.3f, -10.0f,  10.0f);

      glTexCoord2f(1.0f, 0.0f); glVertex3f( 13.3f, -10.0f,  10.0f);

      glTexCoord2f(1.0f, 1.0f); glVertex3f( 13.3f,  10.0f,  10.0f);

      glTexCoord2f(0.0f, 1.0f); glVertex3f(-13.3f,  10.0f,  10.0f);

    glEnd();

 

  glPopMatrix();

 

  xrot+=xspeed;

  yrot+=yspeed;

  return TRUE; // Продолжим

}

        if (keys[' '] && !sp)

        {

          sp=TRUE;

          object++;

          if(object>3)

            object=0;

        }
        
