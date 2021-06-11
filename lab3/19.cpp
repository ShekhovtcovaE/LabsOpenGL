
#include <windows.h>  // Заголовочный файл для Windows

#include <stdio.h>    // Заголовочный файл для стандартной библиотеки ввода/вывода(НОВОЕ)

#include <gl\gl.h>    // Заголовочный файл для библиотеки OpenGL32

#include <gl\glu.h>   // Заголовочный файл для библиотеки GLu32

#include <gl\glaux.h> // Заголовочный файл для библиотеки GLaux

#define MAX_PARTICLES 1000 // Число частиц для создания ( НОВОЕ )

HDC        hDC=NULL;  // Приватный контекст устройства GDI

HGLRC      hRC=NULL;  // Постоянный контекст рендеринга

HWND       hWnd=NULL; // Сохраняет дескриптор окна

HINSTANCE  hInstance; // Сохраняет экземпляр приложения

 

bool  keys[256];      // Массив для работы с клавиатурой

bool  active=TRUE;    // Флаг активации окна, по умолчанию = TRUE

bool  fullscreen=TRUE;// Флаг полноэкранного режима


bool rainbow=true; // Режим радуги? ( НОВОЕ )
bool sp; // Пробел нажат? ( НОВОЕ )
bool rp; // Ввод нажат? ( НОВОЕ)

float slowdown=2.0f; // Торможение частиц
float xspeed; // Основная скорость по X (с клавиатуры изменяется направление хвоста)
float yspeed; // Основная скорость по Y (с клавиатуры изменяется направление хвоста)
float zoom=-40.0f; // Масштаб пучка частиц


GLuint loop;       // Переменная цикла
GLuint col;        // Текущий выбранный цвет
GLuint delay;      // Задержка для эффекта радуги
GLuint texture[1]; // Память для нашей текстуры


typedef struct // Структура частицы
{
  bool active; // Активность (Да/нет)
  float life;  // Жизнь
  float fade;  // Скорость угасания


  float r; // Красное значение
  float g; // Зеленное значение
  float b; // Синие значение


  float x; // X позиция
  float y; // Y позиция
  float z; // Z позиция


  float xi; // X направление
  float yi; // Y направление
  float zi; // Z направление


  float xg; // X гравитация
  float yg; // Y гравитация
  float zg; // Z гравитация

particles - название нашей структуры.

}
particles; // Структура Частиц


static GLfloat colors[12][3]= // Цветовая радуга

{
  {1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
  {0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
  {0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
};

LRESULT  CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Объявление WndProc

Код загрузки картинки не изменился.

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

 
if (TextureImage[0]=LoadBMP("Data/Particle.bmp")) // Загрузка текстуры частицы

{

  Status=TRUE; // Задать статус в TRUE
  glGenTextures(1, &texture[0]); // Создать одну текстуру

  glBindTexture(GL_TEXTURE_2D, texture[0]);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY,

    0, GL_RGB,   GL_UNSIGNED_BYTE, TextureImage[0]->data);
}

if (TextureImage[0])            // Если текстура существует

{

 if (TextureImage[0]->data)     // Если изображение текстуры существует

 {

   free(TextureImage[0]->data); // Освобождение памяти изображения текстуры

 }

 free(TextureImage[0]);         // Освобождение памяти под структуру

}

 

  return Status;        // Возвращаем статус

}


// Изменение размеров и инициализация окна GL

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)
{
  if (height==0) // Предотвращение деления на ноль, если окно слишком мало
  {
    height=1; // Сделать высоту равной единице
  }


  //Сброс текущей области вывода и перспективных преобразований
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION); // Выбор матрицы проекций
  glLoadIdentity(); // Сброс матрицы проекции

  // Вычисление соотношения геометрических размеров для окна
  gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,200.0f); // ( МОДИФИЦИРОВАНО )

  glMatrixMode(GL_MODELVIEW); // Выбор матрицы просмотра модели
  glLoadIdentity(); // Сброс матрицы просмотра модели
}


int InitGL(GLvoid)       // Все начальные настройки OpenGL здесь
{
  if (!LoadGLTextures()) // Переход на процедуру загрузки текстуры
  {
    return FALSE;        // Если текстура не загружена возвращаем FALSE
  }

Мы разрешаем плавное затенение, очищаем фон черным цветом, запрещаем тест глубины, разрешаем смешивание и наложение текстуры. После разрешения наложения текстуры мы выбираем нашу текстуру частицы.

  glShadeModel(GL_SMOOTH);    // Разрешить плавное затенение

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Черный фон

  glClearDepth(1.0f);         // Установка буфера глубины

  glDisable(GL_DEPTH_TEST);   // Запрещение теста глубины

  glEnable(GL_BLEND);         // Разрешаем смешивание
  glBlendFunc(GL_SRC_ALPHA,GL_ONE); // Тип смешивания

  // Улучшенные вычисления перспективы
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
  glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);  // Улучшенные точечное смешение
  glEnable(GL_TEXTURE_2D);                 // Разрешение наложения текстуры
  glBindTexture(GL_TEXTURE_2D,texture[0]); // Выбор нашей текстуры


for (loop=0;loop<MAX_PARTICLES;loop++) // Инициализация всех частиц
{
  particle[loop].active=true; // Сделать все частицы активными
  particle[loop].life=1.0f;   // Сделать все частицы с полной жизнью


  //Случайная скорость угасания
  particle[loop].fade=float(rand()%100)/1000.0f+0.003f;

  

  particle[loop].r=colors[loop*(12/MAX_PARTICLES)][0]; // Выбор красного цвета радуги
  particle[loop].g=colors[loop*(12/MAX_PARTICLES)][1]; // Выбор зеленного цвета радуги
  particle[loop].b=colors[loop*(12/MAX_PARTICLES)][2]; // Выбор синего цвета радуги


  particle[loop].xi=float((rand()%50)-26.0f)*10.0f; // Случайная скорость по оси X
  particle[loop].yi=float((rand()%50)-25.0f)*10.0f; // Случайная скорость по оси Y
  particle[loop].zi=float((rand()%50)-25.0f)*10.0f; // Случайная скорость по оси Z


  particle[loop].xg=0.0f;  // Зададим горизонтальное притяжение в ноль
  particle[loop].yg=-0.8f; // Зададим вертикальное притяжение вниз
  particle[loop].zg=0.0f;  // зададим притяжение по оси Z в ноль
}
return TRUE; // Инициализация завершена OK
}


int DrawGLScene(GLvoid) // Здесь мы все рисуем
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Очистка экрана и буфера глубины

  glLoadIdentity(); // Сброс матрицы просмотра модели


  for (loop=0;loop<MAX_PARTICLES;loop++) // Цикл по всем частицам
  {


    if (particle[loop].active) // Если частицы не активны
    {


      float x=particle[loop].x; // Захватим позицию X нашей частицы
      float y=particle[loop].y; // Захватим позицию Н нашей частицы
      float z=particle[loop].z+zoom; // Позиция частицы по Z + Zoom


      // Вывод частицы, используя наши RGB значения, угасание частицы согласно её жизни
      glColor4f(particle[loop].r,particle[loop].g,particle[loop].b,particle[loop].life);


      glBegin(GL_TRIANGLE_STRIP); // Построение четырехугольника из треугольной полоски


      glTexCoord2d(1,1); glVertex3f(x+0.5f,y+0.5f,z); // Верхняя правая
      glTexCoord2d(0,1); glVertex3f(x-0.5f,y+0.5f,z); // Верхняя левая
      glTexCoord2d(1,0); glVertex3f(x+0.5f,y-0.5f,z); // Нижняя правая
      glTexCoord2d(0,0); glVertex3f(x-0.5f,y-0.5f,z); // Нижняя левая


      glEnd(); // Завершение построения полоски треугольников


      // Передвижение по оси X на скорость по X

      particle[loop].x+=particle[loop].xi/(slowdown*1000);
      // Передвижение по оси Y на скорость по Y

      particle[loop].y+=particle[loop].yi/(slowdown*1000);
      // Передвижение по оси Z на скорость по Z

      particle[loop].z+=particle[loop].zi/(slowdown*1000);


      particle[loop].xi+=particle[loop].xg; // Притяжение по X для этой записи
      particle[loop].yi+=particle[loop].yg; // Притяжение по Y для этой записи
      particle[loop].zi+=particle[loop].zg; // Притяжение по Z для этой записи


      particle[loop].life-=particle[loop].fade; // Уменьшить жизнь частицы на ‘угасание’


      if (particle[loop].life<0.0f) // Если частица погасла
      {


        particle[loop].life=1.0f; // Дать новую жизнь
        // Случайное значение угасания
        particle[loop].fade=float(rand()%100)/1000.0f+0.003f;


        particle[loop].x=0.0f; // На центр оси X
        particle[loop].y=0.0f; // На центр оси Y
        particle[loop].z=0.0f; // На центр оси Z


        particle[loop].xi=xspeed+float((rand()%60)-32.0f);//Скорость и направление по оси X
        particle[loop].yi=yspeed+float((rand()%60)-30.0f);//Скорость и направление по оси Y
        particle[loop].zi=float((rand()%60)-30.0f);       //Скорость и направление по оси Z


        particle[loop].r=colors[col][0]; // Выбор красного из таблицы цветов
        particle[loop].g=colors[col][1]; // Выбор зеленого из таблицы цветов
        particle[loop].b=colors[col][2]; // Выбор синего из таблицы цветов
      }


      // Если клавиша 8 на цифровой клавиатуре нажата и гравитация меньше чем 1.5

      // тогда увеличим притяжение вверх
      if (keys[VK_NUMPAD8] && (particle[loop].yg<1.5f)) particle[loop].yg+=0.01f;


      // Если клавиша 2 на цифровой клавиатуре нажата и гравитация больше чем -1.5

      // тогда увеличим притяжение вниз
      if (keys[VK_NUMPAD2] && (particle[loop].yg>-1.5f)) particle[loop].yg-=0.01f;


      // Если клавиша 6 на цифровой клавиатуре нажата и гравитация меньше чем 1.5

      // тогда увеличим притяжение вправо
      if (keys[VK_NUMPAD6] && (particle[loop].xg<1.5f)) particle[loop].xg+=0.01f;

    // Если клавиша 4 на цифровой клавиатуре нажата и гравитация больше чем -1.5

      // тогда увеличим притяжение влево
      if (keys[VK_NUMPAD4] && (particle[loop].xg>-1.5f)) particle[loop].xg-=0.01f;


      if (keys[VK_TAB]) // Клавиша табуляции вызывает взрыв
      {
        particle[loop].x=0.0f; // Центр по оси X
        particle[loop].y=0.0f; // Центр по оси Y
        particle[loop].z=0.0f; // Центр по оси Z
        particle[loop].xi=float((rand()%50)-26.0f)*10.0f; // Случайная скорость по оси X
        particle[loop].yi=float((rand()%50)-25.0f)*10.0f; // Случайная скорость по оси Y
        particle[loop].zi=float((rand()%50)-25.0f)*10.0f; // Случайная скорость по оси Z
      }
    }
  }
  return TRUE; // Все OK
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

  if (!CreateGLWindow("NeHe's Particle Tutorial",640,480,16,fullscreen))

  {

    return 0;                  // Выходим если окно не было создано

  }

  
  if (fullscreen)  // Полноэкранный режим ( ДОБАВЛЕНО )
  {
    slowdown=1.0f; // Скорость частиц (для 3dfx) ( ДОБАВЛЕНО )
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

        SwapBuffers(hDC); // Переключаем буферы (Двойная буфферизация)

 
        if (keys[VK_ADD] && (slowdown>1.0f)) slowdown-=0.01f;//Скорость частицы увеличилась


        if (keys[VK_SUBTRACT] && (slowdown<4.0f)) slowdown+=0.01f; // Торможение частиц


        if (keys[VK_PRIOR]) zoom+=0.1f; // Крупный план


        if (keys[VK_NEXT]) zoom-=0.1f; // Мелкий план


        if (keys[VK_RETURN] && !rp) // нажата клавиша Enter
        {
          rp=true; // Установка флага, что клавиша нажата
          rainbow=!rainbow; // Переключение режима радуги в Вкл/Выкл
        }
        if (!keys[VK_RETURN]) rp=false; // Если клавиша Enter не нажата – сбросить флаг


        if ((keys[' '] && !sp) || (rainbow && (delay>25))) // Пробел или режим радуги
        {


          if (keys[' ']) rainbow=false; // Если пробел нажат запрет режима радуги


          sp=true; // Установка флага нам скажет, что пробел нажат
          delay=0; // Сброс задержки циклической смены цветов радуги
          col++;   // Изменить цвет частицы


          if (col>11) col=0; // Если цвет выше, то сбросить его
        }


        if (!keys[' ']) sp=false; // Если клавиша пробел не нажата, то сбросим флаг


        //Если нажата клавиша вверх и скорость по Y меньше чем 200, то увеличим скорость
        if (keys[VK_UP] && (yspeed<200)) yspeed+=1.0f;


        // Если стрелка вниз и скорость по Y больше чем –200, то увеличим скорость падения
        if (keys[VK_DOWN] && (yspeed>-200)) yspeed-=1.0f;


        // Если стрелка вправо и X скорость меньше чем 200, то увеличить скорость вправо
        if (keys[VK_RIGHT] && (xspeed<200)) xspeed+=1.0f;


        // Если стрелка влево и X скорость больше чем –200, то увеличить скорость влево
        if (keys[VK_LEFT] && (xspeed>-200)) xspeed-=1.0f;


        delay++; // Увеличить счетчик задержки циклической смены цветов в режиме радуги


        if (keys[VK_F1])             // Была нажата кнопка F1?

        {

          keys[VK_F1]=FALSE;         // Если так - установим значение FALSE

          KillGLWindow();            // Закроем текущее окно OpenGL 

          fullscreen=!fullscreen;    // Переключим режим "Полный экран"/"Оконный"

          // Заново создадим наше окно OpenGL

          if (!CreateGLWindow("NeHe's Particle Tutorial",640,480,16,fullscreen))

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
