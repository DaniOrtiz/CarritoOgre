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
float duration = 4.0f;
float duration2 = 4.0f;

//Monedas
int cantMonedas = 20;
Ogre::SceneNode* nodoMonedas[20];
bool atrapada[20];

class FrameListenerClase : public Ogre::FrameListener { // Hereda de la clase FrameListener de Ogre, escucha algo

private:
    Ogre::Camera* _cam;
    OIS::InputManager* _man;
    OIS::Keyboard* _key; // Teclado
    OIS::Mouse* _mouse; // Mouse

public:
    // Constructor que le asignamos el nodo que creamos
    FrameListenerClase(Ogre::Camera* cam, 
                       RenderWindow* win) {

        // Configuracion captura teclado y mouse
        // ESTO ES ASI PORQUE SI, NO CAMBIA
        size_t windowHnd = 0;
        std::stringstream windowHndStr;
        win->getCustomAttribute("WINDOW", &windowHnd);
        windowHndStr << windowHnd;

        OIS::ParamList pl;
        pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

        // Eventos

        _man = OIS::InputManager::createInputSystem(pl);
        _key = static_cast< OIS::Keyboard*>(_man->createInputObject(OIS::OISKeyboard, false));
        _mouse = static_cast< OIS::Mouse*>(_man->createInputObject(OIS::OISMouse, false));

        _cam = cam;
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
        float a = 0.0;
        bool abiertas = false;

        Ogre::Vector3 x;

        if (_key->isKeyDown(OIS::KC_ESCAPE))
            return false;

        // Teclas para la mover la camara
            
        // Si presionamos la tecla w
        if(_key->isKeyDown(OIS::KC_W)){     
            tcam += Ogre::Vector3(0,0,-10);
        }

        // Si presionamos la tecla a
        if(_key->isKeyDown(OIS::KC_A)){ 
            tcam += Ogre::Vector3(-10,0,0); 
        }

        // Si presionamos la tecla d
        if(_key->isKeyDown(OIS::KC_D)){ 
            tcam += Ogre::Vector3(10,0,0);
        }


        //if(_key->isKeyDown(OIS::KC_E)){   
        

        //if(_key->isKeyDown(OIS::KC_R)){       
        
        // Camara Control
        float rotX = _mouse->getMouseState().X.rel * evt.timeSinceLastFrame * -1;
        float rotY = _mouse->getMouseState().Y.rel * evt.timeSinceLastFrame * -1;
        _cam->yaw(Ogre::Radian(rotX));
        _cam->pitch(Ogre::Radian(rotY));

        // Usados en el proyecto 1 de ogre:
        _cam->moveRelative(tcam*movSpeed*evt.timeSinceLastFrame);
        //_nodoNave->translate(tcam*movSpeed*evt.timeSinceLastFrame);

        // Animacion Obstaculos
        animationObs01 -> addTime(evt.timeSinceLastFrame);
        animationObs02 -> addTime(evt.timeSinceLastFrame);
        animationObs03 -> addTime(evt.timeSinceLastFrame);
        animationObs04 -> addTime(evt.timeSinceLastFrame);

        return true;

    }
};


class Example1 : public ExampleApplication
{

public:

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
        FrameListener01 = new FrameListenerClase(mCamera, mWindow); 
        mRoot->addFrameListener(FrameListener01);
    }

    void createCamera() {

        mCamera = mSceneMgr->createCamera("MyCamera1");
        mCamera->setPosition(0,100,-200);
        mCamera->lookAt(0,0,100);
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
        Ogre::SceneNode* _nodeChasis01 = mSceneMgr->createSceneNode("Chasis01");
        mSceneMgr->getRootSceneNode()->addChild(_nodeChasis01);
            
        Ogre::Entity* _entChasis01 = mSceneMgr->createEntity("entChasis01", "chasisCarro.mesh");
        _entChasis01->setMaterialName("shCarro01");
        _nodeChasis01->attachObject(_entChasis01);

        //Rueda 01 Delantera Derecha
        Ogre::SceneNode* _nodeRueda01 = mSceneMgr->createSceneNode("Rueda01");
        _nodeChasis01->addChild(_nodeRueda01);
            
        Ogre::Entity* _entRueda01 = mSceneMgr->createEntity("entRueda01", "ruedaDetallada.mesh");
        _nodeRueda01->translate(-5.77,3.517,9.462);
        _entRueda01->setMaterialName("shRueda02");
        _nodeRueda01->attachObject(_entRueda01);

        //Rueda 02 Delantera Izquierda
        Ogre::SceneNode* _nodeRueda02 = mSceneMgr->createSceneNode("Rueda02");
        _nodeChasis01->addChild(_nodeRueda02);
            
        Ogre::Entity* _entRueda02 = mSceneMgr->createEntity("entRueda02", "ruedaDetallada.mesh");
        _nodeRueda02->translate(8,3.517,9.462);
        _entRueda02->setMaterialName("shRueda02");
        _nodeRueda02->attachObject(_entRueda02);

        //Rueda 03 Trasera Derecha
        Ogre::SceneNode* _nodeRueda03 = mSceneMgr->createSceneNode("Rueda03");
        _nodeChasis01->addChild(_nodeRueda03);
            
        Ogre::Entity* _entRueda03 = mSceneMgr->createEntity("entRueda03", "ruedaDetallada.mesh");
        _nodeRueda03->translate(-5.77,3.517,-9.462);
        _entRueda03->setMaterialName("shRueda02");
        _nodeRueda03->attachObject(_entRueda03);

        //Rueda 04 Trasera Izquierda
        Ogre::SceneNode* _nodeRueda04 = mSceneMgr->createSceneNode("Rueda04");
        _nodeChasis01->addChild(_nodeRueda04);
            
        Ogre::Entity* _entRueda04 = mSceneMgr->createEntity("entRueda04", "ruedaDetallada.mesh");
        _nodeRueda04->translate(8,3.517,-9.462);
        _entRueda04->setMaterialName("shRueda02");
        _nodeRueda04->attachObject(_entRueda04);

        //ALAS
        Ogre::SceneNode* nodoAlaI;
        Ogre::SceneNode* nodoAlaD;
        Ogre::Entity* entAlaI;
        Ogre::Entity* entAlaD;

        //CARA DERECHA
        entAlaD  = mSceneMgr->createEntity("MeshAlaCara");
        nodoAlaD = mSceneMgr->createSceneNode("NodoAlaD");
        _nodeChasis01->addChild(nodoAlaD);
        nodoAlaD->attachObject(entAlaD);
        nodoAlaD->translate(-1.0,6.0,-4.0);
        nodoAlaD->setScale(2.5,2.0,2.0);
        //borde
        Ogre::Entity* entAlaDB = mSceneMgr->createEntity("MeshAlaBorde");
        Ogre::SceneNode* nodoAlaDB = mSceneMgr->createSceneNode("NodoAlaDB");
        nodoAlaD->addChild(nodoAlaDB);
        nodoAlaDB->attachObject(entAlaDB);

        //CARA IZQUIERDA
        entAlaI  = mSceneMgr->createEntity("MeshAlaCara");
        nodoAlaI = mSceneMgr->createSceneNode("NodoAlaI");
        _nodeChasis01->addChild(nodoAlaI);
        nodoAlaI->attachObject(entAlaI);
        nodoAlaI->translate(1.0,6.0,-4.0);
        nodoAlaI->yaw(Ogre::Degree( 180 ) );
        nodoAlaI->setScale(2.5,2.0,2.0);
        //borde
        Ogre::Entity* entAlaIB = mSceneMgr->createEntity("MeshAlaBorde");
        Ogre::SceneNode* nodoAlaIB = mSceneMgr->createSceneNode("NodoAlaIB");
        nodoAlaI->addChild(nodoAlaIB);
        nodoAlaIB->attachObject(entAlaIB);

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
        key = trackObstaculos01 -> createNodeKeyFrame(4.0);
        key ->setScale(Vector3(3.0,3.0,3.0));
        key -> setTranslate(Vector3(150,3.517,1500));
        key = trackObstaculos01 -> createNodeKeyFrame(8.0);
        key ->setScale(Vector3(3.0,3.0,3.0));
        key -> setTranslate(Vector3(450,3.517,1300));
        key = trackObstaculos01 -> createNodeKeyFrame(12.0);
        key ->setScale(Vector3(3.0,3.0,3.0));
        key -> setTranslate(Vector3(750,3.517,1100));
        key = trackObstaculos01 -> createNodeKeyFrame(16.0);
        key ->setScale(Vector3(3.0,3.0,3.0));
        key -> setTranslate(Vector3(1050,3.517,900));
        
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
        key2 = trackObstaculos02 -> createNodeKeyFrame(4.0);
        key2 ->setScale(Vector3(3.0,3.0,3.0));
        key2 -> setTranslate(Vector3(150,3.517,1700));
                
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
        key3 = trackObstaculos03 -> createNodeKeyFrame(4.0);
        key3 ->setScale(Vector3(3.0,3.0,3.0));
        key3 -> setTranslate(Vector3(-150,3.517,2000));
        key3 = trackObstaculos03 -> createNodeKeyFrame(10.0);
        key3 ->setScale(Vector3(3.0,3.0,3.0));
        key3 -> setTranslate(Vector3(-350,3.517,2000));
        
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
        key4 -> setTranslate(Vector3(-60,3.517,1800));
        key4 = trackObstaculos04 -> createNodeKeyFrame(4.0);
        key4 ->setScale(Vector3(3.0,3.0,3.0));
        key4 -> setTranslate(Vector3(-150,3.517,1800));
            
        animationObs04 = mSceneMgr -> createAnimationState("animationObstaculos04");
        animationObs04 -> setEnabled(true);
        animationObs04 -> setLoop(true);
		
        colocarMonedas();

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