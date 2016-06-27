#include "Ogre\ExampleApplication.h"
#include <string>
#include "Ogre\Ogre.h"
#include "OIS\OIS.h"
#include <time.h>


// Para la animacion de los obstaculos
Ogre::AnimationState* animationObs01;
Ogre::AnimationState* animationObs02;
Ogre::AnimationState* animationObs03;
Ogre::AnimationState* animationObs04;
float duration = 10.0f;
float duration2 = 10.0f;

// animacion carro-nave
Ogre::AnimationState* animationCar[6];
int rotRx = 0;
int rotRy = 0;

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

    Ogre::SceneNode* cameraYawNode;
    Ogre::SceneNode* cameraNode;
    
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
        cameraYawNode = _nodoCarro->createChildSceneNode();
    }

    ~FrameListenerClase() {
        _man ->destroyInputObject(_key);
        _man->destroyInputObject(_mouse);
        OIS::InputManager::destroyInputSystem(_man);
    }

    bool frameStarted(const Ogre::FrameEvent &evt) {
            
        _key->capture();
        _mouse->capture();

        float movSpeed = 10.0f;
        Ogre::Vector3 tcam(0,0,0);
        Ogre::Vector3 tcar(0,0,0);

        float rotX;

        Ogre::Vector3 x;

        if (_key->isKeyDown(OIS::KC_ESCAPE))
            return false;

        // Teclas para la mover la camara
            
        // Si presionamos la tecla w
        if(_key->isKeyDown(OIS::KC_W)){     
            tcam += Ogre::Vector3(0,0,-15);
            tcar += Ogre::Vector3(0,0, 15);
            
            rotRx = (rotRx + 1) % 360;
            for(int i =0; i < 4; i++){
                _nodoRuedas[i]->pitch(Ogre::Degree(rotRx));
            }

            cameraYawNode->setOrientation(Ogre::Quaternion::IDENTITY);
        }

        // Si presionamos la tecla s
        if(_key->isKeyDown(OIS::KC_S)){
            tcam += Ogre::Vector3(0,0, 15);
            tcar += Ogre::Vector3(0,0,-15);

            rotRx = (rotRx - 1) % 360;
            for(int i =0; i < 4; i++){
                _nodoRuedas[i]->pitch(Ogre::Degree(rotRx));
            }

            cameraYawNode->setOrientation(Ogre::Quaternion::IDENTITY);
        } 

        // Si presionamos la tecla a
        if(_key->isKeyDown(OIS::KC_A)){ 
            tcam += Ogre::Vector3(-15,0,0); 
            tcar += Ogre::Vector3( 15,0,0);

            rotX = evt.timeSinceLastFrame;
            _nodoCarro->yaw(Ogre::Radian(rotX));
            cameraYawNode->yaw(Ogre::Radian(rotX));
            //_cam->yaw(Ogre::Radian(rotX));

            //if(rotRy < 10){
            //    rotRy = 10;
            //    for(int i =0; i < 2; i++){
            //        _nodoRuedas[i]->yaw(Ogre::Degree(rotRy));
            //    }
            //}
        }

        // Si presionamos la tecla d
        if(_key->isKeyDown(OIS::KC_D)){ 
            tcam += Ogre::Vector3( 15,0,0);
            tcar += Ogre::Vector3(-15,0,0);

            rotX = evt.timeSinceLastFrame * -1;
            _nodoCarro->yaw(Ogre::Radian(rotX));
            cameraYawNode->yaw(Ogre::Radian(rotX));
            //_cam->yaw(Ogre::Radian(rotX));

            //if(rotRy > -10){
            //    rotRy = -10;
            //    for(int i =0; i < 2; i++){
            //        _nodoRuedas[i]->yaw(Ogre::Degree(rotRy));
            //    }
            //}
        }           
        
        if(_key->isKeyDown(OIS::KC_T)){ 
            for(int i =0; i <6 ; i++){
                animationCar[i]->setEnabled(true);
            }
            //animationCar[1]->setEnabled(true);
        }       

        // Camara Control
        //float rotX = _mouse->getMouseState().X.rel * evt.timeSinceLastFrame * -1;
        //float rotY = _mouse->getMouseState().Y.rel * evt.timeSinceLastFrame * -1;
        
        //_cam->pitch(Ogre::Radian(rotY));
        

        // Usados en el proyecto 1 de ogre:
        _cam->moveRelative(tcam*movSpeed*evt.timeSinceLastFrame);
        _nodoCarro->translate(tcar*movSpeed*evt.timeSinceLastFrame);
        //_nodoCarro->translate(cameraYawNode->getOrientation() * tcar*movSpeed*evt.timeSinceLastFrame);

        // Animacion Obstaculos
        animationObs01 -> addTime(evt.timeSinceLastFrame);
        animationObs02 -> addTime(evt.timeSinceLastFrame);
        animationObs03 -> addTime(evt.timeSinceLastFrame);
        animationObs04 -> addTime(evt.timeSinceLastFrame);

        for(int i =0; i <6 ; i++){
            animationCar[i] -> addTime(evt.timeSinceLastFrame);
        }
        //_nodoCarro->translate(cameraYawNode->getOrientation() * tcar*movSpeed*evt.timeSinceLastFrame,Ogre::SceneNode::TS_LOCAL);
        //_nodoCarro->translate(this->cameraYawNode->getOrientation(), Ogre::SceneNode::TS_LOCAL);

        return true;

    }
};


class Example1 : public ExampleApplication
{

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
            manualAlaCara->begin("DanielaOrtiz_Nave/Gris", RenderOperation::OT_TRIANGLE_STRIP);

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
            manualAlaBorde->begin("DanielaOrtiz_Nave/Gris", RenderOperation::OT_TRIANGLE_STRIP);

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

    void createScene()
    {
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
            LucesTunel[i]->setPosition(Ogre::Vector3(0.0,10.0,posicion));
            LucesTunel[i]->setCastShadows(false);
            LucesTunel[i]->setAttenuation(65, 1.0, 0.07, 0.017);

            posicion = posicion + 200;
        }
        
        alasMesh();

        mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));
        mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
        // Cielo estrellado
        mSceneMgr->setSkyBox(true, "AndreaCenteno_Estrellas/SkyBox");

        /* ___    __ _   _ __   _ __    ___  
          / __|  / _` | | '__| | '__|  / _ \ 
         | (__  | (_| | | |    | |    | (_) |
          \___|  \__,_| |_|    |_|     \___/ 
        */

        //Chasis
        _nodeChasis01 = mSceneMgr->createSceneNode("Chasis01");
        mSceneMgr->getRootSceneNode()->addChild(_nodeChasis01);
        _nodeChasis01->setScale(1.2,1.2,1.2);
            
        Ogre::Entity* _entChasis01 = mSceneMgr->createEntity("entChasis01", "chasisCarro.mesh");
        _entChasis01->setMaterialName("shCarro01");
        _nodeChasis01->attachObject(_entChasis01);

        //Rueda 01 Delantera Derecha
        nodoRuedas[0] = mSceneMgr->createSceneNode("Rueda01");
        _nodeChasis01->addChild(nodoRuedas[0]);
        //mSceneMgr->getRootSceneNode()->addChild(nodoRuedas[0]);
        Ogre::Entity* _entRueda01 = mSceneMgr->createEntity("entRueda01", "ruedaDetallada.mesh");
        nodoRuedas[0]->translate(-5.77,3.517,9.462);
        _entRueda01->setMaterialName("shRueda02");
        nodoRuedas[0]->attachObject(_entRueda01);
        //animacion
        Ogre::Animation* animationCarR00 = mSceneMgr -> createAnimation("animationCarR00",duration);
        animationCarR00 -> setInterpolationMode(Animation::IM_SPLINE);
        Ogre::NodeAnimationTrack* trackCarR00 = animationCarR00->createNodeTrack(0,nodoRuedas[0]);
        Ogre::TransformKeyFrame* keyCarR00;
        keyCarR00 = trackCarR00 -> createNodeKeyFrame(0.0);
        keyCarR00->setTranslate(Vector3(-5.77,3.517,9.462));
        keyCarR00->setRotation(Quaternion(Degree(0), Vector3::UNIT_Z));
        keyCarR00 = trackCarR00 -> createNodeKeyFrame(4.0);
        keyCarR00->setTranslate(Vector3(-5.77,3.517,9.462));
        keyCarR00->setRotation(Quaternion(Degree(90), Vector3::UNIT_Z)); 
        animationCar[2] = mSceneMgr -> createAnimationState("animationCarR00");
        animationCar[2]->setEnabled(false);
        animationCar[2]->setLoop(false);

        //Rueda 02 Delantera Izquierda
        nodoRuedas[1] = mSceneMgr->createSceneNode("Rueda02");
        _nodeChasis01->addChild(nodoRuedas[1]);
        //mSceneMgr->getRootSceneNode()->addChild(nodoRuedas[1]);
        Ogre::Entity* _entRueda02 = mSceneMgr->createEntity("entRueda02", "ruedaDetallada.mesh");
        nodoRuedas[1]->translate(8,3.517,9.462);
        _entRueda02->setMaterialName("shRueda02");
        nodoRuedas[1]->attachObject(_entRueda02);
        //animacion
        Ogre::Animation* animationCarR01 = mSceneMgr -> createAnimation("animationCarR01",duration);
        animationCarR01 -> setInterpolationMode(Animation::IM_SPLINE);
        Ogre::NodeAnimationTrack* trackCarR01 = animationCarR01->createNodeTrack(0,nodoRuedas[1]);
        Ogre::TransformKeyFrame* keyCarR01;
        keyCarR01 = trackCarR01 -> createNodeKeyFrame(0.0);
        keyCarR01->setTranslate(Vector3(8,3.517,9.462));
        keyCarR01->setRotation(Quaternion(Degree(0), Vector3::UNIT_Z));
        keyCarR01 = trackCarR01 -> createNodeKeyFrame(4.0);
        keyCarR01->setTranslate(Vector3(8,3.517,9.462));
        keyCarR01->setRotation(Quaternion(Degree(-90), Vector3::UNIT_Z)); 
        animationCar[3] = mSceneMgr -> createAnimationState("animationCarR01");
        animationCar[3]->setEnabled(false);
        animationCar[3]->setLoop(false);

        //Rueda 03 Trasera Derecha
        nodoRuedas[2] = mSceneMgr->createSceneNode("Rueda03");
        _nodeChasis01->addChild(nodoRuedas[2]);
        //mSceneMgr->getRootSceneNode()->addChild(nodoRuedas[2]);
        Ogre::Entity* _entRueda03 = mSceneMgr->createEntity("entRueda03", "ruedaDetallada.mesh");
        nodoRuedas[2]->translate(-5.77,3.517,-9.462);
        _entRueda03->setMaterialName("shRueda02");
        nodoRuedas[2]->attachObject(_entRueda03);
        //animacion
        Ogre::Animation* animationCarR02 = mSceneMgr -> createAnimation("animationCarR02",duration);
        animationCarR02 -> setInterpolationMode(Animation::IM_SPLINE);
        Ogre::NodeAnimationTrack* trackCarR02 = animationCarR02->createNodeTrack(0,nodoRuedas[2]);
        Ogre::TransformKeyFrame* keyCarR02;
        keyCarR02 = trackCarR02 -> createNodeKeyFrame(0.0);
        keyCarR02->setTranslate(Vector3(-5.77,3.517,-9.462));
        keyCarR02->setRotation(Quaternion(Degree(0), Vector3::UNIT_Z));
        keyCarR02 = trackCarR02 -> createNodeKeyFrame(4.0);
        keyCarR02->setTranslate(Vector3(-5.77,3.517,-9.462));
        keyCarR02->setRotation(Quaternion(Degree(90), Vector3::UNIT_Z)); 
        animationCar[4] = mSceneMgr -> createAnimationState("animationCarR02");
        animationCar[4]->setEnabled(false);
        animationCar[4]->setLoop(false);

        //Rueda 04 Trasera Izquierda
        nodoRuedas[3] = mSceneMgr->createSceneNode("Rueda04");
        _nodeChasis01->addChild(nodoRuedas[3]);
        //mSceneMgr->getRootSceneNode()->addChild(nodoRuedas[3]);
        Ogre::Entity* _entRueda04 = mSceneMgr->createEntity("entRueda04", "ruedaDetallada.mesh");
        nodoRuedas[3]->translate(8,3.517,-9.462);
        _entRueda04->setMaterialName("shRueda02");
        nodoRuedas[3]->attachObject(_entRueda04);
        //animacion
        Ogre::Animation* animationCarR03 = mSceneMgr -> createAnimation("animationCarR03",duration);
        animationCarR03 -> setInterpolationMode(Animation::IM_SPLINE);
        Ogre::NodeAnimationTrack* trackCarR03 = animationCarR03->createNodeTrack(0,nodoRuedas[3]);
        Ogre::TransformKeyFrame* keyCarR03;
        keyCarR03 = trackCarR03 -> createNodeKeyFrame(0.0);
        keyCarR03->setTranslate(Vector3(8,3.517,-9.462));
        keyCarR03->setRotation(Quaternion(Degree(0), Vector3::UNIT_Z));
        keyCarR03 = trackCarR03 -> createNodeKeyFrame(4.0);
        keyCarR03->setTranslate(Vector3(8,3.517,-9.462));
        keyCarR03->setRotation(Quaternion(Degree(-90), Vector3::UNIT_Z)); 
        animationCar[5] = mSceneMgr -> createAnimationState("animationCarR03");
        animationCar[5]->setEnabled(false);
        animationCar[5]->setLoop(false);

        //ALAS
        Ogre::SceneNode* nodoAla[2];
        Ogre::Entity* entAla[2];

        //CARA DERECHA
        entAla[0]  = mSceneMgr->createEntity("MeshAlaCara");
        nodoAla[0] = mSceneMgr->createSceneNode("NodoAlaD");
        _nodeChasis01->addChild(nodoAla[0]);
        nodoAla[0]->attachObject(entAla[0]);
        nodoAla[0]->setScale(0.0,0.0,0.0);
        //borde
        Ogre::Entity* entAlaDB = mSceneMgr->createEntity("MeshAlaBorde");
        Ogre::SceneNode* nodoAlaDB = mSceneMgr->createSceneNode("NodoAlaDB");
        nodoAla[0]->addChild(nodoAlaDB);
        nodoAlaDB->attachObject(entAlaDB);
        //animacion
        Ogre::Animation* animationCarrito00 = mSceneMgr -> createAnimation("animationCarrito00",duration);
        animationCarrito00 -> setInterpolationMode(Animation::IM_SPLINE);
        Ogre::NodeAnimationTrack* trackCarrito00 = animationCarrito00->createNodeTrack(0,nodoAla[0]);
        Ogre::TransformKeyFrame* keyCar00;
        keyCar00 = trackCarrito00 -> createNodeKeyFrame(0.0);
        keyCar00->setScale(Vector3(0.0,0.0,0.0));
        keyCar00->setTranslate(Vector3(-6,7.0,-4.0));
        keyCar00 = trackCarrito00 -> createNodeKeyFrame(4.0);
        keyCar00->setScale(Vector3(2.5,2.0,2.0));
        keyCar00->setTranslate(Vector3(-1.0,7.0,-4.0));
        animationCar[0] = mSceneMgr -> createAnimationState("animationCarrito00");
        animationCar[0]->setEnabled(false);
        animationCar[0]->setLoop(false);

        //CARA IZQUIERDA
        entAla[1]  = mSceneMgr->createEntity("MeshAlaCara");
        nodoAla[1] = mSceneMgr->createSceneNode("NodoAlaI");
        _nodeChasis01->addChild(nodoAla[1]);
        nodoAla[1]->attachObject(entAla[1]);
        nodoAla[1]->setScale(0.0,0.0,0.0);
        //borde
        Ogre::Entity* entAlaIB = mSceneMgr->createEntity("MeshAlaBorde");
        Ogre::SceneNode* nodoAlaIB = mSceneMgr->createSceneNode("NodoAlaIB");
        nodoAla[1]->addChild(nodoAlaIB);
        nodoAlaIB->attachObject(entAlaIB);
        //animacion
        Ogre::Animation* animationCarrito01 = mSceneMgr -> createAnimation("animationCarrito01",duration);
        animationCarrito01 -> setInterpolationMode(Animation::IM_SPLINE);
        Ogre::NodeAnimationTrack* trackCarrito01 = animationCarrito01->createNodeTrack(0,nodoAla[1]);
        Ogre::TransformKeyFrame* keyCar01;
        keyCar01 = trackCarrito01 -> createNodeKeyFrame(0.0);
        keyCar01->setTranslate(Vector3(6,7.0,-4.0));
        keyCar01->setRotation(Quaternion(Degree(180), Vector3::UNIT_Y)); 
        keyCar01->setScale(Vector3(0.0,0.0,0.0));
        keyCar01 = trackCarrito01 -> createNodeKeyFrame(4.0);
        keyCar01->setTranslate(Vector3(1.0,7.0,-4.0));
        keyCar01->setRotation(Quaternion(Degree(180), Vector3::UNIT_Y)); 
        keyCar01->setScale(Vector3(2.5,2.0,2.0));
        animationCar[1] = mSceneMgr -> createAnimationState("animationCarrito01");
        animationCar[1]->setEnabled(false);
        animationCar[1]->setLoop(false);      

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
        _entPNOObstaculo->setMaterialName("cubito");

        //PosterInicioFinal
        Ogre::SceneNode* _nodePoster = mSceneMgr->createSceneNode("PosterInicioFinal");
        mSceneMgr->getRootSceneNode()->addChild(_nodePoster);
            
        Ogre::Entity* _entPoster = mSceneMgr->createEntity("PosterInicioFinal", "posterInicioFinal.mesh");
        _nodePoster->attachObject(_entPoster);

                
        //BanderaInicial
        Ogre::SceneNode* _nodeBInicial = mSceneMgr->createSceneNode("BanderaInicial");
        mSceneMgr->getRootSceneNode()->addChild(_nodeBInicial);
                
        Ogre::Entity* _entBanderaI = mSceneMgr->createEntity("BanderaInicial", "banderaInicial.mesh");              
        _entBanderaI->setMaterialName("lambert1");              
        _nodeBInicial->attachObject(_entBanderaI);


        //BanderaFinal
        Ogre::SceneNode* _nodeBFinal = mSceneMgr->createSceneNode("BanderaFinal");
        mSceneMgr->getRootSceneNode()->addChild(_nodeBFinal);
        
        Ogre::Entity* _entBanderaF = mSceneMgr->createEntity("BanderaFinal", "banderaFinal.mesh");
        _entBanderaF->setMaterialName("lambert1");
        _nodeBFinal->attachObject(_entBanderaF);
        
        //Obstaculos
        Ogre::SceneNode* nodosObstaculos[12];
        for (int i = 0; i < 12; i++) {
            nodosObstaculos[i] = mSceneMgr->createSceneNode("NodoObstaculo"+std::to_string(i+1));
            mSceneMgr->getRootSceneNode()->addChild(nodosObstaculos[i]);

            Ogre::Entity* entObstaculos = mSceneMgr->createEntity("NodoObstaculo"+std::to_string(i+1), "cubo01.mesh");

            nodosObstaculos[i] ->attachObject(entObstaculos);
            nodosObstaculos[i] ->setScale(3.0,3.0,3.0);
            
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
        key = trackObstaculos01 -> createNodeKeyFrame(1.0);
        key ->setScale(Vector3(3.0,3.0,3.0));
        key -> setTranslate(Vector3(60,3.517,1600));
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
        key2 = trackObstaculos02 -> createNodeKeyFrame(1.0);
        key2 ->setScale(Vector3(3.0,3.0,3.0));
        key2 -> setTranslate(Vector3(-70,3.517,1700));
        /*key2 = trackObstaculos02 -> createNodeKeyFrame(2.0);
        key2 ->setScale(Vector3(3.0,3.0,3.0));
        key2 -> setTranslate(Vector3(50,3.517,1600));
        */        
        animationObs02 = mSceneMgr -> createAnimationState("animationObstaculos02");
        animationObs02 -> setEnabled(true);
        animationObs02 -> setLoop(true);

        // Primero a la izquierda
        
        Ogre::Animation* animationObstaculos03 = mSceneMgr -> createAnimation("animationObstaculos03",duration2);
        animationObstaculos03 -> setInterpolationMode(Animation::IM_SPLINE);
        Ogre::NodeAnimationTrack* trackObstaculos03 = animationObstaculos03->createNodeTrack(0,nodosObstaculos[7]);
        Ogre::TransformKeyFrame* key3;
        key3 = trackObstaculos03 -> createNodeKeyFrame(0.0);
        key3 ->setScale(Vector3(3.0,3.0,3.0));
        key3 -> setTranslate(Vector3(150,3.517,1700));
        key3 = trackObstaculos03 -> createNodeKeyFrame(1.0);
        key3 ->setScale(Vector3(3.0,3.0,3.0));
        key3 -> setTranslate(Vector3(60,3.517,1850));
        /*key3 = trackObstaculos03 -> createNodeKeyFrame(4.0);
        key3 ->setScale(Vector3(3.0,3.0,3.0));
        key3 -> setTranslate(Vector3(-350,3.517,2000));
        */
        animationObs03 = mSceneMgr -> createAnimationState("animationObstaculos03");
        animationObs03 -> setEnabled(true);
        animationObs03 -> setLoop(true);
        
        
        // Segundo a la izquierda
        Ogre::Animation* animationObstaculos04 = mSceneMgr -> createAnimation("animationObstaculos04",duration2);
        animationObstaculos04 -> setInterpolationMode(Animation::IM_SPLINE);
        Ogre::NodeAnimationTrack* trackObstaculos04 = animationObstaculos04->createNodeTrack(0,nodosObstaculos[6]);
        Ogre::TransformKeyFrame* key4;
        key4 = trackObstaculos04 -> createNodeKeyFrame(0.0);
        key4 ->setScale(Vector3(3.0,3.0,3.0));
        key4 -> setTranslate(Vector3(60,3.517,1800));
        key4 = trackObstaculos04 -> createNodeKeyFrame(1.0);
        key4 ->setScale(Vector3(3.0,3.0,3.0));
        key4 -> setTranslate(Vector3(-60,3.517,1900));
        
        animationObs04 = mSceneMgr -> createAnimationState("animationObstaculos04");
        animationObs04 -> setEnabled(true);
        animationObs04 -> setLoop(true);
        
        colocarMonedas();

        // Luna / Sol Decoración
        Ogre::SceneNode* nodoLuna = mSceneMgr->createSceneNode("Luna");
        mSceneMgr->getRootSceneNode()->addChild(nodoLuna);
        
        Ogre::Entity* entLuna = mSceneMgr->createEntity("Luna", "sphere.mesh");
        //entLuna->setMaterialName("AndreaCenteno_Estrellas/Estrellita");
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
        for( int i = 10 ; i < 15 ; ++i ){
            crearMonedas(i,200,5505,2351);
        }
                
        // Vacio
        for( int i = 15 ; i < 20 ; ++i ){
            crearMonedas(i,200,6535,10305);
        }
    }
    void crearMonedas(int i , int maxX , int minZ , int maxZ ){
                
        float cx = rand() % maxX;

        if ( (rand() % 2) == 1 ){
            cx = -1;
        }

        float cz = (rand() % (maxZ-minZ)) + minZ;

        Ogre::SceneNode* nodoMonedas[20];
        nodoMonedas[i] = mSceneMgr->createSceneNode("Moneda"+std::to_string(i));
        mSceneMgr->getRootSceneNode()->addChild(nodoMonedas[i]);
        
        Ogre::Entity* _entMoneda = mSceneMgr->createEntity("cilindro01.mesh");
        _entMoneda->setMaterialName("AndreaCenteno_Estrellas/Monedas");
        nodoMonedas[i]->setPosition(cx,10.0,cz);
        nodoMonedas[i]->setScale(3.5f,0.5f,3.5f);
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