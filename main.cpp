#include "TXLib.h"
#include <cmath>
#include "WinUser.h"

struct array
{
    int *arr;
    int size;
};

array initArray (int n);
bool keyPress (int vKey);
void drawTable (int n, int m, int w, int h);
void changePixel (int x, int y, int n, int m, int w, int h, int c);
void render (array gameArr, int n, int m);
int neighborsCount (array gameArr, int i, int n, int m);
void makeStep (array gameArr, int n, int m);
void play (array gameArr, int n, int m);
int getMouse (int n, int m, int w, int h);
void edit (array gameArr, int n, int m);

array initArray (int n)
{
    array a;
    a.arr = (int*)malloc(n*sizeof(int));
    a.size = n;
    return a;
}

bool keyPress (int vKey)
{
    return GetAsyncKeyState(vKey) & 0x8000;
}

void drawTable (int n, int m, int w, int h)
{
    for (int i = 0; i < n+1; i++) txLine(w/n*i-1, 0, w/n*i-1, h);
    for (int i = 0; i < m+1; i++) txLine(0, h/m*i-1, w, h/m*i-1);
}

void changePixel (int x, int y, int n, int m, int w, int h, int c)
{
    int color = c ? 50 : 255;
    txSetFillColor(RGB(color, color, color));
    txRectangle((w+1)/n*x-1, (h+1)/m*y-1, (w+1)/n*x+w/n, (h+1)/m*y+h/m);
}

void render (array gameArr, int n, int m)
{
    for (int i = 0; i < gameArr.size; i++)
    {
        changePixel(i%n, i/n, n, m, txGetExtentX(), txGetExtentY(), gameArr.arr[i]);
    }
}

int neighborsCount (array gameArr, int i, int n, int m)
{
    int count = 0;
    for (int p = -1; p < 2; p++)
    {
        for (int q = -1; q < 2; q++)
        {
            if (p == 0 && q == 0) continue;
            int x = i%n + p, y = i/n + q;
            if (x < 0) x = n+x;
            else if (x > n-1) x = x-n;
            if (y < 0) y = m+y;
            else if (y > m-1) y = y-m;
            if (gameArr.arr[x + y*n] == 1) count++;
        }
    }
    return count;
}

void makeStep (array gameArr, int n, int m)
{
    array neighbors = initArray(gameArr.size);
    for (int i = 0; i < gameArr.size; i++)
    {
        neighbors.arr[i] = neighborsCount(gameArr, i, n, m);
    }
    for (int i = 0; i < gameArr.size; i++)
    {
        int count = neighbors.arr[i];
        if (count == 3)
        {
            if (gameArr.arr[i] == 1) continue;
            gameArr.arr[i] = 1;
            changePixel(i%n, i/n, n, m, txGetExtentX(), txGetExtentY(), 1);
        }
        else if (count < 2 || count > 3)
        {
            if (gameArr.arr[i] == 0) continue;
            gameArr.arr[i] = 0;
            changePixel(i%n, i/n, n, m, txGetExtentX(), txGetExtentY(), 0);
        }
    }
}

void play (array gameArr, int n, int m)
{
    txSleep(100);
    makeStep(gameArr, n, m);
}

int getMouse (int n, int m, int w, int h)
{
    int x = txMouseX(), y = txMouseY();
    return x*n/w + y*m/h * n;
}

void edit (array gameArr, int n, int m)
{
    int i , mode = (keyPress(VK_LBUTTON) ? 1 : 0);
    while ((mode && keyPress(VK_LBUTTON)) || (!mode && keyPress(VK_RBUTTON)))
    {
        i = getMouse(n, m, txGetExtentX(), txGetExtentY());
        gameArr.arr[i] = mode;
        changePixel(i%n, i/n, n, m, txGetExtentX(), txGetExtentY(), gameArr.arr[i]);
    }
    return;
}

int main ()
{
    int width = 1000, height = 500, cols = 100, rows = 50;
    txCreateWindow(width, height);

    drawTable(cols, rows, width, height);

    array gameArr = initArray(cols*rows);
    for(int i = 0; i < gameArr.size; i++) gameArr.arr[i] = 0;
    render(gameArr, cols, rows);
    while (!keyPress(VK_RETURN))
    {
        if (keyPress(VK_LBUTTON) || keyPress(VK_RBUTTON)) edit(gameArr, cols, rows);
        txSleep(0);
    }
    txSleep(500);
    while (!keyPress(VK_ESCAPE))
    {
        if (keyPress(VK_SPACE))
        {
            while (keyPress(VK_SPACE)) txSleep(0);
            while (!keyPress(VK_SPACE))
            {
                if (keyPress(VK_RIGHT))
                {
                    while (keyPress(VK_RIGHT)) txSleep(0);
                    makeStep(gameArr, cols, rows);
                }
                if (keyPress(VK_LBUTTON) || keyPress(VK_RBUTTON)) edit(gameArr, cols, rows);
            }
            while (keyPress(VK_SPACE)) txSleep(0);
        }
        play(gameArr, cols, rows);
    }

    return 0;
}