#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxOpenCv.h"

#define CAM_WIDTH 320
#define CAM_HEIGHT 240
#define nFotos 3
class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
        void exit(); 
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    
        // GUI
        void generaGui();
        void generaGuiRt();
        void generaAtajos();
        void posicionaGui();
        void cambiaEstilo(string estilo);
        void guiEvent(ofxUIEventArgs &e);

        ofxUICanvas *gui;        
        ofxUICanvas *guiRt;
        ofxUICanvas *atajos;
        ofxUIMovingGraph *mg; 
        float *buffer, initialRtGuiPosX; 
        ofImage *imagen; 
    
    
        // Logica
        void initVideo();
        void initImagen();
        void procesaVideo();
        void procesaImagen();
		int* procesaHoughesP(ofxCvColorImage img,int posX,int posY);
        ofxCvColorImage img[nFotos];
        ofxCvGrayscaleImage imgOutput;
        ofVideoGrabber vidGrabber;
        ofImage Imagen;
        float g_iAnchoLineas,g_iNLineas, width, height,g_iGris,g_iThreshold,max_imageW,
              g_HoughlineLength, g_HoughPixelGap, g_HoughThreshold;
        int    numLineas, tipoFiltro,g_iFotos,margin;
        string modoOperacion;
};
