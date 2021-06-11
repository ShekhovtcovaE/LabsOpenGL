
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


int DrawGLScene(GLvoid)                 // Здесь мы всё рисуем
{
        // Очищаем буфер цвета и глубины
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Выбираем нашу текстуру
        glBindTexture(GL_TEXTURE_2D, texture[0]);

        for (loop=0; loop<num; loop++)               // Цикл по всем звездам
        {
                // Обнуляем видовую матрицу (Model Matrix) перед каждой звездой
                glLoadIdentity();
                // Переносим по оси z на 'zoom'
                glTranslatef(0.0f,0.0f,zoom);
                // Вращаем вокруг оси x на угол 'tilt'
                glRotatef(tilt,1.0f,0.0f,0.0f);


                // Поворачиваем на угол звезды вокруг оси y
                glRotatef(star[loop].angle,0.0f,1.0f,0.0f);
                // Двигаемся вперед по оси x
                glTranslatef(star[loop].dist,0.0f,0.0f);


                glRotatef(-star[loop].angle,0.0f,1.0f,0.0f);
                // Отменяет текущий поворот звезды
                glRotatef(-tilt,1.0f,0.0f,0.0f);        // Отменяет поворот экрана


                if (twinkle)                            // Если Twinkling включен
                {
                        // Данный цвет использует байты
                        glColor4ub(star[(num-loop)-1].r,star[(num-loop)-1].g,
                        star[(num-loop)-1].b,255);
                        glBegin(GL_QUADS);// Начинаем рисовать текстурированый квадрат
                                glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
                                glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
                                glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
                                glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
                        glEnd();                                // Закончили рисовать
                }


                glRotatef(spin,0.0f,0.0f,1.0f);// Поворачиваем звезду по оси z
                // Цвет использует байты
                glColor4ub(star[loop].r,star[loop].g,star[loop].b,255);
                glBegin(GL_QUADS);              // Начинаем рисовать текстурный квадрат
                        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
                        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
                        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
                        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
                glEnd();                                        // Закончили рисовать


                spin+=0.01f;                    // Используется для вращения звезды
                star[loop].angle+=float(loop)/num;// Меняем угол звезды
                star[loop].dist-=0.01f; // Меняем растояние до центра


                if (star[loop].dist<0.0f)    // Звезда в центре экрана?
                {
                        star[loop].dist+=5.0f;  // Перемещаем на 5 единиц от центра
                        // Новое значение красной компоненты цвета
                        star[loop].r=rand()%256;
                        // Новое значение зеленной компоненты цвета
                        star[loop].g=rand()%256;
                        // Новое значение синей компоненты цвета
                        star[loop].b=rand()%256;
                }
        }
        return TRUE;                                            // Всё ок
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

                if (keys[VK_DOWN])              // Стрелка вниз нажата?
                {
                        tilt+=0.5f;             // Вращаем экран вниз
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

