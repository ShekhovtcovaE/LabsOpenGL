
#include <windows.h>                      // Заголовочный файл для Windows
#include <stdio.h>                        // Заголовочный файл для стандартного ввода/вывода
#include <gl\gl.h>                        // Заголовочный файл для библиотеки OpenGL32
#include <gl\glu.h>                       // Заголовочный файл для для библиотеки GLu32
#include <gl\glaux.h>                     // Заголовочный файл для библиотеки GLaux

HDC             hDC=NULL;               // Служебный контекст GDI устройства
HGLRC           hRC=NULL;               // Постоянный контекст для визуализации
HWND            hWnd=NULL;              // Содержит дискриптор для окна
HINSTANCE       hInstance;              // Содержит данные для нашей программы

bool    keys[256];      // Массив, использующийся для сохранения состояния клавиатуры
bool    active=TRUE;    // Флаг состояния активности приложения (по умолчанию: TRUE)
bool    fullscreen=TRUE;// Флаг полноэкранного режима (по умолчанию: полноэкранное)


BOOL    twinkle;                        // Twinkling Stars (Вращающиеся звезды)
BOOL    tp;                             // 'T' клавиша нажата?


const   num=50;                         // Количество рисуемых звезд


typedef struct                          // Создаём структуру для звезд
{
        int r, g, b;                    // Цвет звезды
        GLfloat dist;                   // Расстояние от центра
        GLfloat angle;                  // Текущий угол звезды
}

typedef struct tagSECTOR         // Создаём структуру нашего сектора

{

      int numtriangles;          // Кол-во треугольников в секторе

      TRIANGLE* triangle         // Ссылка на массив треугольников

} SECTOR;                        // Обзовём структуру словом SECTOR

typedef struct tagTRIANGLE       // Создаём стр-ру нашего треугольника

{

      VERTEX vertex[3];          // Массив трёх вершин

} TRIANGLE;                      // Обзовём это TRIANGLE

typedef struct tagVERTEX         // Создаём стр-ру нашей вершины

{

      float x, y, z;    // 3D координаты

      float u, v;                // Координаты на текстуре

} VERTEX;                        // Обзовём это VERTEX

void SetupWorld()                        // Установка нашего мира

{

      FILE *filein;                      // Файл для работы

      filein = fopen(worldfile, "rt");   // Открываем наш файл

                ...

      (считываем наши данные)

                ...

      fclose(filein);                     // Закрываем наш файл

      return;                             // Возвращаемся назад

}

void readstr(FILE *f,char *string)  // Считать в строку

 

{

      do                            // Начинаем цикл

      {

            fgets(string, 255, f);  // Считываем одну линию

            // Проверяем её на условие повт. цикла

      } while ((string[0] == '/') || (string[0] == '\n'));

      return;                       // Возврат

}

 NUMPOLLIES n
 

int numtriangles;            // Кол-во треугольников в секторе

char oneline[255];           // Строка для сохранения данных

...

readstr(filein,oneline);     // Считать одну линию данных

// Считать кол-во треугольников

sscanf(oneline, "NUMPOLLIES %d\n", &numtriangles);

/ Декларация выше: SECTOR sector1;

char oneline[255];                 // Строка для сохранения данных

int numtriangles;                  // Кол-во треугольников в секторе

float x, y, z, u, v;               // 3D и текстурные координаты

...

// Выделяем память для  numtriangles и устанавливаем ссылку

sector1.triangle = new TRIANGLE[numtriangles];

// Определяем кол-во треугольников в Секторе 1

sector1.numtriangles = numtriangles;

// Цикл для всех треугольников

// За каждый шаг цикла – один треугольник в секторе

for (int triloop = 0; triloop < numtriangles; triloop++)

{

      // Цикл для всех вершин

// За каждый шаг цикла – одна вершина в треугольнике

      for (int vertloop = 0; vertloop < 3; vertloop++) {

            readstr(filein,oneline); // Считать строку для работы

            // Считать данные в соответствующие переменные вершин

            sscanf(oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);

            // Сохранить эти данные

            sector1.triangle[triloop].vertex[vertloop].x = x;

        // Сектор 1, Треугольник  triloop, Вершина vertloop, Значение x = x

            sector1.triangle[triloop].vertex[vertloop].y = y;

       // Сектор 1, Треугольник  triloop, Вершина vertloop, Значение y = y

            sector1.triangle[triloop].vertex[vertloop].z = z;

        // Сектор 1, Треугольник  triloop, Вершина vertloop, Значение z = z

            sector1.triangle[triloop].vertex[vertloop].u = u;

        // Сектор 1, Треугольник  triloop, Вершина vertloop, Значение u = u

            sector1.triangle[triloop].vertex[vertloop].v = v;

        // Сектор 1, Треугольник  triloop, Вершина vertloop, Значение y = v

      }

}



stars;                                  // Имя структуры - Stars
stars star[num];                        // Делаем массив 'star' длинной 'num',
                                        // где элементом является структура 'stars'


GLfloat zoom=-15.0f;                    // Расстояние от наблюдателя до звезд
GLfloat tilt=90.0f;                     // Начальный угол
GLfloat spin;                           // Для вращения звезд

GLuint  loop;                           // Используется для циклов
GLuint  texture[1];                     // Массив для одной текстуры

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);   // Объявления для WndProc


AUX_RGBImageRec *LoadBMP(char *Filename)// Функция для загрузки bmp файлов
{
        FILE *File=NULL;                // Переменная для файла

        if (!Filename)                  // Нужно убедиться в правильности переданого имени
        {
                return NULL;            // Если неправильное имя, то возвращаем NULL
        }

        File=fopen(Filename,"r");       // Открываем и проверяем на наличие файла

        if (File)                       // Файл существует?
        {
                fclose(File);           // Если да, то закрываем файл
        // И загружаем его с помощью библиотеки AUX, возращая ссылку на изображение
                return auxDIBImageLoad(Filename);
        }
        // Если загрузить не удалось или файл не найден, то возращаем NULL
        return NULL;
}


int LoadGLTextures()    // Функция загрузки изображения и конвертирования в текстуру
{
        int Status=FALSE;               // Индикатор статуса

        AUX_RGBImageRec *TextureImage[1];// Создаём место для хранения текстуры

        memset(TextureImage,0,sizeof(void *)*1);// устанавливаем ссылку на NULL

        // Загружаем изображение, Проверяем на ошибки, Если файл не найден то выходим
        if (TextureImage[0]=LoadBMP("Data/Star.bmp"))
        {
                Status=TRUE;            // Ставим статус в TRUE

                glGenTextures(1, &texture[0]);  // Генерируем один индификатор текстуры

                // Создаём текстуру с линейной фильтрацией (Linear Filtered)
                glBindTexture(GL_TEXTURE_2D, texture[0]);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX,
        TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
        }

        if (TextureImage[0])            // Если текстура существует
        {
                if (TextureImage[0]->data)   // Если изображение существует
                {
                        // Освобождаем место выделенное под изображение
                        free(TextureImage[0]->data);
                }

                free(TextureImage[0]);  // Освобождаем структуры изображения
        }

        return Status;                  // Возвращаем статус
}


int InitGL(GLvoid)                      // Всё установки OpenGL будут здесь
{
        if (!LoadGLTextures())          // Загружаем текстуру
        {
                return FALSE;           // Если не загрузилась, то возвращаем FALSE
        }

        glEnable(GL_TEXTURE_2D);        // Включаем текстурирование
        // Включаем плавную ракраску (интерполирование по вершинам)
        glShadeModel(GL_SMOOTH);
        glClearColor(0.0f, 0.0f, 0.0f, 0.5f);   // Фоном будет черный цвет
        glClearDepth(1.0f);                     // Установки буфера глубины (Depth Buffer)
        // Максимальное качество перспективной коррекции
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        // Устанавливаем функцию смешивания
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        glEnable(GL_BLEND);                     // Включаем смешивание


        for (loop=0; loop<num; loop++)       // Делаем цикл и бежим по всем звездам
        {
                star[loop].angle=0.0f;  // Устанавливаем всё углы в 0


                // Вычисляем растояние до центра
                star[loop].dist=(float(loop)/num)*5.0f;
                // Присваиваем star[loop] случайное значение (красный).
                star[loop].r=rand()%256;
                // Присваиваем star[loop] случайное значение (зеленый)
                star[loop].g=rand()%256;
                // Присваиваем star[loop] случайное значение (голубой)
                star[loop].b=rand()%256;
        }
        return TRUE;                    // Инициализация прошла нормально.

}

int DrawGLScene(GLvoid)             // Нарисовать сцену OpenGL

{

// Очистить сцену и буфер глубины

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glLoadIdentity();             // Сбросить текущую матрицу

 

      // Вещ. перем. для временных X, Y, Z, U и V

      GLfloat x_m, y_m, z_m, u_m, v_m;

      GLfloat xtrans = -xpos;       // Проекция игрока на ось X

      GLfloat ztrans = -zpos;       // Проекция игрока на ось Z

      // Для смещения изображения вверх и вниз

      GLfloat ytrans = -walkbias-0.25f;

      // 360 градусный угол для поворота игрока

      GLfloat sceneroty = 360.0f - yrot;

 

      int numtriangles;           // Количество треугольников

 

      glRotatef(lookupdown,1.0f,0,0);// Вращать вверх и вниз

// Вращать в соответствии с направлением взгляда игрока

      glRotatef(sceneroty,0,1.0f,0);

     

      // Проецировать сцену относительно игрока

      glTranslatef(xtrans, ytrans, ztrans);

      // Выбрать текстуру filter

      glBindTexture(GL_TEXTURE_2D, texture[filter]);

     

      // Получить кол-во треугольников Сектора 1

      numtriangles = sector1.numtriangles;

     

      // Процесс для каждого треугольника

      // Цикл по треугольникам

      for (int loop_m = 0; loop_m < numtriangles; loop_m++)

      {

            glBegin(GL_TRIANGLES);  // Начинаем рисовать треугольники

// Нормализованный указатель вперёд

                  glNormal3f( 0.0f, 0.0f, 1.0f);

                  x_m = sector1.triangle[loop_m].vertex[0].x;// X 1-ой точки

                  y_m = sector1.triangle[loop_m].vertex[0].y;// Y 1-ой точки

                  z_m = sector1.triangle[loop_m].vertex[0].z;// Z 1-ой точки

                  // U текстурная координата

                  u_m = sector1.triangle[loop_m].vertex[0].u;

                  // V текстурная координата

                  v_m = sector1.triangle[loop_m].vertex[0].v;

                  glTexCoord2f(u_m,v_m); glVertex3f(x_m,y_m,z_m);

                                    // Установить TexCoord и грань

                 

                  x_m = sector1.triangle[loop_m].vertex[1].x;// X 2-ой точки

                  y_m = sector1.triangle[loop_m].vertex[1].y;// Y 2-ой точки

                  z_m = sector1.triangle[loop_m].vertex[1].z;// Z 2-ой точки

                  // U текстурная координата

                  u_m = sector1.triangle[loop_m].vertex[1].u;

      // V текстурная координата

                  v_m = sector1.triangle[loop_m].vertex[1].v;

                  glTexCoord2f(u_m,v_m); glVertex3f(x_m,y_m,z_m);

                                    // Установить TexCoord и грань

                 

                  x_m = sector1.triangle[loop_m].vertex[2].x;// X 3-ой точки

                  y_m = sector1.triangle[loop_m].vertex[2].y;// Y 3-ой точки

                  z_m = sector1.triangle[loop_m].vertex[2].z;// Z 3-ой точки

      // U текстурная координата

u_m = sector1.triangle[loop_m].vertex[2].u;

      // V текстурная координата

                  v_m = sector1.triangle[loop_m].vertex[2].v;

                  glTexCoord2f(u_m,v_m); glVertex3f(x_m,y_m,z_m);

                                    // Установить TexCoord и грань

            glEnd();                // Заканчиваем рисовать треугольники

      }

      return TRUE;             // Возвращаемся

}


                SwapBuffers(hDC);               // Смена буфера (Double Buffering)
                if (keys['T'] && !tp)           // Если 'T' нажата и tp равно FALSE
                {
                        tp=TRUE;                // то делаем tp равным TRUE
                        twinkle=!twinkle;       // Меняем значение twinkle на обратное
                }


                if (!keys['T'])                 // Клавиша 'T' была отключена
                {
                        tp=FALSE;               // Делаем tp равное FALSE
                }


                if (keys[VK_UP])                // Стрелка вверх была нажата?
                {
                        tilt-=0.5f;             // Вращаем экран вверх
                }

                if (keys[VK_RIGHT])              // Была ли нажата правая стрелка?

{

      yrot -= 1.5f;              // Вращать сцену влево

}

 

if (keys[VK_LEFT])               // Была ли нажата левая стрелка?

{

      yrot += 1.5f;              // Вращать сцену вправо

}

 

if (keys[VK_UP])                 // Была ли нажата стрелка вверх?

{

      // Переместиться на X-плоскости, базируемой на направлении игрока

      xpos -= (float)sin(heading*piover180) * 0.05f;

      // Переместиться на Z-плоскости, базируемой на направлении игрока

      zpos -= (float)cos(heading*piover180) * 0.05f;

      if (walkbiasangle >= 359.0f)// walkbiasangle>=359?

      {

            walkbiasangle = 0.0f; // Присвоить walkbiasangle  0

      }

      else                        // В противном случае

      {

      // Если walkbiasangle < 359 увеличить его на 10

             walkbiasangle+= 10;

      }

      // Иммитация походки человека

      walkbias = (float)sin(walkbiasangle * piover180)/20.0f;

}

 

if (keys[VK_DOWN])               // Была ли нажата стрелка вниз?

{

      // Переместиться на X-плоскости, базируемой на направлении игрока

      xpos += (float)sin(heading*piover180) * 0.05f;

      // Переместиться на Z-плоскости, базируемой на направлении игрока

      zpos += (float)cos(heading*piover180) * 0.05f;

      if (walkbiasangle <= 1.0f)    // walkbiasangle<=1?

      {

            walkbiasangle = 359.0f; // Присвоить walkbiasangle 359

      }

      else                          // В противном случае

      {

// Если walkbiasangle >1 уменьшить его на 10

walkbiasangle-= 10;

      }

      // Иммитация походки человека

      walkbias = (float)sin(walkbiasangle * piover180)/20.0f;

}

                if (keys[VK_PRIOR])             // Page Up нажат?
                {
                        zoom-=0.2f;             // Уменьшаем
                }

                if (keys[VK_NEXT])              // Page Down нажата?
                {
                        zoom+=0.2f;             // Увеличиваем
                }


                if (keys[VK_F1])                // Если F1 нажата?
                {
                        keys[VK_F1]=FALSE;      // Делаем клавишу равной FALSE
                        KillGLWindow();         // Закрываем текущее окно
                        fullscreen=!fullscreen;
        // Переключаем режимы Fullscreen (полноэкранный) / Windowed (обычный)
                        // Пересоздаём OpenGL окно
                        if (!CreateGLWindow("NeHe's Textures,
                         Lighting & Keyboard Tutorial",640,480,16,fullscreen))
                        {
                                return 0;       //Выходим если не получилось
                        }
                }
        }
}
