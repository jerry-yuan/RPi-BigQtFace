#include "LCPanel.h"
#include "ui_LCPanel.h"

LCPanel::LCPanel(QWidget *parent) :
    FuncPanel(parent),ui(new Ui::LCPanel)
{

}

LCPanel::~LCPanel(){
    delete ui;
}
void LCPanel::active(){
    ui->setupUi(this);
}

void LCPanel::deactive(){

}
