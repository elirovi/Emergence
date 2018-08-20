#include "ColorPlugin.h"

void ColorPlugin::updateUI(QMenu* insertMenu, NodeBox*box, Workspace*ws) const{
	QMenu* colorMenu = new QMenu("Color",insertMenu);
	insertMenu->addAction(colorMenu->menuAction());
	QAction* colorAction= new QAction(QIcon(":/color.png"),"Color");
	QAction* rgbAction= new QAction("RGB");
	QAction* hsvAction= new QAction("HSV");

	connect(colorAction,&QAction::triggered,ws,[=]{ws->addNode(Node::nodeMalloc("color"));});
	connect(rgbAction,&QAction::triggered,ws,[=]{ws->addNode(Node::nodeMalloc("rgb"));});
	connect(hsvAction,&QAction::triggered,ws,[=]{ws->addNode(Node::nodeMalloc("hsv"));});

	colorMenu->addAction(colorAction);
	colorMenu->addAction(rgbAction);
	colorMenu->addAction(hsvAction);

	box->addTool("color","Color",QIcon(":/color.png"),"Color");
	box->addTool("rgb","RGB","Color");
	box->addTool("hsv","HSV","Color");
}

void ColorPlugin::addNodes()const{
	Node::makeNodeMethods["color"] = &ColorNode::makeNode;
	Node::makeNodeMethods["rgb"] = &RGBNode::makeNode;
	Node::makeNodeMethods["hsv"] = &HSVNode::makeNode;
}
