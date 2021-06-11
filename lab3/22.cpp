
#include <windows.h>         // Файл заголовков функций Windows

#include <stdio.h>           // Файл заголовков для библиотеки ввода-вывода

#include <gl\gl.h>           // Файл заголовков для библиотеки OpenGL32

#include <gl\glu.h>          // Файл заголовков для библиотеки GLu32

#include <gl\glaux.h>        // Файл заголовков для библиотеки GLaux

#include "glext.h"           // Файл заголовков для мультитекстурирования

#include <string.h>          // Файл заголовков для работы со строками

#include <math.h>            // Файл заголовков для математической библиотеки

 
// Коэффициент рельефности. Увеличьте, чтобы усилить эффект

#define MAX_EMBOSS (GLfloat)0.008f



#define __ARB_ENABLE true          // Используется, чтобы полностью отключить расширения

// #define EXT_INFO        // Раскомментируйте, чтобы увидеть при запуске доступные расширения

#define MAX_EXTENSION_SPACE 10240  // Символы строк-описателей расширений

#define MAX_EXTENSION_LENGTH 256   // Максимальное число символов в одной строке-описателе

bool multitextureSupported=false;  // Флаг, определяющий, поддерживается ли мультитекстурирование

bool useMultitexture=true;         // Использовать его, если оно доступно?

GLint maxTexelUnits=1;             // Число текстурных блоков. Как минимум 1 есть всегда

 

PFNGLMULTITEXCOORD1FARBPROC  glMultiTexCoord1fARB  = NULL;

PFNGLMULTITEXCOORD2FARBPROC  glMultiTexCoord2fARB  = NULL;

PFNGLMULTITEXCOORD3FARBPROC  glMultiTexCoord3fARB  = NULL;

PFNGLMULTITEXCOORD4FARBPROC  glMultiTexCoord4fARB  = NULL;

PFNGLACTIVETEXTUREARBPROC  glActiveTextureARB  = NULL;

PFNGLCLIENTACTIVETEXTUREARBPROC  glClientActiveTextureARB= NULL;

GLuint  filter=1;           // Какой фильтр использовать

GLuint  texture[3];         // Хранит 3 текстуры

GLuint  bump[3];            // Рельефы

GLuint  invbump[3];         // Инвертированные рельефы

GLuint  glLogo;             // Указатель на OpenGL-логотип

GLuint  multiLogo;          // Указатель на мультитекстурированный логотип

GLfloat LightAmbient[]  = { 0.2f, 0.2f, 0.2f};  // Фоновое освещение - 20% белого

GLfloat LightDiffuse[]  = { 1.0f, 1.0f, 1.0f};  // Рассеянный свет -  чисто белый

GLfloat LightPosition[]  = { 0.0f, 0.0f, 2.0f}; // Положение источника - перед экраном

GLfloat Gray[]    = { 0.5f, 0.5f, 0.5f, 1.0f};

  
// Данные содержат грани куба в формате "2 текстурные координаты, 3 вершинные".

// Обратите внимание, что мозаичность куба минимальна.

 

GLfloat data[]= {

  // ЛИЦЕВАЯ ГРАНЬ

  0.0f, 0.0f,    -1.0f, -1.0f, +1.0f,

  1.0f, 0.0f,    +1.0f, -1.0f, +1.0f,

  1.0f, 1.0f,    +1.0f, +1.0f, +1.0f,

  0.0f, 1.0f,    -1.0f, +1.0f, +1.0f,

  // ЗАДНЯЯ ГРАНЬ

  1.0f, 0.0f,    -1.0f, -1.0f, -1.0f,

  1.0f, 1.0f,    -1.0f, +1.0f, -1.0f,

  0.0f, 1.0f,    +1.0f, +1.0f, -1.0f,

  0.0f, 0.0f,    +1.0f, -1.0f, -1.0f,

  // ВЕРХНЯЯ ГРАНЬ

  0.0f, 1.0f,    -1.0f, +1.0f, -1.0f,

  0.0f, 0.0f,    -1.0f, +1.0f, +1.0f,

  1.0f, 0.0f,    +1.0f, +1.0f, +1.0f,

  1.0f, 1.0f,    +1.0f, +1.0f, -1.0f,

  // НИЖНЯЯ ГРАНЬ

  1.0f, 1.0f,    -1.0f, -1.0f, -1.0f,

  0.0f, 1.0f,    +1.0f, -1.0f, -1.0f,

  0.0f, 0.0f,    +1.0f, -1.0f, +1.0f,

  1.0f, 0.0f,    -1.0f, -1.0f, +1.0f,

  // ПРАВАЯ ГРАНЬ

  1.0f, 0.0f,    +1.0f, -1.0f, -1.0f,

  1.0f, 1.0f,    +1.0f, +1.0f, -1.0f,

  0.0f, 1.0f,    +1.0f, +1.0f, +1.0f,

  0.0f, 0.0f,    +1.0f, -1.0f, +1.0f,

  // ЛЕВАЯ ГРАНЬ

  0.0f, 0.0f,    -1.0f, -1.0f, -1.0f,

  1.0f, 0.0f,    -1.0f, -1.0f, +1.0f,

  1.0f, 1.0f,    -1.0f, +1.0f, +1.0f,

  0.0f, 1.0f,    -1.0f, +1.0f, -1.0f

};

 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);        // Объявление WndProc

 
bool isInString(char *string, const char *search) {

  int pos=0;

  int maxpos=strlen(search)-1;

  int len=strlen(string);

  char *other;

  for (int i=0; i<len; i++) {

    if ((i==0) || ((i>1) && string[i-1]=='\n')) { // Новые расширения начинаются здесь!

      other=&string[i];

      pos=0; // Начать новый поиск

      while (string[i]!='\n') { // Поиск по всей строке расширения

        if (string[i]==search[pos]) pos++; // Следующий символ

        if ((pos>maxpos) && string[i+1]=='\n') return true; // А вот и она!

        i++;

      }

    }

  }

  return false; // Простите, не нашли!

}



bool initMultitexture(void) {

  char *extensions;

  extensions=strdup((char *) glGetString(GL_EXTENSIONS)); // Получим строку расширений

  int len=strlen(extensions);

  for (int i=0; i<len; i++) // Разделим ее символами новой строки вместо пробелов

    if (extensions[i]==' ') extensions[i]='\n';

 

#ifdef EXT_INFO

  MessageBox(hWnd,extensions,"поддерживаются расширения GL:",MB_OK | MB_ICONINFORMATION);

#endif

 

  if (isInString(extensions,"GL_ARB_multitexture") // Мультитекстурирование поддерживается?

    && __ARB_ENABLE  // Проверим флаг

    // Поддерживается среда комбинирования текстур?

    && isInString(extensions,"GL_EXT_texture_env_combine"))

  {      

    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&maxTexelUnits);

    glMultiTexCoord1fARB = (PFNGLMULTITEXCOORD1FARBPROC) wglGetProcAddress("glMultiTexCoord1fARB");

    glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");

    glMultiTexCoord3fARB = (PFNGLMULTITEXCOORD3FARBPROC) wglGetProcAddress("glMultiTexCoord3fARB");

    glMultiTexCoord4fARB = (PFNGLMULTITEXCOORD4FARBPROC) wglGetProcAddress("glMultiTexCoord4fARB");

    glActiveTextureARB   = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");

    glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");

              

#ifdef EXT_INFO

    MessageBox(hWnd,"Будет использовано расширение GL_ARB_multitexture.",

     "опция поддерживается!",MB_OK | MB_ICONINFORMATION);

#endif

 

    return true;

  }

  useMultitexture=false;// Невозможно использовать то, что не поддерживается аппаратурой

  return false;

}

 

InitLights() инициализирует освещение OpenGL, будучи вызвана позже из InitGL().

 

void initLights(void) {

        // Загрузка параметров освещения в GL_LIGHT1

        glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);

        glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);

        glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);

        glEnable(GL_LIGHT1);

}



int LoadGLTextures() {           // Загрузка растра и преобразование в текстуры

  bool status=true;              // Индикатор состояния

  AUX_RGBImageRec *Image=NULL;   // Создадим место для хранения текстур

  char *alpha=NULL;

 

  // Загрузим базовый растр

  if (Image=auxDIBImageLoad("Data/Base.bmp")) {

    glGenTextures(3, texture);    // Создадим три текстуры

 

    // Создаем текстуру с фильтром по ближайшему

    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Image->sizeX, Image->sizeY, 0,

      GL_RGB, GL_UNSIGNED_BYTE, Image->data);

 

    // Создаем текстуру с фильтром усреднения

    glBindTexture(GL_TEXTURE_2D, texture[1]);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Image->sizeX, Image->sizeY, 0,

       GL_RGB, GL_UNSIGNED_BYTE, Image->data);

 

    // Создаем текстуру с мип-наложением

    glBindTexture(GL_TEXTURE_2D, texture[2]);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, Image->sizeX, Image->sizeY,

      GL_RGB, GL_UNSIGNED_BYTE, Image->data);

  }

  else status=false;

 

  if (Image) { // Если текстура существует

    if (Image->data) delete Image->data; // Если изображение существует

    delete Image;

    Image=NULL;

  }

  

  // Загрузим рельефы

  if (Image=auxDIBImageLoad("Data/Bump.bmp")) {

    glPixelTransferf(GL_RED_SCALE,0.5f);   // Промасштабируем яркость до 50%,

    glPixelTransferf(GL_GREEN_SCALE,0.5f); // поскольку нам нужна половинная интенсивность

    glPixelTransferf(GL_BLUE_SCALE,0.5f);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);  // Не укладывать паркетом

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

    glGenTextures(3, bump);  // Создать три текстуры

 

  

    for (int i=0; i<3*Image->sizeX*Image->sizeY; i++) // Проинвертируем растр

      Image->data[i]=255-Image->data[i];

 

    glGenTextures(3, invbump); // Создадим три текстуры

 

  

  }

  else status=false;

  if (Image) { // Если текстура существует

    if (Image->data) delete Image->data; // Если изображение текстуры существует

    delete Image;

    Image=NULL;

  }

  

  // Загрузка картинки логотипа

  if (Image=auxDIBImageLoad("Data/OpenGL_ALPHA.bmp")) {

    alpha=new char[4*Image->sizeX*Image->sizeY];

    // Выделим память для RGBA8-текстуры

    for (int a=0; a<Image->sizeX*Image->sizeY; a++)

      alpha[4*a+3]=Image->data[a*3]; // Берем красную величину как альфа-канал

    if (!(Image=auxDIBImageLoad("Data/OpenGL.bmp"))) status=false;

    for (a=0; a<Image->sizeX*Image->sizeY; a++) {

      alpha[4*a]=Image->data[a*3];        // R

      alpha[4*a+1]=Image->data[a*3+1];    // G

      alpha[4*a+2]=Image->data[a*3+2];    // B

    }

 

    glGenTextures(1, &glLogo);  // Создать одну текстуру

 

    // Создать RGBA8-текстуру с фильтром усреднения

    glBindTexture(GL_TEXTURE_2D, glLogo);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Image->sizeX, Image->sizeY, 0,

       GL_RGBA, GL_UNSIGNED_BYTE, alpha);

    delete alpha;

  }

  else status=false;

 

  if (Image) { // Если текстура существует

    if (Image->data) delete Image->data; // Если изображение текстуры существует

    delete Image;

    Image=NULL;

  }

 

  // Загрузим логотип "Extension Enabled"

  if (Image=auxDIBImageLoad("Data/multi_on_alpha.bmp")) {

    alpha=new char[4*Image->sizeX*Image->sizeY]; // Выделить память для RGBA8-текстуры

    >…<

    glGenTextures(1, &multiLogo); // Создать одну текстуру

    // Создать RGBA8-текстуру с фильтром усреднения

    >…<

    delete alpha;

  }

  else status=false;

 

  if (Image) { // Если текстура существует

    if (Image->data) delete Image->data; // Если изображение текстуры существует

    delete Image;

    Image=NULL;

  }

  return status;  // Вернем состояние

}

 

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)

// Изменить размер и инициализировать окно GL

>…<

 

void doCube (void) {

  int i;

  glBegin(GL_QUADS);

    // Передняя грань

    glNormal3f( 0.0f, 0.0f, +1.0f);

    for (i=0; i<4; i++) {

      glTexCoord2f(data[5*i],data[5*i+1]);

      glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);

    }

    // Задняя грань

    glNormal3f( 0.0f, 0.0f,-1.0f);

    for (i=4; i<8; i++) {

      glTexCoord2f(data[5*i],data[5*i+1]);

      glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);

    }

    // Верхняя грань

    glNormal3f( 0.0f, 1.0f, 0.0f);

    for (i=8; i<12; i++) {

      glTexCoord2f(data[5*i],data[5*i+1]);

      glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);

    }

    // Нижняя грань

    glNormal3f( 0.0f,-1.0f, 0.0f);

    for (i=12; i<16; i++) {

      glTexCoord2f(data[5*i],data[5*i+1]);

      glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);

    }

    // Правая грань

    glNormal3f( 1.0f, 0.0f, 0.0f);

    for (i=16; i<20; i++) {

      glTexCoord2f(data[5*i],data[5*i+1]);

      glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);

    }

    // Левая грань

    glNormal3f(-1.0f, 0.0f, 0.0f);

    for (i=20; i<24; i++) {

      glTexCoord2f(data[5*i],data[5*i+1]);

      glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);

    }

  glEnd();

}

  
 

int InitGL(GLvoid)                      // Все настройки OpenGL проходят здесь

{

  multitextureSupported=initMultitexture();

  if (!LoadGLTextures()) return false;  // Переход к процедуре загрузки текстур

  glEnable(GL_TEXTURE_2D);              // Включить привязку текстур

  glShadeModel(GL_SMOOTH);              // Включит сглаживание

  glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // Черный фон

  glClearDepth(1.0f);                   // Установка буфера глубины

  glEnable(GL_DEPTH_TEST);              // Включить проверку глубины

  glDepthFunc(GL_LEQUAL);               // Тип проверки глубины

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Наилучшая коррекция перспективы

 

  initLights();                         // Инициализация освещения OpenGL

  return true                           // Инициализация закончилась успешно

}

 



"Emboss Bump Mapping" by Michael I. Gold, nVidia Corp. [.ppt, 309K]

 
 



// Вычисляет v=vM, M - матрица 4x4 в порядке столбец-строка, v - четырехмерный вектор-строка (т.е. транспонированный)

void VMatMult(GLfloat *M, GLfloat *v) {

  GLfloat res[3];

  res[0]=M[ 0]*v[0]+M[ 1]*v[1]+M[ 2]*v[2]+M[ 3]*v[3];

  res[1]=M[ 4]*v[0]+M[ 5]*v[1]+M[ 6]*v[2]+M[ 7]*v[3];

  res[2]=M[ 8]*v[0]+M[ 9]*v[1]+M[10]*v[2]+M[11]*v[3];

  v[0]=res[0];

  v[1]=res[1];

  v[2]=res[2];

  v[3]=M[15];  // Гомогенные координаты

}


// Выполнение сдвига текстуры

// n : нормаль к поверхности. Должна иметь длину 1

// c : текущая вершина на поверхности (координаты местоположения)

// l : положение источника света

// s : направление s-координаты текстуры в пространстве объекта

//     (должна быть нормализована!)

// t : направление t-координаты текстуры в пространстве объекта

//     (должна быть нормализована!)

void SetUpBumps(GLfloat *n, GLfloat *c, GLfloat *l, GLfloat *s, GLfloat *t) {

  GLfloat v[3];                // Вектор от текущей точки к свету

  GLfloat lenQ;                // Используется для нормализации

  // Вычислим и нормализуем v

  v[0]=l[0]-c[0];

  v[1]=l[1]-c[1];

  v[2]=l[2]-c[2];

  lenQ=(GLfloat) sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);

  v[0]/=lenQ;

  v[1]/=lenQ;

  v[2]/=lenQ;

  // Получим величины проекции v вдоль каждой оси системы текстурных координат

  c[0]=(s[0]*v[0]+s[1]*v[1]+s[2]*v[2])*MAX_EMBOSS;

  c[1]=(t[0]*v[0]+t[1]*v[1]+t[2]*v[2])*MAX_EMBOSS;

}

  
void doLogo(void) {

  // ВЫЗЫВАТЬ В ПОСЛЕДНЮЮ ОЧЕРЕДЬ!!! отображает два логотипа

  glDepthFunc(GL_ALWAYS);

  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_BLEND);

  glDisable(GL_LIGHTING);

  glLoadIdentity();

  glBindTexture(GL_TEXTURE_2D,glLogo);

  glBegin(GL_QUADS);

    glTexCoord2f(0.0f,0.0f);  glVertex3f(0.23f, -0.4f,-1.0f);

    glTexCoord2f(1.0f,0.0f);  glVertex3f(0.53f, -0.4f,-1.0f);

    glTexCoord2f(1.0f,1.0f);  glVertex3f(0.53f, -0.25f,-1.0f);

    glTexCoord2f(0.0f,1.0f);  glVertex3f(0.23f, -0.25f,-1.0f);

  glEnd();

  if (useMultitexture) {

    glBindTexture(GL_TEXTURE_2D,multiLogo);

    glBegin(GL_QUADS);

      glTexCoord2f(0.0f,0.0f);  glVertex3f(-0.53f, -0.25f,-1.0f);

      glTexCoord2f(1.0f,0.0f);  glVertex3f(-0.33f, -0.25f,-1.0f);

      glTexCoord2f(1.0f,1.0f);  glVertex3f(-0.33f, -0.15f,-1.0f);

      glTexCoord2f(0.0f,1.0f);  glVertex3f(-0.53f, -0.15f,-1.0f);

    glEnd();

  }

}

 
bool doMesh1TexelUnits(void) {

  GLfloat c[4]={0.0f,0.0f,0.0f,1.0f};       // Текущая вершина

  GLfloat n[4]={0.0f,0.0f,0.0f,1.0f};       // Нормаль к текущей поверхности

  GLfloat s[4]={0.0f,0.0f,0.0f,1.0f};       // s-вектор, нормализованный

  GLfloat t[4]={0.0f,0.0f,0.0f,1.0f};       // t-вектор, нормализованный

  GLfloat l[4];                             // Содержит координаты источника освещения,

                                            // который будет переведен в мировые координаты

  GLfloat Minv[16];                         // Инвертированная матрица вида модели

  int i;

 

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очистка экрана и буфера глубины

 

  // Инвертируем матрицу вида модели. Заменяет один Push/Pop и один glLoadIdentity();

  // Выполняется проведением всех преобразований в обратную сторону в обратном порядке

  glLoadIdentity();

  glRotatef(-yrot,0.0f,1.0f,0.0f);

  glRotatef(-xrot,1.0f,0.0f,0.0f);

  glTranslatef(0.0f,0.0f,-z);

  glGetFloatv(GL_MODELVIEW_MATRIX,Minv);

  glLoadIdentity();

  glTranslatef(0.0f,0.0f,z);

  glRotatef(xrot,1.0f,0.0f,0.0f);

  glRotatef(yrot,0.0f,1.0f,0.0f);

 

  // Преобразование положения источника в систему координат объекта:

  l[0]=LightPosition[0];

  l[1]=LightPosition[1];

  l[2]=LightPosition[2];

  l[3]=1.0f;                // Гомогенные координаты

  VMatMult(Minv,l);



  glBindTexture(GL_TEXTURE_2D, bump[filter]);

  glDisable(GL_BLEND);

  glDisable(GL_LIGHTING);

  doCube();



  glBindTexture(GL_TEXTURE_2D,invbump[filter]);

  glBlendFunc(GL_ONE,GL_ONE);

  glDepthFunc(GL_LEQUAL);

  glEnable(GL_BLEND);

 

  glBegin(GL_QUADS);

    // Передняя грань

    n[0]=0.0f;

    n[1]=0.0f;

    n[2]=1.0f;

    s[0]=1.0f;

    s[1]=0.0f;

    s[2]=0.0f;

    t[0]=0.0f;

    t[1]=1.0f;

    t[2]=0.0f;

    for (i=0; i<4; i++) {

      c[0]=data[5*i+2];

      c[1]=data[5*i+3];

      c[2]=data[5*i+4];

      SetUpBumps(n,c,l,s,t);

      glTexCoord2f(data[5*i]+c[0], data[5*i+1]+c[1]);

      glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);

    }

    // Задняя грань

    n[0]=0.0f;

    n[1]=0.0f;

    n[2]=-1.0f;

    s[0]=-1.0f;

    s[1]=0.0f;

    s[2]=0.0f;

    t[0]=0.0f;

    t[1]=1.0f;

    t[2]=0.0f;

    for (i=4; i<8; i++) {

      c[0]=data[5*i+2];

      c[1]=data[5*i+3];

      c[2]=data[5*i+4];

      SetUpBumps(n,c,l,s,t);

      glTexCoord2f(data[5*i]+c[0], data[5*i+1]+c[1]);

      glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);

    }

    // Верхняя грань

    n[0]=0.0f;

    n[1]=1.0f;

    n[2]=0.0f;

    s[0]=1.0f;

    s[1]=0.0f;

    s[2]=0.0f;

    t[0]=0.0f;

    t[1]=0.0f;

    t[2]=-1.0f;

    for (i=8; i<12; i++) {

      c[0]=data[5*i+2];

      c[1]=data[5*i+3];

      c[2]=data[5*i+4];

      SetUpBumps(n,c,l,s,t);

      glTexCoord2f(data[5*i]+c[0], data[5*i+1]+c[1]);

      glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);

    }

    // Нижняя грань

    n[0]=0.0f;

    n[1]=-1.0f;

    n[2]=0.0f;

    s[0]=-1.0f;

    s[1]=0.0f;

    s[2]=0.0f;

    t[0]=0.0f;

    t[1]=0.0f;

    t[2]=-1.0f;

    for (i=12; i<16; i++) {

      c[0]=data[5*i+2];

      c[1]=data[5*i+3];

      c[2]=data[5*i+4];

      SetUpBumps(n,c,l,s,t);

      glTexCoord2f(data[5*i]+c[0], data[5*i+1]+c[1]);

      glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);

    }

    // Правая грань

    n[0]=1.0f;

    n[1]=0.0f;

    n[2]=0.0f;

    s[0]=0.0f;

    s[1]=0.0f;

    s[2]=-1.0f;

    t[0]=0.0f;

    t[1]=1.0f;

    t[2]=0.0f;

    for (i=16; i<20; i++) {

      c[0]=data[5*i+2];

      c[1]=data[5*i+3];

      c[2]=data[5*i+4];

      SetUpBumps(n,c,l,s,t);

      glTexCoord2f(data[5*i]+c[0], data[5*i+1]+c[1]);

      glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);

    }

    // Левая грань

    n[0]=-1.0f;

    n[1]=0.0f;

    n[2]=0.0f;

    s[0]=0.0f;

    s[1]=0.0f;

    s[2]=1.0f;

    t[0]=0.0f;

    t[1]=1.0f;

    t[2]=0.0f;

    for (i=20; i<24; i++) {

      c[0]=data[5*i+2];

      c[1]=data[5*i+3];

      c[2]=data[5*i+4];

      SetUpBumps(n,c,l,s,t);

      glTexCoord2f(data[5*i]+c[0], data[5*i+1]+c[1]);

      glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);

    }

  glEnd();

 



  if (!emboss) {

    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D,texture[filter]);

    glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);

    glEnable(GL_LIGHTING);

    doCube();

  }

 


  xrot+=xspeed;

  yrot+=yspeed;

  if (xrot>360.0f) xrot-=360.0f;

  if (xrot<0.0f) xrot+=360.0f;

  if (yrot>360.0f) yrot-=360.0f;

  if (yrot<0.0f) yrot+=360.0f;

 

  /* ПОСЛЕДНИЙ ПРОХОД: Даешь логотипы! */

  doLogo();

  return true;                // Продолжаем

}

bool doMesh2TexelUnits(void) {

  GLfloat c[4]={0.0f,0.0f,0.0f,1.0f};     // Здесь храним текущую вершину

  GLfloat n[4]={0.0f,0.0f,0.0f,1.0f};     // Вектор нормали к текущей поверхности

  GLfloat s[4]={0.0f,0.0f,0.0f,1.0f};     // s-вектор, нормализованный

  GLfloat t[4]={0.0f,0.0f,0.0f,1.0f};     // t-вектор, нормализованный

  GLfloat l[4];                           // Хранит координаты источника света,

                                          // для перевода в пространство координат объекта

  GLfloat Minv[16];                       // Инвертированная матрица вида модели

  int i;

 

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очистим экран и буфер глубины

 

  // Инвертируем матрицу вида модели. Заменяет один Push/Pop и один glLoadIdentity();

  // Выполняется проведением всех преобразований в обратную сторону в обратном порядке

  glLoadIdentity();

  glRotatef(-yrot,0.0f,1.0f,0.0f);

  glRotatef(-xrot,1.0f,0.0f,0.0f);

  glTranslatef(0.0f,0.0f,-z);

  glGetFloatv(GL_MODELVIEW_MATRIX,Minv);

  glLoadIdentity();

  glTranslatef(0.0f,0.0f,z);

 

  glRotatef(xrot,1.0f,0.0f,0.0f);

  glRotatef(yrot,0.0f,1.0f,0.0f);

 

  // Преобразуем координаты источника света в систему координат объекта

  l[0]=LightPosition[0];

  l[1]=LightPosition[1];

  l[2]=LightPosition[2];

  l[3]=1.0f;                // Гомогенные координаты

  VMatMult(Minv,l);

 


  // ТЕКСТУРНЫЙ БЛОК #0

  glActiveTextureARB(GL_TEXTURE0_ARB);

  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, bump[filter]);

  glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

  glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);

 

  // ТЕКСТУРНЫЙ БЛОК #1

  glActiveTextureARB(GL_TEXTURE1_ARB);

  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, invbump[filter]);

  glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

  glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD);

 

  // Общие флаги

  glDisable(GL_BLEND);

  glDisable(GL_LIGHTING);

 
  glBegin(GL_QUADS);

    // Передняя грань

    n[0]=0.0f;

    n[1]=0.0f;

    n[2]=1.0f;

    s[0]=1.0f;

    s[1]=0.0f;

    s[2]=0.0f;

    t[0]=0.0f;

    t[1]=1.0f;

    t[2]=0.0f;

    for (i=0; i<4; i++) {

      c[0]=data[5*i+2];

      c[1]=data[5*i+3];

      c[2]=data[5*i+4];

      SetUpBumps(n,c,l,s,t);

      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,data[5*i], data[5*i+1]);

      glMultiTexCoord2fARB(GL_TEXTURE1_ARB,data[5*i]+c[0], data[5*i+1]+c[1]);

      glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);

    }

    // Задняя грань

    n[0]=0.0f;

    n[1]=0.0f;

    n[2]=-1.0f;

    s[0]=-1.0f;

    s[1]=0.0f;

    s[2]=0.0f;

    t[0]=0.0f;

    t[1]=1.0f;

    t[2]=0.0f;

    for (i=4; i<8; i++) {

      c[0]=data[5*i+2];

      c[1]=data[5*i+3];

      c[2]=data[5*i+4];

      SetUpBumps(n,c,l,s,t);

      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,data[5*i], data[5*i+1]);

      glMultiTexCoord2fARB(GL_TEXTURE1_ARB,data[5*i]+c[0], data[5*i+1]+c[1]);

      glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);

    }

    // Верхняя грань

    n[0]=0.0f;

    n[1]=1.0f;

    n[2]=0.0f;

    s[0]=1.0f;

    s[1]=0.0f;

    s[2]=0.0f;

    t[0]=0.0f;

    t[1]=0.0f;

    t[2]=-1.0f;

    for (i=8; i<12; i++) {

      c[0]=data[5*i+2];

      c[1]=data[5*i+3];

      c[2]=data[5*i+4];

      SetUpBumps(n,c,l,s,t);

      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,data[5*i], data[5*i+1]);

      glMultiTexCoord2fARB(GL_TEXTURE1_ARB,data[5*i]+c[0], data[5*i+1]+c[1]);

      glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);

    }

    // Нижняя грань

    n[0]=0.0f;

    n[1]=-1.0f;

    n[2]=0.0f;

    s[0]=-1.0f;

    s[1]=0.0f;

    s[2]=0.0f;

    t[0]=0.0f;

    t[1]=0.0f;

    t[2]=-1.0f;

    for (i=12; i<16; i++) {

      c[0]=data[5*i+2];

      c[1]=data[5*i+3];

      c[2]=data[5*i+4];

      SetUpBumps(n,c,l,s,t);

      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,data[5*i], data[5*i+1]);

      glMultiTexCoord2fARB(GL_TEXTURE1_ARB,data[5*i]+c[0], data[5*i+1]+c[1]);

      glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);

    }

    // Правая грань

    n[0]=1.0f;

    n[1]=0.0f;

    n[2]=0.0f;

    s[0]=0.0f;

    s[1]=0.0f;

    s[2]=-1.0f;

    t[0]=0.0f;

    t[1]=1.0f;

    t[2]=0.0f;

    for (i=16; i<20; i++) {

      c[0]=data[5*i+2];

      c[1]=data[5*i+3];

      c[2]=data[5*i+4];

      SetUpBumps(n,c,l,s,t);

      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,data[5*i], data[5*i+1]);

      glMultiTexCoord2fARB(GL_TEXTURE1_ARB,data[5*i]+c[0], data[5*i+1]+c[1]);

      glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);

    }

    // Левая грань

    n[0]=-1.0f;

    n[1]=0.0f;

    n[2]=0.0f;

    s[0]=0.0f;

    s[1]=0.0f;

    s[2]=1.0f;

    t[0]=0.0f;

    t[1]=1.0f;

    t[2]=0.0f;

    for (i=20; i<24; i++) {

      c[0]=data[5*i+2];

      c[1]=data[5*i+3];

      c[2]=data[5*i+4];

      SetUpBumps(n,c,l,s,t);

      glMultiTexCoord2fARB(GL_TEXTURE0_ARB,data[5*i], data[5*i+1]);

      glMultiTexCoord2fARB(GL_TEXTURE1_ARB,data[5*i]+c[0], data[5*i+1]+c[1]);

      glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);

    }

  glEnd();



  glActiveTextureARB(GL_TEXTURE1_ARB);

  glDisable(GL_TEXTURE_2D);

  glActiveTextureARB(GL_TEXTURE0_ARB);

  if (!emboss) {

    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D,texture[filter]);

    glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);

    glEnable(GL_BLEND);

    glEnable(GL_LIGHTING);

    doCube();

  }


  xrot+=xspeed;

  yrot+=yspeed;

  if (xrot>360.0f) xrot-=360.0f;

  if (xrot<0.0f) xrot+=360.0f;

  if (yrot>360.0f) yrot-=360.0f;

  if (yrot<0.0f) yrot+=360.0f;

 

  /* ПОСЛЕДНИЙ ПРОХОД: да будут логотипы! */

  doLogo();

  return true;                // Продолжим

}

bool doMeshNoBumps(void) {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      // Очистить экран и буфер глубины

  glLoadIdentity();              // Сбросить вид

  glTranslatef(0.0f,0.0f,z);

 

  glRotatef(xrot,1.0f,0.0f,0.0f);

  glRotatef(yrot,0.0f,1.0f,0.0f);

 

  if (useMultitexture) {

    glActiveTextureARB(GL_TEXTURE1_ARB);

    glDisable(GL_TEXTURE_2D);

    glActiveTextureARB(GL_TEXTURE0_ARB);

  }

 

  glDisable(GL_BLEND);

  glBindTexture(GL_TEXTURE_2D,texture[filter]);

  glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);

  glEnable(GL_LIGHTING);

  doCube();

 

  xrot+=xspeed;

  yrot+=yspeed;

  if (xrot>360.0f) xrot-=360.0f;

  if (xrot<0.0f) xrot+=360.0f;

  if (yrot>360.0f) yrot-=360.0f;

  if (yrot<0.0f) yrot+=360.0f;

 

  /* ПОСЛЕДНИЙ ПРОХОД: логотипы */

  doLogo();

  return true;                // Продолжим

}


bool DrawGLScene(GLvoid)              // Здесь все рисуется

{

  if (bumps) {

    if (useMultitexture && maxTexelUnits>1)

      return doMesh2TexelUnits();

    else return doMesh1TexelUnits();

  }

  else return doMeshNoBumps();


 

GLvoid KillGLWindow(GLvoid)           // Уничтожим окно корректно



 


 

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)




 

LRESULT CALLBACK WndProc(  HWND hWnd, // Указатель окна

        UINT uMsg,                    // Сообщение для этого окна

        WPARAM wParam,                // Дополнительная информация о сообщении

        LPARAM lParam)                // Дополнительная информация о сообщении

int WINAPI WinMain(  HINSTANCE hInstance, // Экземпляр приложения

      HINSTANCE hPrevInstance,            // Предыдущий экземпляр

      LPSTR lpCmdLine,                    // Параметры командной строки

      int nCmdShow)                       // Показать состояние окна

{

 


 

        if (keys['E'])

        {

          keys['E']=false;

          emboss=!emboss;

        }

 

        if (keys['M'])

        {

          keys['M']=false;

          useMultitexture=((!useMultitexture) && multitextureSupported);

        }

 

        if (keys['B'])

        {

          keys['B']=false;

          bumps=!bumps;

        }

 

        if (keys['F'])

        {

          keys['F']=false;

          filter++;

          filter%=3;

        }

 

        if (keys[VK_PRIOR])

        {

          z-=0.02f;

        }

 

        if (keys[VK_NEXT])

        {

          z+=0.02f;

        }

 

        if (keys[VK_UP])

        {

          xspeed-=0.01f;

        }

 

        if (keys[VK_DOWN])

        {

          xspeed+=0.01f;

        }

 

        if (keys[VK_RIGHT])

        {

          yspeed+=0.01f;

        }

 

        if (keys[VK_LEFT])

        {

          yspeed-=0.01f;

        }

      }

    }

  }

  // Выключаемся

  KillGLWindow();      // Убить окно

  return (msg.wParam); // Выйти из программы

}

