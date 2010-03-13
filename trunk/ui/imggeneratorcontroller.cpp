#include "imggeneratorcontroller.h"
#include "ui_imggeneratorcontroller.h"

ImgGeneratorController::ImgGeneratorController(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImgGeneratorController)
{
    ui->setupUi(this);
}

ImgGeneratorController::~ImgGeneratorController()
{
    delete ui;
}

void ImgGeneratorController::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
