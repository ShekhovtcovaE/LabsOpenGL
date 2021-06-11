
#include <windows.h>       // Заголовочный файл для Windows

#include <math.h>          // Заголовочный файл для математической библиотеки

#include <stdio.h>         // Заголовочный файл для стандартного ввода/вывода

#include <stdlib.h>        // Заголовочный файл для стандартной библиотеки

#include <gl\gl.h>         // Заголовочный файл для библиотеки OpenGL32

#include <gl\glu.h>        // Заголовочный файл для библиотеки GLu32

#include <gl\glaux.h>      // Заголовочный файл для GLaux библиотеки

 

typedef struct point_3d {  // Структура для 3D точки( НОВОЕ )

  double x, y, z;

} POINT_3D;

 

typedef struct bpatch {    // Структура для полинома фрагмента Безье 3 степени (НОВОЕ)

  POINT_3D  anchors[4][4]; // Сетка 4x4 анкерных (anchor) точек

  GLuint    dlBPatch;      // Список для фрагмента Безье

  GLuint    texture;       // Текстура для фрагмента

} BEZIER_PATCH;

 

HDC       hDC=NULL;        // Контекст устройства

HGLRC     hRC=NULL;        // Контекст визуализации

HWND      hWnd=NULL;       // Дескриптор окна

HINSTANCE hInstance;       // Экземпляр приложения

 

bool      keys[256];       // Массив для работы с клавиатурой

bool      active=TRUE;     // Флаг активности приложения

bool      fullscreen=TRUE; // Флаг полноэкранного режима

 

DEVMODE      DMsaved;      // Сохранить настройки предыдущего режима ( НОВОЕ )

 

GLfloat      rotz = 0.0f;  // Вращение по оси Z

BEZIER_PATCH    mybezier;  // Фрагмент Безье для использования ( НОВОЕ )

BOOL      showCPoints=TRUE;// Переключатель отображения контрольных точек сетки ( НОВОЕ )

int      divs = 7;         // Число интерполяции (Контроль разрешения полигона) ( НОВОЕ )

 

LRESULT  CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);      // Декларация для WndProc



// Сложить 2 точки.

POINT_3D pointAdd(POINT_3D p, POINT_3D q) {

  p.x += q.x;    p.y += q.y;    p.z += q.z;

  return p;

}

 

// Умножение точки на константу

POINT_3D pointTimes(double c, POINT_3D p) {

  p.x *= c;  p.y *= c;  p.z *= c;

  return p;

}

 

// Функция для упрощения создания точки

POINT_3D makePoint(double a, double b, double c) {

  POINT_3D p;

  p.x = a;  p.y = b;  p.z = c;

  return p;

}


// Вычисляем полином 3 степени на основании массива из 4 точек

// и переменной u, которая обычно изменяется от 0 до 1

POINT_3D Bernstein(float u, POINT_3D *p) {

  POINT_3D  a, b, c, d, r;

 

  a = pointTimes(pow(u,3), p[0]);

  b = pointTimes(3*pow(u,2)*(1-u), p[1]);

  c = pointTimes(3*u*pow((1-u),2), p[2]);

  d = pointTimes(pow((1-u),3), p[3]);

 

  r = pointAdd(pointAdd(a, b), pointAdd(c, d));

 

  return r;

}

 

// Создание списков отображения на основе данных фрагмента

// и числе разбиений

GLuint genBezier(BEZIER_PATCH patch, int divs) {

  int    u = 0, v;

  float    py, px, pyold;

  GLuint    drawlist = glGenLists(1);  // Создать список отображения

  POINT_3D  temp[4];

  POINT_3D  *last = (POINT_3D*)malloc(sizeof(POINT_3D)*(divs+1));

        // Массив точек для отметки первой линии полигонов

 

  if (patch.dlBPatch != NULL)          // Удалить старые списки отображения

    glDeleteLists(patch.dlBPatch, 1);

 

  temp[0] = patch.anchors[0][3];       // Первая производная кривая (Вдоль оси X)

  temp[1] = patch.anchors[1][3];

  temp[2] = patch.anchors[2][3];

  temp[3] = patch.anchors[3][3];

 

  for (v=0;v<=divs;v++) {              // Создание первой линии точек

    px = ((float)v)/((float)divs);     // Процент вдоль оси Y

    // Используем 4 точки из производной кривой для вычисления точек вдоль кривой

    last[v] = Bernstein(px, temp);

  }

 

  glNewList(drawlist, GL_COMPILE);     // Начнем новый список отображения

  glBindTexture(GL_TEXTURE_2D, patch.texture); // Присоединим к текстуре

 

  for (u=1;u<=divs;u++) {

    py    = ((float)u)/((float)divs);          // Процент вдоль оси Y

    pyold = ((float)u-1.0f)/((float)divs);     // Процент вдоль старой оси Y

 

    temp[0] = Bernstein(py, patch.anchors[0]); // Вычислим новые точки Безье

    temp[1] = Bernstein(py, patch.anchors[1]);

    temp[2] = Bernstein(py, patch.anchors[2]);

    temp[3] = Bernstein(py, patch.anchors[3]);

 

    glBegin(GL_TRIANGLE_STRIP);        // Начнем новую полоску треугольников

 

    for (v=0;v<=divs;v++) {

      px = ((float)v)/((float)divs);   // Процент вдоль оси X

 

      glTexCoord2f(pyold, px);         // Применим старые координаты текстуры

      glVertex3d(last[v].x, last[v].y, last[v].z);  // Старая точка

 

      last[v] = Bernstein(px, temp);   // Генерируем новую точку

      glTexCoord2f(py, px);            // Применим новые координаты текстуры

      glVertex3d(last[v].x, last[v].y, last[v].z);  // Новая точка

    }

 

    glEnd();            // Конец полоски треугольников

  }

 

  glEndList();          // Конец списка

 

  free(last);           // Освободить старый массив вершин

  return drawlist;      // Вернуть список отображения

}

 

void initBezier(void) { 

  mybezier.anchors[0][0] = makePoint(-0.75,  -0.75,  -0.50);  // Вершины Безье

  mybezier.anchors[0][1] = makePoint(-0.25,  -0.75,   0.00);

  mybezier.anchors[0][2] = makePoint( 0.25,  -0.75,   0.00);

  mybezier.anchors[0][3] = makePoint( 0.75,  -0.75,  -0.50);

  mybezier.anchors[1][0] = makePoint(-0.75,  -0.25,  -0.75);

  mybezier.anchors[1][1] = makePoint(-0.25,  -0.25,   0.50);

  mybezier.anchors[1][2] = makePoint( 0.25,  -0.25,   0.50);

  mybezier.anchors[1][3] = makePoint( 0.75,  -0.25,  -0.75);

  mybezier.anchors[2][0] = makePoint(-0.75,   0.25,   0.00);

  mybezier.anchors[2][1] = makePoint(-0.25,   0.25,  -0.50);

  mybezier.anchors[2][2] = makePoint( 0.25,   0.25,  -0.50);

  mybezier.anchors[2][3] = makePoint( 0.75,   0.25,   0.00);

  mybezier.anchors[3][0] = makePoint(-0.75,   0.75,  -0.50);

  mybezier.anchors[3][1] = makePoint(-0.25,   0.75,  -1.00);

  mybezier.anchors[3][2] = makePoint( 0.25,   0.75,  -1.00);

  mybezier.anchors[3][3] = makePoint( 0.75,   0.75,  -0.50);

  mybezier.dlBPatch = NULL;

}



// Загрузить картинку и конвертировать ее в текстуру

 

BOOL LoadGLTexture(GLuint *texPntr, char* name)

{

  BOOL success = FALSE;

  AUX_RGBImageRec *TextureImage = NULL;

 

  glGenTextures(1, texPntr);          // Генерировать 1 текстуру

 

  FILE* test=NULL;

  TextureImage = NULL;

 

  test = fopen(name, "r");            // Существует ли файл?

  if (test != NULL) {                 // Если да

    fclose(test);                     // Закрыть файл

    TextureImage = auxDIBImageLoad(name); // И загрузить текстуру

  }

 

  if (TextureImage != NULL) {         // Если загружена

    success = TRUE;

 

    // Обычная генерация текстура используя данные из картинки

    glBindTexture(GL_TEXTURE_2D, *texPntr);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX, TextureImage->sizeY,

     0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  }

 

  if (TextureImage->data)

    free(TextureImage->data);

 

  return success;

}


int InitGL(GLvoid)              // Настройки OpenGL

{

  glEnable(GL_TEXTURE_2D);      // Разрешить наложение текстуры

  glShadeModel(GL_SMOOTH);      // Разрешить сглаживание

  glClearColor(0.05f, 0.05f, 0.05f, 0.5f); // Фон черный

  glClearDepth(1.0f);           // Настройки буфера глубины

  glEnable(GL_DEPTH_TEST);      // Разрешаем тест глубины

  glDepthFunc(GL_LEQUAL);       // Тип теста глубины

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Улучшенные вычисления перспективы

 

  initBezier();                 // Инициализация контрольной сетки Безье ( НОВОЕ )

  LoadGLTexture(&(mybezier.texture), "./Data/NeHe.bmp"); // Загрузка текстуры ( НОВОЕ )

  mybezier.dlBPatch = genBezier(mybezier, divs); // Создание фрагмента ( НОВОЕ )

 

  return TRUE;

}

 
int DrawGLScene(GLvoid)  {            // Здесь рисуем

  int i, j;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    // Очистка экрана и буфера глубины

  glLoadIdentity();            // Сброс текущей матрицы вида модели

  glTranslatef(0.0f,0.0f,-4.0f);          // Сдвиг налево на 1.5 единицы и вглубь экрана на 6.0

  glRotatef(-75.0f,1.0f,0.0f,0.0f);

  glRotatef(rotz,0.0f,0.0f,1.0f);         // Вращение по оси Z

   

  glCallList(mybezier.dlBPatch);// Вызов списка Безье

                                // Это необходимо только в том случае, когда фрагмент изменился

 

  if (showCPoints) {            // Если отрисовка сетки включена

    glDisable(GL_TEXTURE_2D);

    glColor3f(1.0f,0.0f,0.0f);

    for(i=0;i<4;i++) {          // Нарисовать горизонтальную линию

      glBegin(GL_LINE_STRIP);

      for(j=0;j<4;j++)

        glVertex3d(mybezier.anchors[i][j].x, mybezier.anchors[i][j].y, mybezier.anchors[i][j].z);

      glEnd();

    }

    for(i=0;i<4;i++) {          // Нарисовать вертикальную линию

      glBegin(GL_LINE_STRIP);

      for(j=0;j<4;j++)

        glVertex3d(mybezier.anchors[j][i].x, mybezier.anchors[j][i].y, mybezier.anchors[j][i].z);

      glEnd();

    }

    glColor3f(1.0f,1.0f,1.0f);

    glEnable(GL_TEXTURE_2D);

  }

 

  return TRUE;

}


GLvoid KillGLWindow(GLvoid)    // Убить окно

{

  if (fullscreen)              // Мы в полноэкранном режиме?

  {

    if (!ChangeDisplaySettings(NULL,CDS_TEST)) {// Если это не работает ( НОВОЕ )

      // Сделать это все равно (чтобы получить значения из системного реестра) (НОВОЕ)

      ChangeDisplaySettings(NULL,CDS_RESET);

      ChangeDisplaySettings(&DMsaved,CDS_RESET);// Изменить его на сохраненные настройки (НОВОЕ)

    } else {

      ChangeDisplaySettings(NULL,CDS_RESET);    // Если это работает продолжаем (НОВОЕ)

    }

     

    ShowCursor(TRUE);          // Показать курсор мыши

  }

 

  if (hRC)                     // Мы имеем контекст визуализации?

  {

    if (!wglMakeCurrent(NULL,NULL)) // Можно освободить контексты DC и RC?

    {

      MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",

                 MB_OK | MB_ICONINFORMATION);

    }

 

    if (!wglDeleteContext(hRC))     // Действительно ли мы можем удалить RC?

    {

      MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",

                 MB_OK | MB_ICONINFORMATION);

    }

    hRC=NULL;                  // Установить RC в NULL

  }

 

  if (hDC && !ReleaseDC(hWnd,hDC))  // Действительно ли мы можем удалить DC

  {

    MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",

               MB_OK | MB_ICONINFORMATION);

    hDC=NULL;                  // Установить DC в NULL

  }

 

  if (hWnd && !DestroyWindow(hWnd)) // Действительно ли мы можем удалить окно?

  {

    MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",

               MB_OK | MB_ICONINFORMATION);

    hWnd=NULL;                 // Set hWnd To NULL

  }

 

  // Действительно ли мы можем отменить регистрацию класса

  if (!UnregisterClass("OpenGL",hInstance))

  {

    MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",

               MB_OK | MB_ICONINFORMATION);

    hInstance=NULL;            // Установить hInstance в NULL

  }

}


BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)

{


  wc.lpszClassName  = "OpenGL"; // Имя класса

 

  // Сохранить текущие настройки дисплея (НОВОЕ)

  EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DMsaved);

 

  if (fullscreen)           // Попробовать перейти в полноэкранный режим?

  {


  return TRUE;              // Успех

}


int WINAPI WinMain( HINSTANCE  hInstance,     // Экземпляр

                    HINSTANCE  hPrevInstance, // Предыдущий экземпляр

                    LPSTR      lpCmdLine,     // Параметры командной строки

                    int        nCmdShow)      // Состояние отображения окна

{

 


        SwapBuffers(hDC);      // // Переключаем буферы (Двойная буферизация)

      }

 

      if (keys[VK_LEFT])  rotz -= 0.8f;    // Вращение влево ( НОВОЕ )

      if (keys[VK_RIGHT]) rotz += 0.8f;    // Вращение вправо

      if (keys[VK_UP]) {        // Увеличить разрешение

        divs++;

        mybezier.dlBPatch = genBezier(mybezier, divs);  // Обновить фрагмент

        keys[VK_UP] = FALSE;

      }

      if (keys[VK_DOWN] && divs > 1) {    // Уменьшить разрешения

        divs--;

        mybezier.dlBPatch = genBezier(mybezier, divs);  // Обновить фрагмент

        keys[VK_DOWN] = FALSE;

      }

      if (keys[VK_SPACE]) {        // ПРОБЕЛ переключает showCPoints

        showCPoints = !showCPoints;

        keys[VK_SPACE] = FALSE;

      }

 

   return (msg.wParam);            // Выходим из программы

}
