#include "lut_dialog.h"

#include <QColorDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QGraphicsSceneMouseEvent>

LutDialog::LutDialog(QWidget *parent) :
	QDialog(parent){
	QVBoxLayout* vbl= new QVBoxLayout;
	QColorDialog* colorDialog= new QColorDialog;
	colorDialog->setWindowFlag(Qt::Widget);
	colorDialog->setOptions(QColorDialog::DontUseNativeDialog|QColorDialog::NoButtons);
	QDialogButtonBox* buttons= new QDialogButtonBox(this);
	buttons->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

	buttons->setWindowFlag(Qt::Widget);
	vbl->setMargin(2);
	vbl->addWidget(colorDialog);
	view= new LutView(this);
	vbl->addWidget(view);
	vbl->addWidget(buttons);
	setLayout(vbl);

	connect(buttons,SIGNAL(accepted()),this,SLOT(accept()));
	connect(buttons,SIGNAL(rejected()),this,SLOT(reject()));
	connect(colorDialog,SIGNAL(accepted()),this,SLOT(accept()));
	connect(colorDialog,SIGNAL(rejected()),this,SLOT(reject()));
	setFixedSize(minimumSize());
}
Gradient* LutDialog::getGradient(){
	LutDialog dial;
	if(dial.exec())
		return new Gradient(dial.view->grd);
	return nullptr;
}

LutDialog::~LutDialog(){
	delete view;
}

LutCursor* LutView::curs;
LutView::LutView(QWidget* parent):QGraphicsView(parent){
	setScene(_scene=new QGraphicsScene);
	setAcceptDrops(true);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	background= new LutBackground(&grd);
	_scene->addItem(background);
}
LutView::~LutView(){
	delete background;
	delete _scene;
}
void LutView::dragEnterEvent(QDragEnterEvent *event){
	_scene->setSceneRect(rect());
	LutCursor::setDim(width(),height());
	background->width=width();
	background->height=height();
	if(event->mimeData()->hasColor()){
		event->setAccepted(true);
		curs= new LutCursor(event->mimeData()->colorData().value<QColor>());
		curs->c=grd.add(event->mimeData()->colorData().value<QColor>().rgb(),event->posF().x()/width());
		curs->setPos(event->pos().x(),height()/2);
		_scene->addItem(curs);
	}
	background->update();
}
void LutView::dragMoveEvent(QDragMoveEvent *event){
	curs->setPos(event->pos().x(),height()/2);
	background->update();
}
void LutView::dragLeaveEvent(QDragLeaveEvent *){
	_scene->removeItem(curs);
	grd.remove(curs->c);
	delete curs;
	background->update();
}

int LutCursor::height;
int LutCursor::width;
LutCursor::LutCursor(const QColor& color):color(color){
	setFlag(ItemSendsGeometryChanges);
}
void LutCursor::setDim(int w, int h){
	width=w;
	height=h;
}
QRectF LutCursor::boundingRect()const{
	return QRect(-cursorRadius, -height/2, 2*cursorRadius, height);
}
void LutCursor::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *){
	p->drawLine(0,-height/2,0,height/2);
	p->setBrush(color);
	p->drawEllipse({0,0},cursorRadius,cursorRadius);
}

QVariant LutCursor::itemChange(GraphicsItemChange change, const QVariant &v){
	if(change==ItemPositionChange&&x()){
		QPointF p= v.toPointF();
		p.setY(height/2);
		c->updateAlpha(x()/width);
		return p;
	}
	return QGraphicsItem::itemChange(change, v);
}

void LutCursor::mouseReleaseEvent(QGraphicsSceneMouseEvent*){
	setCursor(Qt::ArrowCursor);
	if(scenePos().x()<0 || scenePos().x()>scene()->width()){
		scene()->removeItem(this);
		c->gr->remove(c);
		delete this;
	}
}
void LutCursor::mouseMoveEvent(QGraphicsSceneMouseEvent*event){
	setCursor(Qt::SizeHorCursor);
	setPos(event->scenePos());
	QGraphicsItem::mouseMoveEvent(event);
}

LutBackground::LutBackground(Gradient *g):grd(g){
	setZValue(INT_MIN);
}
void LutBackground::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *){
	if(!grd->empty())
	for(int x=0;x<width;x++){
		p->setPen(QColor::fromRgb((*grd)[(double)x/width]));
		p->drawLine(x,0,x,height);
	}
	scene()->update();
}
QRectF LutBackground::boundingRect()const{
	return QRectF(0,0,width,height);
}
