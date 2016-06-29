#include "Ogre\ExampleApplication.h"
#include <string>
#include "Ogre\Ogre.h"
#include "OIS\OIS.h"
#include <time.h>
#include "TextRenderer.cpp"

// Para la animacion de los obstaculos
Ogre::AnimationState* animationObs01;
Ogre::AnimationState* animationObs02;
Ogre::AnimationState* animationObs03;
Ogre::AnimationState* animationObs04;
#define duration 5.0f

// animacion carro-nave
Ogre::SceneNode* nodoAla[2];
Ogre::AnimationState* animationCar[6];
#define durationCar 1.2f
bool enEspacio = false;
int rotRx = 0;
int rotRy = 0;

bool final = false;

//colisiones
Ogre::Vector3 minCar;
Ogre::Vector3 maxCar;

//rocas
Ogre::SceneNode* nodosRocas[16];
Ogre::AnimationState* animationMeteoros[16];
#define durationR 8.0f
#define durationRL 10.0f

//Monedas
int cantMonedas = 20;
Ogre::SceneNode* nodoMonedas[20];
bool atrapada[20];

int puntaje = 0;

class FrameListenerClase : public Ogre::FrameListener { // Hereda de la clase FrameListener de Ogre, escucha algo

private:
    Ogre::Camera* _cam;
    OIS::InputManager* _man;
    OIS::Keyboard* _key; // Teclado
    OIS::Mouse* _mouse; // Mouse

    //Ogre::SceneNode* cameraYawNode;
    //Ogre::SceneNode* cameraNode;
    
    Ogre::SceneNode* _nodoCarro; // Creamos nodo
    Ogre::SceneNode* _nodoRuedas[4];

public:
    // Constructor que le asignamos el nodo que creamos
    FrameListenerClase(Ogre::SceneNode* nodoCarro,
                       Ogre::SceneNode* nodoRuedas[4],
                       Ogre::Camera* cam, 
                       RenderWindow* win) {

        // Configuracion captura teclado y mouse
        // ESTO ES ASI PORQUE SI, NO CAMBIA
        size_t windowHnd = 0;
        std::stringstream windowHndStr;
        win->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;

        OIS::ParamList pl;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

        _nodoCarro = nodoCarro;
        for(int i =0; i < 4; i++){
            _nodoRuedas[i] = nodoRuedas[i];
        }

        // Eventos
        _man = OIS::InputManager::createInputSystem(pl);
        _key = static_cast< OIS::Keyboard*>(_man->createInputObject(OIS::OISKeyboard, false));
        _mouse = static_cast< OIS::Mouse*>(_man->createInputObject(OIS::OISMouse, false));

        _cam = cam;

        //_nodoCarro->addChild(cameraYawNode);
        //cameraYawNode = _nodoCarro->createChildSceneNode();
    }

    ~FrameListenerClase() {
        _man ->destroyInputObject(_key);
        _man->destroyInputObject(_mouse);
        OIS::InputManager::destroyInputSystem(_man);
    }

    //colision monedas
    void colisionMonedas(int i, int f, Ogre::Vector3 posicionCar){
        bool colicion=false;
        //variable para las colisiones
        float minPosCx = posicionCar.x + minCar.x;
        float maxPosCx = posicionCar.x + maxCar.x;
        float minPosCy = posicionCar.y + minCar.y;
        float maxPosCy = posicionCar.y + maxCar.y;
        float minPosCz = posicionCar.z + minCar.z;
        float maxPosCz = posicionCar.z + maxCar.z;

        for(int j=i; j<f; j++){
            Ogre::Vector3 posMoneda = nodoMonedas[j]->getPosition();

            if(!atrapada[j] 
              && maxPosCx >= posMoneda.x && minPosCx <= posMoneda.x
              && maxPosCy >= posMoneda.y && minPosCy <= posMoneda.y
              && maxPosCz >= posMoneda.z && minPosCz <= posMoneda.z){
                colicion = true;
                puntaje += 1;
                nodoMonedas[j]->setVisible(false);
                atrapada[j] = true;
            }
            if(colicion) break;
        }
    }

    float paredIzq(float pend, float x1,float y1,Ogre::Vector3 posicionCar){
        float y2 = posicionCar.z + maxCar.z;
        float x2 = ((y2 - y1 ) / pend ) + x1;
        float minPosCx = posicionCar.x + minCar.x;
        float difx =0.0;

        printf("carro z %f\n", y2);
        printf("punt pared z %f\n", y1);
        printf("carro punta x %f\n", minPosCx);
        printf("carro x %f\n", posicionCar.z);
        printf("pendiete %f\n", pend);
        printf("x final %f\n", x2);

        //if( x2 > 0 ) x2 = - x2;
        if(minPosCx < x2) {
            difx = minPosCx - x2;
            printf("diferencia %f\n", difx);
        }
        //
        return difx;
    }

    bool frameStarted(const Ogre::FrameEvent &evt) {
            
        _key->capture();
        _mouse->capture();

        Ogre::Vector3 posicionCar = _nodoCarro->getPosition();
        Ogre::Vector3 posicionCam = _cam->getPosition();
                
        float movSpeed = 16.0f;
        Ogre::Vector3 tcam(0,0,0);
        Ogre::Vector3 tcar(0,0,0);

        float rotX;
        float rotY;

        if(posicionCar.z > 10330){
            final = true;
        }else if(posicionCar.z > 6515){ //animacion carro
            for(int i =0; i <6 ; i++){
                if(!enEspacio){
                    nodoAla[0]->setVisible(true);
                    nodoAla[1]->setVisible(true);
                    animationCar[i]->setEnabled(true);
                }
                animationCar[i]->addTime(evt.timeSinceLastFrame);
            }
            enEspacio = true;
        }

        if (_key->isKeyDown(OIS::KC_ESCAPE)) return false;
        
        if(final){
            if(_key->isKeyDown(OIS::KC_SPACE)){ 
                _nodoCarro->setPosition(0,0,0);
                _cam->setPosition(0,20,-100);

                nodoAla[0]->setVisible(false);
                nodoAla[1]->setVisible(false);

                for(int i =0; i <6 ; i++){
                    animationCar[i]->setEnabled(false);
                }
                for(int i =0; i <20 ; i++){
                    nodoMonedas[i]->setVisible(true);
                    atrapada[i] = false;
                    puntaje = 0;
                }
                enEspacio = false;
                final = false;
            }
        }else{
            // Si presionamos la tecla w
            if(_key->isKeyDown(OIS::KC_W)){     
                tcam += Ogre::Vector3(0,0,-10);
                tcar += Ogre::Vector3(0,0, 10);
                
                rotRx = (rotRx + 1) % 360;
                for(int i =0; i < 4; i++){
                    _nodoRuedas[i]->pitch(Ogre::Degree(rotRx));
                }
            }
            // Si presionamos la tecla s
            if(_key->isKeyDown(OIS::KC_S)){
            
                if(posicionCar.z > -48 ){
                    tcam += Ogre::Vector3(0,0, 10);
                    tcar += Ogre::Vector3(0,0,-10);
                }

                rotRx = (rotRx - 1) % 360;
                for(int i =0; i < 4; i++){
                    _nodoRuedas[i]->pitch(Ogre::Degree(rotRx));
                }
            }   
            //para acelerar
            if(_key->isKeyDown(OIS::KC_T)){ 
                tcam += Ogre::Vector3(0,0,-100);
                tcar += Ogre::Vector3(0,0,100);
            }  

            // Si presionamos la tecla a
            if(_key->isKeyDown(OIS::KC_A)){ 
                tcam += Ogre::Vector3(-10,0,0); 
                tcar += Ogre::Vector3( 10,0,0);
            }
            // Si presionamos la tecla d
            if(_key->isKeyDown(OIS::KC_D)){ 
                tcam += Ogre::Vector3( 10,0,0);
                tcar += Ogre::Vector3(-10,0,0);
            }           
            //flecha arriba
            if(posicionCar.z > 6530 && _key->isKeyDown(OIS::KC_UP)){
                tcar += Ogre::Vector3(0, 10,0);
                tcam += Ogre::Vector3(0, 10,0);     
            }         
            //flecha abajo
            if(posicionCar.z > 6530 && _key->isKeyDown(OIS::KC_DOWN)){
                tcar += Ogre::Vector3(0,-10,0);
                tcam += Ogre::Vector3(0,-10,0);          
            }

            Ogre::Vector3 newPosCar = tcar*movSpeed*evt.timeSinceLastFrame;
            Ogre::Vector3 newPosCam = tcam*movSpeed*evt.timeSinceLastFrame;

            //desplazamiento en y
            float dify = 0.0;
            if(posicionCar.z > 6530){
                if(posicionCar.y > 100) dify = posicionCar.y - 100;
                else if(posicionCar.y < -30) dify = posicionCar.y + 30;
                
                newPosCar.y -= dify;
                newPosCam.y -= dify;                

                //variable para las colisiones
                float minPosCx = posicionCar.x + minCar.x;
                float maxPosCx = posicionCar.x + maxCar.x;
                float minPosCy = posicionCar.y + minCar.y;
                float maxPosCy = posicionCar.y + maxCar.y;
                float minPosCz = posicionCar.z + minCar.z;
                float maxPosCz = posicionCar.z + maxCar.z;

                bool colicion =false;

                for(int i=0; i<16; i++){
                    Ogre::Vector3 posRoca = nodosRocas[i]->getPosition();
                    
                    if(maxPosCx >= posRoca.x && minPosCx <= posRoca.x
                      && maxPosCy >= posRoca.y && minPosCy <= posRoca.y
                      && maxPosCz >= posRoca.z && minPosCz <= posRoca.z){
                        printf("colision roca\n");
                        colicion = true;
                    }
                    if(colicion) break;
                }
            }
           
            //colisiones
            if(posicionCar.z < 2351 && posicionCar.z > 50){
                colisionMonedas(0,5,posicionCar);
            }else if(posicionCar.z < 4929 && posicionCar.z > 2927){
                colisionMonedas(5,10,posicionCar);
            }else if(posicionCar.z < 5505 && posicionCar.z > 4929){
                colisionMonedas(10,13,posicionCar);
            }else if(posicionCar.z < 6535 && posicionCar.z > 5505){
                colisionMonedas(13,15,posicionCar);
            }else if(posicionCar.z < 10305 && posicionCar.z > 6535){
                colisionMonedas(15,20,posicionCar);
            }            

            //desplazamiento en x
            float difx = 0.0;
            if(posicionCar.z < 417.5 && posicionCar.x < -122){
                difx = 122 + posicionCar.x;
            }else if(posicionCar.z < 417.5 && posicionCar.x > 122){
                difx = posicionCar.x - 122; 
            }else if(posicionCar.z < 2344 && posicionCar.x > 201.9){
                difx = posicionCar.x - 201.9;
            }else if(posicionCar.z < 2344 && posicionCar.x < -201.9){
                difx = 201.9 + posicionCar.x;
            }else if(posicionCar.z > 2344 && posicionCar.z < 2913 && posicionCar.x < -36.75){
                difx = paredIzq(3.276, -211.9,2370.47,posicionCar);
            //}else if(posicionCar.z > 2344 && posicionCar.z < 2913 && posicionCar.x < -36.75){
            //    difx = paredIzq(3.276, -211.9,2370.47,posicionCar);
            }else if(posicionCar.z < 2913){
            }else if(posicionCar.z < 4941.7 && posicionCar.x > 30.1){ //tunel
                difx = posicionCar.x - 30.1;
            }else if(posicionCar.z < 4941.7 && posicionCar.x < -25.05){
                difx = 25.05 + posicionCar.x;
            }else if(posicionCar.z < 10339 && posicionCar.x > 203){
                difx = posicionCar.x - 203;
            }else if(posicionCar.z < 10339 && posicionCar.x < -203){
                difx = 203 + posicionCar.x;
            }
            newPosCam.x += difx;
            newPosCar.x -= difx;

            //desplazamiento en z
            float difz = 0.0;
            if(posicionCar.z <= 429 && posicionCar.z >= 417.5
               && (posicionCar.x > 126 || posicionCar.x < -126)){
                difz = 429 - posicionCar.z;
                newPosCar.z += difz;
                newPosCam.z -= difz;
            }else if(posicionCar.z < 6560 && 
                    (posicionCar.y < 0 || posicionCar.y > 10)){
                difz = 6530 - posicionCar.z;
                newPosCar.z += difz;
                newPosCam.z -= difz;
            }else if(posicionCar.z > 10339){
                difz = posicionCar.z - 10339;
                newPosCar.z -= difz;
                newPosCam.z += difz;
            }

            // Animacion Obstaculos
            animationObs01 -> addTime(evt.timeSinceLastFrame);
            animationObs02 -> addTime(evt.timeSinceLastFrame);
            animationObs03 -> addTime(evt.timeSinceLastFrame);
            animationObs04 -> addTime(evt.timeSinceLastFrame);

            //Animacion rocas
            for(int i=0; i<16 ; i++){
                animationMeteoros[i]->addTime(evt.timeSinceLastFrame);
            }

            //printf("carro z %f\n", posicionCar.z);
            //printf("carro x %f\n", posicionCar.x);
            //printf("carro y %f\n", posicionCar.y);
            //printf("camara y %f\n", posicionCam.y);
            //printf("camara z %f\n", posicionCam.z);
            //printf("camara x %f\n", posicionCam.x);

            _cam->moveRelative(newPosCam);
            _nodoCarro->translate(newPosCar);
        }

        TextRenderer::getSingleton().setText("textoPuntaje", "Monedas: "+std::to_string(puntaje));

        return true;
    }
};

class Example1 : public ExampleApplication{

public:
    Ogre::SceneNode* _nodeChasis01;
    Ogre::SceneNode* nodoRuedas[4];

    Ogre::FrameListener* FrameListener01; // Objeto de FrameListener

    // Constructor
    Example1() {
        FrameListener01 = NULL;
    }

    // Para destruir la variable FrameListener cuando acabe el programa
    ~Example1() {
        if (FrameListener01) {
            delete FrameListener01;
        }
    }

    // Metodo
    void createFrameListener() {
        FrameListener01 = new FrameListenerClase(_nodeChasis01, nodoRuedas, mCamera, mWindow); 
        mRoot->addFrameListener(FrameListener01);
    }

    void createCamera() {
        mCamera = mSceneMgr->createCamera("MyCamera1");
        mCamera->setPosition(0,20,-100);
        mCamera->lookAt(0,20,100);
        mCamera->setNearClipDistance(1);
    }

    void alasMesh(){
        //  Mesh de las laas
        Ogre::ManualObject* manualAlaCara = mSceneMgr->createManualObject("manualAlaCara");
            manualAlaCara->begin("Rocasycolor/Verde", RenderOperation::OT_TRIANGLE_STRIP);

            manualAlaCara->position(2, 0.4, 3);
            manualAlaCara->position(8, 0.4, 1);
            manualAlaCara->position(8, 0.4,-1);
            manualAlaCara->position(2, 0.4,-3);

            for(int i = 0; i < 5; i++){
                manualAlaCara->index(i);
            }
        manualAlaCara->end();
        manualAlaCara->convertToMesh("MeshAlaCara");

        Ogre::ManualObject* manualAlaBorde = mSceneMgr->createManualObject("manualAlaBorde");
            manualAlaBorde->begin("Rocasycolor/Verde", RenderOperation::OT_TRIANGLE_STRIP);

            manualAlaBorde->position( 2, 0.4, 3);
            manualAlaBorde->position( 2,-0.4, 3);
            manualAlaBorde->position( 8, 0.4, 1);
            manualAlaBorde->position( 8,-0.4, 1);
            manualAlaBorde->position( 8, 0.4,-1);
            manualAlaBorde->position( 8,-0.4,-1);
            manualAlaBorde->position( 2, 0.4,-3);
            manualAlaBorde->position( 2,-0.4,-3);

            for(int i = 0; i < 9; i++){
                manualAlaBorde->index(i);
            }
        manualAlaBorde->end();
        manualAlaBorde->convertToMesh("MeshAlaBorde");
    }

    void createScene(){
        // Luces Puntuales
        Ogre::Light* LuzPuntual01 = mSceneMgr->createLight("Luz01");
        LuzPuntual01->setType(Ogre::Light::LT_DIRECTIONAL);
        LuzPuntual01->setDiffuseColour(1.0,1.0,1.0);
        LuzPuntual01->setPosition(Ogre::Vector3(0.0,0.0,-1.0));
        LuzPuntual01->setDirection(Ogre::Vector3( 1, -1, 1 ));

        Ogre::Light* LuzPuntual02 = mSceneMgr->createLight("Luz02");
        LuzPuntual02->setType(Ogre::Light::LT_DIRECTIONAL);
        LuzPuntual02->setDiffuseColour(1.0,1.0,1.0);
        LuzPuntual02->setPosition(Ogre::Vector3(0.0,0.0,-1.0));
        LuzPuntual02->setDirection(Ogre::Vector3( -1, -1, -1 ));

        // Luces del tunel
        Ogre::Light* LucesTunel[10];
        int posicion = 3050;
        for (int i = 0; i < 10; ++i) {
            LucesTunel[i] = mSceneMgr->createLight();
            LucesTunel[i]->setType(Ogre::Light::LT_POINT);
            LucesTunel[i]->setDiffuseColour(50.0,50.0,50.0);
            LucesTunel[i]->setPosition(Ogre::Vector3(0.0,3.5,posicion));
            LucesTunel[i]->setCastShadows(false);
            LucesTunel[i]->setAttenuation(65, 1.0, 0.07, 0.017);

            posicion = posicion + 200;
        }
        
        alasMesh();

        mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));
        mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
        // Cielo estrellado
        mSceneMgr->setSkyBox(true, "AndreaCenteno_Estrellas/SkyBox");

        // Puntaje
        new TextRenderer();
        TextRenderer::getSingleton().addTextBox("textoPuntaje", "Monedas:", 10, 10, 50, 20, Ogre::ColourValue::White);

        /* ___    __ _   _ __   _ __    ___  
          / __|  / _` | | '__| | '__|  / _ \ 
         | (__  | (_| | | |    | |    | (_) |
          \___|  \__,_| |_|    |_|     \___/ 
        */

        //Chasis
        _nodeChasis01 = mSceneMgr->createSceneNode("Chasis01");
        mSceneMgr->getRootSceneNode()->addChild(_nodeChasis01);
        Ogre::Entity* _entChasis01 = mSceneMgr->createEntity("entChasis01", "chasisCarro.mesh");
        _entChasis01->setMaterialName("shCarro01");
        _nodeChasis01->attachObject(_entChasis01);

        Ogre::AxisAlignedBox charAABBCar = _entChasis01->getBoundingBox();//getCarorldBoundingBox();
        minCar = charAABBCar.getMinimum();
        maxCar = charAABBCar.getMaximum();
        Ogre::Vector3 centerCar = charAABBCar.getCenter();
        Ogre::Vector3 sizeCar( fabs( maxCar.x - minCar.x), fabs( maxCar.y - minCar.y), fabs( maxCar.z - minCar.z ) );
        printf ("max (%f, %f, %f)\n", maxCar.x, maxCar.y, maxCar.z);
        printf ("min (%f, %f, %f)\n", minCar.x, minCar.y, minCar.z);
        printf ("center (%f, %f, %f)\n", centerCar.x, centerCar.y, centerCar.z);
        printf ("carro is (%f, %f, %f)\n", sizeCar.x, sizeCar.y, sizeCar.z);

        Ogre::Animation* animationCarR[4]; 
        Ogre::NodeAnimationTrack* trackCarR[4];
        for (int i = 0; i < 4; i++) {
            nodoRuedas[i] = mSceneMgr->createSceneNode("Rueda"+std::to_string(i+1));
            _nodeChasis01->addChild(nodoRuedas[i]);
            Ogre::Entity* _entRueda02 = mSceneMgr->createEntity("entRueda"+std::to_string(i+1), "ruedaDetallada.mesh");
            _entRueda02->setMaterialName("shRueda02");
            nodoRuedas[i]->attachObject(_entRueda02);

            animationCarR[i] = mSceneMgr->createAnimation("animationCarR"+std::to_string(i+1),durationCar);
            animationCarR[i]->setInterpolationMode(Animation::IM_SPLINE);
            trackCarR[i] = animationCarR[i]->createNodeTrack(0,nodoRuedas[i]);
            animationCar[i+2] = mSceneMgr->createAnimationState("animationCarR"+std::to_string(i+1));
            animationCar[i+2]->setEnabled(false);
            animationCar[i+2]->setLoop(false);
        }

        nodoRuedas[0]->translate(-5.77,3.517,9.462);
        Ogre::TransformKeyFrame* keyCarR00;
        keyCarR00 = trackCarR[0] -> createNodeKeyFrame(0.0);
        keyCarR00->setTranslate(Vector3(-5.77,3.517,9.462));
        keyCarR00->setRotation(Quaternion(Degree(0), Vector3::UNIT_Z));
        keyCarR00 = trackCarR[0] -> createNodeKeyFrame(durationCar);
        keyCarR00->setTranslate(Vector3(-5.77,3.517,9.462));
        keyCarR00->setRotation(Quaternion(Degree(90), Vector3::UNIT_Z)); 

        nodoRuedas[1]->translate(8,3.517,9.462);
        Ogre::TransformKeyFrame* keyCarR01;
        keyCarR01 = trackCarR[1] -> createNodeKeyFrame(0.0);
        keyCarR01->setTranslate(Vector3(8,3.517,9.462));
        keyCarR01->setRotation(Quaternion(Degree(0), Vector3::UNIT_Z));
        keyCarR01 = trackCarR[1] -> createNodeKeyFrame(durationCar);
        keyCarR01->setTranslate(Vector3(6.5,3.517,9.462));
        keyCarR01->setRotation(Quaternion(Degree(90), Vector3::UNIT_Z)); 

        nodoRuedas[2]->translate(-5.77,3.517,-9.462);
        Ogre::TransformKeyFrame* keyCarR02;
        keyCarR02 = trackCarR[2] -> createNodeKeyFrame(0.0);
        keyCarR02->setTranslate(Vector3(-5.77,3.517,-9.462));
        keyCarR02->setRotation(Quaternion(Degree(0), Vector3::UNIT_Z));
        keyCarR02 = trackCarR[2] -> createNodeKeyFrame(durationCar);
        keyCarR02->setTranslate(Vector3(-5.77,3.517,-9.462));
        keyCarR02->setRotation(Quaternion(Degree(90), Vector3::UNIT_Z)); 

        nodoRuedas[3]->translate(8,3.517,-9.462);
        Ogre::TransformKeyFrame* keyCarR03;
        keyCarR03 = trackCarR[3] -> createNodeKeyFrame(0.0);
        keyCarR03->setTranslate(Vector3(8,3.517,-9.462));
        keyCarR03->setRotation(Quaternion(Degree(0), Vector3::UNIT_Z));
        keyCarR03 = trackCarR[3] -> createNodeKeyFrame(durationCar);
        keyCarR03->setTranslate(Vector3(6.5,3.517,-9.462));
        keyCarR03->setRotation(Quaternion(Degree(90), Vector3::UNIT_Z)); 

        //ALAS
        Ogre::SceneNode* nodoAlaB[2];
        Ogre::Animation* animationCarrito[2];
        Ogre::NodeAnimationTrack* trackCarrito[2];

        for (int i = 0; i < 2; i++) {
            Ogre::Entity* entAla = mSceneMgr->createEntity("MeshAlaCara");
            nodoAla[i] = mSceneMgr->createSceneNode("NodoAla"+std::to_string(i+1));
            _nodeChasis01->addChild(nodoAla[i]);
            nodoAla[i]->attachObject(entAla);
            nodoAla[i]->setVisible(false);
            //borde
            Ogre::Entity* entAlaB = mSceneMgr->createEntity("MeshAlaBorde");
            nodoAlaB[i] = mSceneMgr->createSceneNode("NodoAlaB"+std::to_string(i+1));
            nodoAla[i]->addChild(nodoAlaB[i]);
            nodoAlaB[i]->attachObject(entAlaB);    

            animationCarrito[i] = mSceneMgr -> createAnimation("animationCarrito"+std::to_string(i+1),durationCar);
            animationCarrito[i] -> setInterpolationMode(Animation::IM_SPLINE);
            trackCarrito[i] = animationCarrito[i]->createNodeTrack(0,nodoAla[i]);
            animationCar[i] = mSceneMgr -> createAnimationState("animationCarrito"+std::to_string(i+1));
            animationCar[i]->setEnabled(false);
            animationCar[i]->setLoop(false);  
        }

        //KeyFrame ala der
        Ogre::TransformKeyFrame* keyCar00;
        keyCar00 = trackCarrito[0] -> createNodeKeyFrame(0.0);
        keyCar00->setTranslate(Vector3(-6,7.0,-4.0));
        keyCar00 = trackCarrito[0] -> createNodeKeyFrame(durationCar);
        keyCar00->setScale(Vector3(2.5,2.0,2.0));
        keyCar00->setTranslate(Vector3(-1.0,7.0,-4.0));
        //KeyFrame ala izq
        Ogre::TransformKeyFrame* keyCar01;
        keyCar01 = trackCarrito[1] -> createNodeKeyFrame(0.0);
        keyCar01->setTranslate(Vector3(6,7.0,-4.0));
        keyCar01->setRotation(Quaternion(Degree(180), Vector3::UNIT_Y)); 
        keyCar01 = trackCarrito[1] -> createNodeKeyFrame(durationCar);
        keyCar01->setTranslate(Vector3(1.0,7.0,-4.0));
        keyCar01->setRotation(Quaternion(Degree(180), Vector3::UNIT_Y)); 
        keyCar01->setScale(Vector3(2.5,2.0,2.0));     

        //Rocas
        for (int i = 0; i < 16; i++) {
            nodosRocas[i] = mSceneMgr->createSceneNode("NodoRoca"+std::to_string(i+1));
            mSceneMgr->getRootSceneNode()->addChild(nodosRocas[i]);
            Ogre::Entity* entRocas;
            switch ((rand() % 45) % 3){
            case '0': 
                entRocas = mSceneMgr->createEntity("EntRoca"+std::to_string(i+1), "roca02.mesh");
            break;
            case '1': 
                entRocas = mSceneMgr->createEntity("EntRoca"+std::to_string(i+1), "roca04.mesh");
            break;
            default:
                entRocas = mSceneMgr->createEntity("EntRoca"+std::to_string(i+1), "roca01.mesh");
            break;
            }   
            entRocas->setMaterialName("Rocasycolor/Rocky1"); 
            nodosRocas[i] ->attachObject(entRocas);  
        }

            nodosRocas[0] ->setPosition(-150,3,8800);
            nodosRocas[0] ->setScale(1.1,1.1,0.5);
            nodosRocas[1] ->setPosition(-179,4,7100);
            nodosRocas[1] ->yaw(Degree(80));
            nodosRocas[2] ->setPosition(190,0,8700);
            nodosRocas[2] ->roll(Degree(90));
            nodosRocas[3] ->setPosition(180,-32,6900);
            nodosRocas[3] ->pitch(Degree(80));
            nodosRocas[3] ->setScale(1.0,0.8,0.5);
            nodosRocas[4] ->setPosition(15,1.5,8290);
            nodosRocas[4] ->pitch(Degree(90));
            nodosRocas[4] ->setScale(0.8,1.3,0.8);
            nodosRocas[5] ->setPosition(-80,-30,8180);
            nodosRocas[5] ->yaw(Degree(90));
            nodosRocas[6] ->setPosition(23,-12,7500);
            nodosRocas[6] ->yaw(Degree(50));
            nodosRocas[7] ->setPosition(200,28,6970);
            nodosRocas[7] ->setScale(1.0,1.3,1.5);
            nodosRocas[8] ->roll(Degree(50));
            nodosRocas[8] ->setPosition(-150,3,6800);
            nodosRocas[8] ->setScale(2,2,2);
            nodosRocas[9] ->setPosition(-80,3,9300);
            nodosRocas[9] ->roll(Degree(50));
            nodosRocas[9] ->setScale(0.8,1.0,1.6);
            nodosRocas[10] ->setPosition(160,20,8090);
            nodosRocas[10] ->pitch(Degree(50));
            nodosRocas[10] ->roll(Degree(50));
            nodosRocas[11] ->setPosition(150,-18,6800);
            nodosRocas[11] ->setScale(1.6,1.0,1.6);
            nodosRocas[12] ->setPosition(-123,10,7400);
            nodosRocas[12] ->pitch(Degree(54));
            nodosRocas[13] ->setPosition(0,-13,9300);
            nodosRocas[13] ->pitch(Degree(50));
            nodosRocas[13] ->setScale(2,1.8,2.2);
            nodosRocas[14] ->setPosition(460,-23,9300);
            nodosRocas[14] ->yaw(Degree(-40));
            nodosRocas[15] ->setPosition(-434,10,8400);
            nodosRocas[15] ->pitch(Degree(54));
            
        Ogre::Animation* animationRocas[16];
        for (int i = 0; i < 14; i++) {

            animationRocas[i] = mSceneMgr -> createAnimation("animationRocas"+std::to_string(i+1),durationR);
            animationRocas[i] -> setInterpolationMode(Animation::IM_SPLINE);
            Ogre::NodeAnimationTrack* trackRocas = animationRocas[i]->createNodeTrack(0,nodosRocas[i]);
            animationMeteoros[i] = mSceneMgr -> createAnimationState("animationRocas"+std::to_string(i+1));
            animationMeteoros[i]->setEnabled(true);
            animationMeteoros[i]->setLoop(true); 

            Ogre::Vector3 posRoca = nodosRocas[i]->getPosition();
            Ogre::Vector3 antPostRoca = nodosRocas[i]->getPosition();
            posRoca.x += 8+ rand() % (18-8);
            posRoca.y += 10+ rand() % (20-10);
            posRoca.z += 15+ rand() % (25-15);
            const Ogre::Quaternion rotRoca = nodosRocas[i]->getOrientation();

            Ogre::TransformKeyFrame* keyRoc;
            keyRoc = trackRocas->createNodeKeyFrame(0.0);
            keyRoc->setRotation(rotRoca);
            keyRoc->setTranslate(antPostRoca);
            keyRoc = trackRocas->createNodeKeyFrame(durationR/2);
            keyRoc->setRotation(rotRoca);
            keyRoc->setTranslate(posRoca);
            keyRoc = trackRocas->createNodeKeyFrame(durationR);
            keyRoc->setRotation(rotRoca);
            keyRoc->setTranslate(antPostRoca);
        }

        Ogre::NodeAnimationTrack* trackRocasL[2];
        Ogre::Vector3 antPostRocaL[2];
        Ogre::Quaternion rotRocaL[2];
        for (int i = 14; i < 16; i++) {

            animationRocas[i] = mSceneMgr -> createAnimation("animationRocas"+std::to_string(i+1),durationRL);
            animationRocas[i] -> setInterpolationMode(Animation::IM_SPLINE);
            trackRocasL[i-14] = animationRocas[i]->createNodeTrack(0,nodosRocas[i]);
            animationMeteoros[i] = mSceneMgr -> createAnimationState("animationRocas"+std::to_string(i+1));
            animationMeteoros[i]->setEnabled(true);
            animationMeteoros[i]->setLoop(true); 

            antPostRocaL[i-14] = nodosRocas[i]->getPosition();
            rotRocaL[i-14] = nodosRocas[i]->getOrientation();
        }

        Ogre::TransformKeyFrame* keyRoc01;
        keyRoc01 = trackRocasL[0]->createNodeKeyFrame(0.0);
        keyRoc01->setRotation(rotRocaL[0]);
        keyRoc01->setTranslate(antPostRocaL[0]);
        keyRoc01 = trackRocasL[0]->createNodeKeyFrame(durationRL);
        keyRoc01->setRotation(Quaternion(Degree(180), Vector3::UNIT_Y));
        keyRoc01->setTranslate(Vector3(-100,-20,6515));

        Ogre::TransformKeyFrame* keyRoc02;
        keyRoc02 = trackRocasL[1]->createNodeKeyFrame(0.0);
        keyRoc02->setRotation(rotRocaL[1]);
        keyRoc02->setTranslate(antPostRocaL[1]);
        keyRoc02 = trackRocasL[1]->createNodeKeyFrame(durationRL);
        keyRoc02->setRotation(Quaternion(Degree(180), Vector3::UNIT_Y));
        keyRoc02->setTranslate(Vector3(100,-20,6515));

        /*    _         _           
      _ __   (_)  ___  | |_    __ _ 
     | '_ \  | | / __| | __|  / _` |
     | |_) | | | \__ \ | |_  | (_| |
     | .__/  |_| |___/  \__|  \__,_|
     |_|                            
        */
        //BordePista
        Ogre::SceneNode* _nodeBPista = mSceneMgr->createSceneNode("BordePista");
        mSceneMgr->getRootSceneNode()->addChild(_nodeBPista);
                
        Ogre::Entity* _entBPista = mSceneMgr->createEntity("BordePista01", "bordePista.mesh");
        _nodeBPista->attachObject(_entBPista);
        _entBPista->setMaterialName("Cubito/TransparentTest");

        //PisoObstaculo
        Ogre::SceneNode* _nodePObstaculo = mSceneMgr->createSceneNode("PistaObstaculo");
        mSceneMgr->getRootSceneNode()->addChild(_nodePObstaculo);
                
        Ogre::Entity* _entPObstaculo = mSceneMgr->createEntity("PistaObstaculo", "pisoObstaculo01.mesh");
        _nodePObstaculo->attachObject(_entPObstaculo);
        _entPObstaculo->setMaterialName("circuloneon");

        //PisoNOObstaculo
        Ogre::SceneNode* _nodePNObstaculo = mSceneMgr->createSceneNode("PistaNoObstaculo");
        mSceneMgr->getRootSceneNode()->addChild(_nodePNObstaculo);
                
        Ogre::Entity* _entPNOObstaculo = mSceneMgr->createEntity("PistaNoObstaculo", "pisoNoObstaculo01.mesh");
        _nodePNObstaculo->attachObject(_entPNOObstaculo);
        _entPNOObstaculo->setMaterialName("Cubito/Cubito");

        //PosterInicioFinal
        Ogre::SceneNode* _nodePoster = mSceneMgr->createSceneNode("PosterInicioFinal");
        mSceneMgr->getRootSceneNode()->addChild(_nodePoster);
            
        Ogre::Entity* _entPoster = mSceneMgr->createEntity("PosterInicioFinal", "posterInicioFinal.mesh");
        _nodePoster->attachObject(_entPoster);
        _entPoster->setMaterialName("Bandera/Poster");

        //BanderaInicial
        Ogre::SceneNode* _nodeBInicial = mSceneMgr->createSceneNode("BanderaInicial");
        mSceneMgr->getRootSceneNode()->addChild(_nodeBInicial);
                
        Ogre::Entity* _entBanderaI = mSceneMgr->createEntity("BanderaInicial", "banderaInicial.mesh");              
        _entBanderaI->setMaterialName("Bandera/Inicio");              
        _nodeBInicial->attachObject(_entBanderaI);
        _nodeBInicial->yaw(Degree(180));
        _nodeBInicial->translate(-13,0,125);


        //BanderaFinal
        Ogre::SceneNode* _nodeBFinal = mSceneMgr->createSceneNode("BanderaFinal");
        mSceneMgr->getRootSceneNode()->addChild(_nodeBFinal);
        
        Ogre::Entity* _entBanderaF = mSceneMgr->createEntity("BanderaFinal", "banderaFinal.mesh");
        _entBanderaF->setMaterialName("Bandera/Final"); 
        _nodeBFinal->attachObject(_entBanderaF);
        
        //Obstaculos
        Ogre::SceneNode* nodosObstaculos[12];
        for (int i = 0; i < 12; i++) {
            nodosObstaculos[i] = mSceneMgr->createSceneNode("NodoObstaculo"+std::to_string(i+1));
            mSceneMgr->getRootSceneNode()->addChild(nodosObstaculos[i]);

            Ogre::Entity* entObstaculos = mSceneMgr->createEntity("NodoObstaculo"+std::to_string(i+1), "cubo01.mesh");
            entObstaculos->setMaterialName("Rocasycolor/VidriosBloques");
            
            nodosObstaculos[i] ->attachObject(entObstaculos);
            nodosObstaculos[i] ->setScale(2.6,2.7,2.6);
            
        }
            nodosObstaculos[0] ->setPosition(-150,3.517,600);
            nodosObstaculos[1] ->setPosition(-60,3.517,700);
            nodosObstaculos[2] ->setPosition(60,3.517,700);
            nodosObstaculos[3] ->setPosition(150,3.517,600);

            nodosObstaculos[4] ->setPosition(-150,3.517,1700);
            nodosObstaculos[5] ->setPosition(-60,3.517,1800);
            nodosObstaculos[6] ->setPosition(60,3.517,1800);
            nodosObstaculos[7] ->setPosition(150,3.517,1700);

            nodosObstaculos[8] ->setPosition(-150,3.517,5800);
            nodosObstaculos[9] ->setPosition(-60,3.517,5900);
            nodosObstaculos[10] ->setPosition(60,3.517,5900);
            nodosObstaculos[11] ->setPosition(150,3.517,5800);
        

        // Animacion de los Obstaculos
        
        // Primero a la derecha
        Ogre::Animation* animationObstaculos01 = mSceneMgr -> createAnimation("animationObstaculos01",duration);
        animationObstaculos01 -> setInterpolationMode(Animation::IM_SPLINE);
        Ogre::NodeAnimationTrack* trackObstaculos01 = animationObstaculos01->createNodeTrack(0,nodosObstaculos[4]);
        Ogre::TransformKeyFrame* key;
        key = trackObstaculos01 -> createNodeKeyFrame(0.0);
        key ->setScale(Vector3(3.0,3.0,3.0));
        key -> setTranslate(Vector3(-150,3.517,1700));
        key = trackObstaculos01 -> createNodeKeyFrame(duration/2);
        key ->setScale(Vector3(3.0,3.0,3.0));
        key -> setTranslate(Vector3(60,3.517,1600));
        key = trackObstaculos01 -> createNodeKeyFrame(duration);
        key ->setScale(Vector3(3.0,3.0,3.0));
        key -> setTranslate(Vector3(-150,3.517,1700));
        /*
        key = trackObstaculos01 -> createNodeKeyFrame(2.0);
        key ->setScale(Vector3(3.0,3.0,3.0));
        key -> setTranslate(Vector3(50,3.517,1500));
        key = trackObstaculos01 -> createNodeKeyFrame(3.0);
        key ->setScale(Vector3(3.0,3.0,3.0));
        key -> setTranslate(Vector3(750,3.517,1100));
        key = trackObstaculos01 -> createNodeKeyFrame(4.0);
        key ->setScale(Vector3(3.0,3.0,3.0));
        key -> setTranslate(Vector3(60,3.517,1600));
        */
        animationObs01 = mSceneMgr -> createAnimationState("animationObstaculos01");
        animationObs01 -> setEnabled(true);
        animationObs01 -> setLoop(true);

        // Segundo a la derecha
        Ogre::Animation* animationObstaculos02 = mSceneMgr -> createAnimation("animationObstaculos02",duration);
        animationObstaculos02 -> setInterpolationMode(Animation::IM_SPLINE);
        Ogre::NodeAnimationTrack* trackObstaculos02 = animationObstaculos02->createNodeTrack(0,nodosObstaculos[5]);
        Ogre::TransformKeyFrame* key2;
        key2 = trackObstaculos02 -> createNodeKeyFrame(0.0);
        key2 ->setScale(Vector3(3.0,3.0,3.0));
        key2 -> setTranslate(Vector3(-60,3.517,1800));
        key2 = trackObstaculos02 -> createNodeKeyFrame(duration/2);
        key2 ->setScale(Vector3(3.0,3.0,3.0));
        key2 -> setTranslate(Vector3(-70,3.517,1700));
        key2 = trackObstaculos02 -> createNodeKeyFrame(duration);
        key2 ->setScale(Vector3(3.0,3.0,3.0));
        key2 -> setTranslate(Vector3(-60,3.517,1800));
        /*key2 = trackObstaculos02 -> createNodeKeyFrame(2.0);
        key2 ->setScale(Vector3(3.0,3.0,3.0));
        key2 -> setTranslate(Vector3(50,3.517,1600));
        */        
        animationObs02 = mSceneMgr -> createAnimationState("animationObstaculos02");
        animationObs02 -> setEnabled(true);
        animationObs02 -> setLoop(true);

        // Primero a la izquierda
        
        Ogre::Animation* animationObstaculos03 = mSceneMgr -> createAnimation("animationObstaculos03",duration);
        animationObstaculos03 -> setInterpolationMode(Animation::IM_SPLINE);
        Ogre::NodeAnimationTrack* trackObstaculos03 = animationObstaculos03->createNodeTrack(0,nodosObstaculos[7]);
        Ogre::TransformKeyFrame* key3;
        key3 = trackObstaculos03 -> createNodeKeyFrame(0.0);
        key3 ->setScale(Vector3(3.0,3.0,3.0));
        key3 -> setTranslate(Vector3(150,3.517,1700));
        key3 = trackObstaculos03 -> createNodeKeyFrame(duration/2);
        key3 ->setScale(Vector3(3.0,3.0,3.0));
        key3 -> setTranslate(Vector3(60,3.517,1850));
        key3 = trackObstaculos03 -> createNodeKeyFrame(duration);
        key3 ->setScale(Vector3(3.0,3.0,3.0));
        key3 -> setTranslate(Vector3(150,3.517,1700));
        /*key3 = trackObstaculos03 -> createNodeKeyFrame(4.0);
        key3 ->setScale(Vector3(3.0,3.0,3.0));
        key3 -> setTranslate(Vector3(-350,3.517,2000));
        */
        animationObs03 = mSceneMgr -> createAnimationState("animationObstaculos03");
        animationObs03 -> setEnabled(true);
        animationObs03 -> setLoop(true);
        
        
        // Segundo a la izquierda
        Ogre::Animation* animationObstaculos04 = mSceneMgr -> createAnimation("animationObstaculos04",duration);
        animationObstaculos04 -> setInterpolationMode(Animation::IM_SPLINE);
        Ogre::NodeAnimationTrack* trackObstaculos04 = animationObstaculos04->createNodeTrack(0,nodosObstaculos[6]);
        Ogre::TransformKeyFrame* key4;
        key4 = trackObstaculos04 -> createNodeKeyFrame(0.0);
        key4 ->setScale(Vector3(3.0,3.0,3.0));
        key4 -> setTranslate(Vector3(60,3.517,1800));
        key4 = trackObstaculos04 -> createNodeKeyFrame(duration/2);
        key4 ->setScale(Vector3(3.0,3.0,3.0));
        key4 -> setTranslate(Vector3(-60,3.517,1900));
        key4 = trackObstaculos04 -> createNodeKeyFrame(duration);
        key4 ->setScale(Vector3(3.0,3.0,3.0));
        key4 -> setTranslate(Vector3(60,3.517,1800));
        
        animationObs04 = mSceneMgr -> createAnimationState("animationObstaculos04");
        animationObs04 -> setEnabled(true);
        animationObs04 -> setLoop(true);
        
        colocarMonedas();

        // Luna / Sol Decoración
        Ogre::SceneNode* nodoLuna = mSceneMgr->createSceneNode("Luna");
        mSceneMgr->getRootSceneNode()->addChild(nodoLuna);
        
        Ogre::Entity* entLuna = mSceneMgr->createEntity("Luna", "sphere.mesh");
        entLuna->setMaterialName("Rocasycolor/Luna");
        nodoLuna->attachObject(entLuna);
        nodoLuna->setScale(0.1,0.1,0.1);
        nodoLuna->setPosition(0,200,10000);       

        Ogre::ParticleSystem* partSystem = mSceneMgr->createParticleSystem("FuegosArtificiales","Examples/Fireworks");
        nodoLuna->attachObject(partSystem);

    }
    void colocarMonedas(){
        // Tramo 1
        for( int i = 0 ; i < 5 ; ++i ){
            crearMonedas(i,200,50,2351);
        }
        // Tunel
        for( int i = 5 ; i < 10 ; ++i ){
            crearMonedas(i,32,2927,4929);
        }
        // Tramo 2
        for( int i = 10 ; i < 13 ; ++i ){
            crearMonedas(i,123,4929,5505);
        }
        for( int i = 13 ; i < 15 ; ++i ){
            crearMonedas(i,80,5505,6535);
        }                
        // Vacio
        for( int i = 15 ; i < 20 ; ++i ){
            crearMonedas(i,200,6535,10305);
        }
    }
    void crearMonedas(int i , int maxX , int minZ , int maxZ ){
                
        float cx = rand() % maxX;

        if ( (rand() % 2) == 1 ) cx = -1;

        float cz = (rand() % (maxZ-minZ)) + minZ;

        nodoMonedas[20];
        nodoMonedas[i] = mSceneMgr->createSceneNode("Moneda"+std::to_string(i));
        mSceneMgr->getRootSceneNode()->addChild(nodoMonedas[i]);
        
        Ogre::Entity* _entMoneda = mSceneMgr->createEntity("cilindro01.mesh");
        _entMoneda->setMaterialName("AndreaCenteno_Estrellas/Moneda");
        nodoMonedas[i]->setPosition(cx,10.0,cz);
        nodoMonedas[i]->setScale(3.5f,0.1f,3.5f);
        nodoMonedas[i]->rotate(Ogre::Vector3(1.0,0.0,0.0), Ogre::Radian(Ogre::Degree(90.0)));
        nodoMonedas[i]->attachObject(_entMoneda);
    }
    
};

int main (void)
{
    Example1 app;
    app.go();
    return 0;
}