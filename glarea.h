#ifndef GLAREA_H
#define GLAREA_H

#include "animal.h"
#include "grid.h"
#include "ground.h"
#include "plant.h"
#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class GLArea : public QOpenGLWidget,
               protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLArea(QWidget *parent = nullptr);
    ~GLArea() override;

    Grid grid;
    Ground ground;
    std::vector<std::unique_ptr<Plant>> plants;
    Animal animal;

protected slots:
    void onTimeout();

protected:
    void initializeGL() override;
    void doProjection();
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;

    void advanceTurn();

private:
    float xRot=0.0f, yRot=0.0f, zRot=0.0f;
    float xPos=0.0f,  yPos=1.0f, zPos=-50.0f;
    QTimer *timer = nullptr;
    QElapsedTimer elapsedTimer;
    float dt = 0;
    float windowRatio = 1.0f;
    QPoint lastPos;

    QOpenGLShaderProgram *program_grid;
    QOpenGLShaderProgram *program_plant;
    QOpenGLShaderProgram *program_animal;

    QOpenGLBuffer vbo_grid;
    QOpenGLBuffer vbo_plant;
    QOpenGLBuffer vbo_animal;

    void makeGLObjects();
    void tearGLObjects();

    int gridSize = 2;
    int round = 0;
};

#endif // GLAREA_H
