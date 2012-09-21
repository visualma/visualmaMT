#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofEnableSmoothing(); 
    
  //valores iniciales
    g_iFotos = 0;
	g_iNLineas = 20;
	g_iGris = 255;
	g_iThreshold = 30;
    tipoFiltro = 1;
    modoOperacion = 1;
	g_iAnchoLineas = height/g_iNLineas;
    g_HoughlineLength = 0.5;
    g_HoughThreshold = 5;
    g_HoughPixelGap = 2;
    width = ofGetViewportWidth();
	height = ofGetViewportHeight();
    margin = 256;
    max_imageW = ofGetViewportWidth()-500;
    
    modoOperacion = "Image";
    initImagen();
    
      
  //renderGui
    generaGui();
    generaGuiRt();
    generaAtajos();
    
    cambiaEstilo("dark");
    
}

void testApp::initImagen() {
    Imagen.loadImage("1.jpg");
    imgOutput.allocate(Imagen.width,Imagen.height);
}
void testApp::initVideo() {
    for(int i=0;i<nFotos;i++)
    {
        img[i].allocate(CAM_WIDTH*2,CAM_HEIGHT*2);
        imgOutput.allocate(img[i].width,img[i].height);
    }
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(CAM_WIDTH,CAM_HEIGHT);
	
}

//--------------------------------------------------------------
void testApp::update() {
    mg->addPoint(ofGetFrameRate());
    
    if(modoOperacion == "Live WebCam") {
		if(vidGrabber.isInitialized())
			vidGrabber.grabFrame();
    } else if(modoOperacion == "Image") {
        //imgUpdate
    } else if(modoOperacion == "3D Model") {
        //3dUpdate
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    if(modoOperacion == "Live WebCam") {
        procesaVideo();
    } else if(modoOperacion == "Image") {
        procesaImagen();
    } else if(modoOperacion == "3D Model") {
        //procesa3d();
    }
}

void testApp::procesaImagen() {
    float newHeight = Imagen.getHeight()*(max_imageW/Imagen.getWidth());
    
	img[0].setFromPixels(Imagen.getPixels(),Imagen.width,Imagen.height);
	imgOutput = img[0];
	imgOutput.invert();
	imgOutput.threshold(g_iThreshold);
	imgOutput.invert();
	imgOutput.resize(max_imageW, newHeight);
    Imagen.resize(max_imageW, newHeight);
    ofSetHexColor(0xffffff);
	Imagen.draw(margin,0);
	//imgOutput.draw(imgOutput.width+margin,0);
    int* a = procesaHoughesP(img[0],margin,Imagen.height);
}

void testApp::procesaVideo() {

	img[0].setFromPixels(vidGrabber.getPixels(),vidGrabber.width,vidGrabber.height);
	//draw(img[0].width+margin,0);
	ofSetColor(255);
	vidGrabber.draw(margin,0);
	int* lineas = procesaHoughesP(img[0],margin,vidGrabber.height);
     for(int i=0;i<g_iFotos;i++)
         img[i].draw(i*CAM_WIDTH,CAM_HEIGHT);
     for(int i=0;i<g_iNLineas;i++) {
         if(i%2==0)
             ofRect(width/2,i*g_iAnchoLineas,0,width/2,g_iAnchoLineas);
     }
}


int* testApp::procesaHoughesP(ofxCvColorImage img,int posX,int posY)
{
    float newHeight = img.getHeight()*(max_imageW/img.getWidth());
	ofxCvGrayscaleImage imgOut;
	imgOut = img;
	imgOut.invert();
	imgOut.threshold(g_iThreshold);
	imgOut.invert();
	if(imgOut.width>max_imageW)
		imgOut.resize(max_imageW, newHeight);
	//IplImage* imgData = imgOut.getCvImage();
	IplImage* imgCanny = cvCreateImage(cvSize(imgOut.width,imgOut.height),8,1);
	cvCanny(imgOut.getCvImage(),imgCanny,0,50,5);
	int maxLinesNumber = 1000;
	int *lines = new int[maxLinesNumber*4];
	int threshold = g_HoughThreshold >= 1 ? g_HoughThreshold : 1;
	int lineLenght = g_HoughlineLength;
	int pixelGap = g_HoughPixelGap;
	int linesNumber = cvHoughLinesP(imgCanny, 1, 0.01745, threshold,lineLenght , pixelGap, lines, maxLinesNumber);
	ofPoint P1, P2;
	P1.y = 0;
	//posY+=imgOut.height;
	imgOut.draw(posX,posY);
	ofSetColor(255,0,0);
    ofSetLineWidth(3);
    
    //dibujar en original
    for(int l=0;l<linesNumber;l++) 
	{
		P1.x = (int)lines[ 4*l]+posX;
		P1.y = (int)lines[ 4*l+1];
		P2.x = (int)lines[ 4*l+2]+posX;
		P2.y = (int)lines[ 4*l+3];
		ofLine(P1,P2);
	}
    //dibujar en imgOutput
	for(int l=0;l<linesNumber;l++) 
	{
		P1.x = (int)lines[ 4*l]+posX;
		P1.y = (int)lines[ 4*l+1]+posY;
		P2.x = (int)lines[ 4*l+2]+posX;
		P2.y = (int)lines[ 4*l+3]+posY;
		ofLine(P1,P2);
	}
	return lines;
}

//-------------------------------------------------   RENDER GUI

//------------------------------- Sidebar izquierda
void testApp::generaGui(){
    float dim = 16; 
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = 255-xInit; 
	
    gui = new ofxUIScrollableCanvas(0, 0, length+xInit, ofGetHeight()); 
    
    gui->addWidgetDown(new ofxUIMultiImageButton(128/2, 129/2, false, "GUI/images/icoVisualma128.png", "Visualma",OFX_UI_FONT_LARGE));
    //gui->addWidgetRight(new ofxUILabel(100,0,"Visualma", OFX_UI_FONT_LARGE),OFX_UI_ALIGN_RIGHT); 
    gui->addSpacer(length-xInit, 1);
    
    vector<string> names;
	names.push_back("HoughLines");
	names.push_back("Algoritmo 2");
	names.push_back("Alg3");	
	gui->addRadio("Filtro", names, OFX_UI_ORIENTATION_VERTICAL, dim, dim, 0, 32); 	
    gui->addSpacer(length-xInit, 2);
    
    gui->addWidgetDown(new ofxUILabel("Lineas proyectadas", OFX_UI_FONT_SMALL)); 
	gui->addSlider("numLineas", 0, 1000, &g_iNLineas, length-xInit,dim);
    gui->addSlider("anchoLineas", 0, 200, &g_iAnchoLineas, length-xInit,dim, OFX_UI_FONT_SMALL);

    gui->addSpacer(length-xInit, 2);	
    gui->addWidgetDown(new ofxUILabel("Hough", OFX_UI_FONT_SMALL));
    gui->addSlider("LineLength", 0.2, 200, &g_HoughlineLength, length-xInit,dim, OFX_UI_FONT_SMALL);
    gui->addSlider("Threshold", 0.1, 200, &g_HoughThreshold, length-xInit,dim, OFX_UI_FONT_SMALL);
    gui->addSlider("PixelGap", 0.1, 200, &g_HoughPixelGap, length-xInit,dim, OFX_UI_FONT_SMALL);
    
    gui->addSpacer(length-xInit, 2);
	gui->add2DPad("PAD", ofPoint(0,length-xInit), ofPoint(0,120), ofPoint(g_HoughThreshold,g_iThreshold), length-xInit,120);
    
    gui->addSpacer(length-xInit, 2);			
    gui->addWidgetDown(new ofxUIRotarySlider(dim*4, 0, 100, 50, "Angulo")); 			
    gui->addSpacer(length-xInit, 2);
    
    vector<string> items; 
    items.push_back("Live WebCam"); 
    items.push_back("Image"); 
    items.push_back("3D Model"); 
    gui->addDropDownList("Modo de operacion", items, 200);
  
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
}

//--------------------------------  Sidebar derecha
void testApp::generaGuiRt(){
    float dim = 16; 
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = 255-xInit; 
    initialRtGuiPosX = ofGetWidth()-length+xInit;
    guiRt = new ofxUIScrollableCanvas(initialRtGuiPosX, 0, length+xInit, ofGetHeight());     
    guiRt->loadSettings("GUI/guiSettings.xml"); 
    
    guiRt->addWidgetDown(new ofxUILabel("Herramientas", OFX_UI_FONT_SMALL)); 	
    guiRt->addWidgetRight(new ofxUIImageToggle(dim, dim, true, "GUI/images/Preview.png","FULLSCREEN")); 
    guiRt->addSpacer(length-xInit, 2);
        
    guiRt->addWidgetDown(new ofxUILabel("INput", OFX_UI_FONT_SMALL)); 			
    guiRt->addWidgetDown(new ofxUIRotarySlider((length-xInit)*.25, 0, 250, &g_iGris, "Luminosidad")); 			
    guiRt->addWidgetRight(new ofxUIRotarySlider((length-xInit)*.25, 0, 500, &g_iThreshold, "Threshold"));  
    guiRt->addSpacer(length-xInit, 1);                                
    
    guiRt->addWidgetDown(new ofxUIToggleMatrix(dim*2, dim*2, 4, 4, "Capas")); 
    
    guiRt->addFPSSlider("FPS", length-xInit, dim);
    vector<float> buffer; 
    for(int i = 0; i < margin; i++)
        buffer.push_back(0.0);				    
    mg = (ofxUIMovingGraph *) guiRt->addWidgetDown(new ofxUIMovingGraph(length-xInit, 120, buffer, margin, 0, 400, "MOVING GRAPH"));
	guiRt->addSpacer(length-xInit, 0);	
    
    
    guiRt->addWidgetDown(new ofxUIMultiImageButton(dim, dim, true, "GUI/images/toggle.png", "light"));
    guiRt->addWidgetRight(new ofxUIMultiImageToggle(dim, dim, false, "GUI/images/toggleB.png", "dark"));
    
    guiRt->addToggle("ShortCuts", false, dim, dim);
    ofAddListener(guiRt->newGUIEvent,this,&testApp::guiEvent);
    
}

//-------------------------- Lista de ShortCuts
void testApp::generaAtajos(){
    float dim = 16; 
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = 255-xInit; 
    
    atajos = new ofxUIScrollableCanvas(ofGetWidth()-(length)+(xInit), ofGetHeight()-300, length+xInit, 300);  
    
    atajos->addWidgetDown(new ofxUILabel("Atajos", OFX_UI_FONT_MEDIUM)); 	
    atajos->addWidgetDown(new ofxUILabel("[f] FullScreen", OFX_UI_FONT_SMALL)); 	
    atajos->addWidgetDown(new ofxUILabel("[h] Show/hide Tools", OFX_UI_FONT_SMALL));
    //atajos->addWidgetDown(new ofxUILabel("[f] ", OFX_UI_FONT_SMALL)); 	
    atajos->toggleVisible(); 
}

void testApp::posicionaGui() {
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
    float length = 255-xInit;
    float offsetRtGui;
    gui->setRectParent(new ofxUIRectangle(0, 0, length+xInit, ofGetHeight()));
    
    offsetRtGui = initialRtGuiPosX-ofGetWidth();
    guiRt->setRectParent(new ofxUIRectangle(-offsetRtGui-length, 0, (length+xInit), ofGetHeight()));
    atajos->setRectParent(new ofxUIRectangle(-offsetRtGui-length, 0, (length+xInit), 300));
    
    //max_imageW = ofGetViewportWidth()-500;
    //cout << "posicionaGui - windowWidth:" << ofGetWidth() << "offset" << offsetRtGui << endl;
}

//------------------------------------------------------  UTIL

void testApp::cambiaEstilo(string estilo) {
    if (estilo == "dark"){
            ofBackground(51, 51, 51);
            gui->setTheme(OFX_UI_THEME_MACOSX);
            guiRt->setTheme(OFX_UI_THEME_MACOSX);
    } else
    if (estilo == "light"){
            ofBackground(240, 240, 240); 
            gui->setTheme(OFX_UI_THEME_MINBLACK);
            guiRt->setTheme(OFX_UI_THEME_MINBLACK);
    }
}

//------------------------------------------------  Eventos GUI

void testApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
	cout << "got event from: " << name << endl; 
    
    
    if(name == "FULLSCREEN"){
        ofxUIImageToggle *toggle = (ofxUIImageToggle *) e.widget; 
        ofSetFullscreen(toggle->getValue()); 
    } else if(name == "numLineas") {
        ofxUISlider *slider = (ofxUISlider *) e.widget;    
        //modificar numero de lineas
        g_iNLineas = slider->getScaledValue();
    } else if(name == "Tipo de filtro") {

    } else if(name == "ShortCuts") {
         atajos->toggleVisible(); 
    } else if(name == "Modo de operacion") {
        ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
        vector<ofxUIWidget *> &selected = ddlist->getSelected(); 
        if(selected.size() > 0){
            modoOperacion = selected[0]->getName();
            cout << "ModoOperacion: " << selected[0]->getName() << endl; 
            if(modoOperacion == "Live WebCam") {
                initVideo();
            } else if(modoOperacion == "Image") {
                initImagen();
            } else if(modoOperacion == "3D Model") {
                //init3d();
            }
        }
    } else if(name == "dark") {
        cambiaEstilo("dark");
    } else  if(name == "light") {
        cambiaEstilo("light");
    }
}


//--------------------------------------------  Eventos globales
void testApp::keyPressed(int key){
    switch (key) {
		case 'f':
			ofToggleFullscreen(); 
			break;
        case 'h':
            gui->toggleVisible(); 
            guiRt->toggleVisible(); 
			break;  
        case OF_KEY_UP:
            g_iNLineas++;
            break;
        case OF_KEY_DOWN:
            if(g_iNLineas>0)
                g_iNLineas--;
            break;
        case OF_KEY_RIGHT:
            g_iGris++;
            break;
        case OF_KEY_LEFT:
            if(g_iGris>0)
                g_iGris--;
            break;
    }
}
//--------------------------------------------------------------
void testApp::keyReleased(int key){
}
//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
}
//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}
//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}
//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}
//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    posicionaGui();
}
//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
}
//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
}


//--------------------------------------------------------  EXIT
void testApp::exit()
{
    gui->saveSettings("GUI/guiSettings.xml");     
    delete gui; 
    delete guiRt;
}
//--------------------------------------------------------------
