#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <vertex.h>
#include <halfedge.h>
#include <face.h>
#include <QMainWindow>


namespace Ui {


class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void divide();

private slots:
    void on_actionQuit_triggered();

    void add_vertex(vertex* v);
    void add_halfedge(halfedge* h);
    void add_face(face* f);

    void select_vertex(vertex* v);
    void select_halfedge(halfedge* h);
    void select_face(face* f);

    void addVButtonClick();
    void triButtonClick();


    void updateVX();
    void updateVY();
    void updateVZ();

    void updateFaceR();
    void updateFaceG();
    void updateFaceB();

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
