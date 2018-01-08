#include "include/RenderGate.h"

RenderGate::RenderGate():Gate(50, 50, Qt::white,1,true){}
data_t RenderGate::eval()const{
	return iGates[0]->eval();
}

void RenderGate::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget){
	pen.setColor(Qt::darkGreen);
	Gate::paint(painter, option, widget);
	painter->drawText(boundingRect().center()-QPoint(22,-2),"Output");
}

void RenderGate::removeGate(){
	Gate::removeGate();
	emit deleted();
}
