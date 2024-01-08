#include "glarea.h"
#include <QDebug>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include <iostream>
#include <ostream>

GLArea::GLArea(QWidget *parent) :
    QOpenGLWidget(parent)
{
    QSurfaceFormat sf;
    sf.setDepthBufferSize(24);
    sf.setSamples(16);
    setFormat(sf);

    setEnabled(true);                   // événements clavier et souris
    setFocusPolicy(Qt::StrongFocus);    // accepte focus
    setFocus();                         // donne le focus

    timer = new QTimer(this);
    timer->setInterval(50);           // msec
    connect (timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start();
    elapsedTimer.start();

    ground = Ground(20, 20, gridSize);

}

GLArea::~GLArea()
{
    delete timer;

    makeCurrent();
    tearGLObjects();
    doneCurrent();
}

void GLArea::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    makeGLObjects();

    // shader of grid
    program_grid = new QOpenGLShaderProgram(this);
    program_grid->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/grid.vert");
    program_grid->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/grid.frag");
    if (! program_grid->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << program_grid->log();
    }
    // shader of plants
    program_plant = new QOpenGLShaderProgram(this);
    program_plant->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/plant.vert");
    program_plant->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/plant.frag");
    if (! program_plant->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << program_plant->log();
    }
    // shader of animals
    program_animal = new QOpenGLShaderProgram(this);
    program_animal->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/animal.vert");
    program_animal->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/animal.frag");
    if (! program_animal->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << program_animal->log();
    }
}

void GLArea::makeGLObjects()
{
    //Create Grids
    GLfloat halfSize = gridSize / 2.0f;
    GLfloat vertices_grid[] = {
        -halfSize, -halfSize, 0.0f,
        -halfSize,  halfSize, 0.0f,

        -halfSize,  halfSize, 0.0f,
        halfSize,  halfSize, 0.0f,

        halfSize,  halfSize, 0.0f,
        halfSize, -halfSize, 0.0f,

        halfSize, -halfSize, 0.0f,
        -halfSize, -halfSize, 0.0f
    };
    QVector<GLfloat> vg;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 3; j++) {
            vg.append(vertices_grid[i*3+j]);
        }
    }

    vbo_grid.create();
    vbo_grid.bind();
    vbo_grid.allocate(vg.constData(), vg.count() * int(sizeof(GLfloat)));
    //Create plants
    GLfloat plantSize = gridSize / 2.0f;
    GLfloat height = sqrt(3.0f) / 2.0f * plantSize;

    GLfloat centerX = 0.0f;
    GLfloat centerY = -0.15f;

    GLfloat vertices_plant[] = {
        centerX - plantSize / 2.0f, centerY - height / 3.0f, 0.0f,
        centerX + plantSize / 2.0f, centerY - height / 3.0f, 0.0f,
        centerX, centerY + 2.0f * height / 3.0f, 0.0f
    };

    QVector<GLfloat> vg_plant;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            vg_plant.append(vertices_plant[i * 3 + j]);
        }
    }

    vbo_plant.create();
    vbo_plant.bind();
    vbo_plant.allocate(vg_plant.constData(), vg_plant.count() * int(sizeof(GLfloat)));
    //Create animals
    GLfloat radius = gridSize / 4.0f;
    GLfloat centerX_animal = 0.0f;
    GLfloat centerY_animal = 0.0f;
    int numSegments = 20;

    QVector<GLfloat> vg_animal;
    for (int i = 0; i < numSegments; i++) {
        GLfloat theta = 2.0f * M_PI * float(i) / float(numSegments);
        GLfloat nextTheta = 2.0f * M_PI * float(i + 1) / float(numSegments);

        vg_animal.append(centerX_animal);
        vg_animal.append(centerY_animal);
        vg_animal.append(0.0f);

        vg_animal.append(radius * cosf(theta) + centerX_animal);
        vg_animal.append(radius * sinf(theta) + centerY_animal);
        vg_animal.append(0.0f);

        vg_animal.append(radius * cosf(nextTheta) + centerX_animal);
        vg_animal.append(radius * sinf(nextTheta) + centerY_animal);
        vg_animal.append(0.0f);
    }

    vbo_animal.create();
    vbo_animal.bind();
    vbo_animal.allocate(vg_animal.constData(), vg_animal.count() * int(sizeof(GLfloat)));

}

void GLArea::tearGLObjects()
{
    vbo_grid.destroy();
    vbo_plant.destroy();
    vbo_animal.destroy();
}

void GLArea::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    windowRatio = float(w) / h;
}

void GLArea::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Matrice de projection
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, windowRatio, 1.0f, 1000.0f);

    // Matrice de vue (caméra)
    QMatrix4x4 viewMatrix;
    viewMatrix.translate(xPos, yPos, zPos);
    viewMatrix.rotate(xRot, 1, 0, 0);
    viewMatrix.rotate(yRot, 0, 1, 0);
    viewMatrix.rotate(zRot, 0, 0, 1);

    //Show grid
    vbo_grid.bind();
    program_grid->bind();
    program_grid->setUniformValue("projectionMatrix", projectionMatrix);
    program_grid->setUniformValue("viewMatrix", viewMatrix);
    program_grid->setAttributeBuffer("in_position", GL_FLOAT,0 ,3, 3*sizeof(GLfloat));
    program_grid->enableAttributeArray("in_position");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_BLEND);
    ground.show(program_grid);
    glDisable(GL_BLEND);

    program_grid->disableAttributeArray("in_position");
    program_grid->release();
    vbo_grid.release();

    //Show plant
    vbo_plant.bind();
    program_plant->bind();
    program_plant->setUniformValue("projectionMatrix", projectionMatrix);
    program_plant->setUniformValue("viewMatrix", viewMatrix);
    program_plant->setAttributeBuffer("in_position", GL_FLOAT,0 ,3, 3*sizeof(GLfloat));
    program_plant->enableAttributeArray("in_position");

    auto allGrids = ground.getAllGrids();

    for (auto& grid : allGrids) {
        GridStatus status = grid.get().getStatus();
        if (status.hasPlant) {
            grid.get().getPlant()->show(program_plant);
        }
    }
    program_plant->disableAttributeArray("in_position");
    program_plant->release();
    vbo_plant.release();

    //Show animals
    vbo_animal.bind();
    program_animal->bind();
    program_animal->setUniformValue("projectionMatrix", projectionMatrix);
    program_animal->setUniformValue("viewMatrix", viewMatrix);
    program_animal->setAttributeBuffer("in_position", GL_FLOAT,0 ,3, 3*sizeof(GLfloat));
    program_animal->enableAttributeArray("in_position");

    glEnable(GL_BLEND);
    for (auto& grid : allGrids) {
        GridStatus status = grid.get().getStatus();
        if (status.hasAnimal) {
            grid.get().getAnimal()->show(program_animal, 20);
        }
    }
    glDisable(GL_BLEND);

    program_animal->disableAttributeArray("in_position");
    program_animal->release();
    vbo_animal.release();
}

void GLArea::keyPressEvent(QKeyEvent *ev)
{
    float da = 1.0f;

    switch(ev->key()) {
        case Qt::Key_A :
            xRot -= da;
            update();
            break;

        case Qt::Key_Q :
            xRot += da;
            update();
            break;

        case Qt::Key_Z :
            yRot -= da;
            update();
            break;

        case Qt::Key_S :
            yRot += da;
            update();
            break;

        case Qt::Key_E :
            zRot -= da;
            update();
            break;

        case Qt::Key_D :
            zRot += da;
            update();
            break;

        case Qt::Key_Space:
            advanceTurn();
            break;
    }
}

void GLArea::keyReleaseEvent(QKeyEvent *ev)
{
    //qDebug() << __FUNCTION__ << ev->text();
    std::cout<<"round"<<round++<<std::endl;

}

void GLArea::mousePressEvent(QMouseEvent *ev)
{
    lastPos = ev->pos();
}

void GLArea::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->position().x() << ev->position().y() << ev->button();
}

void GLArea::mouseMoveEvent(QMouseEvent *ev)
{
    int dx = ev->position().x() - lastPos.x();
    int dy = ev->position().y() - lastPos.y();

    if (ev->buttons() & Qt::LeftButton) {
        xRot += dy;
        yRot += dx;
        update();
    } else if (ev->buttons() & Qt::RightButton) {
        xPos += dx/10.0f;
        yPos -= dy/10.0f;
        update();
    } else if (ev->buttons() & Qt::MiddleButton) {
        xPos += dx/10.0f;
        zPos += dy;
        update();
    }

    lastPos = ev->pos();
}

void GLArea::onTimeout()
{
    static qint64 old_chrono = elapsedTimer.elapsed(); // static : initialisation la première fois et conserve la dernière valeur
    qint64 chrono = elapsedTimer.elapsed();
    dt = (chrono - old_chrono) / 1000.0f;
    old_chrono = chrono;

    update();
}

void GLArea::advanceTurn() {

    auto allGrids = ground.getAllGrids();
    for (auto& grid : allGrids) {
        grid.get().updatePlant();
    }
    ground.reproducePlants();

    ground.moveAnimals();
    ground.reproduceAnimals();
    update();
}
