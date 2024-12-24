#include "mainwindow.h"
#include <ui_mainwindow.h>
#include<mygl.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();

    connect(ui->vertsListWidget, SIGNAL(itemClicked(QListWidgetItem*)), ui->mygl, SLOT(selected_vertex(QListWidgetItem*)));
    connect(ui->halfEdgesListWidget, SIGNAL(itemClicked(QListWidgetItem*)), ui->mygl, SLOT(selected_halfedge(QListWidgetItem*)));
    connect(ui->facesListWidget, SIGNAL(itemClicked(QListWidgetItem*)), ui->mygl, SLOT(selected_face(QListWidgetItem*)));
    connect(ui->mygl, SIGNAL(select_vertex(vertex*)), this, SLOT(select_vertex(vertex*)));
    connect(ui->mygl, SIGNAL(select_face(face*)), this, SLOT(select_face(face*)));
    connect(ui->mygl, SIGNAL(select_halfedge(halfedge*)), this, SLOT(select_halfedge(halfedge*)));
    connect(ui->mygl, SIGNAL(s_vertex(vertex*)), this, SLOT(add_vertex(vertex*)));
    connect(ui->mygl, SIGNAL(s_edge(halfedge*)), this, SLOT(add_halfedge(halfedge*)));
    connect(ui->mygl, SIGNAL(s_face(face*)), this, SLOT(add_face(face*)));
    connect(ui->pushButton, SIGNAL(clicked()),
            ui->mygl, SLOT(load_obj()));

    connect(ui->addVButton, SIGNAL(clicked()), ui->mygl, SLOT(addVSlot()));
    connect(ui->triButton, SIGNAL(clicked()), ui->mygl, SLOT(triangSlot()));
    connect(ui->div, SIGNAL(clicked()), ui->mygl, SLOT(subdivide()));

    connect(ui->vertPosXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateVX()));
    connect(ui->vertPosYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateVY()));
    connect(ui->vertPosZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateVZ()));

    connect(ui->faceRedSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateFaceR()));
    connect(ui->faceGreenSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateFaceG()));
    connect(ui->faceBlueSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateFaceB()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}



void MainWindow::select_vertex(vertex* v) {
    if (v) {
        ui->vertsListWidget->setCurrentItem(v);
        disconnect(ui->vertPosXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateVX()));
        disconnect(ui->vertPosYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateVY()));
        disconnect(ui->vertPosZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateVZ()));
        ui->vertPosXSpinBox->setValue(v->vertex_position[0]);
        ui->vertPosYSpinBox->setValue(v->vertex_position[1]);
        ui->vertPosZSpinBox->setValue(v->vertex_position[2]);
        connect(ui->vertPosXSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateVX()));
        connect(ui->vertPosYSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateVY()));
        connect(ui->vertPosZSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateVZ()));
    }else {
        ui->vertsListWidget->clearSelection();
    }
}

void MainWindow::select_halfedge(halfedge* he) {
    if (he) {
        ui->halfEdgesListWidget->setCurrentItem(he);
    } else {
        ui->halfEdgesListWidget->clearSelection();
    }
}

void MainWindow::select_face(face* f) {
    if (f) {
        ui->facesListWidget->setCurrentItem(f);
        disconnect(ui->faceRedSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateFaceR()));
        disconnect(ui->faceGreenSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateFaceG()));
        disconnect(ui->faceBlueSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateFaceB()));
        ui->faceRedSpinBox->setValue(f->face_color[0]);
        ui->faceGreenSpinBox->setValue(f->face_color[1]);
        ui->faceBlueSpinBox->setValue(f->face_color[2]);
        connect(ui->faceRedSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateFaceR()));
        connect(ui->faceGreenSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateFaceG()));
        connect(ui->faceBlueSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateFaceB()));
    } else {
        ui->facesListWidget->clearSelection();
    }
}

void MainWindow::add_vertex(vertex* v) {
    ui->vertsListWidget->addItem(v);
}

void MainWindow::add_halfedge(halfedge* h) {
    ui->halfEdgesListWidget->addItem(h);
}

void MainWindow::add_face(face* f) {
    ui->facesListWidget->addItem(f);
}

void MainWindow::addVButtonClick() {
    ui->mygl->addVSlot();
}

void MainWindow::triButtonClick() {
    ui->mygl->triangSlot();
}

void MainWindow::updateVX() {
    float f = ui->vertPosXSpinBox->value();
    ui->mygl->updateVertX(f);
}

void MainWindow::updateVY() {
    float f = ui->vertPosYSpinBox->value();
    ui->mygl->updateVertY(f);
}

void MainWindow::updateVZ() {
    float f = ui->vertPosZSpinBox->value();
    ui->mygl->updateVertZ(f);
}

void MainWindow::updateFaceR() {
    float r = ui->faceRedSpinBox->value();
    ui->mygl->updateFaceRed(r);
}

void MainWindow::updateFaceG() {
    float g = ui->faceGreenSpinBox->value();
    ui->mygl->updateFaceGreen(g);
}

void MainWindow::updateFaceB() {
    float b = ui->faceBlueSpinBox->value();
    ui->mygl->updateFaceBlue(b);
}
